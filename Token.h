#ifndef TOKEN_H
#define TOKEN_H

#include <string>

// Define token types (keywords, operators, identifiers, literals)
enum class TokenType {
    DECLARE,        // for variable declarations
    PRINT,          // print()
    INPUT,          // input()
    IF,             // if
    ELSE,           // else
    ELIF,           // elif 
    WHILE,          // while 
    FOR,            // for 
    DEF,            // def
    RETURN,         // return

    // Operators
    PLUS,           // +
    MINUS,          // -
    MULTIPLY,       // *
    DIVIDE,         // /
    MODULO,         // %
    ASSIGN,         // =
    EQUAL_EQUAL,    // ==
    NOT_EQUAL,      // !=
    LESS_THAN,      // <
    LESS_EQUAL,     // <=
    GREATER_THAN,   // >
    GREATER_EQUAL,  // >=
    AND,            // and
    OR,             // or
    NOT,            // not

    // Delimiters 
    LPAREN,         // (
    RPAREN,         // )
    LBRACE,         // {
    RBRACE,         // }
    LBRACKET,       // [
    RBRACKET,       // ]
    COMMA,          // ,
    COLON,          // :
    SEMICOLON,      // ; 
    DOT,            // .

    // Literals
    INTEGER_LITERAL,
    FLOAT_LITERAL,
    STRING_LITERAL,
    BOOLEAN_LITERAL,

    // Identifiers
    IDENTIFIER,

    // End of File
    END_OF_FILE,

    // Error Token
    UNKNOWN
};

// Structure to hold token information
struct Token {
    TokenType type;
    std::string lexeme;
    int lineNumber;
    int columnNumber;

    // Constructor for convenience
    Token(TokenType type, const std::string& lexeme, int line, int col)
        : type(type), lexeme(lexeme), lineNumber(line), columnNumber(col) {}
};

#endif