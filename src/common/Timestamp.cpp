#include "Timestamp.h"

Timestamp::Timestamp(const Timestamp & another)
{
    this->myTime=another.myTime;
    this->sencodsToExpire=another.sencodsToExpire;
}


void Timestamp::operator=(const Timestamp & another)
{
    this->myTime=another.myTime;
    this->sencodsToExpire=another.sencodsToExpire;
}

bool Timestamp::expired(Timestamp & actual)
{
    return difftime(actual.getTime(),this->getTime())>=sencodsToExpire;
}
