import socket
import threading
import time
from PinkQueue import PinkQueue
from SpanningTreePayload import SpanningTreePayload
NEIGHBORS_UPDATE_MARK = "NUP"
BROADCAST_UPDATE_MARK = "BRU"


TREE = 0
NEIGHBORSUPDATE = 1
REACHABILITY = 2
MAX_REQUEST_SIZE = 256


class AgentForReceivingRequest(threading.Thread):

    def __init__(self, portForReceivingRequest: int):
        threading.Thread.__init__(self)
        self.modifiedNeighborsList = []
        self.currentNeighborsList = []
        self.tcpSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.ipAddress = 'localhost'
        self.portForReceivingRequest = portForReceivingRequest
        self.keepWorking = True
        self.toDistpatcherAccessPoint = None

    def getSocketConnection(self):
        self.tcpSocket.connect((self.ipAddress, self.portForReceivingRequest))
        self.tcpSocket.setblocking(True)

    def run(self):
        while self.keepWorking:
            request = self.waitForRequest()

            if BROADCAST_UPDATE_MARK in request:
                print("A reachability update has been received", request)
                self.toDistpatcherAccessPoint.push(request)
            else:
                self.toDistpatcherAccessPoint.push(request)
                self.printHumanReadableRequest(request)
                self.keepWorking = request != ''

    def printHumanReadableRequest(self, request):
                
        if NEIGHBORS_UPDATE_MARK in request:
            pass
        else:
            payload = SpanningTreePayload()
            payload.buildFromFormattedMessage(request)
            print(payload)

    # It returns the most recent request (pull request from socket)
    def waitForRequest(self):
        bytesFromPinkProducer = self.tcpSocket.recv(MAX_REQUEST_SIZE)
        return self.removePaddingCharacters(str(bytesFromPinkProducer, 'utf-8'))

    def removePaddingCharacters(self, request:str):
        return request.replace('$', "")

    def setQueue(self, queue: PinkQueue):
        self.toDistpatcherAccessPoint = queue
    
