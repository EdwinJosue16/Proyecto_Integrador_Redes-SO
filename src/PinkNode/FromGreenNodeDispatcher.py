
from PinkQueue import PinkQueue
from threading import Thread
from Printer import Printer

NEIGHBORS_UPDATE_MARK = "NUP"
NEIGHBORS_ASLEEP = "*"
SPANNING_TREE_MARK = "SPT"
FORWARDING_MARK = "BRU"

class FromGreenNodeDispatcher(Thread):

    def __init__(self, printer: Printer):
        Thread.__init__(self)
        self.requestsFromGreenNode = PinkQueue()
        self.toSpanningTreeAccessPoint = None
        self.toForwardingAccessPoint = None
        self.keepWorking = True
        self.printer = printer

    def run(self):
        #self.printer.securePrint(" ***** From Green Node Dispatcher ***** < It has started. >\n")
        while self.keepWorking:

            #self.printer.securePrint(" ***** From Green Node Dispatcher ***** < I am waiting for requests to arrive. >\n")
            request = self.requestsFromGreenNode.pop()
            #self.printer.securePrint(" ***** From Green Node Dispatcher ***** < The received request is " + "<" + str(request) + "> >\n")

            if NEIGHBORS_UPDATE_MARK in request or SPANNING_TREE_MARK in request:
                #self.printer.securePrint(" ***** From Green Node Dispatcher ***** The request <" + request + "> has been redirected to spanning tree mini dispatcher.\n")
                self.toSpanningTreeAccessPoint.push(request)
            
            if FORWARDING_MARK in request:
                #self.printer.securePrint(" ***** From Green Node Dispatcher ***** The request <" + request + "> has been redirected tp forwarding module.\n")
               # if self.toForwardingAccessPoint:
               print("Dispatcher from green: BRU UPDATE")
               self.toForwardingAccessPoint.push(request)

            self.shouldWorkContinue(request)
        #self.printer.securePrint(" ***** From Green Node Dispatcher ***** < It has ended. >\n")

    def shouldWorkContinue(self, request):
        self.keepWorking = request != ''

    def setToSpanningTreeAccessPoint(self, spanningTreeAccessPoint: PinkQueue):
        self.toSpanningTreeAccessPoint = spanningTreeAccessPoint
        return self

    def setToForwardingAccessPoint(self, forwardingAccessPoint: PinkQueue):
        self.toForwardingAccessPoint = forwardingAccessPoint
        return self

    def getQueue(self):
        return self.requestsFromGreenNode
