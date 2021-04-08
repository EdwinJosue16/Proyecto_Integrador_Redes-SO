#include "Visitor.hpp"
#include "Payloads/LinkLayerPayload.hpp"
#include "Payloads/HeartbeatPayload.hpp"

Visitor::Visitor()
    : notification(1)
{
    // Default values
    neighbourId = 0;
    greenNodeId = 0;
    keepWorking = false;
}

Visitor::Visitor(uint16_t neighbourId, uint16_t greenNodeId)
    : neighbourId{neighbourId},
      greenNodeId{greenNodeId},
      notification(1),
      keepWorking{true}
{
}

Visitor::~Visitor()
{
}

/// Visitors works (he will go to knock the neighbour door)
int Visitor::run()
{
    // It ensures that the associated transmitter and its inner components were started.
    this->transmitterReady->wait();

    Log::append("***** Visitor            ***** < Init: Visitor #" + std::to_string(neighbourId) + ". >");
    //Number of attemps for knocking the neighbour door
    int attempts = 3;
    bool receivedReply = false;

    while (keepWorking)
    {
        //He waits for some work
        notification.wait();
        if (keepWorking)
        {
            Log::append("***** Visitor            ***** < Is necessary visit #" + std::to_string(neighbourId) + ". >");
            receivedReply = visitMyNeighbour(attempts);
            updateMyNeighbourStatus(receivedReply);
        }
    }
    Log::append("***** Visitor            ***** < Ends: Visitor #" + std::to_string(neighbourId) + ". >");
    return EXIT_SUCCESS;
}

bool Visitor::visitMyNeighbour(int attempts)
{
    bool receivedReply = false;
    int attempt = 1;

    while (attempt <= attempts && !receivedReply)
    {
        // Knock the neighbors door
        Log::append("***** Visitor            ***** < Visit the neighbour #" + std::to_string(neighbourId) + ". >");

        // I suggest producing directly to the specific transmitter queue.
        produce(createRequest());

        Log::append("***** Visitor            ***** < Wait some time for a reply of the neighbour #" + std::to_string(neighbourId) + ". >");
        receivedReply = this->timerForWaitingReply.putAndWait(TIME_OUT);

        // Verifying if has a reply
        if (!receivedReply)
        {
            //If there is not a reply, then try again
            Log::append("***** Visitor            ***** <  Neighbour #" + std::to_string(neighbourId) + " doesn't respond. >");
            ++attempt;
        }
    }
    return receivedReply;
}

void Visitor::updateMyNeighbourStatus(bool receivedReply)
{
    if (receivedReply)
    {
        actualStatus.status = NeighbourStatus::WAKE_UP;
        Log::append("***** Visitor            ***** <  Neighbour #" + std::to_string(neighbourId) + " is awake. >");
    }
    else
    {
        actualStatus.status = NeighbourStatus::SLEEP;
        Log::append("***** Visitor            ***** <  Neighbour #" + std::to_string(neighbourId) + " is sleeping. >");
    }
    actualStatus.timestamp.upload();
    if(statusOfNeighbors->timestampExpire(neighbourId,actualStatus.timestamp))
    {
        statusOfNeighbors->modifyStatus(neighbourId, actualStatus);
    }
}

payload_ptr Visitor::createRequest()
{
    auto hearbeatPayload = std::make_shared<HeartbeatPayload>();
    hearbeatPayload->type = HeartbeatPayload::HEARBEAT_REQUEST;

    auto linkLayerPayload = std::make_shared<LinkLayerPayload>();

    linkLayerPayload->type = LinkLayerPayload::HEARTBEAT_TYPE;
    linkLayerPayload->sourceId = this->greenNodeId;
    linkLayerPayload->immediateDestinationId = this->neighbourId;
    linkLayerPayload->finalDestinationId = this->neighbourId;
    linkLayerPayload->payloadLength = hearbeatPayload->getBytesRepresentationCount();
    linkLayerPayload->payload = hearbeatPayload;

    return linkLayerPayload;
}

void Visitor::goVisit()
{
    this->notification.signal();
}

void Visitor::notifyReply()
{
    this->timerForWaitingReply.stop();
}

void Visitor::terminateWork()
{
    this->keepWorking = false;
    this->notification.signal();
}

void Visitor::setStatusTable(StatusTable *statusOfNeighbors)
{
    this->statusOfNeighbors = statusOfNeighbors;
}

void Visitor::setTransmitterSempahore(Semaphore *transmitterReady)
{
    this->transmitterReady = transmitterReady;
}