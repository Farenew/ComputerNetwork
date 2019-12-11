#include "HttpParser.h"

#include <iostream>

using std::cout;
using std::endl;

extern constexpr auto MAX_BUFFER = 65536;

// my parse funcion is used to cut a string buffer into buffer and nexttoken
// this function is similar to strtok_s, except this use delim as a whole
static char* myParse(char* buffer, const char* delim, char** nexttoken) {
	size_t limit = strlen(buffer);
	size_t dl = strlen(delim);
	size_t j = 0;

	// use mark to remember the starting point to parse
	size_t mark;
	for (size_t i = 0; i < limit; i++) {
		mark = i;
		// if our buffer matches delimiator, then continue search to make sure all match
		if (buffer[i] == delim[0]) {
			// continue search
			for (j = 1; j < dl; i++, j++) {
				if (buffer[i + 1] != delim[j])
					break;
			}
			// if all match
			if (j == dl) {
				// change delimiator into string stop sign
				for (size_t k = 0; k < dl; k++)
					buffer[mark + k] = '\0';
				// set nexttoken 
				*nexttoken = buffer + i + 1;
				return buffer;
			}
		}
	}
	return buffer;
}

void HttpParser::parse(char* buf) {
	char* line, *nextToken = nullptr;
	const char* newLine = "\r\n";

	// PARSE REQUEST LINE
	// iterate through request line, and parse it by space, store to token
	line = myParse(buf, newLine, &nextToken);
	string token;
	for (unsigned i = 0, cur = 0; i < strlen(line); i++) {
		// cur == 0 means method
		if (line[i] == ' ' && cur == 0) {
			requesLine.method = token;
			cur++;
			token.clear();
		}
		// cur == 1 means url
		else if (line[i] == ' ' && cur == 1) {
			requesLine.url = token;
			cur++;
			token.clear();
		}
		else
			token += line[i];
		
	}
	requesLine.version = token;
	
	// PARSE REQUEST HEADER
	const char* colonStop = ": ";
	char *head, *content;
	char* curBuf;
	while (nextToken != nullptr && nextToken[0] != '\r' && nextToken[1] != '\n') {
		// set current buffer to parse
		curBuf = nextToken;
		// extract one line from curBuf
		line = myParse(curBuf, newLine, &nextToken);

		// parse this line into head and contet
		head = myParse(line, colonStop, &content);

		// add current parse header into headerLines
		headerLines.insert({ string(head), string(content) });
	}

	// PARSE ENTIRY BODY
	// first two characters are \r\n
	nextToken += 2;
	entityBody = string(nextToken);
}

void HttpParser::printRequestLine() {
	cout << requesLine.method << " " 
		<< requesLine.url << " " 
		<< requesLine.version << std::endl;

	cout << endl;
}

void HttpParser::printHeaderLines() {
	for (auto i : headerLines)
		cout << i.first << ": " << i.second << endl;

	cout << endl;
}

void HttpParser::printRequestBody() {
	cout << entityBody;

	cout << endl;
}