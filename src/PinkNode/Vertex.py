"""
Representa un vertice de un grafo. Utilizada
para formar el grafo en el cual se aplica Dijkstra
"""

class Vertex:
    def __init__(self, node):
        self.value = node
        self.neighbors = {}

    # Se indexa el numero de nodo al diccionario y alamecena el costo del camino a ese vecino
    def addNeighbor(self, node, cost):
        self.neighbors[node] = cost

    #Retorna una lista con los ID's de los vecinos
    def getNeighbors(self):
        return list(self.neighbors.keys()) # Llaves del nodo

    #Obtiene el costo del vecino que se especifique
    def getCost(self, neighbor):
        return self.neighbors[neighbor]

    #Retorna el numero de nodo de ese vertice
    def getValue(self):
        return self.value

    # Imprime los valores
    def __str__(self):
        return "(%s)" % self.value
        
    # Establece coneccciones entre vecinos
    def getConnections(self):
        return self.neighbors.keys()