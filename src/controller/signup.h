#ifndef SIGNUP_H
#define SIGNUP_H

#include <Poco/Crypto/Cipher.h>
#include <Poco/Crypto/CipherFactory.h>
#include <Poco/Crypto/CipherKey.h>
#include <Poco/Net/HTMLForm.h>
#include <Poco/Net/HTTPRequestHandler.h>
#include <Poco/Net/HTTPServerRequest.h>
#include <Poco/Net/HTTPServerResponse.h>

class SignUp : public Poco::Net::HTTPRequestHandler {
  public:
	void handleRequest(Poco::Net::HTTPServerRequest &request,
			   Poco::Net::HTTPServerResponse &response);
};

#endif // SIGNUP_H
