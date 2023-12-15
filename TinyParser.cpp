#include "TinyParser.h"

using namespace std;

std::string nodeCount = "1";
std::string SubGraphCount = "1";


#define ADD_EDGE(graph, sourceNode, targetNode) \
    if ((sourceNode) != nullptr && (targetNode) != nullptr) { \
            agedge((graph), (sourceNode), (targetNode), NULL, 1); \
        } \


vector<token> tokensSyntaxTree;

inline std::string matchToken(tokenType type);

inline Agnode_t* DrawStmtSequenceSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawStatementSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawIfStmtSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawRepeatStmtSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawAssignStmtSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawReadStmSyntaxTreet(Agraph_t* graph);

inline Agnode_t* DrawWriteStmtSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawExpSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawComparisonOpSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawSimpleExpSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawAddopSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawTermSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawMulopSyntaxTree(Agraph_t* graph);

inline Agnode_t* DrawFactorSyntaxTree(Agraph_t* graph);

inline std::string matchToken(tokenType type)
{
	std::string t = tokensSyntaxTree.back().value;
	tokensSyntaxTree.pop_back();
	return t;
}


void DrawProgramSyntaxTree(void)
{

#if(DEBUGGING == true)
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
	std::cout << "------------------------" << std::endl;
#endif

	tokensSyntaxTree = getTokens();
	std::reverse(tokensSyntaxTree.begin(), tokensSyntaxTree.end());

	if (checkProgram().first == false || tokensSyntaxTree.empty())
	{
#if(DEBUGGING == true)
		std::cout << "Can't Create Syntax Tree\nCheck Error in Code" << std::endl << std::endl;
#endif
		return;
	}

	// Create a graph context
	GVC_t* gvc;
	Agraph_t* graph;

	gvc = gvContext();
	graph = agopen((char*)"G", UNDIRECTED , 0);

	agattr(graph, AGRAPH, (char*)"ordering", (char*)"in");
	agattr(graph, AGRAPH, (char*)"size", (char*)"795,682");

	DrawStmtSequenceSyntaxTree(graph);


	// Layout the graph
	gvLayout(gvc, graph, "dot");

	// Render the graph to a file
	gvRenderFilename(gvc, graph, "png", "syntaxTree.png");

	// Free up resources
	gvFreeLayout(gvc, graph);
	agclose(graph);
	gvFreeContext(gvc);
}


inline Agnode_t* DrawStmtSequenceSyntaxTree(Agraph_t* graph)
{

	try {
		Agnode_t* nodeStatement1 = DrawStatementSyntaxTree(graph);
		Agnode_t* nodeStatement_i = nodeStatement1;
		if (!tokensSyntaxTree.empty()) {
			while (tokensSyntaxTree.back().type == tokenType::SEMICOLON) {
				matchToken(tokenType::SEMICOLON);
				Agnode_t* nodeStatement2 = DrawStatementSyntaxTree(graph);
				if (nodeStatement_i != nullptr && nodeStatement2 != nullptr)
				{
					Agraph_t* subgraph = agsubg(graph, const_cast<char*>(SubGraphCount.c_str()), 1);
					Agedge_t* edge = agedge(subgraph, nodeStatement_i, nodeStatement2, NULL, 1);
					agsafeset(subgraph, "rank", "same", "");
					SubGraphCount += "1";
				}
				if (tokensSyntaxTree.empty()) {
					break;
				}
				nodeStatement_i = nodeStatement2;
			}
		}
		return nodeStatement1;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}

}

inline Agnode_t* DrawStatementSyntaxTree(Agraph_t* graph)
{
	if (!tokensSyntaxTree.empty()) {
		switch (tokensSyntaxTree.back().type)
		{
		case tokenType::IF:
			return DrawIfStmtSyntaxTree(graph);
		case tokenType::REPEAT:
			return DrawRepeatStmtSyntaxTree(graph);
		case tokenType::IDENTIFIER:
			return DrawAssignStmtSyntaxTree(graph);
		case tokenType::READ:
			return DrawReadStmSyntaxTreet(graph);
		case tokenType::WRITE:
			return DrawWriteStmtSyntaxTree(graph);
		default:
			return nullptr;
		}
	}
	return nullptr;
}

inline Agnode_t* DrawIfStmtSyntaxTree(Agraph_t* graph)
{
	try {
		std::string ifValue = matchToken(tokenType::IF);
		Agnode_t* nodeIf = agnode(graph, (char*)nodeCount.c_str(), 1);
		nodeCount += "1";
		agsafeset(nodeIf, (char*)"label", (char*)ifValue.c_str(), (char*)"");
		agsafeset(nodeIf, (char*)"shape", (char*)"box", (char*)"");
		Agnode_t* nodeExp = DrawExpSyntaxTree(graph);
		matchToken(tokenType::THEN);
		Agnode_t* nodeStmtSequence1 = DrawStmtSequenceSyntaxTree(graph);
		ADD_EDGE(graph, nodeIf, nodeExp);
		ADD_EDGE(graph, nodeIf, nodeStmtSequence1);
		if (!tokensSyntaxTree.empty()) {
			if (tokensSyntaxTree.back().type == tokenType::ELSE)
			{
				agattr(graph, AGRAPH, (char*)"ordering", (char*)"out");
				std::string elseValue = matchToken(tokenType::ELSE);
				Agnode_t* nodeElse = agnode(graph, (char*)elseValue.c_str(), 1);
				agsafeset(nodeElse, (char*)"shape", (char*)"box", (char*)"");
				Agnode_t* nodeStmtSequence2 = DrawStmtSequenceSyntaxTree(graph);
				ADD_EDGE(graph, nodeElse, nodeStmtSequence2);
				ADD_EDGE(graph, nodeIf, nodeElse);
			}
		}
		matchToken(tokenType::END);
		return nodeIf;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawRepeatStmtSyntaxTree(Agraph_t* graph)
{
	try {
		std::string repeatValue = matchToken(tokenType::REPEAT);
		Agnode_t* nodeRepeat = agnode(graph, (char*)nodeCount.c_str(), 1);
		nodeCount += "1";
		agsafeset(nodeRepeat, (char*)"label", (char*)repeatValue.c_str(), (char*)"");
		agsafeset(nodeRepeat, (char*)"shape", (char*)"box", (char*)"");
		Agnode_t* nodeStmtSequence1 = DrawStmtSequenceSyntaxTree(graph);
		matchToken(tokenType::UNTIL);
		Agnode_t* nodeExp = DrawExpSyntaxTree(graph);
		ADD_EDGE(graph, nodeRepeat, nodeStmtSequence1);
		ADD_EDGE(graph, nodeRepeat, nodeExp);
		return nodeRepeat;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawAssignStmtSyntaxTree(Agraph_t* graph)
{
	try {
		std::string identifierValue = matchToken(tokenType::IDENTIFIER);
		std::string assignValue = "assign";
		std::string assignString = assignValue + "\n" + "(" + identifierValue + ")";
		Agnode_t* nodeAssign = agnode(graph, (char*)nodeCount.c_str(), 1);
		nodeCount += "1";
		agsafeset(nodeAssign, (char*)"label", (char*)assignString.c_str(), (char*)"");
		agsafeset(nodeAssign, (char*)"shape", (char*)"box", (char*)"");
		matchToken(tokenType::ASSIGN);
		Agnode_t* nodeExp = DrawExpSyntaxTree(graph);
		ADD_EDGE(graph, nodeAssign, nodeExp);
		return nodeAssign;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawReadStmSyntaxTreet(Agraph_t* graph)
{
	try {
		std::string readValue = matchToken(tokenType::READ);
		std::string identifierValue = matchToken(tokenType::IDENTIFIER);
		std::string readString = readValue + "\n" + "(" + identifierValue + ")";
		Agnode_t* nodeRead = agnode(graph, (char*)nodeCount.c_str(), 1);
		nodeCount += "1";
		agsafeset(nodeRead, (char*)"label", (char*)readString.c_str(), (char*)"");
		agsafeset(nodeRead, (char*)"shape", (char*)"box", (char*)"");
		return nodeRead;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawWriteStmtSyntaxTree(Agraph_t* graph)
{
	try {
		std::string writeValue = matchToken(tokenType::WRITE);
		Agnode_t* nodeWrite = agnode(graph, (char*)nodeCount.c_str(), 1);
		nodeCount += "1";
		agsafeset(nodeWrite, (char*)"label", (char*)writeValue.c_str(), (char*)"");
		agsafeset(nodeWrite, (char*)"shape", (char*)"box", (char*)"");
		Agnode_t* nodeExp = DrawExpSyntaxTree(graph);
		ADD_EDGE(graph, nodeWrite, nodeExp);
		return nodeWrite;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawExpSyntaxTree(Agraph_t* graph)
{
	try {
		Agnode_t* nodeSimpleExp1 = DrawSimpleExpSyntaxTree(graph);
		if (!tokensSyntaxTree.empty())
		{
			if (tokensSyntaxTree.back().type == tokenType::LESSTHAN || tokensSyntaxTree.back().type == tokenType::EQUAL)
			{
				Agnode_t* nodeComparisonop = DrawComparisonOpSyntaxTree(graph);
				Agnode_t* nodeSimpleExp2 = DrawSimpleExpSyntaxTree(graph);
				ADD_EDGE(graph, nodeComparisonop, nodeSimpleExp1);
				ADD_EDGE(graph, nodeComparisonop, nodeSimpleExp2);
				return nodeComparisonop;
			}
		}
		return nodeSimpleExp1;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawComparisonOpSyntaxTree(Agraph_t* graph)
{
	if (!tokensSyntaxTree.empty()) {
		std::string opValue;
		std::string ComparisonValue;
		std::string ComparisonString;
		Agnode_t* nodeComparison;
		switch (tokensSyntaxTree.back().type)
		{
		case tokenType::LESSTHAN:
			opValue = "op";
			ComparisonValue = matchToken(tokenType::LESSTHAN);
			ComparisonString = opValue + "\n" + "(" + ComparisonValue + ")";
			nodeComparison = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeComparison, (char*)"label", (char*)ComparisonString.c_str(), (char*)"");
			return nodeComparison;
			break;
		case tokenType::EQUAL:
			opValue = "op";
			ComparisonValue = matchToken(tokenType::EQUAL);
			ComparisonString = opValue + "\n" + "(" + ComparisonValue + ")";
			nodeComparison = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeComparison, (char*)"label", (char*)ComparisonString.c_str(), (char*)"");
			return nodeComparison;
			break;
		default:
			return nullptr;
			break;
		}
	}
	return nullptr;
}

inline Agnode_t* DrawSimpleExpSyntaxTree(Agraph_t* graph)
{
	try {
		Agnode_t* nodeTerm1 = DrawTermSyntaxTree(graph);
		Agnode_t* nodeReturn = nodeTerm1;
		if (!tokensSyntaxTree.empty())
		{
			while (tokensSyntaxTree.back().type == tokenType::PLUS || tokensSyntaxTree.back().type == tokenType::MINUS)
			{
				Agnode_t* nodeAddop = DrawAddopSyntaxTree(graph);
				Agnode_t* nodeTerm2 = DrawTermSyntaxTree(graph);
				ADD_EDGE(graph, nodeAddop, nodeTerm1);
				ADD_EDGE(graph, nodeAddop, nodeTerm2);
				nodeTerm1 = nodeAddop;
				nodeReturn = nodeAddop;
				if (tokensSyntaxTree.empty()) {
					break;
				}
			}
		}
		return nodeReturn;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawAddopSyntaxTree(Agraph_t* graph)
{
	if (!tokensSyntaxTree.empty()) {
		std::string opValue;
		std::string AddValue;
		std::string AddString;
		Agnode_t* nodeAdd;
		switch (tokensSyntaxTree.back().type)
		{
		case tokenType::PLUS:
			opValue = "op";
			AddValue = matchToken(tokenType::PLUS);
			AddString = opValue + "\n" + "(" + AddValue + ")";
			nodeAdd = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeAdd, (char*)"label", (char*)AddString.c_str(), (char*)"");
			return nodeAdd;
			break;
		case tokenType::MINUS:
			opValue = "op";
			AddValue = matchToken(tokenType::MINUS);
			AddString = opValue + "\n" + "(" + AddValue + ")";
			nodeAdd = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeAdd, (char*)"label", (char*)AddString.c_str(), (char*)"");
			return nodeAdd;
			break;
		default:
			return nullptr;
			break;
		}
	}
	return nullptr;
}

inline Agnode_t* DrawTermSyntaxTree(Agraph_t* graph)
{
	try {
		Agnode_t* nodeFactor1 = DrawFactorSyntaxTree(graph);
		Agnode_t* nodeReturn = nodeFactor1;
		if (!tokensSyntaxTree.empty())
		{
			while (tokensSyntaxTree.back().type == tokenType::MULT || tokensSyntaxTree.back().type == tokenType::DIV)
			{
				Agnode_t* nodeMultop = DrawMulopSyntaxTree(graph);
				Agnode_t* nodeFactor2 = DrawFactorSyntaxTree(graph);
				ADD_EDGE(graph, nodeMultop, nodeFactor1);
				ADD_EDGE(graph, nodeMultop, nodeFactor2);
				nodeFactor1 = nodeMultop;
				nodeReturn = nodeMultop;
				if (tokensSyntaxTree.empty()) {
					break;
				}
			}
		}
		return nodeReturn;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}

inline Agnode_t* DrawMulopSyntaxTree(Agraph_t* graph)
{
	if (!tokensSyntaxTree.empty()) {
		std::string opValue;
		std::string MultValue;
		std::string MultString;
		Agnode_t* nodeMult;
		switch (tokensSyntaxTree.back().type)
		{
		case tokenType::MULT:
			opValue = "op";
			MultValue = matchToken(tokenType::MULT);
			MultString = opValue + "\n" + "(" + MultValue + ")";
			nodeMult = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeMult, (char*)"label", (char*)MultString.c_str(), (char*)"");
			return nodeMult;
			break;
		case tokenType::DIV:
			opValue = "op";
			MultValue = matchToken(tokenType::DIV);
			MultString = opValue + "\n" + "(" + MultValue + ")";
			nodeMult = agnode(graph, (char*)nodeCount.c_str(), 1);
			nodeCount += "1";
			agsafeset(nodeMult, (char*)"label", (char*)MultString.c_str(), (char*)"");
			return nodeMult;
			break;
		default:
			return nullptr;
			break;
		}
	}
	return nullptr;
}

inline Agnode_t* DrawFactorSyntaxTree(Agraph_t* graph)
{
	try {
		if (!tokensSyntaxTree.empty()) {
			std::string type;
			std::string value;
			std::string factorString;
			Agnode_t* nodeReturn;
			switch (tokensSyntaxTree.back().type) {
			case tokenType::OPENBRACKET:
				matchToken(tokenType::OPENBRACKET);
				nodeReturn = DrawExpSyntaxTree(graph);
				matchToken(tokenType::CLOSEDBRACKET);
				return nodeReturn;
				break;
			case tokenType::IDENTIFIER:
				type = "id";
				value = matchToken(tokenType::IDENTIFIER);
				factorString = type + "\n" + "(" + value + ")";
				nodeReturn = agnode(graph, (char*)nodeCount.c_str(), 1);
				nodeCount += "1";
				agsafeset(nodeReturn, (char*)"label", (char*)factorString.c_str(), (char*)"");
				return nodeReturn;
				break;
			case tokenType::NUMBER:
				type = "const";
				value = matchToken(tokenType::NUMBER);
				factorString = type + "\n" + "(" + value + ")";
				nodeReturn = agnode(graph, (char*)nodeCount.c_str(), 1);
				nodeCount += "1";
				agsafeset(nodeReturn, (char*)"label", (char*)factorString.c_str(), (char*)"");
				return nodeReturn;
				break;
			default:
				return nullptr;
			}
		}
		return nullptr;
	}
	catch (const std::exception& e) {
		// Handle exceptions and print an error message
		std::cerr << "Exception: " << e.what() << std::endl;
		return nullptr;
	}
}
