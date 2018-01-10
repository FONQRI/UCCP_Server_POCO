#include "database.h"

Database::Database()
	: isPoolRunning(false), poolCapacity(16), poolPeakCapacity(256),
	  hostAddress("localhost"), port("27017"), g_db("pocoTestDB")
{
}

void Database::run()
{
	isPoolRunning = true;

	for (auto &thread : threadList) {
		if (thread.joinable()) {
			thread.join();
		}
	}

	isPoolRunning = false;
}

void Database::connect()
{
	std::string connectionString{""};
	connectionString += hostAddress;
	connectionString += std::string(":");
	connectionString += port;
	std::clog << "connected to : " << connectionString << std::endl;
	g_connectionFactory.reset(new MongoDBConnectionFactory(connectionString));
	g_connectionPool.reset(new MongoDBConnectionPool(
	*g_connectionFactory, poolCapacity, poolPeakCapacity));
}

std::string Database::getHostAddress() const
{
	return hostAddress;
}

void Database::setHostAddress(const std::string &value)
{
	hostAddress = value;
}

std::string Database::getPort() const
{
	return port;
}

void Database::setPort(const std::string &value)
{
	port = value;
}

size_t Database::getPoolCapacity() const
{
	return poolCapacity;
}

void Database::setPoolCapacity(const size_t &value)
{
	poolCapacity = value;
}
