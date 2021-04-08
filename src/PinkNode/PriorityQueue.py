class PriorityQueue(object):
	def __init__( self ):
		self.priorityQueue = []
	
	def insert( self, element ):
		if len(self.priorityQueue) == 0 or element[0] >= self.priorityQueue[-1][0]:
			self.priorityQueue.append(element)
		else:
			position = -1
			for i in range( len(self.priorityQueue) ):
				if element[0] < self.priorityQueue[i][0]:
					position = i
					i = len(self.priorityQueue) + 1
			self.priorityQueue.insert( position, element )

	def pop( self ):
		return self.priorityQueue.pop(0)


	def getElement( self, element ):
		for n in self.priorityQueue:
			if n[1] == element[1]:
				return n
		return None
		
	def getPosition( self, position ):
		return self.priorityQueue[position][1]