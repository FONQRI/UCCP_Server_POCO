/**
  database.h
  purpose : manage database connections . MongoDB database client Poco library

  @author FONQRI
  @version 1.0 21/1/2018

  */

// TODO P[2] handle database returned errors
// TODO P[1] add time to liked user for every like

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

/**
 * Database
 * @brief The Database class manage mongodb database and contain add database
 * functions .
 * all functions are static and you can't create object from this class .
 *
 */
class Database {
  public:
	// enums
	enum ResponceType { OK = 0, ERROR };
	enum SharedMood { PRIVATE = 0, PUBLIC };

	/**
	 * @brief The DateTime struct contain Date and time needed variables
	 */
	struct DateTime {
	int day{0};
	int month{0};
	int year{0};
	int hour{0};
	int minute{0};
	int second{0};
	};

	/**
	 * @brief The Comment struct
	 */
	struct Comment {
	std::string id{0};
	bool edited{false};
	std::string username{""};
	DateTime lastEditDateTime;
	std::string content{""};
	std::string repliedTo{""};
	};

	/**
	 * @brief The BookPart struct
	 */
	struct BookPart {
	std::string id{0};
	int version{0};
	int seensCount{0};
	int likesCount{0};
	std::string content{""};
	std::string name{""};
	DateTime publishDateTime;
	DateTime lastEditDateTime;
	std::vector<Comment> comments;
	};

	/**
	 * @brief The Book struct
	 */
	struct Book {
	std::string id{"0"};
	int version{0};
	int seensCount{0};
	int likesCount{0};
	std::string shabakNumber{""};
	std::string author{""};
	std::string name{""};
	std::string type;
	DateTime publishDateTime;
	DateTime lastEditDateTime;
	SharedMood sharedMode;
	std::vector<std::string> tags;
	std::vector<std::string> likedUsers;
	std::vector<std::string> SharedWith;
	std::vector<BookPart> parts;
	std::vector<Comment> comments;
	};

	/**
	 * @brief The User struct
	 */
	struct User {
	long studyDuration{000000000000};
	std::string id{""};
	std::string username{""};
	std::string name{""};
	std::string family{""};
	std::string sex{""};
	std::string email{""};
	std::string phoneNumber{""};
	std::vector<std::string> favoriteTags;
	DateTime berthday;
	};

	/**
	 * @brief connect need to be called afted setting database parameters by
	 * defullt it will connect to localhost:27017 and create connection by
	 * pocoTestDB name
	 */
	static void connect();

	// save functions
	/**
	 * @brief saveBooks : save books in database
	 * @param inputBook : an struct of Database::Book
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType
	saveBooks(std::vector<std::shared_ptr<Book>> &inputBooks);

	/**
	 * @brief saveUser : save book in database
	 * @param user : uses Database::User struct
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType saveUser(User &user);

	// get functions
	/**
	 * @brief getBooks : gets all books of an author
	 * @param author : author username
	 * @return json that contains books
	 */
	static std::string getBooks(std::string &author);

	/**
	 * @brief getUser : gets user json
	 * @param username : username of user
	 * @return user info json
	 */
	static std::string getUser(std::string &username);

	// edits functions
	/**
	 * @brief editBookInfo : edits book info
	 * @param inputBook : Database::Book with new info
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType editBookInfo(std::shared_ptr<Book> &inputBook);

	/**
	 * @brief editBookParts : edits book parts that given
	 * @param bookParts : a std::vector of new or changed book parts
	 * @param bookId : book id that should be changed
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType editBookParts(std::vector<BookPart> &bookParts,
					  std::string &bookId);

	/**
	 * @brief editBookComment : edits comment that is on a book
	 * @param comment : Database::Comment struct of new comment
	 * @param bookId : book id that should be changed
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType editBookComment(Comment &comment, std::string &bookId);

	/**
	 * @brief editBookPartComment : edits comment that is on a book part
	 * @param comment : Database::Comment struct of new comment
	 * @param bookId : book id that should be changed
	 * @param partindex : index of part that comment is in it
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType
	editBookPartComment(Comment &comment, std::string &bookId, int &partindex);

	// updates
	/**
	 * @brief updateUser : update an existing user
	 * @param user : Database::User struct by new info
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType updateUser(User &user);

	// dletes
	/**
	 * @brief deleteBook : delete an existing book
	 * @param bookId : book id that should be deleted
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType deleteBook(std::string &bookId);

	/**
	 * @brief deleteBookPart : delete a part of existing book
	 * @param bookId : book id
	 * @param partIndex : part index that should be deleted
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType deleteBookPart(std::string &bookId, int &partIndex);

	/**
	 * @brief deleteBookComment : delete a comment from book comments
	 * @param bookId : book id
	 * @param commentIndex : index of comment that should be deleted from book
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType deleteBookComment(std::string &bookId,
					  int &commentIndex);

	/**
	 * @brief deleteBookPartComment : delete a comment from book part comments
	 * @param bookId : book id
	 * @param partIndex : part index that contains comment
	 * @param commentIndex : index of comment that should be deleted from book
	 * part
	 * @return responce code uses Database::ResponceType
	 */
	static ResponceType deleteBookPartComment(std::string &bookId,
						  int &partIndex,
						  int &commentIndex);

	/**
	 * @brief deleteLikedUser : delete a user from users that liked book
	 * @param bookId : book id
	 * @param username : user name that unliked book
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType deleteLikedUser(std::string &bookId,
						  std::string &likeId);

	/**
	 * @brief deleteSharedWithUsers : delete users that deleted from shared with
	 * users of this book
	 * @param bookId : book id
	 * @param usernames : std::vector of usernames
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType
	deleteSharedWithUsers(std::string &bookId,
			  std::vector<std::string> &usernames);

	// inserts
	/**
	 * @brief insertPart : inserts a part to book
	 * @param inputBookPart : Database::BookPart of new part
	 * @param bookId : book id
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType insertPart(BookPart &inputBookPart,
						 std::string &bookId);

	// TODO P[1] addd insertParts function
	/**
	 * @brief insertBookComment : inserts a comment to book
	 * @param comment : Database::Comment of comment
	 * @param bookId : book id
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType insertBookComment(Comment &comment,
							std::string &bookId);

	/**
	 * @brief insertBookPartComment :  inserts a comment to book part
	 * @param comment :  Database::Comment of comment
	 * @param bookId : book id
	 * @param partindex : index of part
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType insertBookPartComment(Comment &comment,
							std::string &bookId,
							int &partindex);

	/**
	 * @brief insertLikedUser : add a user when user likes book
	 * @param bookId : book id
	 * @param username : username of an existing user
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType insertLikedUser(std::string &bookId,
						  std::string &username);

	/**
	 * @brief insertSharedWithUsers : inserts users that private book is shared
	 * with theme
	 * @param bookId : book id
	 * @param usernames : username
	 * @return responce code uses Database::ResponceType
	 */
	static Database::ResponceType
	insertSharedWithUsers(std::string &bookId,
			  std::vector<std::string> &usernames);

	// getter and setters
	/**
	 * @brief getConnectionName : gets connection name
	 * @return connection name
	 */
	static std::string getConnectionName();

	/**
	 * @brief setConnectionName : set connection name it has a defult value
	 * @param value : new connection name
	 */
	static void setConnectionName(const std::string &value);

	/**
	 * @brief getHostAddress : gets host address
	 * @return host address
	 */
	static std::string getHostAddress();

	/**
	 * @brief setHostAddress : sets host address
	 * @param value : new host address
	 */
	static void setHostAddress(const std::string &value);

	/**
	 * @brief getPort : gets port
	 * @return port number
	 */
	static std::string getPort();

	/**
	 * @brief setPort : sets port
	 * @param value : new port value
	 */
	static void setPort(const std::string &value);

	/**
	 * @brief getPoolCapacity : gets pool capacity
	 * @return pool capcity
	 */
	static size_t getPoolCapacity();

	/**
	 * @brief setPoolCapacity : sets pool capacity
	 * @param value : new pool capacity
	 */
	static void setPoolCapacity(const size_t &value);

  private:
	/**
	 * @brief Database : private constructor
	 */
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
	static MongoDBConnectionFactoryPtr g_connectionFactory;
	static MongoDBConnectionPoolPtr g_connectionPool;
	static Poco::MongoDB::Database g_db;
	static std::string connectionName;

	// functions
	/**
	 * @brief takeConnection : takes a connection to work with database
	 * @return Poco::MongoDB::PooledConnection
	 */
	static Poco::MongoDB::PooledConnection takeConnection();

	/**
	 * @brief extractInt64
	 * @param d
	 * @param name
	 * @return
	 */
	static Int64 extractInt64(const MongoDB::Document &d,
				  const std::string &name);

	/**
	 * @brief verifyResponse
	 * @param response
	 * @param expectOK
	 */
	static void verifyResponse(const MongoDB::Document &response,
				   bool expectOK = true);

	static std::shared_ptr<DateTime> getDateTime();
};

#endif // DATABASE_H
