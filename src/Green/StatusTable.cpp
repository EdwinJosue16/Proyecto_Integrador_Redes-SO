#include "StatusTable.h"

StatusTable::StatusTable(const neighbors_identity_t &neighborsIdentity)
{
    fillTable(neighborsIdentity);
}

void StatusTable::fillTable(const neighbors_identity_t &neighborsIdentity)
{
    NeighbourStatus neighbourStatus;
    for (auto neighbour : neighborsIdentity)
    {
        neighbourStatus.status = NeighbourStatus::SLEEP;
        neighbourStatus.timestamp.setSecodsToExpire(SECONDS_TO_EXPIRE);
        tableInfo.insert({neighbour.id, neighbourStatus});
    }
}

void StatusTable::modifyStatus(DestinationId neighbourId, NeighbourStatus &neighbourStatus)
{
    this->padlock.lock();
    this->tableInfo[neighbourId].status = neighbourStatus.status;
    this->tableInfo[neighbourId].timestamp.upload();
    this->padlock.unlock();
}

int8_t StatusTable::getStatus(DestinationId neighbourId)
{
    return this->tableInfo[neighbourId].status;
}

Timestamp StatusTable::getTimestamp(DestinationId neighbourId)
{
    return this->tableInfo[neighbourId].timestamp;
}

bool StatusTable::neighbourNeedsReview(DestinationId neighbourId, Timestamp actual)
{
    bool timestampExpire = this->timestampExpire(neighbourId, actual);
    uint8_t status = this->getStatus(neighbourId);
    Log::append("***** StatusTable             ***** < The neighbour #" +
                std::to_string(neighbourId)+    " status is " +
                std::to_string(status)+
                " and his timestamp expired (1 or 0) " + std::to_string(timestampExpire)
                +" .>");
    return timestampExpire || status == NeighbourStatus::UNKWON;
}

bool StatusTable::timestampExpire(DestinationId neighbourId, Timestamp actual)
{
    return tableInfo[neighbourId].timestamp.expired(actual);
}

std::string StatusTable::getLivingNeighbors()
{
    std::string livingNeighbors = MARK_FOR_PAYLOAD;
    Timestamp currentTime;
    bool someoneIsAlive =false;
    for(auto & neighbourStatusPair : tableInfo)
    {
        if( neighbourStatusPair.second.status==NeighbourStatus::WAKE_UP &&
            !neighbourStatusPair.second.timestamp.expired(currentTime)
        )
        {
            livingNeighbors+=std::to_string(neighbourStatusPair.first)+",";
            someoneIsAlive=true;
        }
    }
    if(someoneIsAlive)
    {
        livingNeighbors.pop_back();
    }
    else
    {
        livingNeighbors+=NO_ONE_ALIVE;
    }
    return livingNeighbors;
}