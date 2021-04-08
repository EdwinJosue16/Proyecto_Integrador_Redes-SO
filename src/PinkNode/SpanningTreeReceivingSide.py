from random import randint
from threading import Event
from threading import Thread
from PinkQueue import PinkQueue
from SpanningTreePayload import SpanningTreePayload
from Semaphore import  Sempahore
from Printer import Printer
from SharedNotifier import SharedNotifier
from SpanningTreeUpdatesNotifier import  SpanningTreeUpdatesNotifier

REQUEST = 2
ANSWER_YES = 3
ANSWER_NO = 4
ANSWER_ACK = 5
FINAL_ACK = 6
DISCONNECTION = 7
SENDING_SIDE_EXPECTED_RESQUESTS = {ANSWER_YES, ANSWER_NO, FINAL_ACK}
RECEIVING_SIDE_EXPECTED_RESQUESTS = {REQUEST, ANSWER_ACK}

EMPTY_NEIGHBORS_UPDATE = "EMP"

ROOTNODE = 1
ATTEMPTS = 3
TIMEOUT = 10.0
WITHOUT_PARENT =-1000

NEIGHBORS_IN_SPANNING_TREE_TABLE = "NSP"
SPANNING_TREE_MARK = "SPT"
REQUEST_DENIED = "DENIED"
REQUEST_SUCCESSFULLY_COMPLETED = "COMPLETED"

class SpanningTreeReceivingSide(Thread):
    
    def __init__(self, printer: Printer):
        Thread.__init__(self)

        self.printer = printer
        
        self.myIdentity = 0

        self.tentativeChildrenRequests = PinkQueue()
        self.neighborsUpdateQueue = PinkQueue()
        self.timeoutForChildAnswer = Event()
        self.updatedNeighborsList = []

        self.agentForSendingQueue = None
        self.spanningTreeUpdatesNotifier = None
        self.sharedNotifier = None
        self.childrenInSpanningTree = None

        self.communicatingWithATentativeChild = False
        self.rn = 0

        self.neighborsHearbeatStatusUpdater = Thread(target=self.waitForAMeaningfullNeighborsUpdate, args=())
     
    def run(self):
        self.respondParentshipRequest()
    
    def respondParentshipRequest(self):

        self.neighborsHearbeatStatusUpdater.start()

        while True:

            self.printer.securePrint("***** Spanning Tree Parent ***** < I am waiting for a parentship request >.\n")

            # The parent waits until a neighbors hearbeat update and a child request have both been received.
            tentativeChildRequest = self.tentativeChildrenRequests.pop()
            tentativeChildId = tentativeChildRequest.sourceId

            if self.openToAcceptRequests():

                self.printer.securePrint("***** Spanning Tree Parent ***** < Processing parenship request from node: " + str(tentativeChildRequest.sourceId) + " >.\n")
                self.communicatingWithATentativeChild = True
                self.rn = randint(1,1000)
                answer = self.buildRequest(tentativeChildRequest.sourceId, ANSWER_YES, tentativeChildRequest.sn + 1, self.rn)

                self.rn += 1

                expectedRequestNumberReceived = False
                acknowledgementReceived = False
                childAnswer: SpanningTreePayload
                counter = 0

                self.printer.securePrint("Current processing child id " + str(tentativeChildId))
                self.printer.securePrint("Current updated neighbors list")
                print(self.updatedNeighborsList)


                while not expectedRequestNumberReceived and self.childIsAlive(tentativeChildId):

                    counter += 1
                    self.sendAnswerToChild(answer)
                    acknowledgementReceived = self.timeoutForChildAnswer.wait(TIMEOUT)

                    if acknowledgementReceived:
                        childAnswer = self.tentativeChildrenRequests.pop()
                        expectedRequestNumberReceived =  childAnswer.rn == self.rn and childAnswer.requestType == ANSWER_ACK
                        self.printer.securePrint("***** Spanning Tree Parent ***** < The ack was received from child: " + str(tentativeChildRequest.sourceId) +  ". Expected RN:" + str(self.rn) 
                                                    + ". Received RN: " + str(childAnswer.rn)  + ".Type " + str(childAnswer.requestType) + ". >\n")
                    else:
                        self.printer.securePrint("***** Spanning Tree Parent ***** < Oh wow, something was wrong in the ack. >\n")

                        if not childAnswer.rn == self.rn:
                            self.printer.securePrint("***** Spanning Tree Parent ***** < Expected RN: " + str(self.rn) + ". Received RN: " + str(childAnswer.rn) + ". >\n")
                    
                self.printer.securePrint("***** Spanning Tree Parent ***** < The cycle was executed " + str(counter) + " times >\n")

                if self.childIsAlive(tentativeChildId):

                    self.rn += 1
                    self.printer.securePrint("***** Spanning Tree Parent ***** < The closing ack was sent to child: " + str(tentativeChildRequest.sourceId) + " >.\n")
                    closingAckAnswer = self.buildRequest(childAnswer.sourceId, FINAL_ACK, childAnswer.sn + 1, self.rn)
                    self.sendAnswerToChild(closingAckAnswer)

                    self.spanningTreeUpdatesNotifier.addChild(tentativeChildId)#self.childrenInSpanningTree.append(tentativeChildId)
                    self.communicatingWithATentativeChild = False
                    self.spanningTreeUpdatesNotifier.notifySpanningTreeTableUpdate()

                else:
                    self.printer.securePrint("***** Spanning Tree Parent ***** < After the child " +  str(tentativeChildId) + " answer ack was received it was detected as sleeping. >")
                    self.communicatingWithATentativeChild = False
                    self.sendNoAnswerToTentativeChild(tentativeChildRequest)
            else:
                if not self.inSpanningTree():
                    self.printer.securePrint("***** Spanning Tree Parent ***** < Unfortunately, I am giving a no response to child" + str(tentativeChildId) + " because I am not in spanning tree >.\n")
                elif self.communicatingWithATentativeChild:
                    self.printer.securePrint("***** Spanning Tree Parent ***** < Unfortunately, I am giving a no response to child" + str(tentativeChildId) + " because currently I am communicating with other node. >\n")
                self.sendNoAnswerToTentativeChild(tentativeChildRequest)

        self.neighborsHearbeatStatusUpdater.join()
    
    def sendNoAnswerToTentativeChild(self, tentativeChildRequest: SpanningTreePayload):
        answer = self.buildRequest(tentativeChildRequest.sourceId, ANSWER_NO, tentativeChildRequest.sn + 1, self.rn)
        self.sendAnswerToChild(answer)
        self.communicatingWithATentativeChild = False

    def waitForAMeaningfullNeighborsUpdate(self):

        keepWorking = True
        keepWaiting = True

        while keepWorking:
            while keepWaiting:
                neighborsHearbeatUpdate = self.neighborsUpdateQueue.pop()
                self.updatedNeighborsList = []

                if not EMPTY_NEIGHBORS_UPDATE in neighborsHearbeatUpdate:
                    self.updatedNeighborsList = neighborsHearbeatUpdate

                    #for neighbor in  self.updatedNeighborsList:
                        #print("***** Spanning Tree Parent ***** < Neighbor in update: " + str(neighbor) + "> .\n")
                                             
    """ Utility methods """
    def sendAnswerToChild(self, answer):
         self.agentForSendingQueue.push(answer)

    def openToAcceptRequests(self):
        return self.inSpanningTree() and not self.communicatingWithATentativeChild

    def changeCommunicationStatus(self, status):
        self.communicatingWithATentativeChild = status

    def buildRequest(self, destinationId, requestType, sn, rn):
        return SPANNING_TREE_MARK + ":" + str(self.myIdentity) + ";" + str(destinationId) + ";" + str(requestType) + "," + str(sn) + "," + str(rn)

    def childIsAlive(self, childId):
        return childId in self.updatedNeighborsList

    def inSpanningTree(self):
        return self.sharedNotifier.inSpanningTree()

    """ Getters and setters """
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
        self.childrenInSpanningTree = self.spanningTreeUpdatesNotifier.getChildrenInSpanningTree()
        return self
            
    def getNeighborsUpdateQueue(self):
        return self.neighborsUpdateQueue

    def getQueue(self):
        return self.tentativeChildrenRequests

    def getTimeoutForChildAnswer(self):
        return self.timeoutForChildAnswer

    




