#include "database.h"
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
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

Database::ResponceType Database::saveBook(Book &inputBook)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Array::Ptr bookList(new MongoDB::Array());

		MongoDB::Document::Ptr bookObj(new MongoDB::Document());
		bookObj->add("author", inputBook.author);

		MongoDB::Document::Ptr idInc(new MongoDB::Document());
		MongoDB::Document::Ptr idInc1(new MongoDB::Document());
		idInc1->add("sec", 1);
		idInc->add("$inc", idInc1);
		bookObj->add("id", idInc);
		// bookObj->add("id", inputBook.id);
		bookObj->add("shabakNumber", inputBook.shabakNumber);
		bookObj->add("name", inputBook.name);
		bookObj->add("type", std::to_string(inputBook.type));
		bookObj->add("version", inputBook.version);

		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", inputBook.publishDateTime.day);
		publishDateTime->add("month", inputBook.publishDateTime.month);
		publishDateTime->add("year", inputBook.publishDateTime.year);
		publishDateTime->add("hour", inputBook.publishDateTime.hour);
		publishDateTime->add("minute", inputBook.publishDateTime.minute);
		publishDateTime->add("second", inputBook.publishDateTime.second);

		bookObj->add("publishDateTime", publishDateTime);

		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());
		lastEditDateTime->add("day", inputBook.lastEditDateTime.day);
		lastEditDateTime->add("month", inputBook.lastEditDateTime.month);
		lastEditDateTime->add("year", inputBook.lastEditDateTime.year);
		lastEditDateTime->add("hour", inputBook.lastEditDateTime.hour);
		lastEditDateTime->add("minute", inputBook.lastEditDateTime.minute);
		lastEditDateTime->add("second", inputBook.lastEditDateTime.second);

		bookObj->add("lastEditDateTime", lastEditDateTime);

		MongoDB::Array::Ptr bookTags(new MongoDB::Array());

		for (size_t i = 0; i < inputBook.tags.size(); i++) {
			bookTags->add(std::to_string(i), inputBook.tags.at(i));
		}

		bookObj->add("tags", bookTags);

		bookObj->add("sharedMode", std::to_string(inputBook.sharedMode));
		bookObj->add("seensCount", inputBook.seensCount);
		bookObj->add("likesCount", inputBook.likesCount);

		MongoDB::Array::Ptr likedUsersList(new MongoDB::Array());

		for (size_t i = 0; i < inputBook.likedUsers.size(); i++) {
			likedUsersList->add(std::to_string(i),
					inputBook.likedUsers.at(i));
		}

		bookObj->add("likedUsers", likedUsersList);

		MongoDB::Array::Ptr sharedWithList(new MongoDB::Array());

		for (size_t i = 0; i < inputBook.SharedWith.size(); i++) {
			sharedWithList->add(std::to_string(i),
					inputBook.SharedWith.at(i));
		}

		bookObj->add("SharedWith", sharedWithList);

		MongoDB::Array::Ptr bookCommentsArray(new MongoDB::Array());

		for (size_t i = 0; i < inputBook.comments.size(); i++) {
			MongoDB::Document::Ptr bookCommentObj(
			new MongoDB::Document());

			bookCommentObj->add("id", inputBook.comments.at(i).id);
			bookCommentObj->add("id", inputBook.comments.at(i).id);

			MongoDB::Document::Ptr publishDateTime(
			new MongoDB::Document());
			publishDateTime->add(
			"day", inputBook.comments.at(i).lastEditDateTime.day);
			publishDateTime->add(
			"month",
			inputBook.comments.at(i).lastEditDateTime.month);
			publishDateTime->add(
			"year", inputBook.comments.at(i).lastEditDateTime.year);
			publishDateTime->add(
			"hour", inputBook.comments.at(i).lastEditDateTime.hour);
			publishDateTime->add(
			"minute",
			inputBook.comments.at(i).lastEditDateTime.minute);
			publishDateTime->add(
			"second",
			inputBook.comments.at(i).lastEditDateTime.second);

			bookCommentObj->add("dateTime", publishDateTime);

			bookCommentObj->add("edited",
					inputBook.comments.at(i).edited);
			bookCommentObj->add("content",
					inputBook.comments.at(i).content);
			bookCommentsArray->add(std::to_string(i), bookCommentObj);
		}
		bookObj->add("comments", bookCommentsArray);

		MongoDB::Array::Ptr bookPartsArray(new MongoDB::Array());

		for (size_t i = 0; i < inputBook.parts.size(); i++) {
			MongoDB::Document::Ptr bookPartObj(new MongoDB::Document());

			bookPartObj->add("id", inputBook.parts.at(i).id);
			bookPartObj->add("name", inputBook.parts.at(i).name);

			MongoDB::Document::Ptr publishDateTime(
			new MongoDB::Document());
			publishDateTime->add(
			"day", inputBook.parts.at(i).publishDateTime.day);
			publishDateTime->add(
			"month", inputBook.parts.at(i).publishDateTime.month);
			publishDateTime->add(
			"year", inputBook.parts.at(i).publishDateTime.year);
			publishDateTime->add(
			"hour", inputBook.parts.at(i).publishDateTime.hour);
			publishDateTime->add(
			"minute", inputBook.parts.at(i).publishDateTime.minute);
			publishDateTime->add(
			"second", inputBook.parts.at(i).publishDateTime.second);

			bookPartObj->add("publishDateTime", publishDateTime);

			MongoDB::Document::Ptr lastEditDateTime(
			new MongoDB::Document());
			lastEditDateTime->add(
			"day", inputBook.parts.at(i).lastEditDateTime.day);
			lastEditDateTime->add(
			"month", inputBook.parts.at(i).lastEditDateTime.month);
			lastEditDateTime->add(
			"year", inputBook.parts.at(i).lastEditDateTime.year);
			lastEditDateTime->add(
			"hour", inputBook.parts.at(i).lastEditDateTime.hour);
			lastEditDateTime->add(
			"minute",
			inputBook.parts.at(i).lastEditDateTime.minute);
			lastEditDateTime->add(
			"second",
			inputBook.parts.at(i).lastEditDateTime.second);

			bookPartObj->add("lastEditDateTime", lastEditDateTime);

			bookPartObj->add("version", inputBook.parts.at(i).version);
			bookPartObj->add("seensCount",
					 inputBook.parts.at(i).seensCount);
			bookPartObj->add("likesCount",
					 inputBook.parts.at(i).likesCount);
			bookPartObj->add("content", inputBook.parts.at(i).content);

			MongoDB::Array::Ptr partCommentsArray(new MongoDB::Array());

			for (size_t commentIndex = 0;
				 commentIndex <
				 inputBook.parts.at(i).comments.size();
				 commentIndex++) {

				MongoDB::Document::Ptr partCommentsObj(
				new MongoDB::Document());

				partCommentsObj->add("id",
						 inputBook.parts.at(i)
							 .comments.at(commentIndex)
							 .id);

				MongoDB::Document::Ptr publishDateTime(
				new MongoDB::Document());
				publishDateTime->add("day",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.day);
				publishDateTime->add("month",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.month);
				publishDateTime->add("year",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.year);
				publishDateTime->add("hour",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.hour);
				publishDateTime->add("minute",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.minute);
				publishDateTime->add("second",
						 inputBook.parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.second);

				partCommentsObj->add("dateTime", publishDateTime);

				partCommentsObj->add("edited",
						 inputBook.parts.at(i)
							 .comments.at(commentIndex)
							 .edited);
				bookPartObj->add("content",
						 inputBook.parts.at(i)
						 .comments.at(commentIndex)
						 .content);

				partCommentsArray->add(std::to_string(commentIndex),
						   partCommentsObj);
			}

			bookPartObj->add("comments", partCommentsArray);
			bookPartsArray->add(std::to_string(i), bookPartObj);
		}

		bookObj->add("parts", bookPartsArray);

		MongoDB::Array::Ptr booksList(new MongoDB::Array());
		booksList->add(std::to_string(0), bookObj);
		auto insert = g_db.createCommand();
		insert->selector()
		.add("insert", "Books")
		.add("documents", booksList);

		MongoDB::ResponseMessage response;
		c->sendRequest(*insert, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			// std::cout << i->toString(2) << std::endl;
		}
		return Database::ResponceType::OK;
	}
	catch (const Exception &e) {
		// TODO find a better way for returning error message
		//		std::cerr << "INSERT "
		//			  << " failed: " << e.displayText() <<
		// std::endl;
		return Database::ResponceType::ERROR;
	}
}

std::string Database::getBooks(std::string &author)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		auto queryPtr = g_db.createQueryRequest("Books");
		queryPtr->selector().add("author", author);

		// limit return numbers
		queryPtr->setNumberToReturn(1);

		// std::cout << "QUERY : " << author << std::endl;
		MongoDB::ResponseMessage response;
		c->sendRequest(*queryPtr, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc, false);

		return response.documents()[0]->toString(2);
	}
	catch (const Exception &e) {
		std::cerr << "QUERY " << author << "failed: " << e.displayText()
			  << std::endl;
	}
	return "";
}

Database::ResponceType Database::insertPart(BookPart &inputBookPart,
						std::string &bookName)
{
	// TODO
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookPartObj(new MongoDB::Document());

		bookPartObj->add("id", inputBookPart.id);
		bookPartObj->add("name", inputBookPart.name);

		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", inputBookPart.publishDateTime.day);
		publishDateTime->add("month", inputBookPart.publishDateTime.month);
		publishDateTime->add("year", inputBookPart.publishDateTime.year);
		publishDateTime->add("hour", inputBookPart.publishDateTime.hour);
		publishDateTime->add("minute",
				 inputBookPart.publishDateTime.minute);
		publishDateTime->add("second",
				 inputBookPart.publishDateTime.second);

		bookPartObj->add("publishDateTime", publishDateTime);

		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());
		lastEditDateTime->add("day", inputBookPart.lastEditDateTime.day);
		lastEditDateTime->add("month",
				  inputBookPart.lastEditDateTime.month);
		lastEditDateTime->add("year", inputBookPart.lastEditDateTime.year);
		lastEditDateTime->add("hour", inputBookPart.lastEditDateTime.hour);
		lastEditDateTime->add("minute",
				  inputBookPart.lastEditDateTime.minute);
		lastEditDateTime->add("second",
				  inputBookPart.lastEditDateTime.second);

		bookPartObj->add("lastEditDateTime", lastEditDateTime);

		bookPartObj->add("version", inputBookPart.version);
		bookPartObj->add("seensCount", inputBookPart.seensCount);
		bookPartObj->add("likesCount", inputBookPart.likesCount);
		bookPartObj->add("content", inputBookPart.content);

		MongoDB::Array::Ptr partCommentsArray(new MongoDB::Array());

		for (size_t commentIndex = 0;
			 commentIndex < inputBookPart.comments.size();
			 commentIndex++) {

			MongoDB::Document::Ptr partCommentsObj(
			new MongoDB::Document());

			partCommentsObj->add(
			"id", inputBookPart.comments.at(commentIndex).id);

			MongoDB::Document::Ptr publishDateTime(
			new MongoDB::Document());
			publishDateTime->add("day",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.day);
			publishDateTime->add("month",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.month);
			publishDateTime->add("year",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.year);
			publishDateTime->add("hour",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.hour);
			publishDateTime->add("minute",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.minute);
			publishDateTime->add("second",
					 inputBookPart.comments.at(commentIndex)
						 .lastEditDateTime.second);

			partCommentsObj->add("dateTime", publishDateTime);

			partCommentsObj->add(
			"edited",
			inputBookPart.comments.at(commentIndex).edited);
			bookPartObj->add(
			"content",
			inputBookPart.comments.at(commentIndex).content);

			partCommentsArray->add(std::to_string(commentIndex),
					   partCommentsObj);
		}

		bookPartObj->add("comments", partCommentsArray);

		// MongoDB::Array::Ptr partCommentsArray(new MongoDB::Array());
		MongoDB::Document::Ptr testDoc(new MongoDB::Document());
		testDoc->add("parts", bookPartObj);
		document->add("$addToSet", testDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("author", bookName);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			// std::cout << i->toString(2) << std::endl;
		}
		return ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT " << bookName << " failed: " << e.displayText()
			  << std::endl;
		return ResponceType::ERROR;
	}
}

Database::ResponceType Database::insertBookComment(Comment &comment,
						   std::string &bookName)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("id", comment.id);

		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", comment.lastEditDateTime.day);
		publishDateTime->add("month", comment.lastEditDateTime.month);
		publishDateTime->add("year", comment.lastEditDateTime.year);
		publishDateTime->add("hour", comment.lastEditDateTime.hour);
		publishDateTime->add("minute", comment.lastEditDateTime.minute);
		publishDateTime->add("second", comment.lastEditDateTime.second);

		bookCommentObj->add("dateTime", publishDateTime);

		bookCommentObj->add("edited", comment.edited);
		bookCommentObj->add("content", comment.content);

		// MongoDB::Array::Ptr partCommentsArray(new MongoDB::Array());
		MongoDB::Document::Ptr testDoc(new MongoDB::Document());
		testDoc->add("comments", bookCommentObj);
		document->add("$addToSet", testDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("author", bookName);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			// std::cout << i->toString(2) << std::endl;
		}
		return ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT " << bookName << " failed: " << e.displayText()
			  << std::endl;
		return ResponceType::ERROR;
	}
}

Database::ResponceType
Database::insertBookPartComment(Database::Comment &comment,
				std::string &bookName, std::string &author,
				int &partindex)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("id", comment.id);

		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", comment.lastEditDateTime.day);
		publishDateTime->add("month", comment.lastEditDateTime.month);
		publishDateTime->add("year", comment.lastEditDateTime.year);
		publishDateTime->add("hour", comment.lastEditDateTime.hour);
		publishDateTime->add("minute", comment.lastEditDateTime.minute);
		publishDateTime->add("second", comment.lastEditDateTime.second);

		bookCommentObj->add("dateTime", publishDateTime);

		bookCommentObj->add("edited", comment.edited);
		bookCommentObj->add("content", comment.content);

		MongoDB::Document::Ptr testDoc(new MongoDB::Document());
		testDoc->add("parts." + std::to_string(partindex) + ".comments",
			 bookCommentObj);
		document->add("$addToSet", testDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		MongoDB::Document::Ptr subQuery(new MongoDB::Document());
		MongoDB::Document::Ptr matchElementQuery(new MongoDB::Document());

		query->add("author", author);
		query->add("name", bookName);
		std::clog << query->toString() << std::endl;
		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			std::cout << i->toString(2) << std::endl;
		}
		return ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT " << bookName << " failed: " << e.displayText()
			  << std::endl;
		return ResponceType::ERROR;
	}
}

Database::ResponceType Database::saveUser(Database::User &user)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Document::Ptr document(new MongoDB::Document());
		document->add("userName", user.userName);
		document->add("name", user.name);
		document->add("family", user.family);
		document->add("sex", user.sex);

		MongoDB::Array::Ptr favoriteTags(new MongoDB::Array());
		for (size_t i = 0; i < user.favoriteTags.size(); i++) {
			favoriteTags->add(std::to_string(i),
					  user.favoriteTags.at(i));
		}
		document->add("favoriteTags", favoriteTags);

		document->add("email", user.email);
		document->add("phoneNumber", user.phoneNumber);
		document->add("studyDuration", user.studyDuration);

		MongoDB::Document::Ptr dateDocument(new MongoDB::Document());
		dateDocument->add("day", user.berthday.day);
		dateDocument->add("month", user.berthday.month);
		dateDocument->add("year", user.berthday.year);
		dateDocument->add("hour", user.berthday.hour);
		dateDocument->add("minute", user.berthday.minute);
		dateDocument->add("second", user.berthday.second);

		document->add("berthday", dateDocument);

		MongoDB::Array::Ptr documents(new MongoDB::Array());
		documents->add(std::to_string(0), document);

		auto insert = g_db.createCommand();
		insert->selector()
		.add("insert", "Users")
		.add("documents", documents);

		MongoDB::ResponseMessage response;
		c->sendRequest(*insert, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			std::cout << i->toString(2) << std::endl;
		}
		return ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT " << user.userName
			  << " failed: " << e.displayText() << std::endl;
		return ResponceType::ERROR;
	}
}

std::string Database::getUser(std::string &userName)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		auto queryPtr = g_db.createQueryRequest("Users");
		queryPtr->selector().add("userName", userName);

		// limit return numbers
		queryPtr->setNumberToReturn(1);

		std::cout << "QUERY : " << userName << std::endl;
		MongoDB::ResponseMessage response;
		c->sendRequest(*queryPtr, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc, false);

		return response.documents()[0]->toString(2);
	}
	catch (const Exception &e) {
		std::cerr << "QUERY " << userName << "failed: " << e.displayText()
			  << std::endl;
	}
	return "";
}

Database::ResponceType Database::updateUser(Database::User &user)
{
	try {
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		MongoDB::Document::Ptr document(new MongoDB::Document());
		document->add("userName", user.userName);
		document->add("name", user.name);
		document->add("family", user.family);
		document->add("sex", user.sex);

		MongoDB::Array::Ptr favoriteTags(new MongoDB::Array());
		for (size_t i = 0; i < user.favoriteTags.size(); i++) {
			favoriteTags->add(std::to_string(i),
					  user.favoriteTags.at(i));
		}
		document->add("favoriteTags", favoriteTags);

		document->add("email", user.email);
		document->add("phoneNumber", user.phoneNumber);
		document->add("studyDuration", user.studyDuration);

		MongoDB::Document::Ptr dateDocument(new MongoDB::Document());
		dateDocument->add("day", user.berthday.day);
		dateDocument->add("month", user.berthday.month);
		dateDocument->add("year", user.berthday.year);
		dateDocument->add("hour", user.berthday.hour);
		dateDocument->add("minute", user.berthday.minute);
		dateDocument->add("second", user.berthday.second);

		document->add("berthday", dateDocument);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("userName", user.userName);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Users")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			std::cout << i->toString(2) << std::endl;
		}
		return ResponceType::OK;
	}
	catch (const Exception &e) {
		std::cerr << "INSERT " << user.userName
			  << " failed: " << e.displayText() << std::endl;
		return ResponceType::ERROR;
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
