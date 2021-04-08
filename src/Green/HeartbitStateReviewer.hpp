#ifndef HEARTBIT_STATE_REVIEWER
#define HEARTBIT_STATE_REVIEWER

#include "../common/Thread.hpp"
#include "../common/Timer.h"
#include "../common/Timestamp.h"
#include "StatusTable.h"
#include "Visitor.hpp"
#include "Log.hpp"
class HeartbitStateReviewer : public Thread
{

    DISABLE_COPY(HeartbitStateReviewer);

private:

    ///This time (in seconds ) is for doing periodic review 
    static const int TIME_TO_REVIEW = 40; 
    ///
    std::map<DestinationId, Visitor *> visitors;

    ///
    StatusTable *statusOfNeighbors;

    /// Is for waiting 2 minutes
    Timer timer;

    bool keepWorking;

private:
    ///This method check status table every 2 minutes
    virtual int run() override;

    ///This method is for check the status of all neighbors and put visitor to visit (if is necessary)
    void checkStatusTable();

public:
    HeartbitStateReviewer();

    inline void setVisitors(std::map<DestinationId, Visitor *> &visitors)
    {
        this->visitors = visitors;
    }

    inline StatusTable *setStatusTable(StatusTable *statusOfNeighbors)
    {
        return this->statusOfNeighbors = statusOfNeighbors;
    }

    inline void terminateWork()
    {
        this->keepWorking = false;
        this->timer.stop();
    }

    /// This method put neighbour with destinationId # in UNKWON status and put visitor to visit
    void reviewAndModifyNeighbourStatus(DestinationId, NeighbourStatus &);
};
#endif