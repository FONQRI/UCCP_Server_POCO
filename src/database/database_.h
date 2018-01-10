//#ifndef DATABASE_H
//#define DATABASE_H

//#include <iostream>
//#include <memory>
//#include <mutex>
//#include <thread>

//#include <Poco/UUIDGenerator.h>

//#include <Poco/MongoDB/Array.h>
//#include <Poco/MongoDB/Connection.h>
//#include <Poco/MongoDB/Cursor.h>
//#include <Poco/MongoDB/Database.h>
//#include <Poco/MongoDB/Document.h>
//#include <Poco/MongoDB/Element.h>
//#include <Poco/MongoDB/PoolableConnectionFactory.h>

// namespace {

// using namespace Poco;
// typedef Poco::PoolableObjectFactory<Poco::MongoDB::Connection,
//					Poco::MongoDB::Connection::Ptr>
//	MongoDBConnectionFactory;
// typedef std::unique_ptr<MongoDBConnectionFactory>
// MongoDBConnectionFactoryPtr;

// typedef Poco::ObjectPool<Poco::MongoDB::Connection,
//			 Poco::MongoDB::Connection::Ptr>
//	MongoDBConnectionPool;
// typedef std::unique_ptr<MongoDBConnectionPool> MongoDBConnectionPoolPtr;

// MongoDBConnectionFactoryPtr g_connectionFactory;
// MongoDBConnectionPoolPtr g_connectionPool;

// Poco::MongoDB::Database g_db("pocoTestDB");
// static const std::size_t poolCapacity = 16;
// static const std::size_t poolPeakCapacity = 256;
//}
// Poco::MongoDB::PooledConnection takeConnection();
// void insert(const std::string id);
// void query(const std::string id);
// void remove(const std::string id);
// void connect();

// Poco::MongoDB::PooledConnection takeConnection()
//{
//	static std::mutex connectionPoolLock;
//	std::lock_guard<std::mutex> l(connectionPoolLock);

//	Poco::MongoDB::PooledConnection pooledConnection(*g_connectionPool);
//	auto c = static_cast<Poco::MongoDB::Connection::Ptr>(pooledConnection);

//	if (!c) {
//		// Connection pool can return null if the pool is full
//		// TODO: Gracefully handle this here or implement
//		// ObjectPool::borrowObjectWithTimeout
//	}

//	return std::move(pooledConnection);
//}

// void connect()
//{
//	g_connectionFactory.reset(new
//MongoDBConnectionFactory("localhost:27017"));
//	g_connectionPool.reset(new MongoDBConnectionPool(
//	*g_connectionFactory, poolCapacity, poolPeakCapacity));
//}

// static Int64 extractInt64(const MongoDB::Document &d, const std::string
// &name)
//{
//	Int64 num = -1;
//	if (d.isType<Int32>(name)) {
//		num = d.get<Int32>(name);
//	}
//	else if (d.isType<Int64>(name)) {
//		num = d.get<Int64>(name);
//	}
//	else if (d.isType<double>(name)) {
//		num = static_cast<double>(d.get<double>(name));
//	}
//	else {
//		throw Exception(name + " is not a number.");
//	}
//	return num;
//}

// static void verifyResponse(const MongoDB::Document &response,
//			   bool expectOK = true)
//{
//	// TODO: Remove when updated MongoDB::Document header is used.
//	auto &r = const_cast<MongoDB::Document &>(response);
//	/*
//	 *
// http://docs.mongodb.org/manual/reference/command/insert/#insert-command-output
//	 *
// http://docs.mongodb.org/manual/reference/command/update/#update-command-output
//	 * http://docs.mongodb.org/manual/reference/command/delete/
//	 * http://docs.mongodb.org/manual/reference/command/findAndModify/
//	 */

//	std::ostringstream ostr;
//	try {
//		if (r.exists("ok")) {
//			const auto ok = extractInt64(r, "ok");
//			if (ok != 1) {
//				ostr << "Command failed: ok = " << ok << ". ";
//			}
//		}
//		else if (expectOK) {
//			ostr << "UNEXPECTED: Missing 'ok' in response.";
//		}
//		else {
//			// Document that does not have embedded status response,
//			// e.g.from
//			// find cursor
//			return;
//		}

//		// Find, aggregate commands
//		Int64 code = -1;
//		if (r.exists("code")) {
//			code = extractInt64(r, "code");
//		}
//		if (r.exists("errmsg")) {
//			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
//			 << r.get<std::string>("errmsg");
//		}
//		if (r.exists("$err")) {
//			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
//			 << r.get<std::string>("$err");
//		}

//		// insert, update, delete commands
//		if (r.exists("writeErrors")) {
//			const auto ev =
//r.get<MongoDB::Array::Ptr>("writeErrors");
//			for (size_t i = 0; i < ev->size(); ++i) {
//				auto err = ev->get<MongoDB::Document::Ptr>(i);
//				const auto index = extractInt64(*err, "index");
//				const auto code = extractInt64(*err, "code");
//				ostr << index << ": " << code << ": "
//				 << err->get<std::string>("errmsg");
//				if ((i + 1) < ev->size()) {
//					ostr << std::endl;
//				}
//			}
//		}
//		if (r.exists("writeConcernError")) {
//			const auto err =
//			r.get<MongoDB::Document::Ptr>("writeConcernError");
//			if (ostr.tellp() > 0) {
//				ostr << std::endl;
//			}
//			const auto index = extractInt64(*err, "index");
//			const auto code = extractInt64(*err, "code");
//			ostr << index << ": " << code << ": "
//			 << err->get<std::string>("errmsg");
//		}
//	}
//	catch (const Exception &e) {
//		std::cout << "Response error exception " << e.displayText()
//			  << std::endl;
//		throw;
//	}

//	if (ostr.tellp() > 0) {
//		std::cout << "Error response from server: " <<
//response.toString(2)
//			  << std::endl;
//		throw Exception("MongoDB: " + ostr.str());
//	}
//}

// void insert(const std::string id)
//{
//	try {
//		auto con = takeConnection();
//		auto c = static_cast<MongoDB::Connection::Ptr>(con);

//		MongoDB::Document::Ptr document(new MongoDB::Document());
//		document->add("_id", id);

//		MongoDB::Array::Ptr documents(new MongoDB::Array());
//		documents->add(std::to_string(0), document);

//		auto insert = g_db.createCommand();
//		insert->selector()
//		.add("insert", "Object")
//		.add("documents", documents);

//		std::cout << "INSERT : " << id << std::endl;
//		MongoDB::ResponseMessage response;
//		c->sendRequest(*insert, response);
//		auto doc = *(response.documents()[0]);
//		verifyResponse(doc);
//		for (auto i : response.documents()) {
//			std::cout << i->toString(2) << std::endl;
//		}
//	}
//	catch (const Exception &e) {
//		std::cerr << "INSERT " << id << " failed: " << e.displayText()
//			  << std::endl;
//	}
//}

// void query(const std::string id)
//{
//	try {
//		auto con = takeConnection();
//		auto c = static_cast<MongoDB::Connection::Ptr>(con);

//		auto queryPtr = g_db.createQueryRequest("Object");
//		queryPtr->selector().add("_id", id);
//		queryPtr->setNumberToReturn(1);

//		std::cout << "QUERY : " << id << std::endl;
//		MongoDB::ResponseMessage response;
//		c->sendRequest(*queryPtr, response);
//		auto doc = *(response.documents()[0]);
//		verifyResponse(doc, false);
//		for (auto i : response.documents()) {
//			std::cout << i->toString(2) << std::endl;
//		}
//	}
//	catch (const Exception &e) {
//		std::cerr << "QUERY " << id << "failed: " << e.displayText()
//			  << std::endl;
//	}
//}

// void remove(const std::string id)
//{
//	try {
//		auto con = takeConnection();
//		auto c = static_cast<MongoDB::Connection::Ptr>(con);

//		MongoDB::Document::Ptr query(new MongoDB::Document());
//		query->add("_id", id);

//		MongoDB::Document::Ptr del(new MongoDB::Document());
//		del->add("q", query).add("limit", 1);

//		MongoDB::Array::Ptr deletes(new MongoDB::Array());
//		deletes->add(std::to_string(0), del);

//		auto deleteCmd = g_db.createCommand();
//		deleteCmd->selector()
//		.add("delete", "Object")
//		.add("deletes", deletes);

//		std::cout << "REMOVE : " << id << std::endl;
//		MongoDB::ResponseMessage response;
//		c->sendRequest(*deleteCmd, response);
//		auto doc = *(response.documents()[0]);
//		verifyResponse(doc);
//		for (auto i : response.documents()) {
//			std::cout << i->toString(2) << std::endl;
//		}
//	}
//	catch (const Exception &e) {
//		std::cerr << "REMOVE " << id << " failed: " << e.displayText()
//			  << std::endl;
//	}
//}
//}

//#endif // DATABASE_H
