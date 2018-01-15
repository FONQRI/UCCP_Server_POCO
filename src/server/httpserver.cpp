#include "httpserver.h"
#include "HandlerFactory.h"

#include <Poco/Net/HTTPServer.h>
#include <Poco/Net/ServerSocket.h>
#include <Poco/Util/ServerApplication.h>

#include <iostream>

#include "database/database.h"
HttpServer::HttpServer() {}

int HttpServer::main(const std::vector<std::string> &args)
{
	Database::connect();

	Poco::TimerCallback<HttpServer> callback(*this, &HttpServer::SafeExit);

	Poco::Timer timer(250, 500);

	//	timer.start(callback);
	//	_sw.start();

	Poco::UInt16 port = 8888;

	Poco::Net::ServerSocket socket(port);

	Poco::Net::HTTPServerParams *pParams = new Poco::Net::HTTPServerParams();
	pParams->setMaxQueued(100);
	pParams->setMaxThreads(16);

	Poco::Net::HTTPServer server(new HandlerFactory(), socket, pParams);

	server.start();

	waitForTerminationRequest();
	server.stop();

	return EXIT_OK;
}

void HttpServer::SafeExit(Poco::Timer &timer)
{
	std::cout << "Callback called after " << _sw.elapsed() / 1000
		  << "      milliseconds." << std::endl;
}
