#include "checkErrors.h"


using namespace std;

std::string errorString;

token t;

/*
* Array of strings corresponding to tokenType
*/
std::string tokenTypeString[] = {
	";", "if", "then", "else" , "end", "repeat", "until", "IDENTIFIER", ":=",
	"read", "write", "<" , "=", "+", "-", "*", "/",
	"(", ")", "NUMBER","{","}"
};

vector<token> tokensCheck;


#define CHECK(valid) \
    if (!valid) \
    { \
        return valid; \
    }


inline bool matchToken(tokenType type);

inline bool checkStmtSequence(void);

inline bool checkStatement(void);

inline bool checkIfStmt(void);

inline bool checkRepeatStmt(void);

inline bool checkAssignStmt(void);

inline bool checkReadStmt(void);

inline bool checkWriteStmt(void);

inline bool checkExp(void);

inline bool checkComparisonOp(void);

inline bool checkSimpleExp(void);

inline bool checkAddop(void);

inline bool checkTerm(void);

inline bool checkMulop(void);

inline bool checkFactor(void);

inline bool matchToken(tokenType type)
{
	if (!tokensCheck.empty())
	{
#if(DEBUGGING == true)
		cout << "Token: " << tokensCheck.back().value << endl << endl;
#endif
		if (tokensCheck.back().type == type)
		{
			t = tokensCheck.back();
			tokensCheck.pop_back();
			return true;
		}
	}
#if(DEBUGGING == true)
	cout << "Error : ";
	cout << "After " << t.value << " --> ";
	if (type == tokenType::END || type == tokenType::UNTIL || type == tokenType::ELSE)
	{
		cout << "Expected " << tokenTypeString[static_cast<uint8_t>(type)] << " or ;" << endl << endl;
	}
	else
	{
		cout << "Expected " << tokenTypeString[static_cast<uint8_t>(type)] << endl << endl;
	}
#endif
	errorString = "";
	errorString += "Error : ";
	errorString += "After " + t.value + " --> ";
	if (type == tokenType::END || type == tokenType::UNTIL || type == tokenType::ELSE) {
		errorString += "Expected " + tokenTypeString[static_cast<uint8_t>(type)] + " or ;\n\n";
	}
	else {
		errorString += "Expected " + tokenTypeString[static_cast<uint8_t>(type)] + "\n\n";
	}
	return false;
}

pair<bool, std::string> checkProgram(void)
{
	bool valid = true;

#if(DEBUGGING == true)
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
#endif
	tokensCheck = getTokens();
	std::reverse(tokensCheck.begin(), tokensCheck.end());

	if (tokensCheck.empty())
	{
		valid = false;
		errorString = "";
		errorString += "Error : No Code Entered\n\n";
		pair<bool, std::string> checkPair(valid, errorString);
		return checkPair;
	}

	valid = checkStmtSequence();

	if (!tokensCheck.empty() && valid)
	{
#if(DEBUGGING == true)
	cout << "Error : ";
	cout << "After " << t.value << " --> ";
	cout << "Expected ;" << endl << endl;
#endif
	errorString = "";
	errorString += "Error : ";
	errorString += "After " + t.value + " --> ";
	errorString += "Expected ;\n\n";
	valid = false;
	}
	if (valid == true)
	{
		pair<bool, std::string> checkPair(valid, "No Error");
		return checkPair;
	}
	pair<bool, std::string> checkPair(valid, errorString);
	return checkPair;
}

inline bool checkStmtSequence(void)
{
	bool valid = true;

	valid = checkStatement();
	CHECK(valid);
	if (!tokensCheck.empty())
	{
		while (tokensCheck.back().type == tokenType::SEMICOLON)
		{
			valid = matchToken(tokenType::SEMICOLON);
			CHECK(valid);
			valid = checkStatement();
			CHECK(valid);
			if (tokensCheck.empty())
			{
				break;
			}
		}
	}

	return valid;
}

inline bool checkStatement(void)
{
	bool valid = true;

	if (!tokensCheck.empty()) {
		switch (tokensCheck.back().type)
		{
		case tokenType::IF:
			return checkIfStmt();
		case tokenType::REPEAT:
			return checkRepeatStmt();
		case tokenType::IDENTIFIER:
			return checkAssignStmt();
		case tokenType::READ:
			return checkReadStmt();
		case tokenType::WRITE:
			return checkWriteStmt();
		default:
			valid = false;
#if(DEBUGGING == true)
			cout << "Error : ";
			cout << "After " << t.value << " --> ";
			cout << "Expected if or repeat or IDENTIFIER or read or write" << endl << endl;
#endif
			errorString = "";
			errorString += "Error : ";
			errorString += "After " + t.value + " --> ";
			errorString += "Expected if or repeat or IDENTIFIER or read or write\n\n";
			return valid;
		}
	}
	else
	{
		valid = false;
#if(DEBUGGING == true)
		cout << "Error : ";
		cout << "After " << t.value << " --> ";
		cout << "Expected if or repeat or IDENTIFIER or read or write" << endl << endl;
#endif
		errorString = "";
		errorString += "Error : ";
		errorString += "After " + t.value + " --> ";
		errorString += "Expected if or repeat or IDENTIFIER or read or write\n\n";
		return valid;
	}

	return valid;
}

inline bool checkIfStmt(void)
{
	bool valid = true;

	valid = matchToken(tokenType::IF);
	CHECK(valid);
	valid = checkExp();
	CHECK(valid);
	valid = matchToken(tokenType::THEN);
	CHECK(valid);
	valid = checkStmtSequence();
	CHECK(valid);
	if (!tokensCheck.empty()) {
		if (tokensCheck.back().type == tokenType::ELSE)
		{
			valid = matchToken(tokenType::ELSE);
			CHECK(valid);
			valid = checkStmtSequence();
			CHECK(valid);
		}
	}
	valid = matchToken(tokenType::END);
	CHECK(valid);

	return valid;
}

inline bool checkRepeatStmt(void)
{
	bool valid = true;

	valid = matchToken(tokenType::REPEAT);
	CHECK(valid);
	valid = checkStmtSequence();
	CHECK(valid);
	valid = matchToken(tokenType::UNTIL);
	CHECK(valid);
	valid = checkExp();
	CHECK(valid);

	return valid;
}

inline bool checkAssignStmt(void)
{
	bool valid = true;

	valid = matchToken(tokenType::IDENTIFIER);
	CHECK(valid);
	valid = matchToken(tokenType::ASSIGN);
	CHECK(valid);
	valid = checkExp();
	CHECK(valid);

	return valid;
}

inline bool checkReadStmt(void)
{
	bool valid = true;

	valid = matchToken(tokenType::READ);
	CHECK(valid);
	valid = matchToken(tokenType::IDENTIFIER);
	CHECK(valid);

	return valid;
}

inline bool checkWriteStmt(void)
{
	bool valid = true;

	valid = matchToken(tokenType::WRITE);
	CHECK(valid);
	valid = checkExp();
	CHECK(valid);

	return valid;
}

inline bool checkExp(void)
{
	bool valid = true;

	valid = checkSimpleExp();
	CHECK(valid);
	if (!tokensCheck.empty())
	{
		if (tokensCheck.back().type == tokenType::LESSTHAN || tokensCheck.back().type == tokenType::EQUAL)
		{
			valid = checkComparisonOp();
			CHECK(valid);
			valid = checkSimpleExp();
			CHECK(valid);
		}
	}

	return valid;
}

bool checkComparisonOp(void)
{
	bool valid = true;

	if (!tokensCheck.empty()) {
		switch (tokensCheck.back().type)
		{
		case tokenType::LESSTHAN:
			valid = matchToken(tokenType::LESSTHAN);
			CHECK(valid);
			break;
		case tokenType::EQUAL:
			valid = matchToken(tokenType::EQUAL);
			CHECK(valid);
			break;
		default:
			valid = false;
#if(DEBUGGING == true)
			cout << "Error : ";
			cout << "After " << t.value << " --> ";
			cout << "Expected < or =" << endl << endl;
#endif
			errorString = "";
			errorString += "Error : ";
			errorString += "After " + t.value + " --> ";
			errorString += "Expected < or =\n\n";
			return valid;
			break;
		}
	}
	else
	{
		valid = false;
#if(DEBUGGING == true)
		cout << "Error : ";
		cout << "After " << t.value << " --> ";
		cout << "Expected < or =" << endl << endl;
#endif
		errorString = "";
		errorString += "Error : ";
		errorString += "After " + t.value + " --> ";
		errorString += "Expected < or =\n\n";
		return valid;
	}

	return valid;
}

bool checkSimpleExp(void)
{
	bool valid = true;

	valid = checkTerm();
	CHECK(valid);
	if (!tokensCheck.empty()) {
		while (tokensCheck.back().type == tokenType::PLUS || tokensCheck.back().type == tokenType::MINUS)
		{
			valid = checkAddop();
			CHECK(valid);
			valid = checkTerm();
			CHECK(valid);
			if (tokensCheck.empty())
			{
				break;
			}
		}
	}
	
	return valid;
}

bool checkAddop(void)
{
	bool valid = true;

	if (!tokensCheck.empty()) {
		switch (tokensCheck.back().type) {
		case tokenType::PLUS:
			valid = matchToken(tokenType::PLUS);
			CHECK(valid);
			break;
		case tokenType::MINUS:
			valid = matchToken(tokenType::MINUS);
			CHECK(valid);
			break;
		default:
			valid = false;
#if(DEBUGGING == true)
			cout << "Error : ";
			cout << "After " << t.value << " --> ";
			cout << "Expected + or -" << endl << endl;
#endif
			errorString = "";
			errorString += "Error : ";
			errorString += "After " + t.value + " --> ";
			errorString += "Expected + or -\n\n";
			return valid;
		}
	}
	else
	{
		valid = false;
#if(DEBUGGING == true)
		cout << "Error : ";
		cout << "After " << t.value << " --> ";
		cout << "Expected + or -" << endl << endl;
#endif
		errorString = "";
		errorString += "Error : ";
		errorString += "After " + t.value + " --> ";
		errorString += "Expected + or -\n\n";
		return valid;
	}

	return valid;
}

bool checkTerm(void)
{
	bool valid = true;

	valid = checkFactor();
	CHECK(valid);
	if (!tokensCheck.empty()) {
		while (!tokensCheck.empty() && (tokensCheck.back().type == tokenType::MULT || tokensCheck.back().type == tokenType::DIV)) {
			valid = checkMulop();
			CHECK(valid);
			valid = checkFactor();
			CHECK(valid);
			if (tokensCheck.empty())
			{
				break;
			}
		}
	}

	return valid;

}

bool checkMulop(void)
{
	bool valid = true;

	if (!tokensCheck.empty()) {
		switch (tokensCheck.back().type) {
		case tokenType::MULT:
			valid = matchToken(tokenType::MULT);
			CHECK(valid);
			break;
		case tokenType::DIV:
			valid = matchToken(tokenType::DIV);
			CHECK(valid);
			break;
		default:
			valid = false;
#if(DEBUGGING == true)
			cout << "Error : ";
			cout << "After " << t.value << " --> ";
			cout << "Expected * or /" << endl << endl;
#endif
			errorString = "";
			errorString += "Error : ";
			errorString += "After " + t.value + " --> ";
			errorString += "Expected * or /\n\n";
			return valid;
		}
	}
	else
	{
		valid = false;
#if(DEBUGGING == true)
		cout << "Error : ";
		cout << "After " << t.value << " --> ";
		cout << "Expected * or /" << endl << endl;
#endif
		errorString = "";
		errorString += "Error : ";
		errorString += "After " + t.value + " --> ";
		errorString += "Expected * or /\n\n";
		return valid;
	}

	return valid;

}

bool checkFactor(void)
{
	bool valid = true;

	if (!tokensCheck.empty()) {
		switch (tokensCheck.back().type) {
		case tokenType::OPENBRACKET:
			valid = matchToken(tokenType::OPENBRACKET);
			CHECK(valid);
			valid = checkExp();
			CHECK(valid);
			valid = matchToken(tokenType::CLOSEDBRACKET);
			CHECK(valid);
			break;
		case tokenType::IDENTIFIER:
			valid = matchToken(tokenType::IDENTIFIER);
			CHECK(valid);
			break;
		case tokenType::NUMBER:
			valid = matchToken(tokenType::NUMBER);
			CHECK(valid);
			break;
		default:
			valid = false;
#if(DEBUGGING == true)
			cout << "Error : ";
			cout << "After " << t.value << " --> ";
			cout << "Expected ( or IDENTIFIER or NUMBER" << endl << endl;
#endif
			errorString = "";
			errorString += "Error : ";
			errorString += "After " + t.value + " --> ";
			errorString += "Expected ( or IDENTIFIER or NUMBER\n\n";
			return valid;
		}
	}
	else
	{
		valid = false;
#if(DEBUGGING == true)
		cout << "Error : ";
		cout << "After " << t.value << " --> ";
		cout << "Expected ( or IDENTIFIER or NUMBER" << endl << endl;
#endif
		errorString = "";
		errorString += "Error : ";
		errorString += "After " + t.value + " --> ";
		errorString += "Expected ( or IDENTIFIER or NUMBER\n\n";
		return valid;
	}

	return valid;
}




