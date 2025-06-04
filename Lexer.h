#ifndef LEXER_H
#define LEXER_H

#include <string>
#include <vector>
#include <map>
#include <fstream> // For reading file content
#include <sstream>

#include "Token.h"
#include "ErrorHandler.h"

class Lexer {
private:
    std::string sourceCode;
    size_t currentIndex;
    int currentLine;
    int currentCol;
    ErrorHandler& errorHandler;

    std::map<std::string, TokenType> keywords;
    std::map<char, TokenType> singleCharTokens;
    std::map<std::string, TokenType> multiCharTokens;

    // Helper functions
    char peek();
    char advance();
    void skipWhitespace();
    Token identifyIdentifierOrKeyword();
    Token identifyNumber();
    Token identifyString();
    Token identifyOperator();

public:
    Lexer(const std::string& code, ErrorHandler& handler);
    std::vector<Token> tokenize();

    // Getter for lexemes and tokens table
    void printLexemesAndTokens(const std::vector<Token>& tokens) const;
};

#endif