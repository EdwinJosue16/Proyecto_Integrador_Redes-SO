#include "NeighborsTableDeliver.h"

///
NeighborsTableDeliver::NeighborsTableDeliver()
{
    heartbeat = nullptr;
    myTimer = new Timer();
    keepWorking = true;
}

NeighborsTableDeliver::~NeighborsTableDeliver()
{
    if (heartbeat == nullptr)
    {
        delete heartbeat;
        heartbeat = nullptr;
    }

    if (pinkConsumerAccesPoint == nullptr)
    {
        delete pinkConsumerAccesPoint;
        pinkConsumerAccesPoint = nullptr;
    }

    if (blueConsumerAccesPoint == nullptr)
    {
        delete blueConsumerAccesPoint;
        blueConsumerAccesPoint = nullptr;
    }
}

void NeighborsTableDeliver::setPinkConsumerQueue(Queue<payload_ptr> *pinkConsumerAccesPoint)
{
    this->pinkConsumerAccesPoint = pinkConsumerAccesPoint;
}

void NeighborsTableDeliver::setBlueConsumerQueue(Queue<payload_ptr> *blueConsumerAccesPoint)
{
    this->blueConsumerAccesPoint = blueConsumerAccesPoint;
}

void NeighborsTableDeliver::setHeartbeatComponent(Heartbeat *heartbeat)
{
    this->heartbeat = heartbeat;
}

void NeighborsTableDeliver::setNeighborsIdentities(neighbors_identity_t &myNeighbors)
{
    this->myNeighbors = myNeighbors;
}

int NeighborsTableDeliver::run()
{

    while (keepWorking)
    {
        std::string neighborsUpdate;
        if (heartbeat != nullptr)
        {
            for (auto &neighbour : myNeighbors)
            {
                Log::append("***** NeighborsTableDeliver             ***** < Is using heartbeat component >");
                heartbeat->knockDoorTo(neighbour.id);
            }
            myTimer->putAndWait(SECONDS_TO_SEND_ACTUALIZATION);
            neighborsUpdate = heartbeat->getStatusTable()->getLivingNeighbors();

            auto pinkPayload = std::make_shared<PinkPayload>();
            auto appPayload = std::make_shared<ApplicationPayload>();
           
            pinkPayload->pinkMessage = neighborsUpdate;
            appPayload->message = neighborsUpdate;
            appPayload->type = LIVING_NEIGHBORS;
           
            pinkConsumerAccesPoint->push(pinkPayload);
            blueConsumerAccesPoint->push(appPayload);
           
            Log::append("***** NeighborsTableDeliver             ***** < Status of neighbors was uploading and the new info was send to Nodes >");
            Log::append("***** NeighborsTableDeliver             ***** < The list of awake neighbors is " + pinkPayload->pinkMessage + " >");
        }
        else
        {
            Log::append("***** NeighborsTableDeliver             ***** < Heart is nullpointer. >");
        }
    }
    return EXIT_SUCCESS;
}