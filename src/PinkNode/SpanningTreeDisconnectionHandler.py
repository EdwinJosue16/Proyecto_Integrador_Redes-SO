
from threading import Thread
from PinkQueue import PinkQueue
from Printer import Printer
from SpanningTreeUpdatesNotifier import SpanningTreeUpdatesNotifier
from SharedNotifier import SharedNotifier

SPANNING_TREE_MARK = "SPT"
DISCONNECTION_ORDER = "DISCONNECT_FROM_SPANNING_TREE"
DISCONNECTION = 7
ROOT_NODE = 1


class SpanningTreeDisconnectionHandler(Thread):

    def __init__(self, printer: Printer):

        Thread.__init__(self)
        self.printer = printer

        self.updatedNeighborsListQueue = PinkQueue()
        self.updatedNeighborsList = []

        self.spanningTreeUpdatesNotifier = None
        self.sharedNotifier = None
        self.sendingAgentQueue = None

        self.keepWorking = True

    def run(self):

        while self.keepWorking:
            
            self.updatedNeighborsList = self.updatedNeighborsListQueue.pop()

            if not self.amITheRootNode():
                if self.disconnectionReceived() or self.parentWentSleeping():
                    self.runDisconnectionProtocol()

            self.checkIfAnyChildWentSleeping()

    def runDisconnectionProtocol(self):
        self.printer.securePrint(
            "***** Spanning Tree Disconnection Checker ***** < I am no longer in spanning tree because my parent is sleeping. I will replicate disconnection to my children.>")
        self.removeParentFromSpanningTreeAnNotifyReconnection()
        self.replicateDisconnectionToMyChildren()
        self.spanningTreeUpdatesNotifier.notifySpanningTreeTableUpdate()

    def replicateDisconnectionToMyChildren(self):
        childrenInSpanningTree = self.spanningTreeUpdatesNotifier.getChildrenInSpanningTree()
        for child in childrenInSpanningTree:
            childrenInSpanningTree.remove(child)
            self.sendingAgentQueue.push(
                self.buildRequest(child, DISCONNECTION, 0, 0))

    def checkIfAnyChildWentSleeping(self):
        anyChange = False
        childrenInSpanningTree = list(map(int, self.spanningTreeUpdatesNotifier.getChildrenInSpanningTree()))
        
        for child in childrenInSpanningTree:

            missedChild = True
            for neighbor in self.updatedNeighborsList:
                if int(neighbor) == int(child):
                    missedChild = False

            if missedChild:
                self.printer.securePrint("***** Spanning Tree Disconnection Checker ***** < My child with id: " + str(child) + " is no longer in my spanning tree.>")
                anyChange = True
                self.removeChildFromSpanningTree(child)

        if anyChange:
             self.spanningTreeUpdatesNotifier.notifySpanningTreeTableUpdate()


    def disconnectionReceived(self):
        return DISCONNECTION_ORDER in self.updatedNeighborsList

    def parentWentSleeping(self):
        return (not self.amITheRootNode()) and (self.doIHaveParent()) and (self.isMyParentSleeping())

    def setUpdatesNotifier(self, spanningTreeUpdatesNotifier: SpanningTreeUpdatesNotifier):
        self.spanningTreeUpdatesNotifier = spanningTreeUpdatesNotifier
        return self

    def setSharedNotifier(self, sharedNotifier: SharedNotifier):
        self.sharedNotifier = sharedNotifier
        return self

    def getQueue(self):
        return self.updatedNeighborsListQueue

    def isMyParentSleeping(self):        
        isSleeping = True   
        parent = self.sharedNotifier.getParent()

        for neighbor in self.updatedNeighborsList:
            if neighbor == parent:
                isSleeping = False
        return isSleeping

    def amITheRootNode(self):
        return self.sharedNotifier.amITheRootNode()

    def doIHaveParent(self):
        return self.sharedNotifier.doIHaveParent()

    def setSendingAgentQueue(self, sendingAgentQueue: PinkQueue):
        self.sendingAgentQueue = sendingAgentQueue
        return self

    def removeParentFromSpanningTreeAnNotifyReconnection(self):
        self.sharedNotifier.removeParentFromSpanningTree()

    def buildRequest(self, destinationId, requestType, sn, rn):
        return SPANNING_TREE_MARK + ":" + str(self.sharedNotifier.getIdentity()) + ";" + str(destinationId) + ";" + str(requestType) + "," + str(sn) + "," + str(rn)

    def removeChildFromSpanningTree(self, neighborId):
        self.spanningTreeUpdatesNotifier.removeChild(neighborId)

    def getIdentity(self):
        return self.sharedNotifier.getIdentity()
