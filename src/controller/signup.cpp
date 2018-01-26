#include "signup.h"
#include <Poco/DigestStream.h>
#include <Poco/Dynamic/Var.h>
#include <Poco/JSON/JSON.h>
#include <Poco/JSON/Parser.h>
#include <Poco/JSON/Parser.h>
#include <chrono>
#include <iostream>
#include <sstream>
#include <string>

#include "database/database.h"

void SignUp::handleRequest(Poco::Net::HTTPServerRequest &request,
			   Poco::Net::HTTPServerResponse &response)
{
	//	Poco::Crypto::CipherFactory &factory =
	//	Poco::Crypto::CipherFactory::defaultFactory();
	//	// Creates a 256-bit AES cipher
	//	Poco::Crypto::Cipher *pCipher =
	//	factory.createCipher(Poco::Crypto::CipherKey("aes-256"));
	//	// Poco::Crypto::Cipher* pRSACipher =
	//	//
	//	//
	// factory.createCipher(Poco::Crypto::CipherFactory::RSAKey(RSAKey::KL_1024,
	//	// RSAKey::EXP_SMALL));

	//	std::string plainText = "This is my secret information";
	//	std::string encrypted =
	//	pCipher->encryptString(plainText, Poco::Crypto::Cipher::ENC_BASE64);
	//	std::string decrypted =
	//	pCipher->decryptString(encrypted, Poco::Crypto::Cipher::ENC_BASE64);
	//	std::clog << "encrypted : " << encrypted << std::endl;
	//	std::clog << "decrypted : " << decrypted << std::endl;

	std::istream &i = request.stream();
	int len = request.getContentLength();
	char *buffer = new char[len];
	i.read(buffer, len);

	Database::User user;

	// objects
	std::string json = buffer;
	Poco::JSON::Parser parser;
	Poco::Dynamic::Var result = parser.parse(json);
	Poco::JSON::Object::Ptr object = result.extract<Poco::JSON::Object::Ptr>();

	Poco::Dynamic::Var tempValue = object->get("username");
	user.username = tempValue.convert<std::string>();

	tempValue = object->get("password");
	user.password = tempValue.convert<std::string>();

	tempValue = object->get("name");
	user.name = tempValue.convert<std::string>();

	tempValue = object->get("family");
	user.family = tempValue.convert<std::string>();

	tempValue = object->get("sex");
	user.sex = tempValue.convert<std::string>();

	tempValue = object->get("email");
	user.email = tempValue.convert<std::string>();

	tempValue = object->get("phoneNumber");
	user.phoneNumber = tempValue.convert<std::string>();

	tempValue = object->get("berthday");
	object = tempValue.extract<Poco::JSON::Object::Ptr>();
	tempValue = object->get("property");

	tempValue = object->get("year");
	user.berthday.year = tempValue.convert<int>();

	tempValue = object->get("month");
	user.berthday.month = tempValue.convert<int>();

	tempValue = object->get("day");
	user.berthday.day = tempValue.convert<int>();

	std::ostream &responseStream = response.send();
	responseStream << Database::saveUser(user);
}
