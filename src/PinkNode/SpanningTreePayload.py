# Module constants.

MESSAGE_WITOUT_TYPE_INDEX = 1

SOURCE_ID_INDEX = 0
DESTINATION_ID_INDEX = 1
SPANNING_TREE_VALUES_INDEX = 2

REQUEST_TYPE_INDEX = 0
SN_INDEX = 1
RN_INDEX = 2

REQUEST = 2
ANSWER_YES = 3
ANSWER_NO = 4
ANSWER_ACK = 5
FINAL_ACK = 6

class SpanningTreePayload:

    def __init__(self, sourceId = 0, destinationId = 0, requestType = 0, sn = 0, rn = 0):
        self.sourceId = sourceId
        self.destinationId = destinationId
        self.requestType = requestType
        self.sn = sn 
        self.rn = rn

    def buildFromFormattedMessage(self, message):

        # Expected message: SPT:source-id;destination-id;type,SN,RN.
        messageSplittedByType = message.split(":")
        messageSplittedById = messageSplittedByType[MESSAGE_WITOUT_TYPE_INDEX].split(";")

        self.sourceId = int(messageSplittedById[SOURCE_ID_INDEX])
        self.destinationId = int(messageSplittedById[DESTINATION_ID_INDEX])

        messageSplittedBySpanningTreeValues = messageSplittedById[SPANNING_TREE_VALUES_INDEX].split(",")

        self.requestType = int(messageSplittedBySpanningTreeValues[REQUEST_TYPE_INDEX])
        self.sn = int(messageSplittedBySpanningTreeValues[SN_INDEX])
        self.rn = int(messageSplittedBySpanningTreeValues[RN_INDEX])

    
    def __str__(self):

        stringRepresentation =  "The request comes from node " + str(self.sourceId) + "\n" + "The request was meant to reach node " + str(self.destinationId) + "\n"
        if self.requestType == REQUEST:
            stringRepresentation += "Request type Request" + "\n"
        elif self.requestType == ANSWER_YES:
            stringRepresentation += "Request type Yes answer" + "\n"
        elif self.requestType == ANSWER_NO:
            stringRepresentation += "Request type No answer" + "\n"
        elif self.requestType == ANSWER_ACK:
            stringRepresentation += "Request type ACK" + "\n"
        elif self.requestType == FINAL_ACK:
            stringRepresentation += "Request type Final ACK" + "\n"

        stringRepresentation += "SN = " + str(self.sn) + "\n"
        stringRepresentation += "RN = " + str(self.rn) + "\n"

        return stringRepresentation

