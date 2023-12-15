#include "TinyScanner.h"

using namespace std;



/*
* Array of strings corresponding to tokenType
*/
std::string tokenTypeStrings[] = {
	"SEMICOLON", "IF", "THEN", "ELSE" , "END", "REPEAT", "UNTIL", "IDENTIFIER", "ASSIGN",
	"READ", "WRITE", "LESSTHAN" , "EQUAL", "PLUS", "MINUS", "MULT", "DIV",
	"OPENBRACKET", "CLOSEDBRACKET", "NUMBER","OPENCOMMENT","CLOSEDCOMMENT"
};


//array of tokens
vector<token> tokens;

/*
* hash function for all reserved word in tiny language
*/
unordered_map<string, tokenType> reservedWords;


//string displays all tokens
string tokenList = "";

void scannerInit(void)
{
	reservedWords[";"] = tokenType::SEMICOLON;
	reservedWords["if"] = tokenType::IF;
	reservedWords["then"] = tokenType::THEN;
	reservedWords["else"] = tokenType::ELSE;
	reservedWords["end"] = tokenType::END;
	reservedWords["repeat"] = tokenType::REPEAT;
	reservedWords["until"] = tokenType::UNTIL;
	reservedWords[":="] = tokenType::ASSIGN;
	reservedWords["read"] = tokenType::READ;
	reservedWords["write"] = tokenType::WRITE;
	reservedWords["<"] = tokenType::LESSTHAN;
	reservedWords["="] = tokenType::EQUAL;
	reservedWords["+"] = tokenType::PLUS;
	reservedWords["-"] = tokenType::MINUS;
	reservedWords["*"] = tokenType::MULT;
	reservedWords["/"] = tokenType::DIV;
	reservedWords["("] = tokenType::OPENBRACKET;
	reservedWords[")"] = tokenType::CLOSEDBRACKET;
	reservedWords["{"] = tokenType::OPENCOMMENT;
	reservedWords["}"] = tokenType::CLOSEDCOMMENT;
	tokenList = "";
	tokens.clear();
}




// Convert System::String^ to std::string
static void System2StdString(System::String^ s, std::string& os) {
	if (System::String::IsNullOrEmpty(s)) {
		// Handle the case where the System::String^ is empty or null
		os.clear(); // Clear the std::string
	}
	else {
		using namespace System::Runtime::InteropServices;
		const char* chars =
			(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		os = chars;
		Marshal::FreeHGlobal(System::IntPtr((void*)chars));
	}
}


//function to check identifiers and numbers
void checkIdentifiersNumbers(string value,token &t)
{
	/*check if string name is number or string of character
	* if it contains both we will ignore it as it is not considered to be token
	*/
	bool hasDigit = false;
	bool hasChar = false;
	bool notValidToken = false;

	// Check if the string contains both digits and alphabets
	for (char c : value)
	{
		if (c >= '0' && c <= '9')
		{
			hasDigit = true;
		}
		else if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'))
		{
			hasChar = true;
		}
		else
		{
			notValidToken = true;
			break;
		}
	}

	if (hasDigit && !hasChar && !notValidToken)
	{
#if(DEBUGGING == true)
		cout << "Number: " << value << endl;
#endif
		t = {stoi(value), value, tokenType::NUMBER};
		tokens.push_back(t);
	}
	else if(!hasDigit && hasChar && !notValidToken)
	{
#if(DEBUGGING == true)
		cout << "Identifier: " << value << endl;
#endif
		t = {0, value, tokenType::IDENTIFIER };
		tokens.push_back(t);
	}
	else if (notValidToken || (hasDigit && hasChar))
	{
#if(DEBUGGING == true)
		cout << value << " is not Number nor IDENTIFIER" << endl;
#endif
	}

}



//function for scanner phase in compiler for tiny language
void scanCode(System::String^ codeContentSystem) {
	scannerInit();
	string codeContent;
	System2StdString(codeContentSystem, codeContent);

	string value = "";
	for (int i = 0; i < codeContent.size(); i++) {
		char ch = codeContent[i];

		//special character found or new line
		if (ch == ';' ||  ch == '<' || ch == '/' || ch == '*' || ch == '+' || ch == '-'
			|| ch == '(' || ch == ')' || ch == '=' || ch == '\r' || ch == '\r\n' || ch == '\n')
		{
			token t;
			if (value != "")
			{
				//check if token is a reserved word or identifier or number
				if (reservedWords.find(value) != reservedWords.end()) {
#if(DEBUGGING ==  true)
					cout << "Found reserved word: " << value << " with type " <<
						tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
					t = {0, value, reservedWords[value] };
					tokens.push_back(t);
				}
				else {

					checkIdentifiersNumbers(value, t);
				}
				value = "";
			}
			if (ch != '\r' && ch != '\n' && ch != '\r\n')
			{
				//store special character in a token
				string str(1, ch);
#if(DEBUGGING ==  true)
				cout << "Found reserved word: " << str << " with type " <<
					tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
				token t2 = { 0,str,reservedWords[str] };
				tokens.push_back(t2);
			}
		}

		//ignore whitespaces and check new token
		else if (ch == ' ' && value != "") {
			token t;
			//check if it is a reserved word or identifier or number
			if (reservedWords.find(value) != reservedWords.end()) {
#if(DEBUGGING ==  true)
				cout << "Found reserved word: " << value << " with type " <<
					tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
				t = {0,value, reservedWords[value] };
				tokens.push_back(t);
			}
			else {
				checkIdentifiersNumbers(value, t);
			}
			value = "";
		}


		/*
		* if found character : then afterwards we will find = and a new token is found assign(:=)
		* after that check token before assign
		*/
		else if (i + 1 < codeContent.size() && ch == ':') {
			if (codeContent[i + 1] == '=')
			{
				token t;
				if (value != "")
				{
					//check if token is a reserved word or identifier or number
					if (reservedWords.find(value) != reservedWords.end()) {
#if(DEBUGGING ==  true)
						cout << "Found reserved word: " << value << " with type " <<
							tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
						t = { 0, value, reservedWords[value] };
						tokens.push_back(t);
					}
					else {
						checkIdentifiersNumbers(value, t);
					}
					value = "";
				}
				//store assign(:=) in a token
				string str;
				str.push_back(ch);
				i++;
				ch = codeContent[i];
				str.push_back(ch);
				token t2 = { 0, str,reservedWords[str] };
				tokens.push_back(t2);
			}
		}

		//found a comment then we will ignore it and store any token before that comment
		else if (ch == '{') {
			token t;
			if (value != "")
			{
				//check if it is a reserved word or identifier or number
				if (reservedWords.find(value) != reservedWords.end()) {
#if(DEBUGGING ==  true)
					cout << "Found reserved word: " << value << " with type " <<
						tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
					t = {0, value, reservedWords[value] };
					tokens.push_back(t);
				}
				else {
					checkIdentifiersNumbers(value, t);
				}
				value = "";
			}
			//ignore comment
			while (ch != '}')
			{
				i++;
				ch = codeContent[i];
			}
		}


		else
		{
			//ignore new lines,tabs,white spaces in token value
			if (ch != '\n' && ch != '\r' && ch != '\r\n' && ch != ' ' && ch != '\t') {
				value.push_back(ch);
			}
		}
	}

	/*
	* check last token
	*/
	token t;

	//check if token is a reserved word or identifier or number
	if (value != "")
	{
		if (reservedWords.find(value) != reservedWords.end()) {
#if(DEBUGGING ==  true)
			cout << "Found reserved word: " << value << " with type " <<
				tokenTypeStrings[static_cast<uint8_t>(reservedWords[value])] << endl;
#endif
			t = {0, value, reservedWords[value] };
			tokens.push_back(t);
		}
		else {
			checkIdentifiersNumbers(value, t);
		}
		value = "";
	}

#if(DEBUGGING == true)
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
#endif

	int i = 1;
/*
	for (const token& t : tokens) {
#if(DEBUGGING == true)
		std::cout << "Token No." << i << std::endl << std::endl;
		std::cout << "Token :" << std::endl << t.value << std::endl << std::endl;
		std::cout << "Token Type :" << std::endl << tokenTypeStrings[t.type] << std::endl;
		std::cout << "------------------------" << std::endl;
#endif
		tokenList += "Token No.";
		string strNumber = to_string(i);
		tokenList += strNumber;
		tokenList += "\r\n";
		tokenList += "\r\n";
		tokenList += "Token :";
		tokenList += "\r\n";
		tokenList += "  ";
		tokenList += t.value;
		tokenList += "\r\n";
		tokenList += "\r\n";
		tokenList += "Token Type :";
		tokenList += "\r\n";
		tokenList += "  ";
		tokenList += tokenTypeStrings[t.type];
		tokenList += "\r\n";
		tokenList += "------------------------";
		tokenList += "\r\n";
		i++;
	}
*/
	for (const token& t : tokens) {
#if(DEBUGGING == true)
		std::cout << t.value << " , " << tokenTypeStrings[static_cast<uint8_t>(t.type)] << std::endl << std::endl;
#endif
		tokenList += t.value;
		tokenList += " , ";
		tokenList += tokenTypeStrings[static_cast<uint8_t>(t.type)];
		tokenList += "\r\n";
		tokenList += "\r\n";
		i++;
	}

}

//function to show tokens in gui and save it in text file
void printTokens(System::String^& tokenListSystem)
{
	tokenListSystem = gcnew System::String(tokenList.data());
	string filename = "Token List";
	ofstream out(filename + ".txt");
	out << tokenList;
	out.close();
}

std::vector<token> getTokens(void)
{
	return tokens;
}
