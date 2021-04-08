import os 
import sys
import time
import queue
import threading
import struct
from threading import Thread, Semaphore
from Graph import *
from PinkQueue import PinkQueue
from Reachability import Reachability
from datetime import datetime
from AgentForSendingRequest import AgentForSendingRequest
from PriorityQueue import PriorityQueue

FORWARDINGUPDATE = "FWD"
BROADCASTUPDATE = "BRU"
COLON_DELIMITER_POSITION = 3

SPOT_FOR_ID=0
SPOT_FOR_COST=1
SPOT_FOR_ROUTE=2

class Dijkstra(threading.Thread): 
    def __init__(self):
        threading.Thread.__init__(self)
        self.ID = 0
        
        self.agentForSendingQueue = None
        self.semaphore = Semaphore(0)
        
        self.processedUpdatesList=[]
        self.adjTable = []
        self.forwardingTable = []
        self.neighborsAndCosts = []
        self.routingTable = []
        
        self.dijkstraQueue = PinkQueue()
        self.forwardingQueue = PinkQueue()
        self.getTable = PinkQueue()
   
        self.greenAgent = None
        self.adjacenciesPacket = None
        

       # self.log = open("Files/Logs/DijkstraLog" +  str(self.ID) + ".txt", "w+")
       # self.log.write('[' + str(datetime.now()) + '] ' + "The Dijkstra module has been built.")

    def setAgentForSending(self, agentQueue):
        self.agentForSendingQueue=agentQueue
    
    def setID(self, id):
        self.ID = id
    
    def getForwardingQueue(self):
        return self.forwardingQueue
    
    def run(self):
        self.workForGreenNode()
        
    
    def workForGreenNode(self):
        while True:
                update = self.forwardingQueue.pop()
                print("DIJKSTRA is being executed")
                print(update)
                self.processUpdatesFromNeighbors(update)
               
            
            
    #Crea instancias de hilos, agentes
    def setInstances(self, agent):
      #  self.log.write('[' + str(datetime.now()) + '] ' + "All threads have started.")
        self.greenAgent = agent
        for x in self.threads:
            x.start()

    def __joinThreads__(self):
        for x in self.threads:
            x.join()
        self.join()

    """ Example: Consider the following received reachability update: BRU:150*3;170,1,145,2,200,3:
        Source id is: 150, Number of neighbors is: 3, The neighbors reachability is (170,1), (145,2), (200,3).
        This method parses the received string and returns the actual numbers and pair of reachabilities.
        The neighbors reachability is returned in the form of a list of reachability objects which contain the neighbor id and distance."""

    # Actualiza la cola de enrutamiento
    def forwardingUpdate(self, buffer):
        self.forwardingQueue.push(buffer)
    
    # Metodo que se encarga del envio de las adyacencias 
    def sendAdjacencies(self):
        while True:
            self.semaphore.acquire()
            self.getTable.push(self.adjacenciesPacket)
    
    #Semaforo para envio de adyacencias
    def adjacenciesSendUnlock(self):
        self.semaphore.release()
        
        
    def sendFTable(self):
        packet = "FWD:"    #Formato del protocolo escogido
        nodesCount = len(self.forwardingTable)
        packetSize = len(packet)
        for i in range(nodesCount):
            packet += str(self.forwardingTable[i][0]) + "," + str(self.forwardingTable[i][1]) + ";"
            packetSize = len(packet)
        packet +="!"
        packet = packet.strip(";!")
        print("DIJKSTRA: sending FWD update")
        self.agentForSendingQueue.push(packet)
        
    def processUpdatesFromNeighbors(self, updateToProcess):
        separatedUpdate= updateToProcess.split(";")
        idPart=separatedUpdate[0]
        print(idPart)
        updateId= int(idPart.split("*")[0].strip("BRU:"))
        repeatedUpdate=False
        
        print("DIJKSTRA: checking repeatedUpdate")
        for processedUpdate in self.processedUpdatesList:
            if (processedUpdate[SPOT_FOR_ID] == updateId):
                repeatedUpdate = True
        
        if (repeatedUpdate == False):
            reachabilityPart=separatedUpdate[1]
            neighborsAndCostsFromUpdate= reachabilityPart.split(",")
           
            print("DIJKSTRA  update is new")
            if(self.ID == updateId):
                print("DIJKSTRA this update is mine")
                for dataFromUpdate in range (1,len(neighborsAndCostsFromUpdate), 2):
                    neighbor=(int(neighborsAndCostsFromUpdate[dataFromUpdate-1]), 
                    int(neighborsAndCostsFromUpdate[dataFromUpdate]), 
                    neighborsAndCostsFromUpdate[dataFromUpdate-1])
           
                    self.neighborsAndCosts.append(neighbor)
            
            reachabilityToStore=""
            for dataFromUpdate in range (1,len(neighborsAndCostsFromUpdate), 2):
                reachabilityToStore+= neighborsAndCostsFromUpdate[dataFromUpdate-1]
                reachabilityToStore+=","
                reachabilityToStore+=neighborsAndCostsFromUpdate[dataFromUpdate]
                reachabilityToStore+="/"
            
            reachabilityToStore+= "!"
            formattedUpdate = (updateId, reachabilityToStore)
            
            print("DIJKSTRA update formatted")
            self.processedUpdatesList.append(formattedUpdate)
        
        self.forwardingTable.clear()
        self.routingTable.clear()
        
        self.executeDjikstra()
            
        
        
    def updateOptionsForDjikstra(self,update, optionsList):
        infoToAppend=update[1].strip("/!")
        infoToAppend=infoToAppend.split("/")
        updateSource=update[0]
        
        #Since update was provided by the most recent entry in the routing table, the route to every new option must include the route to such entry, as well as its cost.
        routeToUpdateSource=self.routingTable[-1][SPOT_FOR_ROUTE]
        costToUpdateSource= self.routingTable[-1][SPOT_FOR_COST]
        
        for infoFromUpdate in infoToAppend:            
            sourceNeighbours=infoFromUpdate.split(",")
            optionId=int(sourceNeighbours[SPOT_FOR_ID])
            
            if((optionId!=self.ID) and (optionId!=updateSource)):
                routeToOption=routeToUpdateSource+"-> "+"#"+str(sourceNeighbours[SPOT_FOR_ID])
                costToOption=costToUpdateSource+int(sourceNeighbours[SPOT_FOR_COST])
             
                newOption= (optionId,costToOption,routeToOption)
            
                optionsList.append(newOption)
            
        
        
    def checkRoutingTableFor(self,idToFind):
        searchResult=(-1,-1," ")
        
        for entry in self.routingTable:
            if entry[SPOT_FOR_ID] == idToFind:
                searchResult=entry
                break
                
        return searchResult
        
    def executeDjikstra(self):
        optionsForDjikstra=[]
        minimum=0
        candidatePosition=0
       
        index=0
        
        candidateFound=0
        print("DIJKSTRA running algorithtm")
        for neighbour in self.neighborsAndCosts:
            optionsForDjikstra.append(neighbour)
        
        while(True):
            
            candidateFound=0
            forwardingReady=False
            
            candidate= (0,0)
            
            #Assume that a very big number is the minimum value at the beginning of execution
            minimum= sys.maxsize
            candidatePosition=0
            
            index=0
            
            for option in optionsForDjikstra:
                
                if option[1]<=minimum:
                    search=self.checkRoutingTableFor(option[SPOT_FOR_ID])
                    
                    #Check if option is already part of routing table. If it is not, it is a potential candidate to be included, so save its values a compare them with the rest of options.
                    if search[SPOT_FOR_ID]<0:
                        print("DIJKSTRA candidateFound")
                        candidate=option
                        minimum=candidate[SPOT_FOR_COST]
                        candidatePosition=index
                        candidateFound=1
                        forwardingReady=True
                        
                    #else:
                        #optionsForDjikstra.pop(index)
                        
                index=index+1
             
             #If no candidate at all was found, then all updates sent to pink node have been considered. End execution when that happens.
            if(candidateFound==0):
                if(len(self.routingTable) > 0):
                    print("DIJKSTRA: Sending FW table...")
                    self.setForwardingTable()
                    self.sendFTable()
                print("DIJKSTRA Stopping algorithtm")
                break
             
            #Append the candidate that had the minimum value among the optionsForDjikstra list and was not part of the routing table. Then, eliminate its info from optionsForDjikstra
            
            self.routingTable.append(candidate)
            optionsForDjikstra.pop(candidatePosition)
            
            #Find the update sent by the recently included entry and append its info to optionsForDjikstra
            for update in self.processedUpdatesList:
                if(update[0]==candidate[SPOT_FOR_ID]):
                    self.updateOptionsForDjikstra(update,optionsForDjikstra)
                    break
         
        
        
    def setForwardingTable(self):        
        for routingEntry in self.routingTable:
            route= routingEntry[2]
            directions= route.split("->",1)
            directionToForward= directions[0]
            
            forwardingEntry= (routingEntry[SPOT_FOR_ID],directionToForward)
            self.forwardingTable.append(forwardingEntry)

 
        """agent = AgentForSendingRequest(8080)
    g = Dijkstra()
    prueba = "BRU:150*3;170,1,145,2,200,4"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()
    prueba = "BRU:200*3;170,1,145,1,150,4"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()
    prueba = "BRU:170*2;150,1,200,1"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()
    prueba = "BRU:145*3;300,1,200,1,150,2"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()
    prueba = "BRU:300*2;145,1,500,1"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()
    prueba = "BRU:500*1;300,1"
    g.forwardingQueue.push(prueba)
    g.adjTableInit()

"""
