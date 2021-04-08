#ifndef NEIGHBORS_TABLE_DELIVER
#define NEIGHBORS_TABLE_DELIVER

#include "../common/Thread.hpp"
#include "../common/Queue.hpp"
#include "../common/Timer.h"
#include "Heartbeat.hpp"
#include "Log.hpp"
#include "./Payloads/PinkPayload.h"
#include "./Payloads/ApplicationPayload.h"


class NeighborsTableDeliver : public Thread

{
    DISABLE_COPY(NeighborsTableDeliver);

private:
    
    const int SECONDS_TO_SEND_ACTUALIZATION = 6;

    ///
    Heartbeat *heartbeat;

    /// for ending operations
    bool keepWorking;

    ///For waiting some time between send actualization
    Timer* myTimer;

    ///It is for send living neighbors to Pink Node
    Queue<payload_ptr>* blueConsumerAccesPoint;
    
    ///It is for send living neighbors to Blue Node
    Queue<payload_ptr>* pinkConsumerAccesPoint;
    
    ///It is for holding each id of my neighbors
    neighbors_identity_t myNeighbors;

public:
    ///
    NeighborsTableDeliver();
    ///
    ~NeighborsTableDeliver();

    ///
    inline void terminateWork()
    {
        keepWorking=false;
        myTimer->stop();
        this->waitToFinish();
    }

    void setPinkConsumerQueue(Queue<payload_ptr>* pinkConsumerAccesPoint);
    
    void setBlueConsumerQueue(Queue<payload_ptr>* blueConsumerAccesPoint);

    void setHeartbeatComponent(Heartbeat * heartbeat);

    void setNeighborsIdentities(neighbors_identity_t & myNeighbors);

private:
    ///
    int run() override;
};

#endif