import os
import sys
import socket
import threading
import time
from datetime import datetime

from AgentForReceivingRequest import AgentForReceivingRequest
from AgentForSendingRequest import AgentForSendingRequest
from Dijkstra import Dijkstra

from PinkQueue import PinkQueue
from FromGreenNodeDispatcher import FromGreenNodeDispatcher
from SpanningTreeMiniDispatcher import SpanningTreeMiniDispatcher
from SharedNotifier import SharedNotifier
from SpanningTreeUpdatesNotifier import SpanningTreeUpdatesNotifier
from SpanningTreeSendingSide import SpanningTreeSendingSide
from SpanningTreeReceivingSide import SpanningTreeReceivingSide
from SpanningTreeDisconnectionHandler import SpanningTreeDisconnectionHandler
from Printer import Printer

TIME_FOR_WAITING_AGENT = 5
EXIT_SUCCESS = 0


class PinkNode:

    def __init__(self):

        self.portForReceiving = 0
        self.portForSending = 0
        self.id = 0

        self.printer = Printer()

        # Green node comunication points.
        self.agentForReceiving = None
        self.agentForSending = None
        self.fromGreenNodeDispatcher = FromGreenNodeDispatcher(self.printer)

        # Spanning Tree
        self.spanningTreeMiniDispatcher = SpanningTreeMiniDispatcher(
            self.printer)
        self.sharedNotifier = SharedNotifier()
        self.spanningTreeSendingSide = SpanningTreeSendingSide(self.printer)
        self.spanningTreeReceivingSide = SpanningTreeReceivingSide(
            self.printer)
        self.spanningTreeUpdatesNotifier = SpanningTreeUpdatesNotifier()
        self.spanningTreeDisconnectionHandler = SpanningTreeDisconnectionHandler(
            self.printer)
        
        self.dijkstraModule=Dijkstra()

    def start(self, argv):

        self.processArguments(argv)
        self.configurePrinter()
        self.configureCommunicationAgents()
        self.configureFromGreenNodeDistpatcher()
        self.dijkstraModule.setAgentForSending(self.agentForSending.getQueue())
        self.dijkstraModule.setID(self.id)

        self.configureSpanningTreeMiniDispatcher()
        self.configureUpdatesNotifier()
        self.configureSharedNotifier()
        self.configureSpanningTreeSendingSide()
        self.configureSpanningTreeReceivingSide()
        self.configureSpanningTreeDisconectionChecker()

        self.startThreads()
        self.waitToFinish()

        return EXIT_SUCCESS

    def processArguments(self, argv):
        self.portForReceiving = int(argv[0])
        self.portForSending = int(argv[1])
        self.id = int(argv[2])

    def configurePrinter(self):
        self.printer.setNodeIdentity(self.id)

    def configureCommunicationAgents(self):
        self.agentForReceiving = AgentForReceivingRequest(
            self.portForReceiving)
        self.agentForSending = AgentForSendingRequest(self.portForSending)
        self.agentForReceiving.setQueue(
            self.fromGreenNodeDispatcher.getQueue())

        self.agentForReceiving.getSocketConnection()
        self.agentForSending.getSocketConnection()
        time.sleep(TIME_FOR_WAITING_AGENT)

    def configureUpdatesNotifier(self):
        self.spanningTreeUpdatesNotifier.setSharedNotifier(
            self.sharedNotifier).setSendingQueue(self.agentForSending.getQueue())

    def configureSharedNotifier(self):
        self.sharedNotifier.setReconnectionSempahore(
            self.spanningTreeSendingSide.getReconnectionSempahore()).setIdentity(self.id)

    def configureSpanningTreeSendingSide(self):
        self.spanningTreeSendingSide.setIdentity(self.id).setAgentForSendingQueue(self.agentForSending.getQueue(
        )).setSharedNotifier(self.sharedNotifier).setSpanningTreeUpdatesNotifier(self.spanningTreeUpdatesNotifier)

    def configureSpanningTreeReceivingSide(self):

        self.spanningTreeReceivingSide.setIdentity(self.id).setSpanningTreeUpdatesNotifier(
            self.spanningTreeUpdatesNotifier).setSharedNotifier(self.sharedNotifier).setAgentForSendingQueue(self.agentForSending.getQueue())

    def configureSpanningTreeMiniDispatcher(self):
        self.spanningTreeMiniDispatcher.setSendingSideAccessPoint(
            self.spanningTreeSendingSide.getQueue()).setSendingSideNeighborsUpdate(
            self.spanningTreeSendingSide.getNeighborsUpdateQueue()).setTimeoutForParentAnswer(
            self.spanningTreeSendingSide.getTimeoutForParentAnswer()).setReceivingSideAccessPoint(
            self.spanningTreeReceivingSide.getQueue()).setReceivingSideNeighborsUpdate(
            self.spanningTreeReceivingSide.getNeighborsUpdateQueue()).setTimeoutForChildAnswer(
            self.spanningTreeReceivingSide.getTimeoutForChildAnswer()).setDisconnectionChecker(
            self.spanningTreeDisconnectionHandler.getQueue())

    def configureFromGreenNodeDistpatcher(self):
        self.fromGreenNodeDispatcher.setToSpanningTreeAccessPoint(
            self.spanningTreeMiniDispatcher.getQueue())

        # ToDo Configure forwarding access point.
        self.fromGreenNodeDispatcher.setToForwardingAccessPoint(self.dijkstraModule.getForwardingQueue())

    def configureSpanningTreeDisconectionChecker(self):
        self.spanningTreeDisconnectionHandler.setUpdatesNotifier(
            self.spanningTreeUpdatesNotifier).setSharedNotifier(
                self.sharedNotifier).setSendingAgentQueue(
                self.agentForSending.getQueue())

    def startThreads(self):
        self.agentForReceiving.start()
        self.agentForSending.start()
        self.dijkstraModule.start()
        self.spanningTreeMiniDispatcher.start()
        self.sharedNotifier.start()
        self.spanningTreeSendingSide.start()
        self.spanningTreeReceivingSide.start()
        self.fromGreenNodeDispatcher.start()
        self.spanningTreeDisconnectionHandler.start()

    def waitToFinish(self):
        self.agentForReceiving.join()
        self.agentForSending.join()
        self.fromGreenNodeDispatcher.join()
        self.spanningTreeMiniDispatcher.join()
        self.spanningTreeSendingSide.join()
        self.spanningTreeReceivingSide.join()
        self.spanningTreeDisconnectionHandler.join()
        self.agentForSending.tcpSocket.close()
        self.agentForReceiving.tcpSocket.close()
