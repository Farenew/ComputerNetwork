#pragma once

#include <string>
#include <map>
#include <sstream>

using std::string;
using std::map;

struct RequestLine {
	string method;
	string url;
	string version;
};

class HttpParser
{
public:
	RequestLine requesLine;
	map<string, string> headerLines;
	string entityBody;

	HttpParser() {};
	void parse(char* buf);

	void printRequestLine();
	void printHeaderLines();
	void printRequestBody();
};

