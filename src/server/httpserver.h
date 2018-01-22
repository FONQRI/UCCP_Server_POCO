/**
  httpserver.h
  purpose : This class is base of http server

  @author FONQRI
  @version 1.0 22/1/2018

  */
#ifndef HTTPSERVER_H
#define HTTPSERVER_H

#include "Poco/Stopwatch.h"
#include <Poco/Timer.h>
#include <Poco/Util/ServerApplication.h>

class HttpServer : public Poco::Util::ServerApplication {
  public:
	HttpServer();
	void SafeExit(Poco::Timer &timer);
	Poco::Stopwatch _sw;

  protected:
	int main(const std::vector<std::string> &args);
};

#endif // HTTPSERVER_H
