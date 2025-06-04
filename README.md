# PyLexSyn

## Overview
This is a parser that takes a source file and performs lexical analysis (breaks code into individual tokens such as keywords and identifiers), and syntax analysis which checks the syntax of the code. It reports both lexical and syntax errors found within the code.

## Defined Grammar
The following language constructs are supported:
- **Conditional statements:** `if`, `elif`, `else`
- **Iterative statements:** `while`, `for`
- **Assignment statements:** e.g., `x = 2`
- **Print statement:** `print()`
- **Input statement:** `input()`
- **Arithmetic expressions**
- **Comparison expressions**

## Screenshots

<p align="center">
    <img src="https://github.com/kentlance/PyLexSyn/blob/master/Screenshots/SymbolTable.png" alt="Symbols Table" width="600">
    <br><strong>Symbols Table</strong>
</p>

<p align="center">
    <img src="https://github.com/kentlance/PyLexSyn/blob/master/Screenshots/Lexems.png" alt="Lexemes" width="600">
    <br><strong>Lexemes</strong>
</p>

<p align="center">
    <img src="https://github.com/kentlance/PyLexSyn/blob/master/Screenshots/Error.png" alt="Error Detection" width="600">
    <br><strong>Error Detection</strong>
</p>
