#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

#include <Poco/MongoDB/Array.h>
#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Element.h>
#include <Poco/MongoDB/PoolableConnectionFactory.h>
#include <Poco/UUIDGenerator.h>

using namespace Poco;

class Database {
  public:
	enum BookType { NOVEL = 0, STORY };
	enum ResponceType { OK = 0, ERROR };

	enum SharedMood { PRIVATE = 0, PUBLIC };
	struct Comment {
	int id;
	std::string dateTime;
	bool edited;
	std::string content;
	};

	struct BookPart {
	int id;
	std::string name;
	int version;
	int seensCount;
	int likesCount;
	std::string content;
	std::vector<Comment> comments;
	};

	struct Book {
	std::string author;
	int id;
	std::string name;
	BookType type;
	int version;
	std::vector<std::string> tags;
	SharedMood sharedMode;
	int seensCount;
	int likesCount;
	std::vector<std::string> likedUsers;
	std::vector<std::string> SharedWith;
	std::vector<BookPart> parts;
	std::vector<Comment> comments;
	};
	static void connect();

	static std::string getHostAddress();
	static void setHostAddress(const std::string &value);

	static std::string getPort();
	static void setPort(const std::string &value);

	static size_t getPoolCapacity();
	static void setPoolCapacity(const size_t &value);

	static ResponceType insertBooks(std::vector<Book> &inputBooks);

  private:
	Database();

	// typedefs
	typedef Poco::PoolableObjectFactory<Poco::MongoDB::Connection,
					Poco::MongoDB::Connection::Ptr>
	MongoDBConnectionFactory;
	typedef std::unique_ptr<MongoDBConnectionFactory>
	MongoDBConnectionFactoryPtr;

	typedef Poco::ObjectPool<Poco::MongoDB::Connection,
				 Poco::MongoDB::Connection::Ptr>
	MongoDBConnectionPool;
	typedef std::unique_ptr<MongoDBConnectionPool> MongoDBConnectionPoolPtr;

	// variables & objects
	static bool isPoolRunning;
	static size_t poolCapacity;
	static size_t poolPeakCapacity;
	static std::string hostAddress;
	static std::string port;
	static std::vector<std::thread> threadList;
	static MongoDBConnectionFactoryPtr g_connectionFactory;
	static MongoDBConnectionPoolPtr g_connectionPool;
	static Poco::MongoDB::Database g_db;

	// functions
	static void run();
	static Poco::MongoDB::PooledConnection takeConnection();
	static Int64 extractInt64(const MongoDB::Document &d,
				  const std::string &name);
	static void verifyResponse(const MongoDB::Document &response,
				   bool expectOK = true);
};

#endif // DATABASE_H
