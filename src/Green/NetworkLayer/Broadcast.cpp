#include "Broadcast.h"

Broadcast::Broadcast()
{
}

Broadcast::~Broadcast()
{
}

void Broadcast::consume(const payload_ptr &package)
{
	if (package->type == 1)
	{
		// Avoid compilation warning in the meantime.
	}
	//for (auto &neighborId : this->spanningTree)
	//{
	//	// There is no need to replicate the message to the neighbor that has sent it.
	//	if (package.payload.sourceId != neighborId)
	//	{
	//		this->linkLayerAccessPoint->push(package);
	//	}
	//}
}

int Broadcast::run()
{
	this->consumeForever();
	return EXIT_SUCCESS;
}

void Broadcast::setLinkLayerAcessPoint(Queue<payload_ptr> *linkLayerAccessPoint)
{
	this->linkLayerAccessPoint = linkLayerAccessPoint;
}