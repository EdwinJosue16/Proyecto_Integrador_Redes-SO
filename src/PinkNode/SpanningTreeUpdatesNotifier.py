
from SharedNotifier import SharedNotifier
from PinkQueue import PinkQueue
import time
from threading import  Lock

WITHOUT_PARENT = -1000
NEIGHBORS_IN_SPANNING_TREE_TABLE = "NSP"
SLEEPING_TIME_FOR_FINAL_ACK_TO_REACH = 3
ROOTNODE=1
class SpanningTreeUpdatesNotifier:

    def __init__(self):
        self.sharedNotifier = None
        self.childrenInSpanningTree = []
        self.sendingQueue = None
        self.mutex = Lock()

    def notifySpanningTreeTableUpdate(self):
        
        formattedNeighborsTable = NEIGHBORS_IN_SPANNING_TREE_TABLE + ":"

        if self.disconnectionOcurred() and (self.sharedNotifier.getIdentity() != ROOTNODE):
            formattedNeighborsTable += "EMPTY"
        else:
            for child in self.childrenInSpanningTree:
                formattedNeighborsTable += str(child) + ","
            if(self.sharedNotifier.getIdentity()!=ROOTNODE):
                formattedNeighborsTable += str(
                    self.sharedNotifier.getParent()) + "\0"
        print("SpanningTreeUpdatesNotifier: Spanning Table to send ="+ formattedNeighborsTable)
        self.sendingQueue.push(formattedNeighborsTable)

    def disconnectionOcurred(self):
        return self.sharedNotifier.getParent() == WITHOUT_PARENT

    def setSharedNotifier(self, sharedNotifier: SharedNotifier):
        self.sharedNotifier = sharedNotifier
        return self

    def setSendingQueue(self,  sendingQueue: PinkQueue):
        self.sendingQueue = sendingQueue
        return self

    def getChildrenInSpanningTree(self):
        return self.childrenInSpanningTree

    def addChild(self, child):
        self.mutex.acquire()
        self.childrenInSpanningTree.append(child)
        self.mutex.release()

    def removeChild(self, child):
        self.mutex.acquire()
        self.childrenInSpanningTree.remove(child)
        self.mutex.release()

        
