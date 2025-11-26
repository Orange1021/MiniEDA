/**
 * @file liberty_parser.h
 * @brief Liberty File Format Parser
 * @details Simple recursive descent parser for .lib files
 * Parses library, cell, pin, and timing data into data structures
 */

#ifndef MINI_LIBERTY_PARSER_H
#define MINI_LIBERTY_PARSER_H

#include <string>
#include <memory>
#include "../include/liberty.h"

namespace mini {

/**
 * @class LibertyParser
 * @brief Parser for Liberty (.lib) format files
 * @details Implements simple recursive descent parser for .lib syntax
 */
class LibertyParser {
public:
    /**
     * @brief Constructor
     */
    LibertyParser() = default;

    /**
     * @brief Destructor
     */
    ~LibertyParser() = default;

    // Disable copy, enable move
    LibertyParser(const LibertyParser&) = delete;
    LibertyParser& operator=(const LibertyParser&) = delete;
    LibertyParser(LibertyParser&&) = default;
    LibertyParser& operator=(LibertyParser&&) = default;

    /**
     * @brief Parse a .lib file
     * @param filename Path to .lib file
     * @return Pointer to parsed Library or nullptr on error
     */
    std::unique_ptr<Library> parseFile(const std::string& filename);

private:
    /**
     * @brief Internal parser state
     */
    struct ParserState {
        const char* current;        ///< Current position in text
        const char* end;            ///< End of text
        std::string filename;       ///< Current filename for error reporting
        int line_number;            ///< Current line number

        ParserState() : current(nullptr), end(nullptr), line_number(1) {}
    };

    ParserState state_;             ///< Parser state
    std::string error_message_;     ///< Last error message

    /**
     * @brief Parse library definition
     * @param name Library name
     * @return Parsed library
     */
    std::unique_ptr<Library> parseLibrary(const std::string& name);

    /**
     * @brief Parse cell definition
     * @param lib Parent library
     */
    void parseCell(Library* lib);

    /**
     * @brief Parse pin definition
     * @param cell Parent cell
     */
    void parsePin(LibCell* cell);

    /**
     * @brief Parse timing definition
     * @param pin Parent pin
     */
    void parseTiming(LibPin* pin);

    /**
     * @brief Parse values matrix into LookupTable
     * @param table Table to populate
     */
    void parseValues(LookupTable* table);

    /**
     * @brief Skip whitespace and comments
     */
    void skipWhitespace();

    /**
     * @brief Skip until a specific character
     * @param ch Character to skip to
     */
    void skipUntil(char ch);

    /**
     * @brief Expect and consume a specific token
     * @param expected Expected token string
     * @return true if matched
     */
    bool expect(const std::string& expected);

    /**
     * @brief Parse an identifier (name)
     * @return Parsed identifier or empty string
     */
    std::string parseIdentifier();

    /**
     * @brief Parse a quoted string
     * @return Parsed string (without quotes)
     */
    std::string parseQuotedString();

    /**
     * @brief Parse a number (double)
     * @return Parsed number
     */
    double parseNumber();

    /**
     * @brief Parse a list of numbers
     * @param numbers Vector to populate
     */
    void parseNumberList(std::vector<double>* numbers);

    /**
     * @brief Get current character
     */
    char peek() const;

    /**
     * @brief Consume current character
     */
    void consume();

    /**
     * @brief Check if at end of file
     */
    bool eof() const;

    /**
     * @brief Report error with context
     * @param message Error message
     */
    void error(const std::string& message);
};

} // namespace mini

#endif // MINI_LIBERTY_PARSER_H
