from random import randint
from threading import Event
from threading import Thread
from PinkQueue import PinkQueue
from SpanningTreePayload import SpanningTreePayload
from Semaphore import  Sempahore
from Printer import Printer
from SharedNotifier import  SharedNotifier
from SpanningTreeUpdatesNotifier import  SpanningTreeUpdatesNotifier

# import Routing   
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

ROOT_NODE = 1
ATTEMPTS = 3
TIMEOUT = 10.0
WITHOUT_PARENT =-1000

EMPTY_NEIGHBORS_UPDATE = "EMP"
SPANNING_TREE_MARK = "SPT"
REQUEST_DENIED = "DENIED"
REQUEST_SUCCESSFULLY_COMPLETED = "COMPLETED"

class SpanningTreeSendingSide(threading.Thread):

    def __init__(   self, printer:Printer):
        threading.Thread.__init__(self)
        
        self.printer=printer

        self.myIdentity = 0
        self.forSpanningTreeMiniDispatcherQueue = PinkQueue()
        self.neighborsUpdateQueueForSendingSide = PinkQueue()
        self.doIneedToReconnect = Sempahore(1)
        self.timeoutForParentResponses = Event()

        self.currentLivingNeighbor = []
        self.agentForSendingQueue = None
        self.sharedNotifier =  None
        self.spanningTreeUpdatesNotifier = None

        self.sn = 0
        self.parent = WITHOUT_PARENT

    def run(self):
        self.requestParentship()
    
    def waitToFinish(self):
        self.join()

    def requestParentship(self):

        if self.canRequestParentship():

            self.printer.securePrint("Child's perspective: The finding parent process has started\n")

            while True:
                # At the first time the Semaphore has 1
                self.printer.securePrint("Child's perspective: waiting find parentship signal...\n")
                self.doIneedToReconnect.wait()
                self.printer.securePrint("Child's perspective: received find parentship signal...\n")

                while not self.inSpanningTree():
                    self.printer.securePrint("Child's perspective: I am not in spanning tree... I need to find a parent...\n")
                    self.printer.securePrint("Child's perspective: waiting living neighbors update...\n")
                    self.waitForNeighborsUpdate()
                    self.printer.securePrint("Child's perspective: I receive a neighbors update \n")

                    for neighborId in self.currentLivingNeighbor:
                        self.sendParentshipRequestTo(neighborId)

                        if  not self.inSpanningTree():
                            self.printer.securePrint("Child's perspective: I need to continue finding my parent... \n")
                            pass
                        else:
                            self.printer.securePrint("Child's perspective: *** Now, I am in spanning tree :D ***  \n")
                            break
    
    def sendParentshipRequestTo(self, neighborId):
        self.printer.securePrint("Child's perspective: I will send request to " +str(neighborId) +"\n")
        self.sn = randint(0,1000)
        attempt = 0
        rn = 0
        initialRequest = self.buildRequest(neighborId, REQUEST, self.sn, rn)
        receivedReply = False
        while  attempt < ATTEMPTS and not receivedReply:
            self.printer.securePrint("Child's perspective: Try establishing connection with " +str(neighborId) +"\n")
            self.agentForSendingQueue.push(initialRequest)
            receivedReply = self.timeoutForParentResponses.wait(TIMEOUT)
            attempt+= 1

        if receivedReply:
            self.printer.securePrint("Child's perspective: I receive a package, I will check it...\n")
            reply = self.forSpanningTreeMiniDispatcherQueue.pop()
            self.reviewParentshipReply(reply)
        else:
            self.printer.securePrint("Child's perspective: Try establishing connection with "+ str(neighborId)+ "\n")

    def reviewParentshipReply(self, reply:SpanningTreePayload):

        # The SN is incremented after the request was sended, so it will be the next expected request number.
        self.sn += 1

        if reply.sn == self.sn:
            self.printer.securePrint("Child's perspective: Nice I received correct reply from "+ str(reply.sourceId)+ "\n")
            if reply.requestType == ANSWER_YES:
                self.printer.securePrint("Child's perspective: Answer yes from:  "+ str(reply.sourceId)+ "\n")
                self.finalComunicationForGetingParent(reply)
            else:
                self.printer.securePrint("Child's perspective: Answer no from:  "+ str(reply.sourceId)+ "\n")


    def finalComunicationForGetingParent(self, reply:SpanningTreePayload):
        self.sn += 1

        rn = reply.rn + 1
        answerAckRequest = self.buildRequest(reply.sourceId, ANSWER_ACK, self.sn, rn)

        # After sending my answer ack I will be expecting a reply with an incremented one.
        self.sn +=1 

        self.agentForSendingQueue.push(answerAckRequest)
        receivedReply = self.timeoutForParentResponses.wait(TIMEOUT)

        if receivedReply:
            self.printer.securePrint("Child's perspective: In final comunication I receive a package, I will check it... \n")
            finalAckReply = self.forSpanningTreeMiniDispatcherQueue.pop()
            if finalAckReply.sn == self.sn and finalAckReply.requestType == FINAL_ACK:
                self.printer.securePrint("Child's perspective: In final comunication I receive my final ACK correctly \n")
                self.modifySpanningTreeValues(finalAckReply)
                self.spanningTreeUpdatesNotifier.notifySpanningTreeTableUpdate()
            else:
                 self.printer.securePrint("Child's perspective: In final comunication my final ACK package was wrong \n")
                 self.printer.securePrint("Expected SN: " + str(self.sn + 1) + ". Received SN:" + str(finalAckReply.sn) + ". Request type: " + str(finalAckReply.requestType) + "\n")
        else:
            self.printer.securePrint("Child's perspective: In final comunication Timeout for receiving final ACK..... \n")
            
    def modifySpanningTreeValues(self, finalAckReply:SpanningTreePayload):
        self.sharedNotifier.setParent(finalAckReply.sourceId)

    def buildRequest(self, destinationId, requestType, sn, rn):
        return SPANNING_TREE_MARK + ":" + str(self.myIdentity) + ";" + str(destinationId) + ";" + str(requestType) + "," + str(sn) + "," + str(rn)

    def inSpanningTree(self):
        return self.sharedNotifier.inSpanningTree()

    def getReconnectionSempahore(self):
        return self.doIneedToReconnect

    def canRequestParentship(self):
        return self.myIdentity != ROOT_NODE and not self.inSpanningTree()

    def waitForNeighborsUpdate(self):
        keepWaiting = True
        while keepWaiting:
            neighborsHearbeatUpdate = self.neighborsUpdateQueueForSendingSide.pop()
            if not EMPTY_NEIGHBORS_UPDATE in neighborsHearbeatUpdate:
                self.currentLivingNeighbor = neighborsHearbeatUpdate
                keepWaiting = False


    """ Getters and setters """

    def getQueue(self):
        return self.forSpanningTreeMiniDispatcherQueue

    def getNeighborsUpdateQueue(self):
        return self.neighborsUpdateQueueForSendingSide
    
    def getReconnectionState(self):
        return self.doIneedToReconnect

    def getTimeoutForParentAnswer(self):
        return self.timeoutForParentResponses

    def setIdentity(self, identity: int):
        self.myIdentity = identity
        return self

    def setAgentForSendingQueue(self, agentForSendingQueue: PinkQueue):
        self.agentForSendingQueue = agentForSendingQueue
        return self

    def setSharedNotifier(self, sharedNotifier: SharedNotifier):
        self.sharedNotifier = sharedNotifier
        return self

    def setSpanningTreeUpdatesNotifier(self, spanningTreeUpdatesNotifier: SpanningTreeUpdatesNotifier):
        self.spanningTreeUpdatesNotifier = spanningTreeUpdatesNotifier
        return self
    
        





        
        