#include "database.h"

#include <Poco/DigestStream.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/MD5Engine.h>

#include <ctime>

// connect need to be called first
void Database::connect()
{
	// initilize database parameters
	std::string connectionString{""};
	connectionString += hostAddress + std::string(":") + port;
	std::clog << "connected to : " << connectionString << std::endl;
	g_connectionFactory.reset(new MongoDBConnectionFactory(connectionString));
	g_connectionPool.reset(new MongoDBConnectionPool(
	*g_connectionFactory, poolCapacity, poolPeakCapacity));

	// create unique indexes for collections
	MongoDB::Document::Ptr uniqeBookName(new MongoDB::Document());
	uniqeBookName->add("id", 1);

	MongoDB::Document::Ptr uniqeUserPhoneNumber(new MongoDB::Document());
	uniqeUserPhoneNumber->add("phoneNumber", 1);

	MongoDB::Document::Ptr uniqeUserEmail(new MongoDB::Document());
	uniqeUserEmail->add("email", 1);

	MongoDB::Document::Ptr uniqeUserUserNumber(new MongoDB::Document());
	uniqeUserUserNumber->add("username", 1);

	auto conn = takeConnection();
	auto c = static_cast<MongoDB::Connection::Ptr>(conn);

	g_db.ensureIndex(*c, "Books", "id", uniqeBookName, true);
	g_db.ensureIndex(*c, "Users", "phoneNumber", uniqeUserPhoneNumber, true);
	g_db.ensureIndex(*c, "Users", "email", uniqeUserEmail, true);
	g_db.ensureIndex(*c, "Users", "username", uniqeUserUserNumber, true);
}

std::string
Database::saveBooks(std::vector<std::shared_ptr<Database::Book>> &inputBooks)
{
	try {

		// get connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create Array of Documents
		MongoDB::Array::Ptr booksList(new MongoDB::Array());

		// create book documents
		for (size_t bookIndex = 0; bookIndex < inputBooks.size();
			 bookIndex++) {
			MongoDB::Document::Ptr bookObj(new MongoDB::Document());

			bookObj->add("author", inputBooks[bookIndex]->author);
			bookObj->add("name", inputBooks[bookIndex]->name);
			bookObj->add("shabakNumber",
				 inputBooks[bookIndex]->shabakNumber);
			bookObj->add("type", inputBooks[bookIndex]->type);
			bookObj->add("version", inputBooks[bookIndex]->version);

			// get cueent date time
			auto dateTime = getDateTime();

			MongoDB::Document::Ptr publishDateTime(
			new MongoDB::Document());

			publishDateTime->add("year", dateTime->year);
			publishDateTime->add("month", dateTime->month);
			publishDateTime->add("day", dateTime->day);
			publishDateTime->add("hour", dateTime->hour);
			publishDateTime->add("minute", dateTime->minute);
			publishDateTime->add("second", dateTime->second);

			bookObj->add("publishDateTime", publishDateTime);

			MongoDB::Document::Ptr lastEditDateTime(
			new MongoDB::Document());

			lastEditDateTime->add("year", dateTime->year);
			lastEditDateTime->add("month", dateTime->month);
			lastEditDateTime->add("day", dateTime->day);
			lastEditDateTime->add("hour", dateTime->hour);
			lastEditDateTime->add("minute", dateTime->minute);
			lastEditDateTime->add("second", dateTime->second);

			bookObj->add("lastEditDateTime", lastEditDateTime);

			MongoDB::Array::Ptr bookTags(new MongoDB::Array());

			for (size_t i = 0;
				 i < inputBooks[bookIndex]->tags.size(); i++) {
				bookTags->add(std::to_string(i),
					  inputBooks[bookIndex]->tags.at(i));
			}

			bookObj->add("tags", bookTags);
			bookObj->add(
			"sharedMode",
			std::to_string(inputBooks[bookIndex]->sharedMode));
			bookObj->add("seensCount",
				 inputBooks[bookIndex]->seensCount);

			MongoDB::Array::Ptr sharedWithList(new MongoDB::Array());

			for (size_t i = 0;
				 i < inputBooks[bookIndex]->SharedWith.size();
				 i++) {
				sharedWithList->add(
				std::to_string(i),
				inputBooks[bookIndex]->SharedWith.at(i));
			}

			bookObj->add("sharedWith", sharedWithList);

			MongoDB::Array::Ptr bookCommentsArray(new MongoDB::Array());

			for (size_t i = 0;
				 i < inputBooks[bookIndex]->comments.size(); i++) {
				MongoDB::Document::Ptr bookCommentObj(
				new MongoDB::Document());

				bookCommentObj->add(
				"id", inputBooks[bookIndex]->comments.at(i).id);
				bookCommentObj->add("repliedTo",
						inputBooks[bookIndex]
							->comments.at(i)
							.repliedTo);

				MongoDB::Document::Ptr publishDateTime(
				new MongoDB::Document());
				publishDateTime->add("day",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.day);
				publishDateTime->add("month",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.month);
				publishDateTime->add("year",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.year);
				publishDateTime->add("hour",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.hour);
				publishDateTime->add("minute",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.minute);
				publishDateTime->add("second",
						 inputBooks[bookIndex]
							 ->comments.at(i)
							 .lastEditDateTime.second);

				bookCommentObj->add("dateTime", publishDateTime);

				bookCommentObj->add(
				"edited",
				inputBooks[bookIndex]->comments.at(i).edited);
				bookCommentObj->add(
				"content",
				inputBooks[bookIndex]->comments.at(i).content);
				bookCommentsArray->add(std::to_string(i),
						   bookCommentObj);
			}
			bookObj->add("comments", bookCommentsArray);

			MongoDB::Array::Ptr bookPartsArray(new MongoDB::Array());

			for (size_t i = 0;
				 i < inputBooks[bookIndex]->parts.size(); i++) {
				MongoDB::Document::Ptr bookPartObj(
				new MongoDB::Document());

				bookPartObj->add(
				"id", inputBooks[bookIndex]->parts.at(i).id);
				bookPartObj->add(
				"name",
				inputBooks[bookIndex]->parts.at(i).name);

				MongoDB::Document::Ptr publishDateTime(
				new MongoDB::Document());
				publishDateTime->add("day",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.day);
				publishDateTime->add("month",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.month);
				publishDateTime->add("year",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.year);
				publishDateTime->add("hour",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.hour);
				publishDateTime->add("minute",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.minute);
				publishDateTime->add("second",
						 inputBooks[bookIndex]
							 ->parts.at(i)
							 .publishDateTime.second);

				bookPartObj->add("publishDateTime",
						 publishDateTime);

				MongoDB::Document::Ptr lastEditDateTime(
				new MongoDB::Document());
				lastEditDateTime->add("day",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.day);
				lastEditDateTime->add("month",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.month);
				lastEditDateTime->add("year",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.year);
				lastEditDateTime->add("hour",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.hour);
				lastEditDateTime->add("minute",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.minute);
				lastEditDateTime->add("second",
						  inputBooks[bookIndex]
							  ->parts.at(i)
							  .lastEditDateTime.second);

				bookPartObj->add("lastEditDateTime",
						 lastEditDateTime);

				bookPartObj->add(
				"version",
				inputBooks[bookIndex]->parts.at(i).version);
				bookPartObj->add(
				"seensCount",
				inputBooks[bookIndex]->parts.at(i).seensCount);
				bookPartObj->add(
				"likesCount",
				inputBooks[bookIndex]->parts.at(i).likesCount);
				bookPartObj->add(
				"content",
				inputBooks[bookIndex]->parts.at(i).content);

				MongoDB::Array::Ptr partCommentsArray(
				new MongoDB::Array());

				for (size_t commentIndex = 0;
					 commentIndex < inputBooks[bookIndex]
							->parts.at(i)
							.comments.size();
					 commentIndex++) {

					MongoDB::Document::Ptr partCommentsObj(
					new MongoDB::Document());

					partCommentsObj->add(
					"id", inputBooks[bookIndex]
						  ->parts.at(i)
						  .comments.at(commentIndex)
						  .id);

					MongoDB::Document::Ptr publishDateTime(
					new MongoDB::Document());

					publishDateTime->add(
					"day", inputBooks[bookIndex]
						   ->parts.at(i)
						   .comments.at(i)
						   .lastEditDateTime.day);
					publishDateTime->add(
					"month", inputBooks[bookIndex]
							 ->parts.at(i)
							 .comments.at(i)
							 .lastEditDateTime.month);
					publishDateTime->add(
					"year", inputBooks[bookIndex]
							->parts.at(i)
							.comments.at(i)
							.lastEditDateTime.year);
					publishDateTime->add(
					"hour", inputBooks[bookIndex]
							->parts.at(i)
							.comments.at(i)
							.lastEditDateTime.hour);
					publishDateTime->add(
					"minute", inputBooks[bookIndex]
							  ->parts.at(i)
							  .comments.at(i)
							  .lastEditDateTime.minute);
					publishDateTime->add(
					"second", inputBooks[bookIndex]
							  ->parts.at(i)
							  .comments.at(i)
							  .lastEditDateTime.second);

					partCommentsObj->add("dateTime",
							 publishDateTime);

					partCommentsObj->add(
					"edited", inputBooks[bookIndex]
							  ->parts.at(i)
							  .comments.at(commentIndex)
							  .edited);
					bookPartObj->add(
					"content",
					inputBooks[bookIndex]
						->parts.at(i)
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

			booksList->add(std::to_string(bookIndex), bookObj);
		}

		// create comment
		auto insert = g_db.createCommand();

		// set insert comment
		insert->selector()
		.add("insert", "Books")
		.add("documents", booksList);

		MongoDB::ResponseMessage response;

		// send request to server
		c->sendRequest(*insert, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT "
		//			  << " failed: " << e.displayText() <<
		// std::endl;
	}
}

std::string Database::saveUser(Database::User &user)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// creating user document
		MongoDB::Document::Ptr document(new MongoDB::Document());
		document->add("username", user.username);
		document->add("name", user.name);
		document->add("family", user.family);
		document->add("sex", user.sex);
		document->add("password", user.password);

		MongoDB::Array::Ptr favoriteTags(new MongoDB::Array());
		for (size_t i = 0; i < user.favoriteTags.size(); i++) {
			favoriteTags->add(std::to_string(i),
					  user.favoriteTags.at(i));
		}
		document->add("favoriteTags", favoriteTags);

		document->add("email", user.email);
		document->add("phoneNumber", user.phoneNumber);
		document->add("studyDuration", user.studyDuration);

		MongoDB::Document::Ptr berthDocument(new MongoDB::Document());
		berthDocument->add("day", user.berthday.day);
		berthDocument->add("month", user.berthday.month);
		berthDocument->add("year", user.berthday.year);
		berthDocument->add("hour", user.berthday.hour);
		berthDocument->add("minute", user.berthday.minute);
		berthDocument->add("second", user.berthday.second);

		document->add("berthday", berthDocument);

		// get cueent date time
		auto dateTime = getDateTime();

		MongoDB::Document::Ptr signupDateTime(new MongoDB::Document());

		signupDateTime->add("year", dateTime->year);
		signupDateTime->add("month", dateTime->month);
		signupDateTime->add("day", dateTime->day);
		signupDateTime->add("hour", dateTime->hour);
		signupDateTime->add("minute", dateTime->minute);
		signupDateTime->add("second", dateTime->second);

		document->add("signupDateTime", signupDateTime);

		MongoDB::Array::Ptr documents(new MongoDB::Array());
		documents->add(std::to_string(0), document);

		// create command
		auto insert = g_db.createCommand();

		// make command insert to Users collection
		insert->selector()
		.add("insert", "Users")
		.add("documents", documents);

		MongoDB::ResponseMessage response;

		// send request to server
		c->sendRequest(*insert, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		// std::clog << response.documents()[0]->toString() <<
		// std::endl;

		return response.documents()[0]->toString();
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << user.username
		//			  << " failed: " << e.displayText() <<
		// std::endl;
		return e.displayText();
	}
}

std::string Database::getBooks(std::string &author)
{
	try {

		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		auto queryPtr = g_db.createQueryRequest("Books");
		queryPtr->selector().add("author", author);

		// limit return numbers
		queryPtr->setNumberToReturn(1);

		MongoDB::ResponseMessage response;

		// send request to server
		c->sendRequest(*queryPtr, response);
		if (response.documents().empty()) {
			std::clog << "is empty" << std::endl;
			return "";
		}
		auto doc = *(response.documents()[0]);

		verifyResponse(doc, false);

		return response.documents()[0]->toString(2);
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "QUERY " << author << "failed: " << e.displayText()
			  << std::endl;
	}
	return "";
}

std::string Database::getUser(std::string &username)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		auto queryPtr = g_db.createQueryRequest("Users");
		queryPtr->selector().add("username", username);

		// limit return numbers
		queryPtr->setNumberToReturn(1);

		std::cout << "QUERY : " << username << std::endl;
		MongoDB::ResponseMessage response;

		// send request to server
		c->sendRequest(*queryPtr, response);

		// return empty string if document is empty
		if (response.documents().empty()) {
			return "";
		}
		auto doc = *(response.documents()[0]);
		verifyResponse(doc, false);

		return response.documents()[0]->toString(2);
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "QUERY " << username << "failed: " << e.displayText()
			  << std::endl;
	}
	return "";
}

std::string Database::editBookInfo(std::shared_ptr<Database::Book> &inputBook)
{
	try {
		// take connnection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create book info document
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookObj(new MongoDB::Document());
		bookObj->add("shabakNumber", inputBook->shabakNumber);
		bookObj->add("name", inputBook->name);
		bookObj->add("type", inputBook->type);
		bookObj->add("version", inputBook->version + 1);

		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());

		auto dateTime = getDateTime();
		lastEditDateTime->add("year", dateTime->year);
		lastEditDateTime->add("month", dateTime->month);
		lastEditDateTime->add("day", dateTime->day);
		lastEditDateTime->add("hour", dateTime->hour);
		lastEditDateTime->add("minute", dateTime->minute);
		lastEditDateTime->add("second", dateTime->second);

		bookObj->add("lastEditDateTime", lastEditDateTime);

		MongoDB::Array::Ptr bookTags(new MongoDB::Array());

		for (size_t i = 0; i < inputBook->tags.size(); i++) {
			bookTags->add(std::to_string(i), inputBook->tags.at(i));
		}

		bookObj->add("tags", bookTags);

		bookObj->add("sharedMode", std::to_string(inputBook->sharedMode));
		bookObj->add("seensCount", inputBook->seensCount);

		document->add("$set", bookObj);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", inputBook->id);

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
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << inputBook->name
			  << " failed: " << e.displayText() << std::endl;
	}
}

std::string Database::editBookParts(std::vector<Database::BookPart> &bookParts,
					std::string &bookId)
{
	try {
		// take connection
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// an array to put book parts into that
		MongoDB::Array::Ptr updates(new MongoDB::Array());
		int i = 0;
		for (auto &part : bookParts) {
			MongoDB::Document::Ptr document(new MongoDB::Document());

			MongoDB::Document::Ptr bookPartObj(new MongoDB::Document());
			bookPartObj->add("version", part.version + 1);
			bookPartObj->add("seensCount", part.seensCount);
			bookPartObj->add("likesCount", part.likesCount);
			bookPartObj->add("name", part.name);
			bookPartObj->add("content", part.content);

			auto dateTime = getDateTime();

			MongoDB::Document::Ptr lastEditDateTime(
			new MongoDB::Document());
			lastEditDateTime->add("year", dateTime->year);
			lastEditDateTime->add("month", dateTime->month);
			lastEditDateTime->add("day", dateTime->day);
			lastEditDateTime->add("hour", dateTime->hour);
			lastEditDateTime->add("minute", dateTime->minute);
			lastEditDateTime->add("second", dateTime->second);

			bookPartObj->add("lastEditDateTime", lastEditDateTime);

			MongoDB::Document::Ptr updateValuesDoc(
			new MongoDB::Document());
			updateValuesDoc->add("parts.id", part.id);
			document->add("$set", updateValuesDoc);

			MongoDB::Document::Ptr query(new MongoDB::Document());
			query->add("_id", bookId);

			MongoDB::Document::Ptr update(new MongoDB::Document());
			update->add("q", query).add("limit", 1);
			update->add("u", document);

			updates->add(std::to_string(i), update);
			i++;
		}
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::editBookComment(Database::Comment &comment,
					  std::string &bookId)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document struct
		MongoDB::Array::Ptr updates(new MongoDB::Array());

		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("content", comment.content);
		bookCommentObj->add("edited", true);
		bookCommentObj->add("repliedTo", comment.repliedTo);

		auto dateTime = getDateTime();
		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());
		lastEditDateTime->add("year", dateTime->year);
		lastEditDateTime->add("month", dateTime->month);
		lastEditDateTime->add("day", dateTime->day);
		lastEditDateTime->add("hour", dateTime->hour);
		lastEditDateTime->add("minute", dateTime->minute);
		lastEditDateTime->add("second", dateTime->second);

		bookCommentObj->add("lastEditDateTime", lastEditDateTime);

		//*******************md5

		std::string keyMD5 =
		comment.username + std::to_string(dateTime->year) +
		std::to_string(dateTime->month) +
		std::to_string(dateTime->day) + std::to_string(dateTime->hour) +
		std::to_string(dateTime->minute) +
		std::to_string(dateTime->second);
		Poco::MD5Engine md5;
		Poco::DigestOutputStream outstr(md5);
		outstr << keyMD5;
		outstr.flush(); // to pass everything to the digest engine
		const Poco::DigestEngine::Digest &digest = md5.digest();
		std::string md5string = Poco::DigestEngine::digestToHex(digest);

		//*******************md5 end
		bookCommentObj->add("id", md5string);

		MongoDB::Document::Ptr updateValueDoc(new MongoDB::Document());
		updateValueDoc->add("comments", bookCommentObj);
		document->add("$set", updateValueDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		updates->add(std::to_string(0), update);

		// create command
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::editBookPartComment(Database::Comment &comment,
					  std::string &bookId, int &partindex)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document
		MongoDB::Array::Ptr updates(new MongoDB::Array());

		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookPartCommentObj(new MongoDB::Document());

		bookPartCommentObj->add("content", comment.content);
		bookPartCommentObj->add("edited", true);
		bookPartCommentObj->add("username", comment.username);
		bookPartCommentObj->add("id", comment.id);
		bookPartCommentObj->add("repliedTo", comment.repliedTo);

		auto dateTime = getDateTime();
		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());
		lastEditDateTime->add("day", dateTime->day);
		lastEditDateTime->add("month", dateTime->month);
		lastEditDateTime->add("year", dateTime->year);
		lastEditDateTime->add("hour", dateTime->hour);
		lastEditDateTime->add("minute", dateTime->minute);
		lastEditDateTime->add("second", dateTime->second);

		bookPartCommentObj->add("lastEditDateTime", lastEditDateTime);

		MongoDB::Document::Ptr updateValueDoc(new MongoDB::Document());
		updateValueDoc->add("parts." + std::to_string(partindex) +
					".comments." + std::to_string(partindex),
				bookPartCommentObj);

		document->add("$set", updateValueDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		updates->add(std::to_string(0), update);

		// create command
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::updateUser(Database::User &user)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());
		document->add("username", user.username);
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
		query->add("_id", user.id);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create update command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Users")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << user.username
			  << " failed: " << e.displayText() << std::endl;
	}
}

std::string Database::deleteBook(std::string &bookId)
{

	try {
		// take connection
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create query fro finding book
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr del(new MongoDB::Document());
		del->add("q", query).add("limit", 1);

		MongoDB::Array::Ptr deletes(new MongoDB::Array());
		deletes->add(std::to_string(0), del);

		// create command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("delete", "Books")
		.add("deletes", deletes);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "REMOVE " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::deleteBookPart(std::string &bookId, int &partIndex)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());
		MongoDB::Document::Ptr bookPartObj(new MongoDB::Document());
		bookPartObj->add("id", partIndex);
		MongoDB::Document::Ptr queryPartDoc(new MongoDB::Document());
		queryPartDoc->add("parts", bookPartObj);
		document->add("$pull", queryPartDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create update command
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::deleteBookComment(std::string &bookId,

					int &commentIndex)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document fro request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("id", commentIndex);

		MongoDB::Document::Ptr commentQueryDoc(new MongoDB::Document());
		commentQueryDoc->add("comments", bookCommentObj);

		document->add("$pull", commentQueryDoc);

		// create query
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// sendrequest
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::deleteBookPartComment(std::string &bookId, int &partIndex,
						int &commentIndex)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("id", commentIndex);

		// create find query
		MongoDB::Document::Ptr partDoc(new MongoDB::Document());
		partDoc->add("parts." + std::to_string(partIndex) + ".comments",
			 bookCommentObj);

		// put pull doc see
		// https://docs.mongodb.com/manual/reference/operator/update/pull/
		document->add("$pull", partDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create update command for delete book part comment
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::deleteLikedUser(std::string &bookId, std::string &likeId)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());
		MongoDB::Document::Ptr queryUserDoc(new MongoDB::Document());
		MongoDB::Document::Ptr like(new MongoDB::Document());

		like->add("id", likeId);
		queryUserDoc->add("likedUsers", like);
		document->add("$pull", queryUserDoc);

		// create find quert doc
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::deleteSharedWithUsers(std::string &bookId,
						std::vector<std::string> &usernames)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());
		MongoDB::Document::Ptr queryUserDoc(new MongoDB::Document());
		MongoDB::Array::Ptr usersArray(new MongoDB::Array());

		for (size_t i = 0; i < usernames.size(); i++) {

			usersArray->add(std::to_string(i), usernames[i]);
		}

		queryUserDoc->add("sharedWith", usersArray);
		document->add("$pullAll", queryUserDoc);

		// create find query doc
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create commend
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		std::cerr << "INSERT " << bookId << " failed: " << e.displayText()
			  << std::endl;
	}
}

std::string Database::insertPart(BookPart &inputBookPart, std::string &bookId)
{
	try {
		// take connection
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// ceate document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookPartObj(new MongoDB::Document());

		bookPartObj->add("name", inputBookPart.name);

		// getting current time
		auto dateTime = getDateTime();
		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", dateTime->day);
		publishDateTime->add("month", dateTime->month);
		publishDateTime->add("year", dateTime->year);
		publishDateTime->add("hour", dateTime->hour);
		publishDateTime->add("minute", dateTime->minute);
		publishDateTime->add("second",
				 inputBookPart.publishDateTime.second);

		bookPartObj->add("publishDateTime", publishDateTime);

		//*******************md5

		std::string keyMD5 = inputBookPart.name + inputBookPart.id +
				 std::to_string(dateTime->year) +
				 std::to_string(dateTime->month) +
				 std::to_string(dateTime->day) +
				 std::to_string(dateTime->hour) +
				 std::to_string(dateTime->minute) +
				 std::to_string(dateTime->second);
		Poco::MD5Engine md5;
		Poco::DigestOutputStream outstr(md5);
		outstr << keyMD5;
		outstr.flush(); // to pass everything to the digest engine
		const Poco::DigestEngine::Digest &digest = md5.digest();
		std::string md5string = Poco::DigestEngine::digestToHex(digest);

		//*******************md5 end
		bookPartObj->add("id", md5string);

		MongoDB::Document::Ptr lastEditDateTime(new MongoDB::Document());
		lastEditDateTime->add("day", dateTime->day);
		lastEditDateTime->add("month", dateTime->month);
		lastEditDateTime->add("year", dateTime->year);
		lastEditDateTime->add("hour", dateTime->hour);
		lastEditDateTime->add("minute", dateTime->minute);
		lastEditDateTime->add("second", dateTime->second);

		bookPartObj->add("lastEditDateTime", lastEditDateTime);

		bookPartObj->add("version", inputBookPart.version);
		bookPartObj->add("seensCount", inputBookPart.seensCount);
		bookPartObj->add("content", inputBookPart.content);

		MongoDB::Array::Ptr partCommentsArray(new MongoDB::Array());

		MongoDB::Document::Ptr queryPartDoc(new MongoDB::Document());
		queryPartDoc->add("parts", bookPartObj);
		document->add("$addToSet", queryPartDoc);

		// create find query doc
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto updateCmd = g_db.createCommand();
		updateCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*updateCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::insertBookComment(Comment &comment, std::string &bookId)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("username", comment.username);
		bookCommentObj->add("repliedTo", comment.repliedTo);
		// get cueent date time
		auto dateTime = getDateTime();

		//*******************md5

		std::string keyMD5 = comment.username + comment.content +
				 std::to_string(dateTime->year) +
				 std::to_string(dateTime->month) +
				 std::to_string(dateTime->day) +
				 std::to_string(dateTime->hour) +
				 std::to_string(dateTime->minute) +
				 std::to_string(dateTime->second);
		Poco::MD5Engine md5;
		Poco::DigestOutputStream outstr(md5);
		outstr << keyMD5;
		outstr.flush(); // to pass everything to the digest engine
		const Poco::DigestEngine::Digest &digest = md5.digest();
		std::string md5string = Poco::DigestEngine::digestToHex(digest);

		//*******************md5 end

		bookCommentObj->add("id", md5string);

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
		MongoDB::Document::Ptr queryCommentDoc(new MongoDB::Document());
		queryCommentDoc->add("comments", bookCommentObj);
		document->add("$addToSet", queryCommentDoc);

		// create find query
		MongoDB::Document::Ptr query(new MongoDB::Document());
		query->add("_id", bookId);

		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::insertBookPartComment(Database::Comment &comment,
						std::string &bookId, int &partindex)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr bookCommentObj(new MongoDB::Document());

		bookCommentObj->add("username", comment.username);
		bookCommentObj->add("repliedTo", comment.repliedTo);

		// get cueent date time
		auto dateTime = getDateTime();

		//*******************md5

		std::string keyMD5 = comment.username + comment.content +
				 std::to_string(dateTime->year) +
				 std::to_string(dateTime->month) +
				 std::to_string(dateTime->day) +
				 std::to_string(dateTime->hour) +
				 std::to_string(dateTime->minute) +
				 std::to_string(dateTime->second);
		Poco::MD5Engine md5;
		Poco::DigestOutputStream outstr(md5);
		outstr << keyMD5;
		outstr.flush(); // to pass everything to the digest engine
		const Poco::DigestEngine::Digest &digest = md5.digest();
		std::string md5string = Poco::DigestEngine::digestToHex(digest);

		//*******************md5 end
		bookCommentObj->add("id", md5string);

		MongoDB::Document::Ptr publishDateTime(new MongoDB::Document());
		publishDateTime->add("day", dateTime->day);
		publishDateTime->add("month", dateTime->month);
		publishDateTime->add("year", dateTime->year);
		publishDateTime->add("hour", dateTime->hour);
		publishDateTime->add("minute", dateTime->minute);
		publishDateTime->add("second", dateTime->second);

		bookCommentObj->add("dateTime", publishDateTime);

		bookCommentObj->add("edited", comment.edited);
		bookCommentObj->add("content", comment.content);

		MongoDB::Document::Ptr queryCommentDoc(new MongoDB::Document());
		queryCommentDoc->add("parts." + std::to_string(partindex) +
					 ".comments",
				 bookCommentObj);
		document->add("$addToSet", queryCommentDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		MongoDB::Document::Ptr subQuery(new MongoDB::Document());
		MongoDB::Document::Ptr matchElementQuery(new MongoDB::Document());

		// find query doc
		query->add("_id", bookId);
		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create comment
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::insertLikedUser(std::string &bookId,
					  std::string &username)
{
	try {
		// take connection from pool
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create document for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr addToArray(new MongoDB::Document());
		MongoDB::Document::Ptr like(new MongoDB::Document());
		like->add("username", username);

		MongoDB::Document::Ptr likeDateTime(new MongoDB::Document());
		auto dateTime = getDateTime();
		likeDateTime->add("year", dateTime->year);
		likeDateTime->add("month", dateTime->month);
		likeDateTime->add("day", dateTime->day);
		likeDateTime->add("hour", dateTime->hour);
		likeDateTime->add("minute", dateTime->minute);
		likeDateTime->add("second", dateTime->second);

		like->add("likeDateTime", likeDateTime);

		//*******************md5

		std::string keyMD5 = username + std::to_string(dateTime->year) +
				 std::to_string(dateTime->month) +
				 std::to_string(dateTime->day) +
				 std::to_string(dateTime->hour) +
				 std::to_string(dateTime->minute) +
				 std::to_string(dateTime->second);
		Poco::MD5Engine md5;
		Poco::DigestOutputStream outstr(md5);
		outstr << keyMD5;
		outstr.flush(); // to pass everything to the digest engine
		const Poco::DigestEngine::Digest &digest = md5.digest();
		std::string md5string = Poco::DigestEngine::digestToHex(digest);

		//*******************md5 end

		like->add("id", md5string);
		addToArray->add("likedUsers", like);
		document->add("$addToSet", addToArray);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		MongoDB::Document::Ptr subQuery(new MongoDB::Document());
		MongoDB::Document::Ptr matchElementQuery(new MongoDB::Document());

		// init find query
		query->add("_id", bookId);
		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::insertSharedWithUsers(std::string &bookId,
						std::vector<std::string> &usernames)
{
	try {
		// tke connection
		auto con = takeConnection();
		auto c = static_cast<MongoDB::Connection::Ptr>(con);

		// create doument for request
		MongoDB::Document::Ptr document(new MongoDB::Document());

		MongoDB::Document::Ptr addToArrayDoc(new MongoDB::Document());
		MongoDB::Array::Ptr addArray(new MongoDB::Array());

		// create each doc for doiing for all an a time
		MongoDB::Document::Ptr eachDoc(new MongoDB::Document());

		for (size_t i = 0; i < usernames.size(); i++) {

			addArray->add(std::to_string(i), usernames[i]);
		}
		eachDoc->add("$each", addArray);
		addToArrayDoc->add("sharedWith", eachDoc);
		document->add("$addToSet", addToArrayDoc);

		MongoDB::Document::Ptr query(new MongoDB::Document());
		MongoDB::Document::Ptr subQuery(new MongoDB::Document());
		MongoDB::Document::Ptr matchElementQuery(new MongoDB::Document());

		// init find request
		query->add("_id", bookId);
		MongoDB::Document::Ptr update(new MongoDB::Document());
		update->add("q", query).add("limit", 1);
		update->add("u", document);

		MongoDB::Array::Ptr updates(new MongoDB::Array());
		updates->add(std::to_string(0), update);

		// create command
		auto deleteCmd = g_db.createCommand();
		deleteCmd->selector()
		.add("update", "Books")
		.add("updates", updates);

		MongoDB::ResponseMessage response;

		// send request
		c->sendRequest(*deleteCmd, response);
		auto doc = *(response.documents()[0]);
		verifyResponse(doc);
		for (auto i : response.documents()) {
			return i->toString(2);
		}
	}
	catch (const Exception &e) {
		// TODO P[3] find a better way for returning error message and log
		// TODO P[2] handle errors
		//		std::cerr << "INSERT " << bookId << " failed: " <<
		// e.displayText()
		//			  << std::endl;
	}
}

std::string Database::getConnectionName() { return connectionName; }

void Database::setConnectionName(const std::string &value)
{
	connectionName = value;
}

std::string Database::getHostAddress() { return hostAddress; }

void Database::setHostAddress(const std::string &value) { hostAddress = value; }

std::string Database::getPort() { return port; }

void Database::setPort(const std::string &value) { port = value; }

size_t Database::getPoolCapacity() { return poolCapacity; }

void Database::setPoolCapacity(const size_t &value) { poolCapacity = value; }

Database::Database() {}

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

std::string Database::verifyResponse(const MongoDB::Document &response,
					 bool expectOK)
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
	std::string responseJson;
	try {
		if (r.exists("ok")) {
			const auto ok = extractInt64(r, "ok");
			if (ok != 1) {
				ostr << "Command failed: ok = " << ok << ". ";

				// put response in string
				responseJson +=
				std::string("Command failed: ok = ") +
				std::to_string(ok) + std::string(". ");
			}
		}
		else if (expectOK) {
			ostr << "UNEXPECTED: Missing 'ok' in response.";

			// put response in string
			responseJson += "UNEXPECTED: Missing 'ok' in response.";
		}
		else {
			// Document that does not have embedded status response,
			// e.g.from
			// find cursor
			return "";
		}

		// Find, aggregate commands
		Int64 code = -1;
		if (r.exists("code")) {
			code = extractInt64(r, "code");
		}
		if (r.exists("errmsg")) {
			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
			 << r.get<std::string>("errmsg");

			// put response in string
			responseJson +=
			(code >= 0 ? std::to_string(code) + std::string(": ")
				   : std::string("")) +
			r.get<std::string>("errmsg");
		}
		if (r.exists("$err")) {
			ostr << (code >= 0 ? std::to_string(code) + ": " : "")
			 << r.get<std::string>("$err");

			// put response in string
			responseJson +=
			(code >= 0 ? std::to_string(code) + std::string(": ")
				   : std::string("")) +
			r.get<std::string>("$err");
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

				// put response in string
				responseJson +=
				std::to_string(index) + std::string(": ") +
				std::to_string(code) + std::string(": ") +
				err->get<std::string>("errmsg");
				if ((i + 1) < ev->size()) {
					ostr << std::endl;

					// put response in string
					responseJson += "\n";
				}
			}
		}
		if (r.exists("writeConcernError")) {
			const auto err =
			r.get<MongoDB::Document::Ptr>("writeConcernError");
			if (ostr.tellp() > 0) {
				ostr << std::endl;
				responseJson += "\n";
			}
			const auto index = extractInt64(*err, "index");
			const auto code = extractInt64(*err, "code");
			ostr << index << ": " << code << ": "
			 << err->get<std::string>("errmsg");

			responseJson += std::to_string(index) + std::string(": ") +
					std::to_string(code) + std::string(": ") +
					err->get<std::string>("errmsg");
		}
	}
	catch (const Exception &e) {
		std::cout << "Response error exception " << e.displayText()
			  << std::endl;
		throw;
	}

	if (ostr.tellp() > 0) {
		//		std::cout << "Error response from server: " <<
		//response.toString(2)
		//			  << std::endl;
		responseJson = response.toString(2);
		// throw Exception("MongoDB: " + ostr.str());
	}
	return responseJson;
}

std::shared_ptr<Database::DateTime> Database::getDateTime()
{
	time_t t = time(0); // get time now
	struct tm *now = localtime(&t);
	std::shared_ptr<DateTime> dateTime(std::make_shared<DateTime>());
	dateTime->year = (now->tm_year + 1900);
	dateTime->month = (now->tm_mon + 1);
	dateTime->day = now->tm_mday;
	dateTime->hour = now->tm_hour;
	dateTime->minute = now->tm_min;
	dateTime->second = now->tm_sec;

	return dateTime;
}

// variables & objects
bool Database::isPoolRunning(false);
size_t Database::poolCapacity(16);
size_t Database::poolPeakCapacity(256);
std::string Database::hostAddress("localhost");
std::string Database::port("27017");
std::string Database::connectionName("pocoTestDB");
Database::MongoDBConnectionFactoryPtr Database::g_connectionFactory;
Database::MongoDBConnectionPoolPtr Database::g_connectionPool;
Poco::MongoDB::Database Database::g_db(Database::connectionName);
