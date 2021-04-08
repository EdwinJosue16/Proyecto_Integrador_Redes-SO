## @package websocket.py
# This is the websocket in charge of dispatching messages to the client from the green node network, and vice versa. Its class inherits from WebSocketServerProtocol

from datetime import datetime
import time
import sys #sys.argv[]
import os # read, write
import random 
import threading # thread, join, start 
import struct # struct.unpack
import queue  #queue
from autobahn.twisted.websocket import WebSocketServerProtocol, \
    WebSocketServerFactory


##\brief This is the host the websocket will use to await for connections. It is localhost by default 
HOST = 'localhost'
# ~ PORT= 9002
# ~ ID_VERDE = 47

##\brief This constant determines how many bytes will be transferred over to the pipes, both when receiving and when sending
INPUT_PIPE_MESSAGE_SIZE = 256

##\brief This constant will use getcwd to find out which part leads to the folder that stores files. 
ROUTE= os.getcwd().strip("bin")+"src/Blue/"

##\brief  File descriptor for the receiving pipe.
WE_READPIPE_FD = 0

##\brief  File descriptor for the sending pipe.
RE_WRITEPIPE_FD = 0

##\brief  This constant is the message that must be sent to let blue agent know that all neighbours were succesfully received.
ALL_NEIGHBOURS_RECEIVED = "ALL"

##\brief File that will allow websocket write a log 
blueNodeLog = open("../src/Blue/Files/Logs/BlueNodeLog" + ".txt", "w+")

##\brief  String that stores the name of a file sent from the client
outgoingFileName=''

##\brief  String that stores the destination ID of the file that client wants to send
fileDestination=''

##\brief  String that stores the file name for the file that is for this node's client
incomingFileName=''

##\brief  This list saves all file chunks that are destined for this node's client 
incomingFileChunks=queue.Queue()

##\brief  This is the tag that will let know the websocket that it can start writing the file for client on disk 
entireFileReceivedTag="CKRCVD"

##\brief  This tag will let websocket know when the received bytes are a chunk of a file.
fileChunkTag="FLCHNK"

##\brief This tag identifies an ACK message
acknowledgementTag="ACKNOW"

##\brief  This list is used to store the id numbers of currently active neighbours
vecinosVivos = [0]

##\brief  This list will store messages for the client
messageQueue = queue.Queue()

##\brief  This global variable will store the most recent ACK received when a file is being transferred to another node.
currentAckRn='*'

##\brief  This semaphore will control access to the currentAckRn variable. It will also take care of the sender's timeouts.
senderSemaphore= threading.Semaphore()

##\brief  This semaphore will let the file receiver thread when a new file chunk has arrived. It will also take care of the receiver's timeouts.
receiverSemaphore= threading.Semaphore()

##\brief  This lock will control the access to the RE_WRITEPIPE_FD pipe fd.
pipeLock= threading.Lock()

class WebSocketServer(WebSocketServerProtocol):
## This is the method the websocket uses to act when a connection is established.

#\brief This method is an event handler inherited from websocketServerProtocol, meant to be overriden.

# \details on Connect prints a message on terminal with the request from the client

# \param Request

# \pre This method should work as is. 

# \remark No variable is affected after this method is invoked.

# \return No return value

# \exception No exception 

# \author code provided by Jeisson Hidalgo 
    def onConnect(self, request):
        print(f"Client connecting: {request.peer}")


## This is the method the websocket uses to act when the websocket instance is created.

#\brief This method is an event handler inherited from websocketServerProtocol, meant to be overriden.

# \details on Open prints a message on terminal. Then, sends over the connection the nodeś id and awakens the 2 threads to make them start working. 

# \param No parameter required

# \pre This method needs the ID_VERDE constant to be initialized. 

# \remark No variable is affected.

# \return No return value

# \exception No exception 

# \author Eddy Ruiz, based on code provided by Jeisson Hidalgo 
    def onOpen(self):
        print("WebSocket connection open.")
        self.sendMessage(f"id\t{ID_VERDE}".encode('utf8'), False)
        #self.sendMessage(f"NUP:{vecinosVivos}".encode('utf8'), False)
        self.work()

## This is the method the websocket uses to act when a message arrives from the js client.

#\brief This method is an event handler inherited from websocketServerProtocol, meant to be overriden.

# \details on Message first determines if the message from the client is binary or not. If it is, then websocket starts sending over the pipe chunks of the binary. To know how big each chunk needs to be,
#  the MAX_FRAME_SIZE constant is declared, and the upperLimit and start variables allows the websocket to know on which position a chunk starts and ends, so when a chunk is sent, both start and upperLimit
#  sums the MAX_FRAME_SIZE value to their current values. This process is over when start has a bigger or equal number than than the size of the file to be sent. When the entire file is sent, websocket also 
# sends a special message to let the websocket on the receiving end know when it has received all the chunks from the file, as well as sending a confirmation to the client. It is also worth mentioning  that each chunk 
# gets 2 tags added at the beginning of each: The chunks' destination and the chunk tag. 
# If the message from the client turns out to simply be text, then the websocket determines if it is a file name. If it is, it stores the file name in a global variable, outgoingFileName. If it is not, then it simply sends the message,
# over the pipe.  

# \param payload, message received from client. Binary, boolean to determine message qualities

# \pre This method needs the pipes values to be initialized

# \remark No variable is affected.

# \return No return value

# \exception No exception 

# \author Eddy Ruiz, based on code provided by Jeisson Hidalgo 
    def onMessage(self, payload, isBinary):
        global outgoingFileName
        global fileDestination
        global entireFileReceivedTag
        global fileChunkTag
        global currentAckRn
        global senderSemaphore
        global messageQueue
        
        if isBinary:
            print(f"Binary message received: {len(payload)} bytes")
            MAX_FRAME_SIZE = 184
            upperLimit=MAX_FRAME_SIZE
            start=0
            fileDestinationToBytes= fileDestination.encode()
            tagsToBytes= fileChunkTag.encode()
            sn='0'
            
            while (start < len(payload)): 
                snEncoded= sn.encode()
                
                self.sendMessageOverPipe(fileDestinationToBytes
                + tagsToBytes + snEncoded + payload[start:upperLimit])
                
                while not senderSemaphore.acquire(timeout=1):
                    #Resend when a timeout happens
                    self.sendMessageOverPipe(fileDestinationToBytes
                    + tagsToBytes + snEncoded + payload[start:upperLimit])
                    
                if (currentAckRn!=sn):
                    #Update SN and move to the next chunk of file to send
                    newSn=int(currentAckRn)
                    newSn=(newSn+1)%2
                    sn= str(newSn)
                    start=MAX_FRAME_SIZE+start
                    upperLimit=MAX_FRAME_SIZE+upperLimit
                else:
                    #Resend when a wrong RN is received
                    self.sendMessageOverPipe(fileDestinationToBytes
                    + tagsToBytes + snEncoded + payload[start:upperLimit])
                        
            self.sendMessageOverPipe(fileDestinationToBytes+entireFileReceivedTag.encode())
            
            #Send a confirmation back to client to let them know that the whole file was sent 
            messageQueue.put(f'sent(file){outgoingFileName} enviado correctamente')
            
            # When arrives a file from other green node
        #    file = open(f'{ROUTE}Files/{outgoingFileName}', 'ab')
         #   file.write(payload)
          #  file.close()
            #self.sendMessage(f'file(file)Files/{outgoingFileName}'.encode('utf8'), False)
        
        else:
            text = payload.decode('utf8')
            
            print(f"Text message received: {text}")
            array = text.split('(file)')
            if len(array) == 2:
              keys = array[0].split(":")
              value = array[1]
              if keys[1] == "__filename":
                outgoingFileName = value
                fileDestination=keys[0]+':'
                
                #Append file origin to file name
                text= text + f"(file)+{ID_VERDE}"
                
            self.sendMessageOverPipe(bytearray(text,encoding='utf8'))
            time.sleep(0.001)

        # echo back message verbatim
        # self.sendMessage(payload, isBinary)

    def onClose(self, wasClean, code, reason):
        print(f"WebSocket connection closed: {reason}")


    def sendMessageOverPipe(self,messageToSend):
        global pipeLock
        
        pipeLock.acquire()
        os.write(WE_READPIPE_FD, messageToSend)
        pipeLock.release()

## This is the method the websocket uses to act when a message arrives from the blue consumer.

#\brief This method is to be executed by one of the threads that start on the work method

# \details This method makes the thread to be constantly listening to the receiving pipe. Every time a bunch of bytes is received, receiveFromBlueAgent turs the bytes into a string an classifies
# which kind of message it is. If the message received has a fileChunkTag, that means that the message is a part of an incoming file for the client, in which case, the thread stores the chunk in 
# different list. If the message has a filena tag, then the message is a filename, and the string is stored in a variable to be used later. If the message. however, has a entireFileReceivedTag, then 
# it means that the websocket has received all the chunks of an incoming file, in which case it'll start writing the file on disk chunk by chunk. Once the file is assembled, websocket places a special 
# message on the messageQueue for the client that specifies the path to the file. Finally, if a message has no tag, the websocket simply places it on the messageQueue, then adds to the semaphore.

# \param No parameter required

# \pre This method needs the pipes values to be initialized, as well as the semaphore

# \remark messageQueue, incomingFileChunks, incomingFileName are all variables that may be modified.

# \return No return value

# \exception No exception 

# \author Eddy Ruiz  
    def receiveFromBlueAgent(self):
        global senderSemaphore
        global receiverSemaphore
        global currentAckRn
        global incomingFileName
        global incomingFileChunks
        global entireFileReceivedTag
        global fileChunkTag
        
        try:
            bytes = os.read(RE_WRITEPIPE_FD, INPUT_PIPE_MESSAGE_SIZE)
            mensaje = str(bytes, 'utf-8').strip('\0')
            
            #This string indicates which kind of package has just arrived. 
            #Tag's range is from 2 to 8 because when a bytes object is 
            #transformed into a string, a b' is added at the start of it and 
            #every tag's length is 6 bytes. Text strings do not have this issue
            
            messageTag=mensaje[2:8]
            
            if(messageTag==entireFileReceivedTag):
                incomingFileChunks.put(('2', "EOF"))
                receiverSemaphore.release()
                
            elif(messageTag=="__file"):
            #Details about incoming file are separated by a (file) tag
                fileDetails= mensaje.split("(file)")
                incomingFileName= fileDetails[1]
                fileOrigin= fileDetails[2].strip("'")
                fileAssembler= threading.Thread(target=self.receiveFile, args=(fileOrigin))
                fileAssembler.start()
                
            elif(messageTag==fileChunkTag): 
            #Actual file chunk starts after position 8 of the byte array,
            #and chunk's SN is always in position 8, right after the tag.
                incomingFileChunks.put((mensaje[8],bytes[8:]))
                receiverSemaphore.release()
                
            elif(messageTag==acknowledgementTag):
            
            #An ackonwledgement's RN is always in position 8.
                currentAckRn=mensaje[8]
                senderSemaphore.release()
            else:               
                messageQueue.put(mensaje)
                
            threading.Timer(0, self.receiveFromBlueAgent).start()
        
        except KeyboardInterrupt:
            print("ENDING!")
            blueNodeLog.write('[' + str(datetime.now()) + '] ' +
            "Client Dispatcher: Ending execution.\n")

## This is the method the websocket uses to act when a new message is placed on the messageQueue.

#\brief This method is to be executed by one of the threads that start on the work method

# \details This method makes the thread to wait for the semaphore to be on a positive value. 
# When that happens, the thread takes the message on position 0 on messageQueue and dispatches it to the client

# \param No parameter required

# \pre This method needs the semaphore and messageQueue to be initialized

# \remark messageQueue, semaphore.

# \return No return value

# \exception No exception 

# \author Eddy Ruiz
    def dispatchToClient(self):
        global dispatcherSemaphore
        try:
            messageForClient=messageQueue.get()
            self.sendMessage(messageForClient.encode('utf8'), False)
            
            threading.Timer(0, self.dispatchToClient).start()
        except KeyboardInterrupt:
            blueNodeLog.write('[' + str(datetime.now()) + '] ' +
            "Client Dispatcher: Ending execution.\n")


    def receiveFile(self,fileOrigin):
        global acknowledgementTag
        global receiverSemaphore
        global incomingFileChunks
        global messageQueue
        global incomingFileName
        
        chunksToAssemble=[]
        fileOriginEncoded= fileOrigin.encode() + ':'.encode()
        ackTagEncoded= acknowledgementTag.encode()
        rn='0'
        
        while(True):
            rnEncoded= rn.encode()
            
            while not receiverSemaphore.acquire(timeout=1):
                #Resend ack with current RN if timeout happens
                self.sendMessageOverPipe(fileOriginEncoded + ackTagEncoded 
                + rnEncoded)
                
            chunkOfFile=incomingFileChunks.get()
            
            #Exit condition. It means that whole file has been received
            if (chunkOfFile[0] == '2'):
                break
            elif(chunkOfFile[0] == rn):
                chunksToAssemble.append(chunkOfFile[1])
                newRn= int(chunkOfFile[0])
                newRn= (newRn+1)%2
                rn= str(newRn)
            else:
                #Resend ack if the wrong SN has been received.
                self.sendMessageOverPipe(fileOriginEncoded + ackTagEncoded 
                + rnEncoded)
        
        file= open(f'{ROUTE}Files/{incomingFileName}', 'ab')
        for chunk in chunksToAssemble:
            file.write(chunk)
        file.close()
        
        #Clear possibly dirty queue for next file
        while not incomingFileChunks.empty():
            incomingFileChunks.get_nowait()
        
        #Send link to file to client
        messageQueue.put(f'file(file)Files/{incomingFileName}')
        
        
## This is the method the websocket uses to awaken its threads.

#\brief This method is to be executed by the onOpen method when the socket starts operations.

# \details This method creates and awakens 2 threads. One to listen to blue agent and another used to send to the client.

# \param No parameter required

# \pre This method needs the pipes values to be initialized, as well as the semaphore

# \remark No variables are modified

# \return No return value

# \exception No exception 

# \author Eddy Ruiz    
    def work(self):
        blueAgentListener = threading.Thread(target=self.receiveFromBlueAgent, args=())
        clientDispatcher = threading.Thread(target=self.dispatchToClient, args=())

        blueAgentListener.start()
        clientDispatcher.start()
     
    
   
        
   
   
if __name__ == '__main__':
	
    blueNodeLog.write('[' + str(datetime.now()) + '] ' +
              "The blue node execution has started.\n")
    WE_READPIPE_FD = int(sys.argv[1])
    RE_WRITEPIPE_FD = int(sys.argv[2])

    bytes = os.read(RE_WRITEPIPE_FD, 5)
    mensaje = str(bytes, 'utf-8').strip('\0')

    vecinosVivos.pop(0)
    while(mensaje != ALL_NEIGHBOURS_RECEIVED):
        vecinosVivos.append(mensaje)
        bytes = os.read(RE_WRITEPIPE_FD, 5)
        mensaje = str(bytes, 'utf-8').strip('\0')
    
    #By default, a semaphore starts with a 1 as internal value. 
    #These acquire() calls are made to set their values to 0.
    
    senderSemaphore.acquire()
    receiverSemaphore.acquire()
    
    bytes = os.read(RE_WRITEPIPE_FD, 2)
    ID_VERDE = struct.unpack("h" , bytes[:])[0]
    bytes = os.read(RE_WRITEPIPE_FD, 2)
    PORT = struct.unpack("h" , bytes[:])[0]
    if os.fork() > 0:
       os.execlp('python3', 'python3','../src/Blue/httpd.py',str(ID_VERDE), str(PORT))
       
    import sys

    from twisted.python import log
    from twisted.internet import reactor

    log.startLogging(sys.stdout)

    # PORT y HOST declarados al principio del archivo
    factory = WebSocketServerFactory(f"ws://{HOST}:{PORT}")
    factory.protocol = WebSocketServer
    # factory.setProtocolOptions(maxConnections=2)
    reactor.listenTCP(PORT, factory)
    reactor.run()
    blueNodeLog.write('[' + str(datetime.now()) + '] ' +
              "The blue node execution has finished.\n")
              
    

