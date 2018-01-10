#include "HandlerFactory.h"

#include "template_engine/CurrentTimeHandler.h"

HandlerFactory::HandlerFactory() {}

Poco::Net::HTTPRequestHandler *HandlerFactory::createRequestHandler(
	const Poco::Net::HTTPServerRequest &request)
{
	return new CurrentTimeHandler();
}
