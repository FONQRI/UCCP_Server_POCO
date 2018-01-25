#ifndef HandlerFactory_INCLUDED
#define HandlerFactory_INCLUDED

#include <Poco/Net/HTTPRequestHandlerFactory.h>
#include <Poco/URI.h>
#include <Poco/Util/Util.h>

class HandlerFactory : public Poco::Net::HTTPRequestHandlerFactory {
  public:
	HandlerFactory();

	Poco::Net::HTTPRequestHandler *
	createRequestHandler(const Poco::Net::HTTPServerRequest &request);
};

#endif // HandlerFactory_INCLUDED
