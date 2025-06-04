// implementation of SymbolTable.h

#include "SymbolTable.h"
#include <iomanip>

bool SymbolTable::insert(const std::string& name, const std::string& dataType, size_t size,
                         size_t dimension, int lineOfDeclaration) {
    if (search(name) != SymTabPos::NOT_FOUND) {
        // Entry with this name already exists
        return false;
    }

    entries.emplace_back(name, dataType, size, dimension, lineOfDeclaration);
    return true;
}

SymbolTable::SymTabPos SymbolTable::search(const std::string& name) const {
    if (entries.empty()) {
        return SymTabPos::NOT_FOUND;
    }
    for (auto i = 0u; i < entries.size(); ++i) {
        if (name == entries.at(i).name) {
            return static_cast<SymTabPos>(i);
        }
    }
    return SymTabPos::NOT_FOUND;
}

//Updates the data type of an existing entry
void SymbolTable::updateDataType(const std::string& name, const std::string& newDataType) {
    SymTabPos pos = search(name);
    if (pos != SymTabPos::NOT_FOUND) {
        entries[static_cast<int>(pos)].dataType = newDataType;
    }
}

// Line of usage for an existing entry
void SymbolTable::addLineOfUsage(const std::string& name, int lineNum) {
    SymTabPos pos = search(name);
    if (pos != SymTabPos::NOT_FOUND) {
        entries[static_cast<int>(pos)].linesOfUsage.push_back(lineNum);
    }
}

void SymbolTable::printTable() const {
    std::cout << "\n--- Symbol Table ---" << std::endl;
    std::cout << std::left << std::setw(15) << "Name"
              << std::setw(10) << "Type"
              << std::setw(8) << "Size"
              << std::setw(12) << "Dimension"
              << std::setw(20) << "Decl. Line"
              << std::setw(20) << "Usage Lines" << std::endl;
    std::cout << std::string(85, '-') << std::endl;

    for (const auto& entry : entries) {
        std::cout << std::left << std::setw(15) << entry.name
                  << std::setw(10) << entry.dataType
                  << std::setw(8) << entry.size
                  << std::setw(12) << entry.dimension
                  << std::setw(20) << entry.lineOfDeclaration;

        std::string usageLinesStr = "";
        for (size_t i = 0; i < entry.linesOfUsage.size(); ++i) {
            usageLinesStr += std::to_string(entry.linesOfUsage[i]);
            if (i < entry.linesOfUsage.size() - 1) {
                usageLinesStr += ", ";
            }
        }
        std::cout << std::setw(20) << usageLinesStr << std::endl;
    }
    std::cout << std::string(85, '-') << std::endl;
}