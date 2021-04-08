#ifndef STATUS_TABLE_W
#define STATUS_TABLE_W

#include <mutex> //std::mutex
#include "CommonTypes.h"
#include "Log.hpp"

class StatusTable
{
private:
    const int SECONDS_TO_EXPIRE = 80;
    const std::string MARK_FOR_PAYLOAD = "NUP:";
    const std::string NO_ONE_ALIVE = "*";
    std::mutex padlock;
    std::map<DestinationId, NeighbourStatus> tableInfo;
    void fillTable(const neighbors_identity_t &neighborsIdentity);

public:
    StatusTable()
    {
    }

    StatusTable(const neighbors_identity_t &neighborsIdentity);

    void modifyStatus(DestinationId, NeighbourStatus &);

    bool neighbourNeedsReview(DestinationId, Timestamp);

    bool timestampExpire(DestinationId, Timestamp);

    int8_t getStatus(DestinationId);

    Timestamp getTimestamp(DestinationId);

    std::string getLivingNeighbors();
};
#endif