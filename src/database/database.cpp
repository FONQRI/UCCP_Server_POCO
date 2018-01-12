#include "database.h"

Database::Database() {}

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

std::string Database::getHostAddress() { return hostAddress; }

void Database::setHostAddress(const std::string &value) { hostAddress = value; }

std::string Database::getPort() { return port; }

void Database::setPort(const std::string &value) { port = value; }

size_t Database::getPoolCapacity() { return poolCapacity; }

void Database::setPoolCapacity(const size_t &value) { poolCapacity = value; }

Database::ResponceType Database::insertBooks(std::vector<Book> &inputBooks)
{
	// TODO needs to use Book struct instade of constant values
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);
		MongoDB::Document::Ptr booksObj(new MongoDB::Document());
		booksObj->add("author", inputBooks.at(0).author);

		MongoDB::Array::Ptr bookList(new MongoDB::Array());
		int bookIndex = 0;
		for (auto &book : inputBooks) {
			// MongoDB::Document::Ptr booksObj(new MongoDB::Document());
			MongoDB::Document::Ptr bookObj(new MongoDB::Document());
			bookObj->add("id", book.id);
			bookObj->add("name", book.name);
			bookObj->add("type", std::to_string(book.type));
			bookObj->add("version", book.version);

			MongoDB::Array::Ptr bookTags(new MongoDB::Array());

			for (size_t i = 0; i < book.tags.size(); i++) {
				bookTags->add(std::to_string(i), book.tags.at(i));
			}

			bookObj->add("tags", bookTags);

			bookObj->add("sharedMode", std::to_string(book.sharedMode));
			bookObj->add("seensCount", book.seensCount);
			bookObj->add("likesCount", book.likesCount);

			MongoDB::Array::Ptr likedUsersList(new MongoDB::Array());

			for (size_t i = 0; i < book.likedUsers.size(); i++) {
				likedUsersList->add(std::to_string(i),
						book.likedUsers.at(i));
			}

			bookObj->add("likedUsers", likedUsersList);

			MongoDB::Array::Ptr sharedWithList(new MongoDB::Array());

			for (size_t i = 0; i < book.SharedWith.size(); i++) {
				sharedWithList->add(std::to_string(i),
						book.SharedWith.at(i));
			}

			bookObj->add("SharedWith", sharedWithList);

			MongoDB::Array::Ptr bookCommentsArray(new MongoDB::Array());

			for (size_t i = 0; i < book.comments.size(); i++) {
				MongoDB::Document::Ptr bookCommentObj(
				new MongoDB::Document());

				bookCommentObj->add("id", book.comments.at(i).id);
				bookCommentObj->add("dateTime",
						book.comments.at(i).dateTime);
				bookCommentObj->add("edited",
						book.comments.at(i).edited);
				bookCommentObj->add("content",
						book.comments.at(i).content);
				bookCommentsArray->add(std::to_string(i),
						   bookCommentObj);
			}
			bookObj->add("comments", bookCommentsArray);

			MongoDB::Array::Ptr bookPartsArray(new MongoDB::Array());

			for (size_t i = 0; i < book.parts.size(); i++) {
				MongoDB::Document::Ptr bookPartObj(
				new MongoDB::Document());

				bookPartObj->add("id", book.parts.at(i).id);
				bookPartObj->add("name", book.parts.at(i).name);
				bookPartObj->add("version",
						 book.parts.at(i).version);
				bookPartObj->add("seensCount",
						 book.parts.at(i).seensCount);
				bookPartObj->add("likesCount",
						 book.parts.at(i).likesCount);
				bookPartObj->add("content",
						 book.parts.at(i).content);

				MongoDB::Array::Ptr partCommentsArray(
				new MongoDB::Array());

				for (size_t commentIndex = 0;
					 commentIndex < book.parts.size();
					 commentIndex++) {

					MongoDB::Document::Ptr partCommentsObj(
					new MongoDB::Document());

					partCommentsObj->add(
					"id", book.parts.at(i)
						  .comments.at(commentIndex)
						  .id);
					partCommentsObj->add(
					"dateTime",
					book.parts.at(i)
						.comments.at(commentIndex)
						.dateTime);
					partCommentsObj->add(
					"edited", book.parts.at(i)
							  .comments.at(commentIndex)
							  .edited);
					bookPartObj->add(
					"content",
					book.parts.at(i)
						.comments.at(commentIndex)
						.content);

					partCommentsArray->add(
					std::to_string(commentIndex),
					partCommentsObj);
				}
				bookPartObj->add("comments", partCommentsArray);
				bookPartsArray->add(std::to_string(i), bookPartObj);
			}

			bookObj->add("parts", bookPartsArray);

			bookList->add(std::to_string(0), bookObj);
			bookIndex++;
		}

		booksObj->add("books", bookList);

		MongoDB::Array::Ptr booksList(new MongoDB::Array());
		booksList->add(std::to_string(0), booksObj);
		auto insert = g_db.createCommand();
		insert->selector()
		.add("insert", "Object")
		.add("documents", booksList);

		std::cout << "INSERT : "
			  << "Done" << std::endl;

		MongoDB::ResponseMessage response;
		c->sendRequest(*insert, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			std::cout << i->toString(2) << std::endl;
		}
		return Database::ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT "
			  << " failed: " << e.displayText() << std::endl;
		return Database::ResponceType::ERROR;
	}
}

void Database::run()
{
	isPoolRunning = true;

	std::thread databaseThread([&]() {

	for (auto &thread : threadList) {
		if (thread.joinable()) {
			thread.join();
		}
	}
	});
	isPoolRunning = false;
}

Poco::MongoDB::PooledConnection Database::takeConnection()
{
	static std::mutex connectionPoolLock;
	std::lock_guard<std::mutex> l(connectionPoolLock);

	Poco::MongoDB::PooledConnection pooledConnection(*g_connectionPool);
	auto c = static_cast<Poco::MongoDB::Connection::Ptr>(pooledConnection);

	if (!c) {
		// Connection pool can return null if the pool is full
		// TODO: Gracefully handle this here or implement
		// ObjectPool::borrowObjectWithTimeout
	}

	return std::move(pooledConnection);
}

Int64 Database::extractInt64(const MongoDB::Document &d,
				 const std::string &name)
{
	Int64 num = -1;
	if (d.isType<Int32>(name)) {
		num = d.get<Int32>(name);
	}
	else if (d.isType<Int64>(name)) {
		num = d.get<Int64>(name);
	}
	else if (d.isType<double>(name)) {
		num = static_cast<double>(d.get<double>(name));
	}
	else {
		throw Exception(name + " is not a number.");
	}
	return num;
}

void Database::verifyResponse(const MongoDB::Document &response, bool expectOK)
{
	// TODO: Remove when updated MongoDB::Document header is used.
	auto &r = const_cast<MongoDB::Document &>(response);
	/*
	 *
http://docs.mongodb.org/manual/reference/command/insert/#insert-command-output
	 *
http://docs.mongodb.org/manual/reference/command/update/#update-command-output
	 * http://docs.mongodb.org/manual/reference/command/delete/
	 * http://docs.mongodb.org/manual/reference/command/findAndModify/
	 */

	std::ostringstream ostr;
	try {
		if (r.exists("ok")) {
			const auto ok = extractInt64(r, "ok");
			if (ok != 1) {
				ostr << "Command failed: ok = " << ok << ". ";
			}
		}
		else if (expectOK) {
			ostr << "UNEXPECTED: Missing 'ok' in response.";
		}
		else {
			// Document that does not have embedded status response,
			// e.g.from
			// find cursor
			return;
		}

		// Find, aggregate commands
		Int64 code = -1;
		if (r.exists("code")) {
			code = extractInt64(r, "code");
		}
		if (r.exists("errmsg")) {
			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
			 << r.get<std::string>("errmsg");
		}
		if (r.exists("$err")) {
			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
			 << r.get<std::string>("$err");
		}

		// insert, update, delete commands
		if (r.exists("writeErrors")) {
			const auto ev = r.get<MongoDB::Array::Ptr>("writeErrors");
			for (size_t i = 0; i < ev->size(); ++i) {
				auto err = ev->get<MongoDB::Document::Ptr>(i);
				const auto index = extractInt64(*err, "index");
				const auto code = extractInt64(*err, "code");
				ostr << index << ": " << code << ": "
				 << err->get<std::string>("errmsg");
				if ((i + 1) < ev->size()) {
					ostr << std::endl;
				}
			}
		}
		if (r.exists("writeConcernError")) {
			const auto err =
			r.get<MongoDB::Document::Ptr>("writeConcernError");
			if (ostr.tellp() > 0) {
				ostr << std::endl;
			}
			const auto index = extractInt64(*err, "index");
			const auto code = extractInt64(*err, "code");
			ostr << index << ": " << code << ": "
			 << err->get<std::string>("errmsg");
		}
	}
	catch (const Exception &e) {
		std::cout << "Response error exception " << e.displayText()
			  << std::endl;
		throw;
	}

	if (ostr.tellp() > 0) {
		std::cout << "Error response from server: " << response.toString(2)
			  << std::endl;
		throw Exception("MongoDB: " + ostr.str());
	}
}

// variables & objects
bool Database::isPoolRunning(false);
size_t Database::poolCapacity(16);
size_t Database::poolPeakCapacity(256);
std::string Database::hostAddress("localhost");
std::string Database::port("27017");
std::vector<std::thread> Database::threadList;
Database::MongoDBConnectionFactoryPtr Database::g_connectionFactory;
Database::MongoDBConnectionPoolPtr Database::g_connectionPool;
Poco::MongoDB::Database Database::g_db("pocoTestDB");
