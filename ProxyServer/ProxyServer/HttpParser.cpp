#include "HttpParser.h"

#include <iostream>

using std::cout;
using std::endl;

extern constexpr auto MAX_BUFFER = 65536;

// newparse is used to split buffer by delim, and return first part as a string, 
// while second part is wrote into nextToken
static string newparse(char* buffer, string delim, char **nextToken) {
	size_t limit = strlen(buffer);
	size_t j = 0;

	// use mark to remember the starting point to parse
	size_t mark;
	for (size_t i = 0; i < limit; i++) {
		mark = i;
		// if our buffer matches delimiator, then continue search to make sure all match
		if (buffer[i] == delim[0]) {
			// continue search
			for (j = 1; j < delim.size(); i++, j++) {
				if (buffer[i + 1] != delim[j])
					break;
			}
			// if all match
			if (j == delim.size()) {
				// set nexttoken 
				*nextToken = buffer + i + 1;
				
				return string(buffer, buffer + i - j + 1);
			}
		}
	}
	return string();
}

// used to parse HTTP head, write into each struct
void HttpParser::parse(char* buf) {
	char *nextToken = nullptr;
	constexpr auto newLine = "\r\n";

	// PARSE REQUEST LINE
	// iterate through request line, and parse it by space, store to token
	auto line = newparse(buf, newLine, &nextToken);
	int cur = 0;
	for (unsigned i = 0; i < line.size(); i++) {
		// cur == 0 means method
		if (line[i] == ' ' && cur == 0)
			requesLine.method = line.substr(0, i), cur = i+1;
		// cur != 0 means url
		else if (line[i] == ' ' && cur != 0)
			requesLine.url = line.substr(cur, i-cur+1), cur = i+1;

	}
	requesLine.version = line.substr(cur, line.size()- cur + 1);


	// PARSE REQUEST HEADER
	constexpr auto colonStop = ": ";
	string head;
	char* curBuf;
	while (nextToken != nullptr && nextToken[0] != '\r' && nextToken[1] != '\n') {
		// set current buffer to parse
		curBuf = nextToken;
		// extract one line from curBuf
		line = newparse(curBuf, newLine, &nextToken);

		// find position of colonStop
		int npos = line.find(colonStop);

		headerLines.insert({line.substr(0, npos), line.substr(npos+2, line.size()-npos-1)});
	}

	// PARSE ENTIRY BODY
	// first two characters are \r\n
	if (strlen(nextToken) >= 2) {
		nextToken += 2;
		entityBody = string(nextToken);
	}
	else
		entityBody = string();
}

void HttpParser::printRequestLine() {
	cout << requesLine.method << " " 
		<< requesLine.url << " " 
		<< requesLine.version << std::endl;
}

void HttpParser::printHeaderLines() {
	for (auto i : headerLines)
		cout << i.first << ": " << i.second << endl;
}

void HttpParser::printRequestBody() {
	cout << entityBody;
}

