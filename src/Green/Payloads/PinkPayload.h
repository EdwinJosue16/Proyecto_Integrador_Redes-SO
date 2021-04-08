#ifndef PINK_PAYLOAD_H
#define PINK_PAYLOAD_H

#include "../../common/Payload.hpp"
#include <map>
#include "../CommonTypes.h"
class PinkPayload : public Payload
{
	/// Constants:

public:
	static const uint8_t SPANNING_TREE_MESSAGE = 0;
	static const uint8_t NEIGHBORS_IN_SPANNING_TREE_TABLE_UPDATE = 1;
	static const uint8_t FORWARDING_TABLE_UPDATE = 2;

	/// Class members. (They were made public to ease object construction.)
public:
	std::string pinkMessage;

public:
	/// Default constructor.
	PinkPayload();

	/// Default destructor.
	~PinkPayload();

	/// Inherited must implement methods from base class Payload.
public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();

	std::shared_ptr< std::vector<uint16_t> > neighborsInSpanningTreeUpdateFromString(std::string &str);

	std::shared_ptr< std::map<DestinationId, ThroughNeighborId> >makeForwadingTableFromString(std::string &str);

	void removeHeaderOfTableInStringFormat();
};

#endif // PINK_PAYLOAD_H