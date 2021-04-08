//Constants for Js. MAX_FILE_SIZE indicates the maximum size of a file the js can send to the websocket
//BROADCAST and RANDOM are 2 special values that can be recovered from the xhtml file.
const MAX_FILE_SIZE = 25_000_000; // bytes
const BROADCAST = "BROADCAST";
const RANDOM = "RANDOM";

let webSocketServer = null;

// This array stores the id values everytime a new neighbors update comes. Required for random function
let neighboursIds = [];

//Hardcoded for testing purposes
let hcNeighbors=["1","2","3","4","5"];

// WebSocket ------------------------------------------------------------------

//Method used to include a new message to the html the client is seeing on screen.
function agregarMensaje(texto, clase) { //
//JavaScript crashes if texto is not a string since split() is a method included in strings, that is why this block of code is in a try-catch.
  let itsANeighbourUpdate = false;
  let emptyUpdate= false;
  try {	
    let etiqueta = texto.split(":");
	
	// Check if the incoming message is actually a neighbors update. 
    if (etiqueta[0].localeCompare("NUP") == 0) {
		  
	//Empty list of neighbors to include newest update	  
		  clearNeighboursList();
		  
	//If the update is not empty and is not *NUP, which is a bug, update neighboursIds with new values. * means that no neighbor is currently active.
      if(etiqueta[1]!="*"&&etiqueta[1]!="*NUP"){
			  let negihbours = etiqueta[1].split(",");
			  negihbours.forEach(updateNeighbours);
			  itsANeighbourUpdate = true;
		} else{
			  emptyUpdate=true;
		}
	}
	
	if (etiqueta[0].localeCompare("isBRU") == 0) {
		let reachabilityData = etiqueta[1].split(";");
		let neighborsFromFar = reachabilityData[1].split(",");
		let arrayIndex=0;
		
		for (arrayIndex; arrayIndex< neighborsFromFar.length; arrayIndex = arrayIndex + 2){
			if(neighboursIds.includes(neighborsFromFar[arrayIndex]) == false){
			    updateNeighbours(neighborsFromFar[arrayIndex],0,0);
			}
		}
	}
	
  } catch (err) { }

  //If the neither boolean is true, that means that the new message is a regular text message
  if (itsANeighbourUpdate == false && emptyUpdate == false) {
  
  //This block of code creates the element required to display the new message to the client, then it appends it to the class it belongs to.
    const mensaje = document.createElement('p');
    mensaje.classList.add('mensaje');
    mensaje.classList.add(clase);

    if (typeof texto === 'string') {
      mensaje.innerText = texto;
    } else if (typeof texto === 'object') {
      mensaje.appendChild(texto);
    }

    //mensaje.innerText = texto;
    const mensajes = document.getElementById('mensajes');
    mensajes.appendChild(mensaje);
  }
}

//This method is used to clear the list of neighborId numbers when a new neighbor update arrives
function clearNeighboursList(){
	//Only works if there is any neighborid stored.
	if(neighboursIds.length>0){
		let targetId="";
		let negihbourToRemove=null;
		
		//This loops removes one by one the neighboursIds displayed on screen to the user until there is none to remove.
		while(neighboursIds.length>0){
			targetId = "v"+neighboursIds[neighboursIds.length-1];
			negihbourToRemove = document.getElementById(targetId);
			negihbourToRemove.parentNode.removeChild(negihbourToRemove);
			neighboursIds.pop()
		}
	}
}

//This method is used to create new radio buttons on the xhtml based on the information received in the most recent update/ It also stores the values in neighboursIds array.
function updateNeighbours(value, index, array) {

if(neighboursIds.includes(value) == false){
  let lab, btn, br;

  // A new update may contain ' symbols. This line is used to remove such.
	value = value.replace(/'/g, " ");
	lab = document.createElement("LABEL");
	btn = document.createElement("input");
	br = document.createElement("br");
	const vecinos = document.getElementById("lista_vecinos");

	lab.innerHTML = "Vecino #" + value;
	btn.value = "" + value;
	btn.id=lab.id = "v" + value;
	btn.name = 'vecinos';
	btn.type = 'radio';

	neighboursIds.push(value);
	lab.appendChild(btn);
	lab.appendChild(br);
	vecinos.appendChild(lab);
	}
}

//This method is used when a new file arrives for the client. It creates the direct link to the file.
function agregarEnlaceArchivo(path) {
  // Eg.: <a href="/files/example.png" target="_blank">example.png</a>
  const link = document.createElement('a');

  link.setAttribute('href', path);
  link.setAttribute('target', '_blank');
  link.innerText = path.substr('/Files'.length); // skip the /files/ prefix
  agregarMensaje(link, 'normal');
}

//Invoked by event listener. This method changes the status on the xhtml when the connection to the websocket was successful
function webSocketOpened() { 
  const status = document.getElementById('estado');
  status.className = 'ok';
  status.innerText = 'Conectado';
  //hcNeighbors.forEach(updateNeighbours);  
}

//Invoked by the event listener. This method changes the status on the xhtml when the connection to the websocket drops. 
function webSocketClosed(evento) {
  const status = document.getElementById('estado');
  status.className = 'error';
  status.innerText = 'Desconectado';

  agregarMensaje(`Desconectado del servidor ${evento.reason}`, 'error');
}

//nvoked by the event listener. This method indicates what to do when a new message arrives on the client side.
function webSocketMessageReceived(evento) {
   //First, the method checks if the incoming message has a file tag. It does if the splie function works
  const array = evento.data.split('(file)');
  if (array.length === 2) {
    const key = array[0];
    const value = array[1];
    
	//When the file tag is present, the second value may be a link to the file that was just received or a confirmation from the websocket.
	if (key === 'file') {
      agregarEnlaceArchivo(value);
	} else {
	   if(key === 'sent'){
		   //If it is a confirmation, restore the send and file buttons so the user can keep using the client.js
		    document.getElementById('file_button').disabled = false;
			document.getElementById('send_button').disabled = false;
			agregarMensaje(value, 'normal');
		}else{
			agregarMensaje(evento.data, 'normal');
		}
	}
  } else {
    agregarMensaje(evento.data, 'normal');
  }
}

function webSocketErrorOcurrido() {
  agregarMensaje('Error de conexión con el nodo verde', 'error');
}

// This function establishes connection with websocket.py
function setupWebSocket() { //inicia websocket
  //Search the html for key pieces of data, needed by the .js websocket to connect.
  let port = document.body.dataset.wsPort;
  let host = document.body.dataset.host;
  let intro = 'ws://';
  let addr = intro.concat(host, ':', port);
  webSocketServer = new WebSocket(addr); 

  // Sets events up.
  webSocketServer.addEventListener('open', webSocketOpened); 
  webSocketServer.addEventListener('message', webSocketMessageReceived);
  webSocketServer.addEventListener('error', webSocketErrorOcurrido);
  webSocketServer.addEventListener('close', webSocketClosed);
}

// File attach ----------------------------------------------------------------

function fileChanged() {
  const fileInput = document.getElementById('file');
  const neighbor = document.querySelector('input[name="vecinos"]:checked').value;
  if (fileInput.files.length > 0) {
    const file = fileInput.files[0];
    if (file.size < MAX_FILE_SIZE) {
      agregarMensaje(`Enviando "${file.name}" (${file.size} B)...`, 'normal');
      if (webSocketServer) {
        document.getElementById('file_button').disabled = true;
        document.getElementById('send_button').disabled = true;
		webSocketServer.send(`${neighbor}:__filename(file)${file.name}`);
        webSocketServer.send(file);
      }
    } else {
      agregarMensaje(`Error: "${file.name}" (${file.size} B) excede límite de tamaño ${MAX_FILE_SIZE} B`, 'error');
    }
  }
}

function fileButtonClicked() {
  const fileInput = document.getElementById('file');
  fileInput.click();
}

function setupAttachFile() {
  const fileButton = document.getElementById('file_button');
  fileButton.addEventListener('click', fileButtonClicked);

  const fileInput = document.getElementById('file');
  fileInput.addEventListener('change', fileChanged);
}


// Form -----------------------------------------------------------------------
//Method used to send messages from the client to the websocket.py
function sendMessage() {
  //First, get the text message the user entered.
  const messageText = document.getElementById('message_field').value;
  
  //Second, get the value of the highlighted radio button
  const neighbor = document.querySelector('input[name="vecinos"]:checked').value;
  let neighborId = neighbor;

  console.log("Selected neighbor" + neighborId);

  //Third, determine if the user entered a number. If messageText is not a number, then simply send the message as usual to websocket.py.
  if (isNaN(messageText)) {
	
    if (neighbor == RANDOM) {
      neighborId = neighboursIds[Math.floor(Math.random() * neighboursIds.length)];
    }

    if (neighbor == BROADCAST) {
      agregarMensaje(`Para todos: ${messageText}`, 'enviado');
    }
    else {
      agregarMensaje(`Para ${neighborId}:${messageText}`, 'enviado');
    }

    webSocketServer.send(`${neighborId}:${messageText}`);
    
  } else {
	// If the user did enter a number, then a default message is sent as many times as the user indicated in their message.
    let totalMessagesToSend = parseInt(messageText);
    const stressTestingMessage = "Stress testing message";
    let messageIndex = 0;


    for (messageIndex; messageIndex < totalMessagesToSend; messageIndex++) {
      
	  //If RANDOM was highlighted, then choose randomly from the neighboursIds array.
	  if (neighbor == RANDOM) {
        neighborId = neighboursIds[Math.floor(Math.random() * neighboursIds.length)];
      }

      // The unnecessary \t was removed and the space after :
      webSocketServer.send(`${neighborId}:${stressTestingMessage}`);
      agregarMensaje(`Para ${neighborId}:${stressTestingMessage}`, 'enviado');
    }

  }

}

function setupClient() {
  setupWebSocket();
  setupAttachFile();

  const button = document.getElementById('send_button');
  button.addEventListener('click', sendMessage);
}

window.addEventListener('load', setupClient);
