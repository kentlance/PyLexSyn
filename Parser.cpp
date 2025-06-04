#include "Parser.h"
#include <iostream>
#include <stdexcept> // For std::runtime_error

// Returns the current token
Token Parser::currentToken() const {
    if (currentTokenIndex < tokens.size()) {
        return tokens[currentTokenIndex];
    }
    // Return an EOF token if we're past the end
    return Token(TokenType::END_OF_FILE, "EOF", tokens.back().lineNumber, tokens.back().columnNumber);
}

// Peeks at the next token without consuming
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
        // Prepare a string representation of the expected type for the error message.
        // This would ideally be a helper function or a map in a real project.
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
            // Add more cases for all TokenType enums for better error messages
            default: expectedTypeName = "UNKNOWN_EXPECTED_TYPE"; break;
        }

        syntaxError("Expected " + expectedTypeName +
                    " but found '" + current.lexeme + "' (type: " +
                    std::to_string(static_cast<int>(current.type)) + ")");
        // Attempt rudimentary error recovery by advancing
        // (More sophisticated recovery needed for robust parser)
        currentTokenIndex++; // Advance past the erroneous token
        return Token(TokenType::UNKNOWN, "ERROR", current.lineNumber, current.columnNumber); // Return an error token
    }
}

// Checks if the current token matches the expected type without consuming
bool Parser::match(TokenType expectedType) {
    return currentToken().type == expectedType;
}

// Rudimentary error synchronization
void Parser::synchronize() {
    // Skip tokens until a likely statement boundary is found.
    // This attempts to find a token that typically starts a new statement.
    while (currentToken().type != TokenType::END_OF_FILE) {
        switch (currentToken().type) {
            case TokenType::IF:
            case TokenType::WHILE:
            case TokenType::FOR:
            case TokenType::PRINT:
            case TokenType::INPUT:
            case TokenType::IDENTIFIER: // Could be assignment or function call
                return; // Found a likely start of a new statement
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

// --- Constructor ---
Parser::Parser(const std::vector<Token>& tokens, SymbolTable& symTab, ErrorHandler& errHandler)
    : tokens(tokens), currentTokenIndex(0), symbolTable(symTab), errorHandler(errHandler) {}

// --- Main Parsing Entry Point ---
void Parser::parse() {
    std::cout << "\nStarting syntax analysis..." << std::endl;
    parseProgram();
    if (errorHandler.hasErrors()) {
        std::cout << "Syntax analysis completed with errors." << std::endl;
    } else {
        std::cout << "Syntax analysis completed successfully." << std::endl;
    }
}

// --- Grammar Rule Implementations ---

// Program -> (Statement NEWLINE)* EOF
// Note: Python uses newlines to separate statements. For simplification,
// we'll assume a newline or EOF signifies the end of a statement.
void Parser::parseProgram() {
    while (currentToken().type != TokenType::END_OF_FILE && !errorHandler.hasErrors()) {
        parseStatement();
        // After parsing a statement, consume any trailing newlines.
        // In a real Python parser, you'd manage indentation here.
        // For simplicity, we'll just consume newlines.
        while (match(TokenType::END_OF_FILE) == false && tokens[currentTokenIndex].lexeme == "\n") {
            currentTokenIndex++; // Consume newline
            // Also reset column to 1 if we actually consume a newline.
            // This would be better handled by the Lexer during tokenization,
            // but for simple cases, we can adjust.
            // In a production lexer, newline would simply increment line and reset column.
        }
    }
}

// Statement -> DeclarativeStatement | AssignmentStatement | ArithmeticOperation |
//              ConditionalStatement | IterativeStatement | PrintStatement | InputStatement
void Parser::parseStatement() {
    // Check for each possible statement type based on the lookahead token.
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
            // Assume it's an expression statement (like `a + b`, which is valid in Python but results in no action if not assigned)
            // Or it could be a function call, if you extend your grammar.
            parseExpression();
        }
    } else {
        // If it doesn't match any known statement start, it's a syntax error.
        syntaxError("Unexpected token at start of statement: '" + currentToken().lexeme + "'");
        synchronize(); // Attempt to recover
    }
}

// DeclarativeStatement (Implicit for Python)
// In Python, declaration often happens with the first assignment: `x = 10`
// This function might not be directly called as a rule, but its logic integrated into Assignment.
void Parser::parseDeclarativeStatement() {
    // This rule is primarily for conceptual understanding of declaration in Python.
    // Actual implementation for Python is handled within `parseAssignmentStatement`
    // by checking if an identifier already exists in the symbol table.
    // If not found, it implies a new declaration.
}

// AssignmentStatement -> IDENTIFIER '=' Expression
void Parser::parseAssignmentStatement() {
    Token identifier = consume(TokenType::IDENTIFIER);
    if (errorHandler.hasErrors()) { synchronize(); return; } // Error recovery

    // If identifier not found, declare it with a generic type (dynamic)
    if (symbolTable.search(identifier.lexeme) == SymbolTable::SymTabPos::NOT_FOUND) {
        symbolTable.insert(identifier.lexeme, "dynamic", 0, 0, identifier.lineNumber);
    } else {
        // If already exists, just record usage
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

// ConditionalStatement -> 'if' Expression ':' StatementBlock
//                        | 'if' Expression ':' StatementBlock 'else' ':' StatementBlock
//                        | 'if' Expression ':' StatementBlock ('elif' Expression ':' StatementBlock)* [ 'else' ':' StatementBlock ]
// For simplicity, StatementBlock will be a single parseStatement call.
// In a real Python parser, you'd handle indentation to define blocks.
void Parser::parseConditionalStatement() {
    consume(TokenType::IF);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    parseExpression(); // Condition for 'if'
    consume(TokenType::COLON);
    if (errorHandler.hasErrors()) { synchronize(); return; }

    // This is where you would parse an indented block of statements for the 'if' body.
    // For this simplified parser, we'll assume it's followed by a single statement.
    parseStatement(); // IF body

    // Handle 'elif' chain
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

// IterativeStatement -> 'while' Expression ':' StatementBlock
//                    | 'for' IDENTIFIER 'in' IDENTIFIER_OR_RANGE_CALL ':' StatementBlock
// (Simplified for 'for' loop)
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

        // Add loop variable to symbol table (scope handling would be needed for real Python)
        if (symbolTable.search(loopVar.lexeme) == SymbolTable::SymTabPos::NOT_FOUND) {
            symbolTable.insert(loopVar.lexeme, "dynamic", 0, 0, loopVar.lineNumber);
        } else {
            symbolTable.addLineOfUsage(loopVar.lexeme, loopVar.lineNumber);
        }

        // For simplicity, let's assume 'in' is supported and then an IDENTIFIER
        // A real parser would handle `range(...)` or list literals.
        // You'd need a TokenType::IN if you add 'in' keyword.
        // Example: `for i in my_list:`
        // if (match(TokenType::IN)) { // Assuming TokenType::IN exists
        //     consume(TokenType::IN);
        //     consume(TokenType::IDENTIFIER); // e.g., 'my_list' or 'range_object'
        // } else {
        //    syntaxError("Expected 'in' keyword after loop variable in for statement.");
        //    synchronize(); return;
        // }
        // For the sake of this example, let's just assume `for i:` syntax as an extremely simplified version.
        // Or if you added a `range` function, you'd parse a function call here.
        // For now, if no `in` is present, it's an error.
        syntaxError("Simple 'for' loop syntax `for IDENTIFIER in ITERABLE` not fully implemented. Expected 'in' followed by iterable.");
        synchronize(); // Basic error recovery

        consume(TokenType::COLON);
        if (errorHandler.hasErrors()) { synchronize(); return; }
        parseStatement(); // FOR body
    } else {
        // This case should ideally not be reached if the outer `parseStatement` correctly directs here.
        syntaxError("Internal error: Expected 'while' or 'for'.");
        synchronize();
    }
}

// Expression -> Comparison (('and' | 'or') Comparison)*
void Parser::parseExpression() {
    parseComparison();
    while (match(TokenType::AND) || match(TokenType::OR)) {
        consume(currentToken().type); // Consume 'and' or 'or'
        parseComparison();
    }
}

// Comparison -> ArithmeticExpression ( ('==' | '!=' | '<' | '<=' | '>' | '>=') ArithmeticExpression )*
void Parser::parseComparison() {
    parseArithmeticExpression();
    while (match(TokenType::EQUAL_EQUAL) || match(TokenType::NOT_EQUAL) ||
           match(TokenType::LESS_THAN) || match(TokenType::LESS_EQUAL) ||
           match(TokenType::GREATER_THAN) || match(TokenType::GREATER_EQUAL)) {
        consume(currentToken().type); // Consume the comparison operator
        parseArithmeticExpression();
    }
}

// ArithmeticExpression -> Term ( ('+' | '-') Term )*
void Parser::parseArithmeticExpression() {
    parseTerm();
    while (match(TokenType::PLUS) || match(TokenType::MINUS)) {
        consume(currentToken().type); // Consume '+' or '-'
        parseTerm();
    }
}

// Term -> Factor ( ('*' | '/' | '%') Factor )*
void Parser::parseTerm() {
    parseFactor();
    while (match(TokenType::MULTIPLY) || match(TokenType::DIVIDE) || match(TokenType::MODULO)) {
        consume(currentToken().type); // Consume '*', '/', or '%'
        parseFactor();
    }
}

// Factor -> ('+' | '-')* (INTEGER_LITERAL | FLOAT_LITERAL | STRING_LITERAL | BOOLEAN_LITERAL | IDENTIFIER | '(' Expression ')' | 'input' '(' [STRING_LITERAL] ')')
void Parser::parseFactor() {
    // Handle unary plus/minus/not
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
        parseExpression(); // Recursively parse the expression inside parentheses
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

// PrintStatement -> 'print' '(' Expression ')'
void Parser::parsePrintStatement() {
    consume(TokenType::PRINT);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    consume(TokenType::LPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
    parseExpression(); // The expression to print
    consume(TokenType::RPAREN);
    if (errorHandler.hasErrors()) { synchronize(); return; }
}

// InputStatement -> 'input' '(' [STRING_LITERAL] ')'
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