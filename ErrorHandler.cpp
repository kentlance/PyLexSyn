#include "ErrorHandler.h"

void ErrorHandler::reportError(const std::string& message, int lineNumber, int columnNumber, const std::string& type) {
    errors.emplace_back(message, lineNumber, columnNumber, type);
    hasErrorsFlag = true;
}

bool ErrorHandler::hasErrors() const {
    return hasErrorsFlag;
}

void ErrorHandler::printErrors() const {
    if (!errors.empty()) {
        std::cerr << "\n--- Errors Encountered ---" << std::endl;
        for (const auto& err : errors) {
            std::cerr << err.type << " Error at Line " << err.lineNumber
                      << ", Column " << err.columnNumber << ": " << err.message << std::endl;
        }
        std::cerr << "--------------------------" << std::endl;
    }
}

void ErrorHandler::clearErrors() {
    errors.clear();
    hasErrorsFlag = false;
}