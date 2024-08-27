#ifndef HTTPRESPONSE_HPP
#define HTTPRESPONSE_HPP

#include "EventLogger.hpp"
#include "HttpRequest.hpp"
// #include "NewRequest.hpp"
#include <cstdio>
#include <ctime>
#include <fstream>
#include <iostream>
#include <map>
#include <netinet/in.h>
#include <sstream>
#include <string>
#include <sys/socket.h>
#include <unistd.h>
#include <vector>
#include <sys/stat.h>
#include <chrono>
#include <thread>
#include "Utils.hpp"

extern char **environ;

enum status { OK = 200, NOT_FOUND = 404, BAD_REQUEST = 400 };

class HttpRequest;

class HttpResponse {
	private:
		std::string _status_line;
		std::string _header;
		std::string _body;
		std::string _response;

		const std::map<int, std::string> httpErrorMap
		{
			{200,"OK"},
			{201,"Created"},
			{202,"Accepted"},
			{400,"Bad Request"},
			{401,"Unauthorized"},
			{403,"Forbidden"},
			{404,"Page Not Found"},
			{405,"Method Not Allowed Error"},
			{413,"Payload Too Large"},
			{500,"Internal Server Error"},
			{501,"Not Implemented"},
			{502,"Bad Gateway"},
			{504,"Gateway Timeout Server"}
		};

	public:
		HttpResponse();
		~HttpResponse();

		std::string	metaData(clientState &clientData);
		std::string	webserverStamp(void);
		std::string	errorHandlingGet(int code, clientState &clientData);

		std::string statusCodes(int code);
		std::string generateErrorPage(int code, const std::string& message);
		std::string generateHtml(int code, const std::string& codeMessage);
		std::string respond(clientState &clientData);
		std::string successHandling(int statusCode, clientState &clientData, const std::string &messageBody = "");

		std::string deleteListing(clientState &clientData);
		std::string directoryListing(clientState &clientData);
		std::string handleGetFile(clientState &clientData);

		std::string respond_Get(clientState &clientData);
		std::string response_Post(clientState &clientData);
		std::string responseDelete(clientState &clientData);
		std::string respondRedirect(clientState &clientData);

		std::string processCgi(clientState &clientData);
		void	execute(clientState &clientData);
		std::string parentProcess(clientState &clientData); 

		std::string buildHttpResponse(const std::string& statusLine, const std::string& contentType,
					const std::string& body, const clientState& clientData);

		bool is_valid_str(const std::string &str);
		bool is_valid_char(char c);
		bool checkSuffix(const std::string &str, const std::string &suffix);

		std::string errorHandlingPost(int statusCode, clientState &clientData);
		bool 		write_to_file(clientState &clientData, const std::string& path, const std::string& content);
		void		parse_headers(std::istringstream& contentStream, std::string& fileName, std::string& fileContent);
		std::string	findBoundary(const std::map<std::string, std::string>& headers);
		void		parseRequestBody(clientState &clientData);
		std::string	genericHttpCodeResponse(int statusCode, const std::string& message);
};

#endif