/**
  CurrentTimerHandler.h
  purpose : This is a temp class for tese and development

  @author FONQRI
  @version 1.0 22/1/2018

  */
#ifndef CurrentTimeHandler_INCLUDED
#define CurrentTimeHandler_INCLUDED

#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class CurrentTimeHandler: public Poco::Net::HTTPRequestHandler
{
public:
	void handleRequest(Poco::Net::HTTPServerRequest& request, Poco::Net::HTTPServerResponse& response);
};

#endif // CurrentTimeHandler_INCLUDED
