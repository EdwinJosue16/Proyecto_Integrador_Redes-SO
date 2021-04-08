
#include "HeartbitStateReviewer.hpp"
HeartbitStateReviewer::HeartbitStateReviewer()
{
    keepWorking = true;
}

///
int HeartbitStateReviewer::run()
{
    while (keepWorking)
    {
        //When receive a signal, then it has no more work to do
        if(!timer.putAndWait(TIME_TO_REVIEW))
        {
            Log::append("***** ReviewStatus           ***** < Is time to review the table >");
            this->checkStatusTable();
            Log::append("***** ReviewStatus           ***** < All table was reviewed >");
        }
    }
    Log::append("***** ReviewStatus           ***** < Has finished >");
    return EXIT_SUCCESS;
}

///
void HeartbitStateReviewer::checkStatusTable()
{
    Timestamp actual;
    NeighbourStatus unkwonStatus;
    unkwonStatus.status = NeighbourStatus::UNKWON;
    actual.upload();
    for (auto &visitor : visitors)
    {
        unkwonStatus.timestamp = actual;
        if (statusOfNeighbors->neighbourNeedsReview(visitor.first, actual))
        {
            this->reviewAndModifyNeighbourStatus(visitor.first, unkwonStatus);
        }
    }
}

void HeartbitStateReviewer::reviewAndModifyNeighbourStatus(DestinationId neighbourId, NeighbourStatus &unkwonStatus)
{
    Log::append("***** ReviewStatus           ***** <  Neighbour #" + std::to_string(neighbourId) + " needs review >");
    statusOfNeighbors->modifyStatus(neighbourId, unkwonStatus);
    visitors[neighbourId]->goVisit();
}