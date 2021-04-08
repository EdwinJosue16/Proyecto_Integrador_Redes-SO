#ifndef REACHABILITYTUPLE_H
#define REACHABILITYTUPLE_H
#include <cstdint>

struct ReachabilityTuple
{
	uint16_t neighborId;
	uint8_t neighborDistance;
};

#endif // REACHABILITYTUPLE_H