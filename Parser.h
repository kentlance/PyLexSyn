#ifndef PARSER_H
#define PARSER_H

#include <vector>
#include <string>
#include "Token.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"

// Forward declaration of any AST node classes if you implement them
// class ExpressionNode;
// class StatementNode;

class Parser {
private:
    const std::vector<Token>& tokens;
    size_t currentTokenIndex;
    SymbolTable& symbolTable;
    ErrorHandler& errorHandler;

    // Current token being processed
    Token currentToken() const;
    Token peekNextToken() const;
    Token consume(TokenType expectedType);
    bool match(TokenType expectedType);
    void synchronize(); // Error recovery

    // Parsing functions for grammar rules
    void parseProgram();
    void parseStatement();
    void parseDeclarativeStatement(); // For variable declarations (like "x = 72" after first declaration)
    void parseAssignmentStatement(); // For variable assignments (x = y + 1)
    void parseArithmeticOperation(); // For expressions like "a + b * c"
    void parseConditionalStatement(); // if, elif, else
    void parseIterativeStatement(); // for, while
    void parseExpression();        // Handle all expression types
    void parseComparison();        // Handle comparison operators (==, !=, <, etc.)
    void parseArithmeticExpression(); // Handle + and - operations

    void parseTerm();
    void parseFactor();
    void parsePrintStatement(); // For print()
    void parseInputStatement(); // For input()

    // Helper for error reporting
    void syntaxError(const std::string& message);

public:
    Parser(const std::vector<Token>& tokens, SymbolTable& symTab, ErrorHandler& errHandler);
    void parse();
};

#endif