from Vertex import *

"""
Clase que representa un grafo dirigido a utilizar para aplicar Dijkstra, sobre
la información de adyacencia de los nodos del grafo de la red
"""

class Graph:
    def __init__(self):
        self.vertexN = {}
        self.vertexCount = 0
    
    # Añadir una arista del source a destino, con el costo (por defecto esta en 0) que se indique 
    def addEdge(self, src, dest, cost=0):  
        if src not in self.vertexN:    
            self.addVertex(src)
        if dest not in self.vertexN:
            self.addVertex(dest)
        self.vertexN[src].addNeighbor(self.vertexN[dest], cost) #Pone de vecino al que es destino del cual es fuente

    # Retorna el vertice que se añade diccionario
    def addVertex(self, node):
        self.vertexCount = self.vertexCount + 1  #Se añade al conteo de vertices
        new_vertex = Vertex(node)  #  Se crea el vertice para ese nodo
        self.vertexN[node] = new_vertex # Se mete en el diccionario
        return new_vertex
   
    #Retorna una lista de objetos vertice
    def getAllVertex(self):
        return list(self.vertexN.values())
        
    #Retorna un  vertice en especifico
    def getVertex(self, node):
        if node in self.vertexN:
            return self.vertexN[node]
        else:
            return None
