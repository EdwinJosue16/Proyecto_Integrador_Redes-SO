#ifndef VISITOR_HPP
#define VISITOR_HPP

#include "CommonTypes.h"
#include "../common/Producer.hpp"
#include "../common/Timer.h"
#include "StatusTable.h"
#include "Log.hpp"

#define DEFAULT_VALUE -100
#define TIME_OUT 5
class Visitor : public Producer<payload_ptr>
{
    DISABLE_COPY(Visitor);

private:
    uint16_t neighbourId;
    uint16_t greenNodeId;
    Semaphore notification;
    Semaphore *transmitterReady;
    Timer timerForWaitingReply;
    NeighbourStatus actualStatus;
    StatusTable *statusOfNeighbors;
    bool keepWorking;

public:
    int x = 1;
    /// Default Constructor
    Visitor();

    /// Destructor.
    ~Visitor();

    // Constructor
    /// It initializes the sempahore to 1 so the first time it will go and check each neighbor status.
    Visitor(uint16_t neighbourId, uint16_t greenNodeId);

    /// Visitors works (he will go go to knock the neighbour door)
    virtual int run() override;

    ///Return true if neighbour is wake up, false in otherwise
    bool visitMyNeighbour(int attemps);

    ///Modify the status of neighbour in cache table and put a timestamp
    void updateMyNeighbourStatus(bool receivedReply);

    ///Creates a message type heartbit
    payload_ptr createRequest();

    /// It wakes up a visitor who was sleeping only when it is necessary to work.
    void goVisit();

    /// It allows the request handler to notify that an answer to a hearbeat request was received.
    void notifyReply();

    // End all work.
    void terminateWork();

    // It receives the status table.
    void setStatusTable(StatusTable *statusOfNeighbors);

    void setTransmitterSempahore(Semaphore *transmitterReady);
};

#endif // VISITOR_HPP