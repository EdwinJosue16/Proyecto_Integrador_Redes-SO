
from threading import  Thread
from Semaphore import  Sempahore
from threading import  Lock

WITHOUT_PARENT = -1000
ROOT_NODE = 1

class SharedNotifier(Thread):
    
    def __init__(self):
        Thread.__init__(self)
        self.parent = WITHOUT_PARENT
        self.reconnection = None
        self.mutex = Lock()
        self.myIdentity = 0

    def getParent(self):
        return self.parent

    def setParent(self, parent):
        self.mutex.acquire()
        self.parent = parent
        self.mutex.release()

    def inSpanningTree(self):

        inSpanningTree = False
        if (self.myIdentity == ROOT_NODE) or (self.myIdentity != ROOT_NODE and self.parent != WITHOUT_PARENT):
            inSpanningTree =  True
        return inSpanningTree
        
    def notifyReconnection(self):
        self.reconnection.signal()

    def setReconnectionSempahore(self, reconnection):
        self.reconnection = reconnection
        return self

    def setIdentity(self, myIdentity):
        self.myIdentity = myIdentity

    def amITheRootNode(self):
        return self.myIdentity == ROOT_NODE

    def doIHaveParent(self):
        return self.parent != WITHOUT_PARENT

    def removeParentFromSpanningTree(self):
        self.parent = WITHOUT_PARENT
        self.notifyReconnection()

    def getIdentity(self):
        return self.myIdentity

    

