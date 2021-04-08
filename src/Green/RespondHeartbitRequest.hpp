#ifndef RESPOND_HEARTBIT_REQUEST_H
#define RESPOND_HEARTBIT_REQUEST_H

#include "../common/Consumer.hpp"
#include "CommonTypes.h"
#include "Visitor.hpp"
#include "Transmitter.h"

class RespondHeartbitRequest : public Consumer<payload_ptr>
{
    DISABLE_COPY(RespondHeartbitRequest);

    /// Class members.
private:
    std::map<DestinationId, Visitor *> visitors;

    StatusTable *statusOfNeighbors;

    std::map<DestinationId, Transmitter *> redirectingTable;

    NeighbourStatus wakeUpStatus;

    /// Construction and destruction.
public:
    /// Default constructor.
    RespondHeartbitRequest();

    /// Default destructor;
    ~RespondHeartbitRequest();

    /// Methods used to initialize inner components.
public:
    void setVisitors(std::map<DestinationId, Visitor *>& visitors);

    void setStatusTable(StatusTable *statusOfNeighbors);

    void setRedirectingTable(std::map<DestinationId, Transmitter *>& redirectingTable);

    /// Inherited methods.
public:
    /// It starts the thread execution.
    int virtual run();

    /// It receives and process a payload.
    virtual void consume(const payload_ptr &data);

private:
    /// It notifies destination visitor that an answer to its request was received.
    void notifyReplyToVisitor(uint8_t associatedNeighborId);

    /// It creates and answer to a heartbeat request that was provided by a neighbor.
    void respondRequestTo(payload_ptr neighborRequest);

    /// It updates the status table.
    void uploadStatusTable(uint8_t neighborId);

    /// It process a hearbeat request.
    void attendRequest(payload_ptr request);
};
#endif // RESPOND_HEARTBIT_REQUEST_H