#ifndef ERRORHANDLER_H
#define ERRORHANDLER_H

#include <string>
#include <vector>
#include <iostream>

struct Error {
    std::string message;
    int lineNumber;
    int columnNumber;
    std::string type; // "Lexical" or "Syntax"

    Error(const std::string& msg, int line, int col, const std::string& type)
        : message(msg), lineNumber(line), columnNumber(col), type(type) {}
};

class ErrorHandler {
private:
    std::vector<Error> errors;
    bool hasErrorsFlag;

public:
    ErrorHandler() : hasErrorsFlag(false) {}

    void reportError(const std::string& message, int lineNumber, int columnNumber, const std::string& type);
    bool hasErrors() const;
    void printErrors() const;
    void clearErrors(); // To allow parsing multiple files or attempts
};

#endif