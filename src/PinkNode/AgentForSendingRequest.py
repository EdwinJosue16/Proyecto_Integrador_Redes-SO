import socket
import threading
import time
from PinkQueue import PinkQueue

MAX_REQUEST_SIZE = 1024

class AgentForSendingRequest(threading.Thread):

    def __init__(self, portForSendingRequest:int):
        threading.Thread.__init__(self)
        self.tcpSocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
        self.ipAddress = 'localhost'
        self.portForSendingRequest = portForSendingRequest
        self.keepWorking = True
        self.requestsForGreenNodeQueue = PinkQueue()

    def getSocketConnection(self):
        self.tcpSocket.connect((self.ipAddress,self.portForSendingRequest))

    # It returns the most recent request (pull request from socket)
    def sendRequest(self):
        request = self.requestsForGreenNodeQueue.pop()
        self.tcpSocket.send(bytearray( request + '\0','utf-8'))

    def run(self):
        while self.keepWorking:
            self.sendRequest()

    def getQueue(self):
        return self.requestsForGreenNodeQueue