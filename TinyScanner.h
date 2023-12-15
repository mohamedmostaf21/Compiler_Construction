#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <unordered_map>
#include <queue>


#define DEBUGGING true

void scannerInit();

void scanCode(System::String^ fileContentSystem);

void printTokens(System::String^& tokenListSystem);

/*
* enum for each token type in tiny language
*/
enum class tokenType : uint8_t{
	SEMICOLON, IF, THEN , ELSE, END, REPEAT, UNTIL, IDENTIFIER, ASSIGN, READ, WRITE, LESSTHAN, EQUAL,
	PLUS, MINUS, MULT, DIV, OPENBRACKET, CLOSEDBRACKET, NUMBER, OPENCOMMENT, CLOSEDCOMMENT
};

/*
* structure for token which has:
* number if token is number if not number is assigned to zero
* string for value of token
* type of token
*/
struct token {
	int number;
	std::string value;
	tokenType type;
};

std::vector<token> getTokens(void);

