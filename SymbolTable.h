#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <vector>
#include <string>
#include <iostream> // For printing symbol table

// You can further refine STEntry for Python-specific attributes
struct STEntry {
    std::string name;             // lexeme (identifier name)
    std::string dataType;         // data type (int, float, etc)
    size_t size;                  // size of the data type (4 for int, 8 for float)
    size_t dimension;             // dimension (for arrays/lists)
    int lineOfDeclaration;        // line number where declared
    // int lineOfUsage;           // track multiple usages, maybe a vector<int>
    std::vector<int> linesOfUsage; // Store all lines where the variable is used

    // Constructor
    STEntry(const std::string& name, const std::string& dataType, size_t size,
            size_t dimension, int lineDecl)
        : name(name), dataType(dataType), size(size), dimension(dimension),
          lineOfDeclaration(lineDecl) {}
};

class SymbolTable {
private:
    std::vector<STEntry> entries;

public:
    enum class SymTabPos { NOT_FOUND = -1 };

    // Inserts a new entry into the symbol table
    // Returns true if inserted successfully, false if name already exists
    bool insert(const std::string& name, const std::string& dataType, size_t size,
                size_t dimension, int lineOfDeclaration);

    // Search for an entry by name then returns its position (index)
    SymTabPos search(const std::string& name) const;

    // Updates the data type of an existing entry (useful for inferred types in Python)
    void updateDataType(const std::string& name, const std::string& newDataType);

    // Add a line of usage to an existing entry
    void addLineOfUsage(const std::string& name, int lineNum);

    // Prints the symbol table contents
    void printTable() const;

    // Getter for entries
    const std::vector<STEntry>& getEntries() const {
        return entries;
    }
};

#endif