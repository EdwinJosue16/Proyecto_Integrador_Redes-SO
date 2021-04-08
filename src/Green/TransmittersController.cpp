#include "TransmittersController.h"
#include "Log.hpp"
#include "./Payloads/NetworkPayload.hpp"

#include <memory>
#include <algorithm>

TransmittersController::TransmittersController()
	: transmitterReady{new Semaphore(0)}
{
}

TransmittersController::~TransmittersController()
{
	delete this->transmitterReady;
	this->transmitterReady = nullptr;

	if(heartbeat!=nullptr){
		delete heartbeat;
	}
	heartbeat=nullptr;
}

int TransmittersController::run()
{
	Log::append("***** Transmitters controller ***** <  Run method will begin its execution. >");
	// All transmitter threads start.
	this->startTranmitters();
	this->transmitterReady->signal();

	// Packages come from blue-producer or packages sent from link layer dispatcher such as hearbit and broadcast.
	this->consumeForever();

	Log::append("***** Transmitters controller ***** <  Run method has finished. >");
	return EXIT_SUCCESS;
}

payload_ptr TransmittersController::preparePackage(payload_ptr package)
{
	// It finds the delivery information associated with the destination node which ip is on the package.
	auto temporalPayload = std::static_pointer_cast<LinkLayerPayload>(package);
	Log::append("***** Transmitters controller ***** < Get transmitter in order to prepare package. >");
	//auto transmitter = this->redirectingTable.find(temporalPayload->destinationId)->second;
	Log::append("***** Transmitters controller ***** < The transmitter # is :  >" + std::to_string(temporalPayload->finalDestinationId));
	// ToDo for other phases.
	// Add logic related to other types of packages such as hearbit or broadcast.

	// It builds the package.
	//package.sourceIp = ipToInt32(greenNodeIdentity.ip);
	///******************ESTO HAY QUE REVISARLO PORQUE NO SABEMOS COMO SACAR LA IP PROPIA xD*******************
	//package.sourcePort = greenNodeIdentity.port;
	//package.destIp = ipToInt32(transmitter->getDestinationIp());
	//package.destPort = transmitter->getDestinationPort();
	return package;
}

void TransmittersController::consume(const payload_ptr &package)
{
	/*
		Regarding heartbeat payloads they must be redirected to the Hearbeat componenet access point.
		And when a herbeat request or answer will be sent to a neighbor, then the payload will be redirected directly to the 
		transmitter that comunicates with such neighbor.

		In order to do this in the Herbeat component, a new method will be needed. This method will receive the transmitters table
		and initialize each visitor queue with the transmiter queue. This process must be done in the setup stage.

		Also the same redirecting table is needed in the request handler to ansewer a heartbeat request.

		If it is too complex to introduce such changes the link layer package will be constructed on the network layer.
	*/

	// The final destination id is part of the network payload.

	auto networkLayerPayload = std::static_pointer_cast<NetworkPayload>(package);
	Log::append("***** Transmitters controller ***** < A network payload was received from the network layer. >");

	// Link layer header payload building.

	auto linkLayerPayload = std::make_shared<LinkLayerPayload>();

	linkLayerPayload->type = LinkLayerPayload::NETWORK_TYPE;
	linkLayerPayload->sourceId = (uint16_t)(this->greenNodeIdentity.id);
	linkLayerPayload->finalDestinationId = (uint16_t)networkLayerPayload->finalDestination;

	// In the meantime. In the future it will be a value provided by the forwarding table.
	linkLayerPayload->immediateDestinationId = (uint16_t)networkLayerPayload->immediateDestination;

	// This length is the sum of the network payload length + forwarding/broadcast/spanning tree length.
	linkLayerPayload->payloadLength = networkLayerPayload->getBytesRepresentationCount(); // This does not include the null terminating character.
	linkLayerPayload->payload = networkLayerPayload;

	this->sendPayloadToTranssmiter(linkLayerPayload);
}

void TransmittersController::sendPayloadToTranssmiter(std::shared_ptr<LinkLayerPayload> linkLayerPayload)
{
	DestinationId neighbourId = linkLayerPayload->immediateDestinationId;

	// It finds the delivery information associated with the destination node which ip is on the package.
	auto transmitter = this->redirectingTable.find(neighbourId)->second;
	if(heartbeat!=nullptr)
	{
		if(heartbeat->knockDoorTo(linkLayerPayload->immediateDestinationId))
		{
			Log::append("***** Transmitters controller ***** < (discard package) Was necessary knock door to d# "+std::to_string(neighbourId) +">");
		}
		else if(heartbeat->getStatusTable()->getStatus(neighbourId)==NeighbourStatus::SLEEP)
		{
			Log::append("***** Transmitters controller ***** < The neighbour status is SLEEP... (discard package ) id# "+std::to_string(neighbourId) +">");
		}
		else{
			Log::append("***** Transmitters controller ***** < Retransmit the payload to transmitter. >");
			transmitter->getQueue()->push(linkLayerPayload);
		}
	}
	else
	{
		Log::append("***** Transmitters controller ***** < Heartbeat pointer is null... discart package >");
	}
}


int32_t TransmittersController::ipToInt32(std::string ip)
{
	ip.erase(std::remove(ip.begin(), ip.end(), '.'), ip.end());
	return (int32_t)std::stoi(ip);
}

void TransmittersController::receiveGreenNodeIdentity(green_node_identity_t &greenNodeIdentity)
{
	this->greenNodeIdentity = greenNodeIdentity;
}

void TransmittersController::receiveRedirectingTable(std::map<DestinationId, Transmitter *> &redirectingTable)
{
	this->redirectingTable = redirectingTable;
}

void TransmittersController::startTranmitters()
{
	Log::append("***** Transmitters controller ***** <  All inner transmitters threads will start. >");
	for (auto &element : redirectingTable)
	{
		element.second->startThread();
	}
	Log::append("***** Transmitters controller ***** <  All inner transmitters threads has started. >");
}

Semaphore *TransmittersController::getTransmittersReadySempahore()
{
	return this->transmitterReady;
}

void TransmittersController::receiveHeartbeatComponent(Heartbeat* heartbeat)
{
	this->heartbeat=heartbeat;
}