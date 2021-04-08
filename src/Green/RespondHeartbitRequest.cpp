#include "RespondHeartbitRequest.hpp"
#include "Payloads/LinkLayerPayload.hpp"
#include "Payloads/HeartbeatPayload.hpp"
#include "Log.hpp"

RespondHeartbitRequest::RespondHeartbitRequest()
{
    // This module will receive messages of awake neighbors only.
    this->wakeUpStatus.status = NeighbourStatus::WAKE_UP;
}

RespondHeartbitRequest::~RespondHeartbitRequest()
{
}

int RespondHeartbitRequest::run()
{
    Log::append("***** Respond Request             ***** < The Responder has started its execution. >");

    this->consumeForever();

    Log::append("***** Respond Request             ***** < The Responder has ended its execution. >");
    return EXIT_SUCCESS;
}

void RespondHeartbitRequest::consume(const payload_ptr &data)
{
    this->attendRequest(data);
}

void RespondHeartbitRequest::attendRequest(payload_ptr request)
{

    // It expects to receive herbeat requests only.

    auto linkLayerPayload = std::static_pointer_cast<LinkLayerPayload>(request);
    auto heartbeatPayload = std::static_pointer_cast<HeartbeatPayload>(linkLayerPayload->payload);
    if (heartbeatPayload->type == HeartbeatPayload::HEARBEAT_REQUEST)
    {
        Log::append("***** Respond Request             ***** < A heartbeat request was received. >");
        // It propagates the request to the handler.
        this->respondRequestTo(request);
    }

    // It updload the timestamp of the neighbor who send the request and its marked as awake.
    Log::append("***** Respond Request             ***** < A heartbeat answer was received. >");
    // Final destination id indicates to who the message was meant to.
    this->notifyReplyToVisitor(linkLayerPayload->sourceId);
    this->uploadStatusTable(linkLayerPayload->sourceId);
}

void RespondHeartbitRequest::respondRequestTo(payload_ptr neighborRequest)
{
    Log::append("***** Respond Request             ***** < A heartbeat request will be processed. >");

    auto request = std::static_pointer_cast<LinkLayerPayload>(neighborRequest);

    // It builds up the heartbeat answer.
    auto heartbeatPayload = std::make_shared<HeartbeatPayload>();
    heartbeatPayload->type = HeartbeatPayload::HEARBEAT_ANSWER;

    Log::append("***** Respond Request             ***** < The heartbeat payload was built. >");

    auto answer = std::make_shared<LinkLayerPayload>();

    answer->type = LinkLayerPayload::HEARTBEAT_TYPE;

    // The source id will be the intended final destination id of the request.
    answer->sourceId = request->finalDestinationId;

    // The destination node will be the one who sent the request.
    answer->immediateDestinationId = request->sourceId;
    answer->finalDestinationId = request->sourceId;

    answer->payloadLength = heartbeatPayload->getBytesRepresentationCount();
    answer->payload = heartbeatPayload;

    Log::append("***** Respond Request             ***** < The heartbeat answer was built. >");

    // It sends the heartbeat answer to the correspoding neighbor who sent the request via associated transmitter

    auto transmitter = this->redirectingTable.find(request->sourceId)->second;

    if (transmitter != nullptr)
    {
        Log::append("***** Respond Request             ***** < The transmitter was found: " + std::to_string(request->sourceId) + ". >");
    }
    else
    {
        Log::append("***** Respond Request             ***** < The transmitter was NOT found: " + std::to_string(request->sourceId) + ". >");
    }

    transmitter->getQueue()->push(answer);

    Log::append("***** Respond Request             ***** < An answer has been sent to " +
                std::to_string(request->sourceId) + " neighbor after receiving its heartbet request. >");
}

void RespondHeartbitRequest::notifyReplyToVisitor(uint8_t associatedNeighborId)
{
        Log::append("***** Respond Request             ***** < ...Notifying reply to visitor # >" + std::to_string(associatedNeighborId));
        visitors[associatedNeighborId]->notifyReply();
    
}

void RespondHeartbitRequest::uploadStatusTable(uint8_t neighborId)
{
    Log::append("***** Respond Request             ***** < ...Uploading status (because is awake) of neighbour # >" + std::to_string(neighborId));
    this->wakeUpStatus.timestamp.upload();
    this->statusOfNeighbors->modifyStatus(neighborId, this->wakeUpStatus);
    // It updates the timestamp of the neighbor status.

}

void RespondHeartbitRequest::setVisitors(std::map<DestinationId, Visitor *>& visitors)
{
    this->visitors = visitors;
    Log::append("***** Respond Request             ***** < Set visitors >");
}

void RespondHeartbitRequest::setStatusTable(StatusTable *statusOfNeighbors)
{    
    this->statusOfNeighbors = statusOfNeighbors;
    Log::append("***** Respond Request             ***** < Set status table >");
}

void RespondHeartbitRequest::setRedirectingTable(std::map<DestinationId, Transmitter *> &redirectingTable)
{
    this->redirectingTable = redirectingTable;
    Log::append("***** Respond Request             ***** < Set redirecting table >");
}