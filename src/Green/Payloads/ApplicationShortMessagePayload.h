#ifndef APPLICATION_SHORT_MESSAGE_PAYLOAD_H
#define APPLICATION_SHORT_MESSAGE_PAYLOAD_H

#include "../../common/Payload.hpp"

class ApplicationShortMessagePayload : public Payload
{

	DISABLE_COPY(ApplicationShortMessagePayload);
	// Class members. (They were made public to ease object construction.)
public:
	/// Type is inherited from base class.
	std::string message;
	/// Payload inherited from base class but not used.

public:
	/// Default constructor.
	ApplicationShortMessagePayload();

	/// Default destructor.
	~ApplicationShortMessagePayload();

	/// Inherited methods.
public:
	/// It process a given buffer and initializes *this attributes via casting.
	virtual void deserialize(const char *buffer) override;

	/// It extracts *this attributes and stored them on buffer.
	virtual void serialize(char *buffer) override;

	/// It returns the total bytes that *this will need for serialization. It it recursively called on *this payload.
	virtual size_t getBytesRepresentationCount() override;

	/// It makes a deep copy of *this.
	virtual Payload::payload_ptr makeDeepCopy();

private:
	/// It counts the characters of the buffer.
	uint8_t length(const char *buffer);
};

#endif // APPLICATION_SHORT_MESSAGE_PAYLOAD_H