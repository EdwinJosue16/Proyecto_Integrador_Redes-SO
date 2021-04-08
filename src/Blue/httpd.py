## @package httpd.py
# This is the proyect's HTTP server file. This class is in charge of awaiting for requests from clients and serving them forever. It inherits from http.server.BaseHTTPRequestHandler.


import http.server ##http.server.BaseHTTPRequestHandler
import socket  ##socket.Socket
import sys ##sys.argv[]
import os ##getcwd
import urllib.parse

##\brief IP address the server will listen from. It is set to localhost by using the empty space 
HOST=''

##\brief This integer indicates the node's id. It is received from either blue producer or blue agent when the execlp() call is made
ID=sys.argv[1]

##\brief This is the port that the websocket will use to be ready for requests from clients. HTTP server needs to know this so it can write it to the xhtml file for the javascript. 
PORT_WS = sys.argv[2]

##\brief This number is the same as the number websocket gets, plus 5. This is the port HTTP server will listen to.
PORT=int(PORT_WS)+5

##\brief This tuple joins the two pieces of information the server needs to start working using sockets.
ADDRESS = (HOST, PORT)

##\brief This is the path the server needs to follow in order to get to the files it needs to provide to the client.
ROUTE="/"+os.getcwd().strip("bin")+"src/Blue/"

##\brief Source: https://developer.mozilla.org/en-US/docs/Web/HTTP/Basics_of_HTTP/MIME_types/Common_types
CONTENT_TYPES = {
	'css': 'text/css',
	'csv': 'text/csv',
	'htm': 'text/html',
	'html': 'text/html',
	'jpeg': 'image/jpeg',
	'jpg': 'image/jpeg',
	'js': 'text/javascript',
	'json': 'application/json',
	'png': 'image/png',
	'pdf': 'application/pdf',
	'svg': 'image/svg+xml',
	'txt': 'text/plain',
	'xhtml': 'application/xhtml+xml',
	'xml': 'application/xml',
}

##\brief Dummy socket created only to get the IP address from the computer that executes the server routines.
sock=socket.socket(socket.AF_INET, socket.SOCK_DGRAM) 
sock.connect(("2.2.2.2",1));
socketAddress= sock.getsockname()[0]
sock.close()

##\brief Tuple created to store the different extensions that a file may have.
BINARY_TYPES = ('jpeg', 'jpg', 'png', 'pdf')

##\brief Small dictionary to contain the variables that the server needs to write down on the xhtm file before sending it to the client.
variablesHTML={"node_ip":socketAddress, "ws_port":PORT_WS,"node_id":ID}

class RequestManager(http.server.BaseHTTPRequestHandler):
	"""Respond to HTTP requests"""

## This is the method the server uses to serve requests.

#\brief This method allows the server take care of requests.

# \details  do_GET first determines the kind of file that it must send to the client. If it is an html, the server must read it line by line to
#find the spots where it must replace placeholders for actual data needed by the js, like the websocket information or the node's id. If
#for some reason an exception happens, the server returns a 404 as response. 

# \param No parameter is required

# \pre This method should work as is. However, it's important to make sure that variablesHTML is initialized, since the server needs to place each of them on their spots on the xhtml.

# \remark No variable is affected after this method is invoked.

# \return No return value

# \exception If any exception happens, it is handled by the try except conditional, and a 404 reply is sent to the client. 

# \author Eddy Ruiz, based on code provided by Jeisson Hidalgo 
	def do_GET(self):
		itsHtml=0
		print('GET', self.path)
		path = self.path
		if path == '/': 			# Define root as index file
			path = ROUTE +"index.xhtml"
			itsHtml=1 
		elif path[0] == '/':			# Convert absolute to relative path
			path = ROUTE + path[1:]
		path = urllib.parse.unquote(path)
		try:
			print('path', path)
		
			extension = self.findExtension(path)		
			mode = 'r'
			if extension in BINARY_TYPES:
				mode = 'rb'				
			file=""
			
			if(itsHtml==1):			
				archivo = open(path, mode)					
				htmlLine=archivo .readline()		
				while(htmlLine):
					htmlLine=htmlLine.replace("node_ip",variablesHTML["node_ip"])
					htmlLine=htmlLine.replace("node_id",str(variablesHTML["node_id"]))
					htmlLine=htmlLine.replace("ws_port",str(variablesHTML["ws_port"]))				
					file+=htmlLine
					htmlLine=archivo.readline()
			else:
				file=open(path,mode).read()
				
			self.send_response(200)
		except:
			file = "Not found"
			self.send_response(404)
		self.setContentType(path)
		self.end_headers()
		if extension in BINARY_TYPES:
			self.wfile.write(file)
		else:
			self.wfile.write(bytes(file, 'utf-8'))

## This is the method the server uses to set the format of the file it is about to send .

#\brief  This method determines if a file's extension is supported by the server by checking in CONTENT_TYPES.

# \details This method invokes findExtension to parse the last spots of a path in order to see the extension's name. If such extension is in CONTENT_TYPES, then a header is sent to 
#the client.  

# \param No parameter is required

# \pre A valid path must be passed to findExtension.

# \remark No variable is affected after this method is invoked.

# \return No return value

# \exception No exception 

# \author code provided by Jeisson Hidalgo
	def setContentType(self, path):
		extension = self.findExtension(path)
		if extension in CONTENT_TYPES:
			self.send_header("Content-Type", CONTENT_TYPES[extension])
			print("Content-Type:", CONTENT_TYPES[extension])
            
## This is the method the server uses to determine the format of the file it is about to send .

#\brief  This method determines a file's extension.

# \details This method parse the last spots of a path in order to see the extension's name. To do so, it checks where the position of a . is.

# \param Path, a string containing the the path to the desired file.

# \pre A valid path must be passed to findExtension.

# \remark No variable is affected after this method is invoked.

# \return returns the file's extension

# \exception No exception 

# \author code provided by Jeisson Hidalgo
	def findExtension(self, path):
		pos = path.rfind('.')
		extension = ''
		if pos != -1:
			extension = path[pos + 1:]
		return extension

try:
	# Create a HTTP server and make it to listen HTTP requests
	httpd = http.server.HTTPServer(ADDRESS, RequestManager)
	print("HTTP Server listening at", ADDRESS)
	httpd.serve_forever()
except KeyboardInterrupt:
	print('HTTP Server finished')
httpd.server_close()
