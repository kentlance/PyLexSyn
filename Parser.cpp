#include "Parser.h"
#include <iostream>
#include <stdexcept> // For std::runtime_error

// Returns the current token
Token Parser::currentToken() const {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    // Return an EOF (end of file) token if we're past the end
    return Token(TokenType::END_OF_FILE, "EOF", tokens.back().lineNumber, tokens.back().columnNumber);
}

// Peeks at the next token without consuming and moving on
Token Parser::peekNextToken() const {
    if (currentTokenIndex + 1 < tokens.size()) {
        return tokens[currentTokenIndex + 1];
    }
    return Token(TokenType::END_OF_FILE, "EOF", tokens.back().lineNumber, tokens.back().columnNumber);
}

// Consumes the current token if it matches the expected type, otherwise reports an error
Token Parser::consume(TokenType expectedType) {
    Token current = currentToken();
    if (current.type == expectedType) {
        currentTokenIndex++;
        return current;
    } else {
        std::string expectedTypeName;
        switch (expectedType) {
            case TokenType::IDENTIFIER: expectedTypeName = "IDENTIFIER"; break;
            case TokenType::ASSIGN: expectedTypeName = "ASSIGN (=)"; break;
            case TokenType::LPAREN: expectedTypeName = "LPAREN"; break;
            case TokenType::RPAREN: expectedTypeName = "RPAREN"; break;
            case TokenType::COLON: expectedTypeName = "COLON"; break;
            case TokenType::END_OF_FILE: expectedTypeName = "END_OF_FILE"; break;
            case TokenType::IF: expectedTypeName = "IF"; break;
            case TokenType::ELSE: expectedTypeName = "ELSE"; break;
            case TokenType::ELIF: expectedTypeName = "ELIF"; break;
            case TokenType::WHILE: expectedTypeName = "WHILE"; break;
            case TokenType::FOR: expectedTypeName = "FOR"; break;
            case TokenType::PRINT: expectedTypeName = "PRINT"; break;
            case TokenType::INPUT: expectedTypeName = "INPUT"; break;
            case TokenType::PLUS: expectedTypeName = "PLUS (+)"; break;
            case TokenType::MINUS: expectedTypeName = "MINUS (-)"; break;
            case TokenType::MULTIPLY: expectedTypeName = "MULTIPLY (*)"; break;
            case TokenType::DIVIDE: expectedTypeName = "DIVIDE (/)"; break;
            case TokenType::MODULO: expectedTypeName = "MODULO (%)"; break;
            case TokenType::EQUAL_EQUAL: expectedTypeName = "EQUAL_EQUAL (==)"; break;
            case TokenType::NOT_EQUAL: expectedTypeName = "NOT_EQUAL (!=)"; break;
            case TokenType::LESS_THAN: expectedTypeName = "LESS_THAN (<)"; break;
            case TokenType::LESS_EQUAL: expectedTypeName = "LESS_EQUAL (<=)"; break;
            case TokenType::GREATER_THAN: expectedTypeName = "GREATER_THAN (>)"; break;
            case TokenType::GREATER_EQUAL: expectedTypeName = "GREATER_EQUAL (>=)"; break;
            case TokenType::AND: expectedTypeName = "AND"; break;
            case TokenType::OR: expectedTypeName = "OR"; break;
            case TokenType::NOT: expectedTypeName = "NOT"; break;
            case TokenType::INTEGER_LITERAL: expectedTypeName = "INTEGER_LITERAL"; break;
            case TokenType::FLOAT_LITERAL: expectedTypeName = "FLOAT_LITERAL"; break;
            case TokenType::STRING_LITERAL: expectedTypeName = "STRING_LITERAL"; break;
            case TokenType::BOOLEAN_LITERAL: expectedTypeName = "BOOLEAN_LITERAL"; break;
            // can add more cases
            default: expectedTypeName = "UNKNOWN_EXPECTED_TYPE"; break;
        }

        syntaxError("Expected " + expectedTypeName +
                    " but found '" + current.lexeme + "' (type: " +
                    std::to_string(static_cast<int>(current.type)) + ")");
        currentTokenIndex++; // Advance past the error token
        return Token(TokenType::UNKNOWN, "ERROR", current.lineNumber, current.columnNumber); // Return an error token
    }
}

// Checks if the current token matches the expected type without consuming
bool Parser::match(TokenType expectedType) {
    return currentToken().type == expectedType;
}


// Skip tokens until a likely statement boundary is found.
// This attempts to find a token that typically starts a new statement.
void Parser::synchronize() {
    while (currentToken().type != TokenType::END_OF_FILE) {
        switch (currentToken().type) {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::PRINT:
            case TokenType::INPUT:
            case TokenType::IDENTIFIER: 
                return;
            default:
                // Skip the current token and check the next one
                currentTokenIndex++;
                break;
        }
    }
}

// Reports a syntax error using the error handler
void Parser::syntaxError(const std::string& message) {
    errorHandler.reportError(message, currentToken().lineNumber, currentToken().columnNumber, "Syntax");
}

// Constructor
Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symTab, ErrorHandler& errHandler)
    : tokens(tokens), currentTokenIndex(0), symbolTable(symTab), errorHandler(errHandler) {}

// Main Parsin
void Parser::parse() {
    std::cout << "\nStarting syntax analysis..." << std::endl;
    parseProgram();
    if (errorHandler.hasErrors()) {
        std::cout << "Syntax analysis completed with errors." << std::endl;
    } else {
        std::cout << "Syntax analysis completed successfully." << std::endl;
    }
}

// GRAMMAR RULE IMPLEMENTATION

// Python uses newline to separate statements. For simplification, we will assume a newline or EOF signifies an end of a statement.
void Parser::parseProgram() {
    while (currentToken().type != TokenType::END_OF_FILE && !errorHandler.hasErrors()) {
        parseStatement();
        // After parsing a statement, consume any trailing newlines.
        // Indentation is currently not yet handled
        while (match(TokenType::END_OF_FILE) == false && tokens[currentTokenIndex].lexeme == "\n") {
            currentTokenIndex++; // Consume newline
        }
    }
}

// Statements: DeclarativeStatement | AssignmentStatement | ArithmeticOperation |
//              ConditionalStatement | IterativeStatement | PrintStatement | InputStatement
void Parser::parseStatement() {
    // Check for each possible statement type based on the lookahead (peek) token.
    if (match(TokenType::IF)) {
        parseConditionalStatement();
    } else if (match(TokenType::WHILE) || match(TokenType::FOR)) {
        parseIterativeStatement();
    } else if (match(TokenType::PRINT)) {
        parsePrintStatement();
    } else if (match(TokenType::INPUT)) {
        parseInputStatement();
    } else if (match(TokenType::IDENTIFIER)) {
        // If it's an IDENTIFIER, it could be an assignment or part of an expression.
        // Look at the next token to differentiate.
        if (peekNextToken().type == TokenType::ASSIGN) {
            parseAssignmentStatement();
        } else {
            // Assume it's an expression statement (like "a + b", which is valid)
            parseExpression();
        }
    } else {
        // If it doesn't match any known statement start, it's a syntax error.
        syntaxError("Unexpected token at start of statement: '" + currentToken().lexeme + "'");
        synchronize(); // Attempt to recover
    }
}

// Declaration often happens in initialization for example: x = 10
void Parser::parseDeclarativeStatement() {
    // This is for understanding the declaration in Python. Actual implementation for Python is handled within parseAssignmentStatement
}

// AssignmentStatement that uses IDENTIFIER "=" Expression
void Parser::parseAssignmentStatement() {
    Token identifier = consume(TokenType::IDENTIFIER);
    if (errorHandler.hasErrors()) { synchronize(); return; } // Error recovery

    // If identifier not found, declare it with a generic type (dynamic)
    if (symbolTable.search(identifier.lexeme) == SymbolTable::SymTabPos::NOT_FOUND) {
        symbolTable.insert(identifier.lexeme, "dynamic", 0, 0, identifier.lineNumber);
    } else {
        // If already exists, record usage
        symbolTable.addLineOfUsage(identifier.lexeme, identifier.lineNumber);
    }

    consume(TokenType::ASSIGN);
    if (errorHandler.hasErrors()) { synchronize(); return; }

    parseExpression(); // Parse the value being assigned
}

// ArithmeticOperation -> Expression
void Parser::parseArithmeticOperation() {
    parseExpression();
}

void Parser::parseConditionalStatement() {
    consume(TokenType::IF);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    parseExpression(); // Condition for 'if'
    consume(TokenType::COLON);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    parseStatement(); // IF body
    // Handle 'elif'
    while (match(TokenType::ELIF)) {
        consume(TokenType::ELIF);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseExpression(); // Condition for 'elif'
        consume(TokenType::COLON);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseStatement(); // ELIF body
    }

    // Handle 'else'
    if (match(TokenType::ELSE)) {
        consume(TokenType::ELSE);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        consume(TokenType::COLON);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseStatement(); // ELSE body
    }
}

// IterativeStatement: "while" Expression ":" StatementBlock | "for" IDENTIFIER "in" IDENTIFIER_OR_RANGE_CALL ":" StatementBlock
void Parser::parseIterativeStatement() {
    if (match(TokenType::WHILE)) {
        consume(TokenType::WHILE);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseExpression(); // Loop condition
        consume(TokenType::COLON);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseStatement(); // WHILE body
    } else if (match(TokenType::FOR)) {
        consume(TokenType::FOR);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        Token loopVar = consume(TokenType::IDENTIFIER);
        if (errorHandler.hasErrors()) { synchronize(); return; }

        if (symbolTable.search(loopVar.lexeme) == SymbolTable::SymTabPos::NOT_FOUND) {
            symbolTable.insert(loopVar.lexeme, "dynamic", 0, 0, loopVar.lineNumber);
        } else {
            symbolTable.addLineOfUsage(loopVar.lexeme, loopVar.lineNumber);
        }
        syntaxError("Simple 'for' loop syntax `for IDENTIFIER in ITERABLE` not fully implemented. Expected 'in' followed by iterable.");
        synchronize(); // Basic error recovery to advance

        consume(TokenType::COLON);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseStatement(); // FOR body
    } else {
        // This case should ideally not be reached if the outer `parseStatement` correctly directs here.
        syntaxError("Internal error: Expected 'while' or 'for'.");
        synchronize();
    }
}

// Expression: Comparison (("and" | "or") Comparison)*
void Parser::parseExpression() {
    parseComparison();
    while (match(TokenType::AND) || match(TokenType::OR)) {
        consume(currentToken().type); // Consume 'and' or 'or'
        parseComparison();
    }
}

// Comparison: ArithmeticExpression ( ("==" | "!=" | "<" | "<=" | ">" | ">=") ArithmeticExpression )*
void Parser::parseComparison() {
    parseArithmeticExpression();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL) ||
           match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL)) {
        consume(currentToken().type); // Consume the comparison operator
        parseArithmeticExpression();
    }
}

// ArithmeticExpression: Term ( ("+" | "-") Term )*
void Parser::parseArithmeticExpression() {
    parseTerm();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        consume(currentToken().type); // Consume "+" or "-"
        parseTerm();
    }
}

// Term: Factor ( ("*" | "/" | "%") Factor )*
void Parser::parseTerm() {
    parseFactor();
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        consume(currentToken().type); // Consume "*", "/", or "%"
        parseFactor();
    }
}

// Factor: ("+" | "-"")* (INTEGER_LITERAL | FLOAT_LITERAL | STRING_LITERAL | BOOLEAN_LITERAL | IDENTIFIER | "(" Expression ")" | "input" "(" [STRING_LITERAL] ")")
void Parser::parseFactor() {
    // Handle plus/minus/not
    if (match(TokenType::PLUS) || match(TokenType::MINUS) || match(TokenType::NOT)) {
        consume(currentToken().type);
    }

    if (match(TokenType::INTEGER_LITERAL) || match(TokenType::FLOAT_LITERAL) ||
        match(TokenType::STRING_LITERAL) || match(TokenType::BOOLEAN_LITERAL)) {
        consume(currentToken().type); // Consume the literal
    } else if (match(TokenType::IDENTIFIER)) {
        // If it's an identifier, ensure it's in the symbol table (or report error if undeclared)
        Token idToken = consume(TokenType::IDENTIFIER);
        if (symbolTable.search(idToken.lexeme) == SymbolTable::SymTabPos::NOT_FOUND) {
            errorHandler.reportError("Undeclared identifier: " + idToken.lexeme, idToken.lineNumber, idToken.columnNumber, "Syntax");
        } else {
            symbolTable.addLineOfUsage(idToken.lexeme, idToken.lineNumber);
        }
    } else if (match(TokenType::LPAREN)) {
        consume(TokenType::LPAREN);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseExpression();
        consume(TokenType::RPAREN);
        if (errorHandler.hasErrors()) { synchronize(); return; }
    }
    else if (match(TokenType::INPUT)) {
        // Handle input() as a factor that returns a value
        consume(TokenType::INPUT);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        consume(TokenType::LPAREN);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        if (match(TokenType::STRING_LITERAL)) {
            consume(TokenType::STRING_LITERAL); // Optional prompt string
        }
        consume(TokenType::RPAREN);
        if (errorHandler.hasErrors()) { synchronize(); return; }
    }
    else {
        syntaxError("Expected an expression, literal, identifier, '(', or 'input()' call, but found '" + currentToken().lexeme + "'");
        synchronize(); // Attempt to recover
    }
}

// PrintStatement: "print" "(" Expression ")"
void Parser::parsePrintStatement() {
    consume(TokenType::PRINT);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    consume(TokenType::LPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    parseExpression(); // The expression to print
    consume(TokenType::RPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
}

// InputStatement: "input" "(" [STRING_LITERAL] ")"
void Parser::parseInputStatement() {
    consume(TokenType::INPUT);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    consume(TokenType::LPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    if (match(TokenType::STRING_LITERAL)) {
        consume(TokenType::STRING_LITERAL); // Optional prompt string
    }
    consume(TokenType::RPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
}