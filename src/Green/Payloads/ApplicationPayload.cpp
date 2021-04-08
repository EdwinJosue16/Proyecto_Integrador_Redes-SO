#include "ApplicationPayload.h"

ApplicationPayload::ApplicationPayload()
{
}

ApplicationPayload::~ApplicationPayload()
{
}

void ApplicationPayload::deserialize(const char *buffer)
{
	// This method will not be used.
	if (buffer != nullptr)
	{
		// Avoid compilation warning.
	}
}

void ApplicationPayload::serialize(char *buffer)
{
	// This method will not be used.
	if (buffer != nullptr)
	{
		// Avoid compilation warning.
	}
}

size_t ApplicationPayload::getBytesRepresentationCount()
{
	// This method will not be used.
	return (size_t)EXIT_SUCCESS;
}

Payload::payload_ptr ApplicationPayload::makeDeepCopy()
{
	// To be implemented.
	return std::make_shared<ApplicationPayload>();
}
