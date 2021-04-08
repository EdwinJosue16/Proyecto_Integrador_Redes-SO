from threading import Lock
from datetime import datetime

class Printer:
    
    def __init__(self):
        self._lock = Lock()
        self.log = None
    
    def securePrint(self,string):
        self._lock.acquire()
        print(string)
        self.log.write('[' + str(datetime.now()) + '] ' + string)
        self._lock.release()
    
    def closePrinter(self):
        self.log.close()

    def setNodeIdentity(self, nodeIdentity):
        self.log = open("../src/PinkNode/Files/Logs/SpanningTreeLog" +  str(nodeIdentity) + ".txt", "w+")

