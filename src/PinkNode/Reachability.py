from datetime import datetime

class Reachability():

	def __init__(self, sourceId, neighborId, distance):
		self.sourceId = sourceId
		self.neighborId = neighborId
		self.distance = distance
		self.log = open("Files/Logs/ReachabilityLog" +  str(self.sourceId) + ".txt", "w+")
		self.log.write('[' + str(datetime.now()) + '] ' + "Reachability has been started. ID: " + str(self.sourceId) + 
						", neighborId:" + str(self.neighborId) + ", distance: " + str(self.distance))

	def __str__(self):
		return f"Neighbor id: {self.neighborId}\nDistance to neighbor {self.neighborId}: {self.distance}"
