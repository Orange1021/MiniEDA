/**
 * @file liberty_parser.h
 * @brief Liberty File Format Parser
 * @details Robust recursive descent parser for .lib files with hierarchical structure
 */

#ifndef MINI_LIBERTY_PARSER_H
#define MINI_LIBERTY_PARSER_H

#include "liberty.h"
#include <string>
#include <memory>
#include <vector>

namespace mini {

/**
 * @class LibertyParser
 * @brief Robust Parser for Liberty (.lib) format files
 * @details Implements hierarchical parser with proper Group/Attribute distinction
 */
class LibertyParser {
public:
    LibertyParser();
    ~LibertyParser();

    // Main interface
    std::unique_ptr<Library> parseFile(const std::string& filename);
    std::string getError() const { return error_message_; }

private:
    // Parser state machine
    struct ParserState {
        const char* current;
        const char* end;
        size_t line_number;
        std::string filename;
        
        ParserState() : current(nullptr), end(nullptr), line_number(1) {}
    } state_;

    std::string error_message_;
    bool verbose_ = false;

    // --- Core parsing methods ---
    
    // 1. Generic parser: handle Groups (like cell, pin, library)
    //    Format: group_name ( arg1, arg2... ) { body }
    bool parseGroupHeader(std::string& group_type, std::string& group_name);
    
    // 2. Specific hierarchical parsing
    void parseLibraryBody(Library* lib);
    void parseCellBody(LibCell* cell);
    void parsePinBody(LibPin* pin);
    void parseTimingBody(LibTiming* timing);
    void parseConstraintBody(LibConstraint* constraint, const std::string& constraint_type);
    void parseTimingBodyWithConstraints(LibPin* pin);
    
    // 3. Value parsing
    void parseValuesTable(LookupTable* table);
    
    // --- Helper methods ---
    void skipWhitespace();
    void skipComment();  // Handle // and /* */ comments
    void skipGroup();    // Skip unrecognized group { ... }
    void skipAttribute();// Skip unrecognized attr : ... ;
    
    std::string readIdentifier(); // Read alphanumeric and underscore
    std::string readString();     // Read "..."
    double readNumber();
    bool match(const char* expected); // Similar to expect
    bool check(char c); // Similar to peek == c
    
    // Legacy methods for compatibility
    char peek() const;
    void consume();
    bool eof() const;
    void error(const std::string& message);
    
    // Backward compatibility methods
    bool expect(const std::string& expected);
    std::string parseIdentifier();
    std::string parseQuotedString();
    double parseNumber();
    void parseValues(LookupTable* table);
    void skipUntil(char ch);
    void parseNumberList(std::vector<double>* numbers);
    void parseCell(Library* lib);
    void parsePin(LibCell* cell);
    void parseTiming(LibPin* pin);
    
    // [NEW] Unified timing parser
    void parseTimingBody(LibPin* pin);
    
    // [NEW] Helper to read values that might be quoted or unquoted
    std::string readValue();
};

} // namespace mini

#endif
