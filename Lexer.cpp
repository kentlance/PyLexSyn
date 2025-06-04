#include "Lexer.h"
#include <cctype> // For isalpha, isdigit, isalnum
#include <iomanip>

// Constructor
Lexer::Lexer(const std::string& code, ErrorHandler& handler)
    : sourceCode(code), currentIndex(0), currentLine(1), currentCol(1), errorHandler(handler) {
    // Initialize keywords for Python
    keywords["if"] = TokenType::IF;
    keywords["else"] = TokenType::ELSE;
    keywords["elif"] = TokenType::ELIF; 
    keywords["while"] = TokenType::WHILE;
    keywords["for"] = TokenType::FOR;
    keywords["print"] = TokenType::PRINT;
    keywords["input"] = TokenType::INPUT;
    keywords["True"] = TokenType::BOOLEAN_LITERAL;
    keywords["False"] = TokenType::BOOLEAN_LITERAL;
    keywords["and"] = TokenType::AND;
    keywords["or"] = TokenType::OR;
    keywords["not"] = TokenType::NOT;

    // Initialize single character tokens
    singleCharTokens['+'] = TokenType::PLUS;
    singleCharTokens['-'] = TokenType::MINUS;
    singleCharTokens['*'] = TokenType::MULTIPLY;
    singleCharTokens['/'] = TokenType::DIVIDE;
    singleCharTokens['%'] = TokenType::MODULO;
    singleCharTokens['('] = TokenType::LPAREN;
    singleCharTokens[')'] = TokenType::RPAREN;
    singleCharTokens['['] = TokenType::LBRACKET;
    singleCharTokens[']'] = TokenType::RBRACKET;
    singleCharTokens[','] = TokenType::COMMA;
    singleCharTokens[':'] = TokenType::COLON;
    singleCharTokens['.'] = TokenType::DOT;
    singleCharTokens['='] = TokenType::ASSIGN;      
    singleCharTokens['<'] = TokenType::LESS_THAN;  
    singleCharTokens['>'] = TokenType::GREATER_THAN;
    // blocks not yet added, {}, need further logic constraints

    // Initialize multi-character tokens
    multiCharTokens["=="] = TokenType::EQUAL_EQUAL;
    multiCharTokens["!="] = TokenType::NOT_EQUAL;
    multiCharTokens["<="] = TokenType::LESS_EQUAL;
    multiCharTokens[">="] = TokenType::GREATER_EQUAL;
}

// Looks at the next character without advancing
char Lexer::peek() {
    if (currentIndex >= sourceCode.length()) {
        return '\0'; // End of file
    }
    return sourceCode[currentIndex];
}

// Advances the current index and returns the character
char Lexer::advance() {
    if (currentIndex >= sourceCode.length()) {
        return '\0';
    }
    char c = sourceCode[currentIndex];
    currentIndex++;
    currentCol++;
    return c;
}

// Skips whitespace characters and comments
void Lexer::skipWhitespace() {
    while (currentIndex < sourceCode.length()) {
        char c = peek();
        if (c == ' ' || c == '\t' || c == '\r') {
            advance();
        } else if (c == '\n') {
            advance();
            currentLine++;
            currentCol = 1;
        } else if (c == '#') { // Python comments
            while (currentIndex < sourceCode.length() && peek() != '\n' && peek() != '\0') {
                advance();
            }
        } else {
            break;
        }
    }
}

// Identifies identifiers or keywords
Token Lexer::identifyIdentifierOrKeyword() {
    std::string lexeme;
    int startCol = currentCol;
    while (isalnum(peek()) || peek() == '_') { // Python identifiers can contain letters, numbers, and underscores
        lexeme += advance();
    }

    // Check if it's a keyword
    if (keywords.count(lexeme)) {
        return Token(keywords[lexeme], lexeme, currentLine, startCol);
    } else {
        return Token(TokenType::IDENTIFIER, lexeme, currentLine, startCol);
    }
}

// Identifies numbers (integers and floats)
Token Lexer::identifyNumber() {
    std::string lexeme;
    int startCol = currentCol;
    bool isFloat = false;

    while (isdigit(peek())) {
        lexeme += advance();
    }

    if (peek() == '.' && isdigit(sourceCode[currentIndex + 1])) { // Check for fractional part
        isFloat = true;
        lexeme += advance(); // Consume '.'
        while (isdigit(peek())) {
            lexeme += advance();
        }
    }

    if (isFloat) {
        return Token(TokenType::FLOAT_LITERAL, lexeme, currentLine, startCol);
    } else {
        return Token(TokenType::INTEGER_LITERAL, lexeme, currentLine, startCol);
    }
}

// Identifies strings (enclosed in single or double quotes)
Token Lexer::identifyString() {
    char quoteChar = advance(); // Consume the opening quote (' or ")
    std::string lexeme;
    int startCol = currentCol - 1; // Start column of the quote

    while (peek() != quoteChar && peek() != '\0' && peek() != '\n') {
        lexeme += advance();
    }

    if (peek() == '\0' || peek() == '\n') {
        errorHandler.reportError("Unterminated string literal.", currentLine, startCol, "Lexical");
        return Token(TokenType::UNKNOWN, lexeme, currentLine, startCol); // Return an error token
    } else {
        advance(); // Consume the closing quote
        return Token(TokenType::STRING_LITERAL, lexeme, currentLine, startCol);
    }
}

// Identifies operators (single and multi-character)
Token Lexer::identifyOperator() {
    char c = advance();
    std::string lexeme(1, c);
    int startCol = currentCol - 1; // Column where the operator starts

    // Check for multi-character operators first (since we need to rule out multi-character first)
    // Compare current to next character
    if (c == '=' && peek() == '=') {
        lexeme += advance(); return Token(TokenType::EQUAL_EQUAL, lexeme, currentLine, startCol);
    } else if (c == '!' && peek() == '=') {
        lexeme += advance(); return Token(TokenType::NOT_EQUAL, lexeme, currentLine, startCol);
    } else if (c == '<' && peek() == '=') {
        lexeme += advance(); return Token(TokenType::LESS_EQUAL, lexeme, currentLine, startCol);
    } else if (c == '>' && peek() == '=') {
        lexeme += advance(); return Token(TokenType::GREATER_EQUAL, lexeme, currentLine, startCol);
    }

    // Check for single character tokens if it wasn't a multi-character operator
    if (singleCharTokens.count(c)) {
        return Token(singleCharTokens[c], lexeme, currentLine, startCol);
    }

    // If it's none of above, then it's an unknown character.
    errorHandler.reportError("Unknown character: '" + std::string(1, c) + "'", currentLine, startCol, "Lexical");
    return Token(TokenType::UNKNOWN, lexeme, currentLine, startCol);
}

// Main tokenization function
std::vector<Token> Lexer::tokenize() {
    std::vector<Token> tokens;
    while (currentIndex < sourceCode.length()) {
        skipWhitespace();

        if (currentIndex >= sourceCode.length()) {
            break; // Reached end after skipping whitespace
        }

        char c = peek();
        Token token(TokenType::UNKNOWN, "", currentLine, currentCol); // Default empty token

        if (isalpha(c) || c == '_') {
            token = identifyIdentifierOrKeyword();
        } else if (isdigit(c)) {
            token = identifyNumber();
        } else if (c == '\'' || c == '"') {
            token = identifyString();
        } else if (singleCharTokens.count(c) || c == '=' || c == '!' || c == '<' || c == '>') { // Check for operators
            token = identifyOperator();
        } else {
            errorHandler.reportError("Unexpected character: '" + std::string(1, c) + "'", currentLine, currentCol, "Lexical");
            advance(); // Consume the unknown character to avoid infinite loop
        }
        tokens.push_back(token);
    }

    tokens.emplace_back(TokenType::END_OF_FILE, "EOF", currentLine, currentCol); // Add EOF token
    return tokens;
}

// Prints the lexemes and tokens table
void Lexer::printLexemesAndTokens(const std::vector<Token>& tokens) const {
    std::cout << "\n--- Lexemes and Tokens Table ---" << std::endl;
    std::cout << std::left << std::setw(20) << "Lexeme"
              << std::setw(20) << "Token Type"
              << std::setw(10) << "Line"
              << std::setw(10) << "Column" << std::endl;
    std::cout << std::string(60, '-') << std::endl;

    for (const auto& token : tokens) {
        std::string tokenTypeName;
        switch (token.type) {
            case TokenType::IDENTIFIER: tokenTypeName = "IDENTIFIER"; break;
            case TokenType::INTEGER_LITERAL: tokenTypeName = "INTEGER_LITERAL"; break;
            case TokenType::FLOAT_LITERAL: tokenTypeName = "FLOAT_LITERAL"; break;
            case TokenType::STRING_LITERAL: tokenTypeName = "STRING_LITERAL"; break;
            case TokenType::BOOLEAN_LITERAL: tokenTypeName = "BOOLEAN_LITERAL"; break;
            case TokenType::PLUS: tokenTypeName = "PLUS"; break;
            case TokenType::MINUS: tokenTypeName = "MINUS"; break;
            case TokenType::MULTIPLY: tokenTypeName = "MULTIPLY"; break;
            case TokenType::DIVIDE: tokenTypeName = "DIVIDE"; break;
            case TokenType::MODULO: tokenTypeName = "MODULO"; break; // For %
            case TokenType::ASSIGN: tokenTypeName = "ASSIGN"; break; // For =
            case TokenType::EQUAL_EQUAL: tokenTypeName = "EQUAL_EQUAL"; break; // For ==
            case TokenType::NOT_EQUAL: tokenTypeName = "NOT_EQUAL"; break; // For !=
            case TokenType::LESS_THAN: tokenTypeName = "LESS_THAN"; break;
            case TokenType::LESS_EQUAL: tokenTypeName = "LESS_EQUAL"; break; // For <=
            case TokenType::GREATER_THAN: tokenTypeName = "GREATER_THAN"; break;
            case TokenType::GREATER_EQUAL: tokenTypeName = "GREATER_EQUAL"; break; // For >=
            case TokenType::AND: tokenTypeName = "AND"; break;
            case TokenType::OR: tokenTypeName = "OR"; break;
            case TokenType::NOT: tokenTypeName = "NOT"; break;
            case TokenType::LPAREN: tokenTypeName = "LPAREN"; break;
            case TokenType::RPAREN: tokenTypeName = "RPAREN"; break;
            case TokenType::LBRACE: tokenTypeName = "LBRACE"; break; 
            case TokenType::RBRACE: tokenTypeName = "RBRACE"; break;
            case TokenType::LBRACKET: tokenTypeName = "LBRACKET"; break;
            case TokenType::RBRACKET: tokenTypeName = "RBRACKET"; break;
            case TokenType::COMMA: tokenTypeName = "COMMA"; break;
            case TokenType::COLON: tokenTypeName = "COLON"; break; // For :
            case TokenType::SEMICOLON: tokenTypeName = "SEMICOLON"; break;
            case TokenType::DOT: tokenTypeName = "DOT"; break;
            case TokenType::IF: tokenTypeName = "IF"; break;
            case TokenType::ELSE: tokenTypeName = "ELSE"; break;
            case TokenType::ELIF: tokenTypeName = "ELIF"; break;
            case TokenType::WHILE: tokenTypeName = "WHILE"; break;
            case TokenType::FOR: tokenTypeName = "FOR"; break;
            case TokenType::PRINT: tokenTypeName = "PRINT"; break; 
            case TokenType::INPUT: tokenTypeName = "INPUT"; break;
            case TokenType::DEF: tokenTypeName = "DEF"; break; 
            case TokenType::RETURN: tokenTypeName = "RETURN"; break;
            case TokenType::END_OF_FILE: tokenTypeName = "END_OF_FILE"; break;
            case TokenType::UNKNOWN: tokenTypeName = "UNKNOWN"; break; 
            // more cases can be added
            default: tokenTypeName = "UNKNOWN_TYPE"; break; // Fallback for any unhandled types
        }

        std::cout << std::left << std::setw(20) << token.lexeme
                  << std::setw(20) << tokenTypeName
                  << std::setw(10) << token.lineNumber
                  << std::setw(10) << token.columnNumber << std::endl;
    }
    std::cout << std::string(60, '-') << std::endl;
}