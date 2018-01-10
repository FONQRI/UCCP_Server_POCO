#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <memory>
#include <mutex>
#include <thread>
#include <vector>

#include <Poco/UUIDGenerator.h>

#include <Poco/MongoDB/Array.h>
#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Element.h>
#include <Poco/MongoDB/PoolableConnectionFactory.h>

using namespace Poco;

class Database {
  public:
	Database();
	void connect();

	std::string getHostAddress() const;
	void setHostAddress(const std::string &value);

	std::string getPort() const;
	void setPort(const std::string &value);

	size_t getPoolCapacity() const;
	void setPoolCapacity(const size_t &value);

private:
	bool isPoolRunning;
	size_t poolCapacity;
	size_t poolPeakCapacity;
	std::string hostAddress;
	std::string port;
	std::vector<std::thread> threadList;
	typedef Poco::PoolableObjectFactory<Poco::MongoDB::Connection,
					Poco::MongoDB::Connection::Ptr>
	MongoDBConnectionFactory;
	typedef std::unique_ptr<MongoDBConnectionFactory>
	MongoDBConnectionFactoryPtr;

	typedef Poco::ObjectPool<Poco::MongoDB::Connection,
				 Poco::MongoDB::Connection::Ptr>
	MongoDBConnectionPool;
	typedef std::unique_ptr<MongoDBConnectionPool> MongoDBConnectionPoolPtr;

	MongoDBConnectionFactoryPtr g_connectionFactory;
	MongoDBConnectionPoolPtr g_connectionPool;

	Poco::MongoDB::Database g_db;

	void run();
};

#endif // DATABASE_H
