from PinkQueue import PinkQueue
from threading import Thread
from threading import Event
from SpanningTreePayload import SpanningTreePayload
from Printer import Printer

NEIGHBORS_UPDATE_MARK = "NUP"
NEIGHBORS_ASLEEP = "*"
SPANNING_TREE_MARK = "SPT"
NEIGHBORS_LIST_INDEX = 1
EMPTY_NEIGHBORS_UPDATE = "EMP"

REQUEST = 2
ANSWER_YES = 3
ANSWER_NO = 4
ANSWER_ACK = 5
FINAL_ACK = 6
DISCONNECTION = 7

DISCONNECTION_ORDER = "DISCONNECT_FROM_SPANNING_TREE"

SENDING_SIDE_EXPECTED_RESQUESTS = {ANSWER_YES, ANSWER_NO, FINAL_ACK}
RECEIVING_SIDE_EXPECTED_RESQUESTS = {REQUEST, ANSWER_ACK}


class SpanningTreeMiniDispatcher(Thread):

    def __init__(self, printer: Printer):

        Thread.__init__(self)

        self.printer = printer

        self.receptionQueue = PinkQueue()

        # Sending side.
        self.toSendingSideAccessPoint = None
        self.sendingSideNeighborsUpdatQueue = None
        self.timeoutForParentAnswer = None

        # Receiving side.
        self.toReceivingSideAccessPoint = None
        self.receivingSideNeighborsUpdateQueue = None
        self.timeoutForChildAnswer = None
        self.keepWorking = True

        # Disconnection
        self.toDisconnectionChecker = None

    def run(self):

        while self.keepWorking:

            request = self.receptionQueue.pop()

            if NEIGHBORS_UPDATE_MARK in request:
                self.printer.securePrint("***** Spanning Tree Mini Dispatcher ***** A neighbors update has been received: " + str(request)
                                         + " . It will be dispatched to spanning tree modules.\n")
                self.distpatchNeighborsUpdate(request)

            elif SPANNING_TREE_MARK in request:

                spanningTreePayload = SpanningTreePayload()

                #self.printer.securePrint("Request that causes everyting to die: " + request)
                spanningTreePayload.buildFromFormattedMessage(request)

                if spanningTreePayload.requestType in SENDING_SIDE_EXPECTED_RESQUESTS:
                    #self.printer.securePrint("***** Spanning Tree Mini Dispatcher ***** A spanning tree request for the sending side has been received. It will be dispatched ... " + "Request: " + str(request) + "\n")
                    self.toSendingSideAccessPoint.push(spanningTreePayload)
                    self.timeoutForParentAnswer.set()

                elif spanningTreePayload.requestType in RECEIVING_SIDE_EXPECTED_RESQUESTS:
                    #self.printer.securePrint("***** Spanning Tree Mini Dispatcher ***** A spanning tree request for the receiving side has been received. It will be dispatched ..." + "Request: " + str(request) + "\n")
                    self.toReceivingSideAccessPoint.push(spanningTreePayload)
                    self.timeoutForChildAnswer.set()

                elif spanningTreePayload.requestType == DISCONNECTION:
                     self.toDisconnectionChecker.push([DISCONNECTION_ORDER])

            self.shouldWorkContinue(request)

    def distpatchNeighborsUpdate(self, update):

        neighborsHearbeatStatus = update.split(":")[NEIGHBORS_LIST_INDEX]
        updatedNeighborsList = [EMPTY_NEIGHBORS_UPDATE]

        if neighborsHearbeatStatus != NEIGHBORS_ASLEEP:
            updatedNeighborsList = neighborsHearbeatStatus.split(',')
            updatedNeighborsList = list(map(int, updatedNeighborsList))
            updatedNeighborsList.sort()

        self.sendingSideNeighborsUpdatQueue.push(updatedNeighborsList)
        self.receivingSideNeighborsUpdateQueue.push(updatedNeighborsList)

        if EMPTY_NEIGHBORS_UPDATE in updatedNeighborsList:
            self.toDisconnectionChecker.push([])
        else:
            self.toDisconnectionChecker.push(updatedNeighborsList)


    def shouldWorkContinue(self, request):
        self.keepWorking = request != ''

    def setSendingSideAccessPoint(self, sendingSideAccessPoint: PinkQueue):
        self.toSendingSideAccessPoint = sendingSideAccessPoint
        return self

    def setSendingSideNeighborsUpdate(self, sendingSideNeighborsUpdate: PinkQueue):
        self.sendingSideNeighborsUpdatQueue = sendingSideNeighborsUpdate
        return self

    def setTimeoutForParentAnswer(self, timeoutForParentAnswer: Event):
        self.timeoutForParentAnswer = timeoutForParentAnswer
        return self

    def setReceivingSideAccessPoint(self, receivingSideAccessPoint: PinkQueue):
        self.toReceivingSideAccessPoint = receivingSideAccessPoint
        return self

    def setReceivingSideNeighborsUpdate(self, receivingSideNeighborsUpdate: PinkQueue):
        self.receivingSideNeighborsUpdateQueue = receivingSideNeighborsUpdate
        return self

    def setTimeoutForChildAnswer(self, timeoutForChildAnswer: Event):
        self.timeoutForChildAnswer = timeoutForChildAnswer
        return self

    def setDisconnectionChecker(self, toDisconnectionChecker: PinkQueue):
        self.toDisconnectionChecker = toDisconnectionChecker
        return self

    def getQueue(self):
        return self.receptionQueue
