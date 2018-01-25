#include "HandlerFactory.h"

#include "controller/CurrentTimeHandler.h"
#include <Poco/Net/HTTPServer.h>

#include <iostream>
#include <sstream>

HandlerFactory::HandlerFactory() {}

Poco::Net::HTTPRequestHandler *HandlerFactory::createRequestHandler(
	const Poco::Net::HTTPServerRequest &request)
{
	// separate url
	std::vector<std::string> urlParts;
	std::istringstream urlStreamString{request.getURI()};
	std::string tempString;
	while (getline(urlStreamString, tempString, '/')) {
		urlParts.push_back(tempString);
	}

	// manage request url and return  appropriate controller
	if (urlParts[0] == "") {
		return new CurrentTimeHandler();
	}
	else {
		return new CurrentTimeHandler();
	}
}
