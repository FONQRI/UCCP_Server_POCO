#ifndef DATABASE_H
#define DATABASE_H

#include <iostream>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <tuple>
#include <vector>

#include <Poco/MongoDB/Array.h>
#include <Poco/MongoDB/Connection.h>
#include <Poco/MongoDB/Cursor.h>
#include <Poco/MongoDB/Database.h>
#include <Poco/MongoDB/Document.h>
#include <Poco/MongoDB/Element.h>
#include <Poco/MongoDB/ObjectId.h>
#include <Poco/MongoDB/PoolableConnectionFactory.h>
#include <Poco/UUIDGenerator.h>

using namespace Poco;

class Database {
  public:
	enum BookType { NOVEL = 0, STORY };
	enum ResponceType { OK = 0, ERROR };

	enum SharedMood { PRIVATE = 0, PUBLIC };

	struct DateTime {
	int day{0};
	int month{0};
	int year{0};
	int hour{0};
	int minute{0};
	int second{0};
	};

	struct Comment {
	int id{0};
	bool edited{false};
	DateTime lastEditDateTime;
	std::string content{""};
	};

	struct BookPart {
	int id{0};
	int version{0};
	int seensCount{0};
	int likesCount{0};
	std::string content{""};
	std::string name{""};
	DateTime publishDateTime;
	DateTime lastEditDateTime;

	std::vector<Comment> comments;
	};

	struct Book {
	std::string id{"0"};
	int version{0};
	int seensCount{0};
	int likesCount{0};
	std::string shabakNumber{""};
	std::string author{""};
	std::string name{""};
	BookType type;
	DateTime publishDateTime;
	DateTime lastEditDateTime;
	SharedMood sharedMode;
	std::vector<std::string> tags;
	std::vector<std::string> likedUsers;
	std::vector<std::string> SharedWith;
	std::vector<BookPart> parts;
	std::vector<Comment> comments;
	};

	struct User {
	long studyDuration{000000000000};
	std::string userName{""};
	std::string name{""};
	std::string family{""};
	std::string sex{""};
	std::string email{""};
	std::string phoneNumber{""};
	std::vector<std::string> favoriteTags;
	DateTime berthday;
	};
	static void connect();

	static std::string getHostAddress();
	static void setHostAddress(const std::string &value);

	static std::string getPort();
	static void setPort(const std::string &value);

	static size_t getPoolCapacity();
	static void setPoolCapacity(const size_t &value);

	static ResponceType saveBook(Book &inputBook);
	static ResponceType saveUser(User &user);

	static std::string getBooks(std::string &author);
	static std::string getUser(std::string &userName);

	static ResponceType editBookInfo(Book &inputBook);
	static ResponceType editBookParts(std::vector<BookPart> &bookParts,
					  std::string &bookId);
	static Database::ResponceType updateUser(User &user);

	static ResponceType deleteBook(std::string &bookId);
	static ResponceType deleteBookPart(std::string &bookId, int &partIndex);
	static ResponceType deleteBookComment(std::string &bookId,
					  int &commentIndex);
	static ResponceType deleteBookPartComment(std::string &bookId,
						  int &partIndex,
						  int &commentIndex);

	static Database::ResponceType insertPart(BookPart &inputBookPart,
						 std::string &bookName);

	static Database::ResponceType insertBookComment(Comment &comment,
							std::string &bookName);

	static Database::ResponceType insertBookPartComment(Comment &comment,
							std::string &bookName,
							std::string &author,
							int &partindex);

	static std::string getConnectionName();
	static void setConnectionName(const std::string &value);

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
	static std::string connectionName;

	// functions
	static void run();
	static Poco::MongoDB::PooledConnection takeConnection();
	static Int64 extractInt64(const MongoDB::Document &d,
				  const std::string &name);
	static void verifyResponse(const MongoDB::Document &response,
				   bool expectOK = true);
};

#endif // DATABASE_H
