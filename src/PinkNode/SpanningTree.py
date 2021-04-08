from random import randint
from threading import Event
from threading import Thread
from PinkQueue import PinkQueue
import queue
from datetime import datetime
from SpanningTreePayload import SpanningTreePayload
from Semaphore import  Sempahore
from threading import Lock

# import Routing   
import os 
import sys
import threading

# Definition  of constant values
REQUEST = 2
ANSWER_YES = 3
ANSWER_NO = 4
ANSWER_ACK = 5
FINAL_ACK = 6
DISCONNECTION = 7
SENDING_SIDE_EXPECTED_RESQUESTS = {ANSWER_YES, ANSWER_NO, FINAL_ACK}
RECEIVING_SIDE_EXPECTED_RESQUESTS = {REQUEST, ANSWER_ACK}

ROOTNODE = 1
ATTEMPTS = 3
TIMEOUT = 3
NEIGHBORS_LIST_INDEX = 1
WITHOUT_PARENT =-1000

# General requests types.
SPANNING_TREE_MARK = "SPT"
NEIGHBORS_UPDATE_MARK = "NUP"
NEIGHBORS_IN_SPANNING_TREE_TABLE = "NSP"
NEIGHBORS_ASLEEP = "*"
REQUEST_DENIED = "DENIED"
REQUEST_SUCCESSFULLY_COMPLETED = "COMPLETED"

class SpanningTree:

    # Constructor del arbol generador
    def __init__(self, myIdentity, neighbors, agentForSending, agentForReceiving):

        self.neighborsInSpanningTree = []

        self.generalRequestQueue = PinkQueue()
        self.answersToMyRequestsQueue = PinkQueue()
        self.answersOfTentativeChildrenQueue = PinkQueue()

        self.timeoutForParentResponses = Event()
        self.myIdentity = int(myIdentity)

        self.timeoutForAnswerACK = Event()

        self.modifiedNeighborsList = []
        self.currentNeighborsList = []

        self.inSpanningTree = myIdentity == ROOTNODE
        self.inSpanningTreeMutex = Lock()
        
        self.agentForSending = agentForSending

        self.inCommunicationWithATentativeChild = False
        self.inCommunicationWithATentativeChildMutex = Lock()
        
        self.sn = 0
        self.rn = 0

        # It is used in disconnection.
        self.parent = WITHOUT_PARENT

        # A node can disconnect from the spanning tree in the middle of a parent-child-process.  
        self.tentativeChildId = 0
        self.tentativeParentId = 0 

        self.log = open("../src/PinkNode/Files/Logs/SpanningTreeLog" +  str(myIdentity) + ".txt", "w+")

        self.isThereAnyNeighorsUpdate = Sempahore(0)

        self.doIneedToReconnect = Sempahore(1)

        self.threads = [    Thread(target=self.requestParentship, args=()), 
                            Thread(target=self.processPackagesFromGreenNode, args=()), 
                            Thread(target=self.respondParentshipRequest, args=()) ]
        self.log.write('[' + str(datetime.now()) + '] ' + "The spanning Tree module has been built.")
            
    def start(self):
        self.log.write('[' + str(datetime.now()) + '] ' + "All threads have started.")
        for thread in self.threads:
            thread.start()       

    def waitToFinish(self):
         for thread in self.threads:
            thread.join()  

    def processPackagesFromGreenNode(self):

        while True:
            request = self.generalRequestQueue.pop()
            
            if NEIGHBORS_UPDATE_MARK in request:
                print("Process packages: New list from heartbeat has been received")
                self.updateNeighorsList(request)

            elif SPANNING_TREE_MARK in request:
                print("Process packages: spanning tree package was receive")
                spanningTreePayload = self.buildSpanningTreePayload(request)

                if not self.inSpanningTree and spanningTreePayload.requestType in SENDING_SIDE_EXPECTED_RESQUESTS and not self.inCommunicationWithATentativeChild:
                    print("Process packages: A package from tentative parent has been received")  
                    self.answersToMyRequestsQueue.push(spanningTreePayload)
                    self.timeoutForParentResponses.set()

                elif spanningTreePayload.requestType in RECEIVING_SIDE_EXPECTED_RESQUESTS:
                    print("Process packages: A package from tentative child has been received") 
                    self.answersOfTentativeChildrenQueue.push(spanningTreePayload)
                    self.timeoutForAnswerACK.set()

                elif spanningTreePayload.requestType == DISCONNECTION:
                    print("Process packages: Oh no, I received a DISCONNECTION request")
                    self.removeParentFromMySpanningTree()
                    self.iAmNoLongerInSpanningTree()
                    self.replicateDisconnectionToMyChildren() 

    def requestParentship(self):

            self.log.write('[' + str(datetime.now()) + '] ' + "The finding parent process has started\n" )

            while True and self.myIdentity!=ROOTNODE:

                self.doIneedToReconnect.wait()

                print("Child's perspective: I will go to find my parent...")

                while not self.inSpanningTree:

                    print("Child's perspective: I am not in spanning tree...")
                
                    self.isThereAnyNeighorsUpdate.wait()

                    self.currentNeighborsList = self.modifiedNeighborsList 

                    print("A neighbors update has been received:",  self.currentNeighborsList)
                    for neighbor in self.currentNeighborsList:

                        print("Neighbors list in current interation",  self.currentNeighborsList)

                        if neighbor == ROOTNODE:
                            print("Sending request to root node")

                        print("Trying connecting to neighbor:", neighbor)

                        receivedReply = self.tryEstablishFirstConnection(neighbor)
                    
                        # It continue with protocol in case that EstablishFirstConnection was successfully
                        self.continueCommunication(receivedReply)

                        if self.inSpanningTree:
                            print("Child's perspective: Now, I am in spanning tree...")
                            break
                        else:
                            self.log.write('[' + str(datetime.now()) + '] ' + "The finding parent process: trying with another neighbor\n" )
                            print("Child's perspective: Current neighbor # ", neighbor, "can't be my parent")
                
    
    # This is for send the first request to the current neighbor, return true if the connection has established
    def tryEstablishFirstConnection(self, neighborId):
        print("Try establishing connection before while connection with ", neighborId)
        self.sn = randint(0,1000)
        attempt = 0
        rn = 0
        initialRequest = self.buildRequest(neighborId, REQUEST, self.sn, rn)
        receivedReply = False
        while  attempt < ATTEMPTS and  not receivedReply:
            print("Child's perspective: try establishing connection with ", neighborId)
            self.agentForSending.requestsForGreenNodeQueue.enqueue(initialRequest)
            attempt+= 1
            receivedReply = self.timeoutForParentResponses.wait(TIMEOUT)

        if not receivedReply:
            print("It was impossible to connect with neighbor:", neighborId)

        return receivedReply

    def continueCommunication(self, receivedReply):
         endConnection = False
         while receivedReply and not endConnection:
            # It gets the reply from the queue (YES, NO, ACK)
            reply = self.answersToMyRequestsQueue.pop()
            print("Child's perspective: I received a reply from tentative parent, I will go to check sn....")

            # Expected sequence number + 1.
            if reply.sn == self.sn + 1:
                self.sn +=1 
                print("Child's perspective: I incremented my sn, because the package is ok... new sn=", self.sn)

                endConnection = reply.requestType == FINAL_ACK or reply.requestType == ANSWER_NO
                # It processes the reply, based on reply type
                newRequest = self.checkAndRespondReply(reply)

                if not endConnection:
                    # It sends the new request to Green Node
                    self.agentForSending.requestsForGreenNodeQueue.enqueue(newRequest)
                    # It waits for new reply, after send the request 
                    receivedReply = self.timeoutForParentResponses.wait(TIMEOUT)
                else:
                    self.log.write('[' + str(datetime.now()) + '] ' + "The reply status is: "+ newRequest)
            else:
                print("An unexpected payload was received...")
                self.log.write('[' + str(datetime.now()) + '] ' + "An unexpected payload was received: ")           
                    
    def checkAndRespondReply(self, spanningTreePayload): 
        # Child's perspective.
        answerForRequest = ""
        if spanningTreePayload.requestType == ANSWER_YES:
            
            print("Child's perspective: I received yes answer, the sn is of the package is: ", spanningTreePayload.sn)

            answerForRequest = self.buildRequest(spanningTreePayload.sourceId, ANSWER_ACK, self.sn, spanningTreePayload.rn + 1)
            self.neighborsInSpanningTree.append(spanningTreePayload.sourceId)

            self.parent = spanningTreePayload.sourceId
            self.changeInSpanningTreeStatus(True)
            self.sendNeighborsInSpanningTreeTable()

            self.log.write('[' + str(datetime.now()) + '] ' + "The request from " + str(spanningTreePayload.sourceId) + "was accepted.")
            print("The request from " + str(spanningTreePayload.sourceId) + "was accepted.")

        elif spanningTreePayload.requestType == ANSWER_NO:
            print("Child's perspective: I received my final no answer from my neighbor...")
            self.log.write('[' + str(datetime.now()) + '] ' + "The request to " + str(spanningTreePayload.sourceId) + "was denied.")
            answerForRequest = REQUEST_DENIED

        elif spanningTreePayload.requestType == FINAL_ACK:
            answerForRequest = REQUEST_SUCCESSFULLY_COMPLETED
            print("Child's perspective: I received my final ACK, I got a parent...")
            self.log.write('[' + str(datetime.now()) + '] ' + "The request to " + str(spanningTreePayload.sourceId) + "was .")

        return answerForRequest


    def updateNeighorsList(self, request):

        messageSplittedByType = request.split(":")

        if messageSplittedByType[NEIGHBORS_LIST_INDEX] == NEIGHBORS_ASLEEP:
            self.modifiedNeighborsList = []
            self.log.write('[' + str(datetime.now()) + '] ' + "All neighbors are sleeping.")
        else:
            messageSplittedByNeighborId = messageSplittedByType[NEIGHBORS_LIST_INDEX].split(',')
            self.modifiedNeighborsList = list (map(int, messageSplittedByNeighborId))
            self.modifiedNeighborsList.sort()
            self.isThereAnyNeighorsUpdate.signal()

            if not self.amITheRootNode():
                if self.doIHaveParent():
                    if self.isMyParentSleeping():
                        print("I am no longer in spanning tree because my parent is sleeping. I will replicate disconnection to my children.")
                        self.removeParentFromMySpanningTree()
                        self.iAmNoLongerInSpanningTree()
                        self.replicateDisconnectionToMyChildren()
                        self.sendNeighborsInSpanningTreeTable() # It notifies broadcast module in spanning tree of the new changes.

        self.checkIfAnyChildHasDiedInSpanningTree()
        self.log.write('[' + str(datetime.now()) + '] ' + "Neighbors update." + str(self.modifiedNeighborsList))


    def respondParentshipRequest(self):
        # Parent's perspective.
        while True:
            print("Respond parentship request: I am waiting for a new request")
            spanningTreePayload = self.answersOfTentativeChildrenQueue.pop()
            newSpanningTreePayload = None

            if not self.modifiedNeighborsList and spanningTreePayload:
                print("A resquest was received but I know no one because I have not received the first heateat update.")

            if  not self.inCommunicationWithATentativeChild:
                print("Respond parentship request: In comunication with a tentative child...")
                if spanningTreePayload.requestType == REQUEST:
                    print("Respond parentship request: Received a new request.. from node ", spanningTreePayload.sourceId)

                    if self.inSpanningTree and spanningTreePayload.sourceId in self.modifiedNeighborsList:
                        print("Respond parentship request: I can respond the reply, because I am in spanning tree...")
                        self.rn = randint(1, 1000)
                        print("MY ORIGINAL RN IS", self.rn)
                        answerForRequest = self.buildRequest(spanningTreePayload.sourceId, ANSWER_YES, spanningTreePayload.sn + 1, self.rn)
                        self.changeInCommunicationWithATentativeChildStatus(True)

                        acknowledgement =  False 
                        expectedRn = False
                        self.tentativeChildId = spanningTreePayload.sourceId

                        # It constantly sends the YES answer until the confirmation ACK is received.
                        while not expectedRn and (self.tentativeChildId in self.modifiedNeighborsList):

                            self.agentForSending.requestsForGreenNodeQueue.enqueue(answerForRequest)
                            acknowledgement = self.timeoutForAnswerACK.wait(6)

                            if acknowledgement:
                                newSpanningTreePayload = self.answersOfTentativeChildrenQueue.pop()
                                expectedRn = newSpanningTreePayload.rn == (self.rn + 1)
                                print("RECEIVED RN IS:", newSpanningTreePayload.rn)
                        
                        if self.tentativeChildId in self.modifiedNeighborsList:
                            self.neighborsInSpanningTree.append(newSpanningTreePayload.sourceId)
                            answerForRequest = self.buildRequest(newSpanningTreePayload.sourceId, FINAL_ACK, newSpanningTreePayload.sn + 1,self.rn+1)
                            self.agentForSending.requestsForGreenNodeQueue.enqueue(answerForRequest)
                            self.sendNeighborsInSpanningTreeTable() # It notifies broadcast module in spanning tree of the new changes.
                        else:
                            print("Tentative child is asleep: ", spanningTreePayload.sourceId, self.modifiedNeighborsList)
                        self.changeInCommunicationWithATentativeChildStatus(False)
                    else:
                        print("I am telling no to", spanningTreePayload.sourceId)
                        answerForRequest = self.buildRequest(spanningTreePayload.sourceId, ANSWER_NO, spanningTreePayload.sn + 1, self.rn+1)
                        self.agentForSending.requestsForGreenNodeQueue.enqueue(answerForRequest)
                        self.changeInCommunicationWithATentativeChildStatus(False)

    def replicateDisconnectionToMyChildren(self):
        for child in self.neighborsInSpanningTree:
            self.neighborsInSpanningTree.remove(child)
            self.agentForSending.requestsForGreenNodeQueue.enqueue(self.buildRequest(child, DISCONNECTION, 0, 0))

    def sendNeighborsInSpanningTreeTable(self):
        formattedNeighborsTable = NEIGHBORS_IN_SPANNING_TREE_TABLE + ":"
        for neighbor in self.neighborsInSpanningTree:
                formattedNeighborsTable += str(neighbor) + ","
                
        # It removes the last appended coma.
        formattedNeighborsTable[:-1]
        formattedNeighborsTable += "\0"
        self.agentForSending.requestsForGreenNodeQueue.enqueue(formattedNeighborsTable)

    def changeInCommunicationWithATentativeChildStatus(self, status):
        self.inCommunicationWithATentativeChildMutex.acquire()
        self.inCommunicationWithATentativeChild = status
        self.inCommunicationWithATentativeChildMutex.release()

    def changeInSpanningTreeStatus(self, status):
        self.inSpanningTreeMutex.acquire()
        self.inSpanningTree = status
        self.inSpanningTreeMutex.release()

    def buildRequest(self, destinationId, requestType, sn, rn):
        return SPANNING_TREE_MARK + ":" + str(self.myIdentity) + ";" + str(destinationId) + ";" + str(requestType) + "," + str(sn) + "," + str(rn)

    def buildSpanningTreePayload(self, formattedRequest):
        # Expected request format: SPT:source-id;destination-id;type,SN,RN.
        spanningTreeMessage = SpanningTreePayload()
        spanningTreeMessage.buildFromFormattedMessage(formattedRequest)
        self.log.write('[' + str(datetime.now()) + '] ' + "A spanning tree payload was build from the received formatted message.")
        return spanningTreeMessage

    # Methods to improve code legibility.

    def doIHaveParent(self):
        return self.parent != WITHOUT_PARENT

    def isMyParentSleeping(self):
        return not self.parent in self.modifiedNeighborsList

    def amITheRootNode(self):
        return self.myIdentity == ROOTNODE

    def iAmNoLongerInSpanningTree(self):
        self.changeInSpanningTreeStatus(False)

    def removeParentFromMySpanningTree(self):
        self.neighborsInSpanningTree.remove(self.parent)
        self.parent = WITHOUT_PARENT
        self.doIneedToReconnect.signal()

    def removeChildFromSpanningTree(self, neighborId):
        self.neighborsInSpanningTree.remove(neighborId)

    def checkIfAnyChildHasDiedInSpanningTree(self):
        print("Checking if any child has died")
        anyChange = False
        for neighbor in self.neighborsInSpanningTree:
            if neighbor != self.parent:
                if not neighbor in self.modifiedNeighborsList:
                    print("My child with id", neighbor, "is no longer in my spanning tree")
                    anyChange = True
                    print("Before removing child", self.modifiedNeighborsList)
                    self.removeChildFromSpanningTree(neighbor)
                    print("Oh fuck I just removed my child from my children list", self.modifiedNeighborsList)
        if anyChange:
             self.sendNeighborsInSpanningTreeTable() # It notifies broadcast module in spanning tree of the new changes.
