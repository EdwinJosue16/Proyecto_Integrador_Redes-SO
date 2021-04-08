#ifndef HEARTBEAT_H
#define HEARTBEAT_H

#include "../common/Consumer.hpp"
#include "../common/Semaphore.hpp"
#include "CommonTypes.h"
#include "StatusTable.h"
#include "RespondHeartbitRequest.hpp"
#include "Visitor.hpp"
#include "HeartbitStateReviewer.hpp"
#include "Log.hpp"

class Heartbeat : public Consumer<payload_ptr>
{
    DISABLE_COPY(Heartbeat);

    /// Class members.
private:
    std::map<DestinationId, Visitor *> visitors;

    RespondHeartbitRequest *respondHeartbeatRequest;

    HeartbitStateReviewer *heartbeatStateReviewer;

    StatusTable *statusOfNeighbors;

    Semaphore *transmitterReady;

public:
    /// Default constructor.
    Heartbeat(const neighbors_identity_t &neighborsIdentity, const green_node_identity_t &greenNodeIdentity);

    /// Overloaded destructor.
    ~Heartbeat();

    /// This method activates the visitor # DestinationId (if is necessary) return true if visitor was send to visit.
    bool knockDoorTo(DestinationId);

    /// This method share the transmitters queue with RespondHeartbitRequest and all Visitors and share redirecting table with RespondComponent.
    void setTransmitterInfoToComponents(std::map<DestinationId, Transmitter *> &redirectingTable);

    inline StatusTable* getStatusTable()
    {
        return this->statusOfNeighbors;
    }
    /// Wait all threads of this class
    void waitForHeartbeat();

    void setTransmitterSempahore(Semaphore *transmitterReady);

    /// Inherited methods.
protected:
    /// Consume the messages in its own execution thread and start visitors.
    virtual int run() override;

    /// Override this method to process any data extracted from the queue.
    virtual void consume(const payload_ptr &package) override;
};
#endif // HEARTBEAT_H