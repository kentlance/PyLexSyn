#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

#include "Lexer.h"
#include "Parser.h"
#include "SymbolTable.h"
#include "ErrorHandler.h"
#include "Token.h"

std::string readSourceCode(const std::string& filename) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error: Failed to open file " << filename << std::endl;
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

int main() {
    std::cout << "PYTHON Parser Made Using C++ by Kenneth Lance L. Apolinar" << std::endl;
    // use path so that it's easier to test multiple files
    std::cout << "Enter path to Python source file: ";
    std::string filename;
    std::getline(std::cin, filename); // Get filename from user

    // Read the source code
    std::string sourceCode = readSourceCode(filename);

    if (sourceCode.empty()) {
        if (!filename.empty()) { // Only print error if user entered a filename
            std::cerr << "Exiting due to file read error." << std::endl;
        }
        return 1;
    }

    std::cout << "\n| Source Code Parsed |" << std::endl;
    std::cout << sourceCode << std::endl;
    std::cout << "--------------------------" << std::endl;

    // Initialize components
    ErrorHandler errorHandler;
    SymbolTable symbolTable;

    //  Lexical Analysis
    Lexer lexer(sourceCode, errorHandler);
    std::vector<Token> tokens = lexer.tokenize();

    // Print Lexemes and Tokens Table
    lexer.printLexemesAndTokens(tokens);

    if (errorHandler.hasErrors()) {
        errorHandler.printErrors();
        std::cout << "\nLexical errors found. Cannot proceed parsing." << std::endl;
        return 1;
    }

    // Syntax Analysis
    Parser parser(tokens, symbolTable, errorHandler);
    parser.parse();

    // Print Symbol Table
    symbolTable.printTable();

    // Final Error Report
    if (errorHandler.hasErrors()) {
        errorHandler.printErrors();
        std::cout << "\nParsing completed with errors." << std::endl;
    } else {
        std::cout << "\nParsing completed successfully with no errors!" << std::endl;
    }

    std::cout << "\nPress Enter to exit. Thank you for using!";
    std::cin.ignore(); // Consume the newline character left by previous std::getline
    std::cin.get();

    return 0;
}