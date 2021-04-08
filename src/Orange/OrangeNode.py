import os  # open, close
import os  # open, close
import sys
import csv
import time
from datetime import datetime

# Pipes
RE_WRITEPIPE = 0
WE_READPIPE = 0

# Messages
READ_IDENTITY_FILE  = "1"
READ_NEIGHBORS_FILE = "2"
SEND_NEIGHBOR_IDENTITY = "3"
END_CONNECTION = "4"
CRITICAL_ERROR = "5"
SEND_NEIGHBORS_COUNT  = "6"
MSSG_SIZE = 1

# File paths
greenNodeIdentityFilePath = ""
neighborsIdentityFilePath = ""

# Neighbors information
neighborsIdentity = []

# Green node identity
greenNodeIdentity = ""

iterator = 0

# Log for orange node.
log = open("../src/Orange/Files/Logs/OrangeNodeLog" + ".txt", "w+")

def readGreenNodeIdentityFile(path):
	global greenNodeIdentity

	try:
		csvFile = open(path)
		csv_reader = csv.reader(csvFile, delimiter=',')

		for row in csv_reader:
			greenNodeIdentity = transformToCommaSeparatedString(row)
		csvFile.close()

	except IOError:
		log.write('[' + str(datetime.now()) + '] ' + "An error has ocurred while reading the green node identity file.")


def readNeighborsIdentityFile(path):
	try:
		csvFile = open(path)
		csv_reader = csv.reader(csvFile, delimiter=',')

		neighborsIdentity = []
		
		for row in csv_reader:             #Primero almacenamos los vecinos en una lista
			neighborIdentity = transformToCommaSeparatedString(row)
			neighborsIdentity.append(neighborIdentity)

		csvFile.close()
		return neighborsIdentity

	except IOError:
		log.write('[' + str(datetime.now()) + '] ' + "An error ocurred while reading the neighbors file." + "\n")


def transformToCommaSeparatedString(csvRow):
	message = ""
	for s in csvRow:
		message += str(s) + ','
	message = message.rstrip(',') # It deletes the trailing comma from the string.
	return message

def sendMessageToOrangeAgent(message):
	os.write(WE_READPIPE, bytearray(message + "\0", encoding= 'utf-8'))
	log.write('[' + str(datetime.now()) + '] ' + "A message was sent to the orange agent. The message is: " + str(message) +"\n")


def receiveMessageFromOrangeAgent():

	global iterator

	bytes = os.read(RE_WRITEPIPE, MSSG_SIZE)
	message = str(bytes, 'utf-8').strip('\0')
	log.write('[' + str(datetime.now()) + '] ' + "A message was received from the orange agent. The message is: " + str(message) +"\n")


	if message == READ_IDENTITY_FILE:
			readGreenNodeIdentityFile(greenNodeIdentityFilePath)
			sendMessageToOrangeAgent(greenNodeIdentity)
			log.write('[' + str(datetime.now()) + '] ' + "The green node identity is requested and sent " + greenNodeIdentity + ".\n")

	elif message == READ_NEIGHBORS_FILE:
			global neighborsIdentity
			neighborsIdentity = readNeighborsIdentityFile(neighborsIdentityFilePath)

	elif message == SEND_NEIGHBORS_COUNT:
			sendMessageToOrangeAgent(str(len(neighborsIdentity)))

	elif message == SEND_NEIGHBOR_IDENTITY:
				sendMessageToOrangeAgent(neighborsIdentity[iterator])
				iterator += 1

	elif message == END_CONNECTION:
			return END_CONNECTION

	elif message == CRITICAL_ERROR:
			return CRITICAL_ERROR

	return 0

def parseArguments():

	# Parse incoming arguments.
	# Arguments are as follows:
	# Read-end file descriptor of writepipe.
	# Write-end file descriptor of readpipe.
	# Test round
	# Test case
	# Test node-id

	global RE_WRITEPIPE
	global WE_READPIPE
	global greenNodeIdentityFilePath
	global neighborsIdentityFilePath

	# It obtains the program arguments discarting the program name.
	arguments = sys.argv[1:]

	if len(arguments) != 5:
		print("Usage: Read-end-file-descriptor-of-writepipe Write-end-file-descriptor-of-readpipe Test-round Test-case Test-node-id")
		log.write('[' + str(datetime.now()) + '] ' + "An error ocurred while parsing the program arguments.")
		return -1

	WE_READPIPE = int(arguments[0])
	RE_WRITEPIPE = int(arguments[1])

	round = arguments[2] 
	case = arguments[3]
	nodeId = arguments[4]

	greenNodeIdentityFilePath = "../src/tests/" + "round" + str(round) + "/" + "case" + str(case) + "/computovers.node." + str(nodeId) + ".csv"
	neighborsIdentityFilePath =  "../src/tests/" + "round" + str(round) + "/" + "case" + str(case) + "/computovers.neighbors." + str(nodeId) + ".csv"
	
	log.write('[' + str(datetime.now()) + '] ' + greenNodeIdentityFilePath + "\n")
	log.write('[' + str(datetime.now()) + '] ' + neighborsIdentityFilePath + "\n")	

def main():

	log.write('[' + str(datetime.now()) + '] ' + "The program execution starts.\n")

	if parseArguments() == -1:
		return -1

	while True:

		receptionStatus = receiveMessageFromOrangeAgent()
		if receptionStatus == END_CONNECTION or receptionStatus == CRITICAL_ERROR:
			log.write('[' + str(datetime.now()) + '] ' + "The program execution has ended." +  " Termination value: " + str(receptionStatus))
			os.close(WE_READPIPE)
			os.close(RE_WRITEPIPE)
			break

	log.write('[' + str(datetime.now()) + '] ' + "The orange node execution has finished.\n")
	

if __name__ == "__main__":
	main()