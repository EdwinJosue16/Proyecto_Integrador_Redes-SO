#include "Heartbeat.hpp"
#include "Transmitter.h"

Heartbeat::Heartbeat(const neighbors_identity_t &neighborsIdentity, const green_node_identity_t &greenNodeIdentity)
{
    //It initializes the Status Table.
    this->statusOfNeighbors = new StatusTable(neighborsIdentity);

    // It initializes all visitors.
    for (auto neighbour : neighborsIdentity)
    {
        this->visitors.insert({neighbour.id, new Visitor(neighbour.id, greenNodeIdentity.id)});
        this->visitors[neighbour.id]->setStatusTable(statusOfNeighbors);
    }

    // It initializes Respond Heartbeat Request.
    this->respondHeartbeatRequest = new RespondHeartbitRequest();
    this->respondHeartbeatRequest->setVisitors(visitors);
    this->respondHeartbeatRequest->setStatusTable(statusOfNeighbors);

    // It initializes Heartbeat State Reviewer.
    this->heartbeatStateReviewer = new HeartbitStateReviewer();
    this->heartbeatStateReviewer->setVisitors(visitors);
    this->heartbeatStateReviewer->setStatusTable(statusOfNeighbors);

    Log::append("***** Heartbeat           ***** < Initialize all components >");
}

Heartbeat::~Heartbeat()
{
    for (auto &visitor : this->visitors)
    {
        if (visitor.second != nullptr)
        {
            delete visitor.second;
        }
        visitor.second = nullptr;
    }

    if (this->respondHeartbeatRequest != nullptr)
    {
        delete this->respondHeartbeatRequest;
    }
    this->respondHeartbeatRequest = nullptr;

    if (this->statusOfNeighbors != nullptr)
    {
        delete this->statusOfNeighbors;
    }
    this->statusOfNeighbors = nullptr;

    if (this->heartbeatStateReviewer != nullptr)
    {
        delete this->heartbeatStateReviewer;
    }
    this->heartbeatStateReviewer = nullptr;
}

/// Consume the messages in its own execution thread and start visitors
int Heartbeat::run()
{
    this->transmitterReady->wait();

    Log::append("***** Heartbeat           ***** < Heartbeat Responder was started. >");
    this->respondHeartbeatRequest->startThread();

    Log::append("***** Heartbeat           ***** < Heartbeat reviewer has started. >");
    this->heartbeatStateReviewer->startThread();

    // All visitors start working.
    for (auto &visitor : this->visitors)
    {
        Log::append("***** Heartbeat           ***** < Visitor has started. >");
        visitor.second->startThread();
    }

    Log::append("***** Heartbeat           ***** < Heartbeat has started. >");
    this->consumeForever();
    // After hearbeat has stop consuming.
    for (auto &visitor : this->visitors)
    {
        Log::append("***** Heartbeat           ***** < Say to visitor that the work has finished. >");
        visitor.second->terminateWork();
    }

    Log::append("***** Heartbeat           ***** < Say to reviewer that the work has finished. >");
    this->heartbeatStateReviewer->terminateWork();

    return EXIT_SUCCESS;
}

void Heartbeat::consume(const payload_ptr &package)
{
    Log::append("***** Heartbeat           ***** < Retransmitted the package to respondeRequest. >");
    this->respondHeartbeatRequest->getQueue()->push(package);
}

bool Heartbeat::knockDoorTo(DestinationId neighbourId)
{
    Timestamp actual;
    NeighbourStatus unkwonStatus;
    unkwonStatus.status = NeighbourStatus::UNKWON;
    actual.upload();
    bool isNecessaryVisit = this->statusOfNeighbors->neighbourNeedsReview(neighbourId, actual);

    if (isNecessaryVisit)
    {
        Log::append("***** Heartbeat           ***** < Heartbeat send to visit the visitor # >" + std::to_string(neighbourId));
        heartbeatStateReviewer->reviewAndModifyNeighbourStatus(neighbourId, unkwonStatus);
    }
    else
    {
        Log::append("***** Heartbeat           ***** < Was not necessary visit neighbour # >" + std::to_string(neighbourId));
    }
    return isNecessaryVisit;
}

void Heartbeat::setTransmitterInfoToComponents(std::map<DestinationId, Transmitter *> &redirectingTable)
{
    // It gives each visitor its corresponding transmitter queue.
    for (auto &element : redirectingTable)
    {
        this->visitors[element.first]->setQueue(element.second->getQueue());
        this->visitors[element.first]->setTransmitterSempahore(element.second->getTransmitterSempahore());
    }

    this->respondHeartbeatRequest->setRedirectingTable(redirectingTable);
    Log::append("***** Heartbeat           ***** < The hearbeat request handler received the redirecting table. >");

    Log::append("***** Heartbeat           ***** < All visitors have received their corresponding transmitter queue and sempahore.>");
}


void Heartbeat::waitForHeartbeat()
{
    this->waitToFinish();
    Log::append("***** Heartbeat           ***** < Heartbit has finished. >");

    //Joined visitors threads
    for (auto &visitor : this->visitors)
    {
        visitor.second->waitToFinish();
    }
    Log::append("***** Heartbeat           ***** < All visitors have finished. >");

    this->heartbeatStateReviewer->waitToFinish();
    Log::append("***** Heartbeat           ***** < The Reviewer has finished. >");

    this->respondHeartbeatRequest->waitToFinish();
    Log::append("***** Heartbeat           ***** < The Responder has finished. >");
}

void Heartbeat::setTransmitterSempahore(Semaphore *transmitterReady)
{
    this->transmitterReady = transmitterReady;
}