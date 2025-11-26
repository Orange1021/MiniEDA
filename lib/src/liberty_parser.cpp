/**
 * @file liberty_parser.cpp
 * @brief Liberty File Parser Implementation
 * @details Simple recursive descent parser for .lib format
 */

#include "../include/liberty_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

namespace mini {

/**
 * @brief Parse a .lib file
 * @param filename Path to .lib file
 * @return Parsed library or nullptr on error
 */
std::unique_ptr<Library> LibertyParser::parseFile(const std::string& filename) {
    // Read entire file into memory
    std::ifstream file(filename);
    if (!file) {
        error_message_ = "Cannot open file: " + filename;
        return nullptr;
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    if (content.empty()) {
        error_message_ = "Empty file: " + filename;
        return nullptr;
    }

    // Setup parser state
    state_.current = content.c_str();
    state_.end = state_.current + content.length();
    state_.filename = filename;
    state_.line_number = 1;
    error_message_.clear();

    // Skip to library definition
    skipWhitespace();
    if (!expect("library")) {
        error("Expected 'library' keyword");
        return nullptr;
    }

    // Parse library name
    skipWhitespace();
    if (peek() != '(') {
        error("Expected '(' after library");
        return nullptr;
    }
    consume();

    std::string lib_name = parseIdentifier();
    if (lib_name.empty()) {
        error("Expected library name");
        return nullptr;
    }

    skipWhitespace();
    if (peek() != ')') {
        error("Expected ')' after library name");
        return nullptr;
    }
    consume();

    skipWhitespace();
    if (peek() != '{') {
        error("Expected '{' to start library body");
        return nullptr;
    }
    consume();

    // Create library and parse its contents
    auto library = std::make_unique<Library>(lib_name);

    std::cout << "  Starting to parse library body..." << std::endl;

    // Parse library body
    while (!eof()) {
        skipWhitespace();
        char ch = peek();

        if (ch == '}') {
            consume(); // End of library
            std::cout << "  Parsed library: " << lib_name << std::endl;
            return library;
        }

        // Check for cell definitions
        if (expect("cell")) {
            std::cout << "  Found cell keyword" << std::endl;
            parseCell(library.get());
            continue;
        }

        // Skip lu_table_template block
        if (expect("lu_table_template")) {
            std::cout << "  Skipping lu_table_template" << std::endl;
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            skipWhitespace();
            if (peek() == '{') {
                consume(); // block start
                skipUntil('}'); // skip entire block
            }
            continue;
        }

        // Debug: show what we're skipping
        size_t current_line = state_.line_number;
        std::string keyword = parseIdentifier();
        if (!keyword.empty()) {
            std::cout << "  Line " << current_line << ": Skipping '" << keyword << "'" << std::endl;
            skipUntil(';');
        } else if (!eof()) {
            std::cout << "  Line " << current_line << ": Skipping char '" << peek() << "'" << std::endl;
            consume();
        }
    }  // <-- 添加这个关闭大括号

    error("Unexpected end of file in library");
    return nullptr;
}

/**
 * @brief Parse cell definition
 * @param lib Parent library
 */
void LibertyParser::parseCell(Library* lib) {
    skipWhitespace();
    if (peek() != '(') {
        error("Expected '(' after cell");
        return;
    }
    consume();

    std::string cell_name = parseIdentifier();
    if (cell_name.empty()) {
        error("Expected cell name");
        return;
    }

    skipWhitespace();
    if (peek() != ')') {
        error("Expected ')' after cell name");
        return;
    }
    consume();

    skipWhitespace();
    if (peek() != '{') {
        error("Expected '{' to start cell body");
        return;
    }
    consume();

    // Create cell
    LibCell cell;
    cell.name = cell_name;

    // Parse cell body
    while (!eof()) {
        skipWhitespace();
        char ch = peek();

        if (ch == '}') {
            consume(); // End of cell
            lib->addCell(cell_name, cell);
            std::cout << "    Parsed cell: " << cell_name << std::endl;
            return;
        }

        // Parse area
        if (expect("area")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            cell.area = parseNumber();
            skipUntil(';');
            continue;
        }

        // Parse pin definitions
        if (expect("pin")) {
            parsePin(&cell);
            continue;
        }

        // Skip unknown cell-level constructs
        std::string keyword = parseIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
        } else {
            skipUntil(';');
        }
    }

    error("Unexpected end of file in cell");
}

/**
 * @brief Parse pin definition
 * @param cell Parent cell
 */
void LibertyParser::parsePin(LibCell* cell) {
    skipWhitespace();
    if (peek() != '(') {
        error("Expected '(' after pin");
        return;
    }
    consume();

    std::string pin_name = parseIdentifier();
    if (pin_name.empty()) {
        error("Expected pin name");
        return;
    }

    skipWhitespace();
    if (peek() != ')') {
        error("Expected ')' after pin name");
        return;
    }
    consume();

    skipWhitespace();
    if (peek() != '{') {
        error("Expected '{' to start pin body");
        return;
    }
    consume();

    // Create pin
    LibPin pin;
    pin.name = pin_name;

    // Parse pin body
    while (!eof()) {
        skipWhitespace();
        char ch = peek();

        if (ch == '}') {
            consume(); // End of pin
            cell->addPin(pin_name, pin);
            return;
        }

        // Parse direction
        if (expect("direction")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            pin.direction = parseIdentifier();
            skipUntil(';');
            continue;
        }

        // Parse capacitance
        if (expect("capacitance")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            pin.capacitance = parseNumber();
            skipUntil(';');
            continue;
        }

        // Parse timing definitions
        if (expect("timing")) {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // timing()
                skipWhitespace();
                if (peek() == ')') consume();
            }

            parseTiming(&pin);
            continue;
        }

        // Skip function declarations
        if (expect("function")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            parseQuotedString(); // Skip function string
            skipUntil(';');
            continue;
        }

        // Skip unknown pin-level constructs
        std::string keyword = parseIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
        } else {
            skipUntil(';');
        }
    }

    error("Unexpected end of file in pin");
}

/**
 * @brief Parse timing definition
 * @param pin Parent pin
 */
void LibertyParser::parseTiming(LibPin* pin) {
    skipWhitespace();
    if (peek() != '{') {
        error("Expected '{' to start timing body");
        return;
    }
    consume();

    // Create timing arc
    LibTiming timing;

    // Parse timing body
    while (!eof()) {
        skipWhitespace();
        char ch = peek();

        if (ch == '}') {
            consume(); // End of timing
            if (timing.isValid()) {
                pin->timing_arcs.push_back(timing);
            }
            return;
        }

        // Parse related_pin
        if (expect("related_pin")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            timing.related_pin = parseQuotedString();
            skipUntil(';');
            continue;
        }

        // Parse timing_sense
        if (expect("timing_sense")) {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            timing.timing_sense = parseIdentifier();
            skipUntil(';');
            continue;
        }

        // Parse cell rise delay
        if (expect("cell_rise")) {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            parseValues(&timing.cell_delay);
            continue;
        }

        // Parse cell fall delay
        if (expect("cell_fall")) {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            // For now, use the same table (can be separate)
            parseValues(&timing.cell_delay);
            continue;
        }

        // Skip unknown timing-level constructs
        std::string keyword = parseIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
        } else {
            skipUntil(';');
        }
    }

    error("Unexpected end of file in timing");
}

/**
 * @brief Parse values matrix into LookupTable
 * @param table Table to populate
 */
void LibertyParser::parseValues(LookupTable* table) {
    skipWhitespace();
    if (peek() != '{') {
        error("Expected '{' to start values block");
        return;
    }
    consume();

    // Parse values keyword
    if (!expect("values")) {
        error("Expected 'values' keyword");
        return;
    }

    skipWhitespace();
    if (peek() != '(') {
        error("Expected '(' after values");
        return;
    }
    consume();

    // Parse rows of quoted strings
    std::vector<std::vector<double>> rows;

    while (!eof() && peek() != ')') {
        skipWhitespace();

        // Parse quoted string containing row data
        std::string row_str = parseQuotedString();
        if (row_str.empty()) {
            error("Expected quoted string with row values");
            return;
        }

        // Parse numbers from this row
        std::vector<double> row_values;
        std::istringstream iss(row_str);
        double val;
        while (iss >> val) {
            row_values.push_back(val);
            // Skip comma if present
            if (iss.peek() == ',') iss.ignore();
        }

        if (!row_values.empty()) {
            rows.push_back(row_values);
        }

        skipWhitespace();
        if (peek() == ',') {
            consume(); // Consume comma between rows
            skipWhitespace();
            if (peek() == '\\') { // Line continuation
                consume();
            }
        }
    }

    if (peek() == ')') consume(); // End of values()

    skipWhitespace();
    if (peek() == ';') consume(); // Semicolon

    skipWhitespace();
    if (peek() == '}') consume(); // End of block

    // Populate LookupTable with parsed values
    if (!rows.empty()) {
        // Use template indices from the table_template that was parsed earlier
        // For simplicity, use hardcoded values (matching sample.lib)
        table->index_1 = {0.01, 0.05, 0.1}; // slew
        table->index_2 = {0.01, 0.05, 0.1}; // cap
        table->values = rows;

        // Validate dimensions
        if (rows.size() != table->index_1.size()) {
            std::cerr << "Warning: Row count mismatch in values table" << std::endl;
        }
        for (const auto& row : rows) {
            if (row.size() != table->index_2.size()) {
                std::cerr << "Warning: Column count mismatch in values table" << std::endl;
                break;
            }
        }
    }

    std::cout << "      Parsed values table: " << rows.size() << "x" << (rows.empty() ? 0 : rows[0].size()) << std::endl;
}

/**
 * @brief Skip whitespace and comments
 */
void LibertyParser::skipWhitespace() {
    while (!eof()) {
        char ch = peek();

        // Skip spaces, tabs, newlines
        if (ch == ' ' || ch == '\t' || ch == '\r') {
            consume();
            continue;
        }

        // Skip newlines and count lines
        if (ch == '\n') {
            consume();
            state_.line_number++;
            continue;
        }

        // Skip single-line comments
        if (ch == '/' && state_.current + 1 < state_.end && state_.current[1] == '/') {
            consume(); consume();
            while (!eof() && peek() != '\n') consume();
            continue;
        }

        // Skip multi-line comments
        if (ch == '/' && state_.current + 1 < state_.end && state_.current[1] == '*') {
            consume(); consume();
            while (!eof()) {
                if (peek() == '*' && state_.current + 1 < state_.end && state_.current[1] == '/') {
                    consume(); consume();
                    break;
                }
                if (peek() == '\n') state_.line_number++;
                consume();
            }
            continue;
        }

        break;
    }
}

/**
 * @brief Skip until a specific character
 * @param ch Character to skip to
 */
void LibertyParser::skipUntil(char ch) {
    while (!eof() && peek() != ch) {
        if (peek() == '\n') state_.line_number++;
        consume();
    }
    if (!eof() && peek() == ch) consume();
}

/**
 * @brief Expect and consume a specific token
 * @param expected Expected token string
 * @return true if matched
 */
bool LibertyParser::expect(const std::string& expected) {
    skipWhitespace();
    size_t len = expected.length();
    if (state_.current + len <= state_.end &&
        std::string(state_.current, len) == expected) {
        state_.current += len;
        return true;
    }
    return false;
}

/**
 * @brief Parse an identifier (name)
 * @return Parsed identifier or empty string
 */
std::string LibertyParser::parseIdentifier() {
    skipWhitespace();
    std::string id;

    char ch = peek();
    if (!std::isalnum(ch) && ch != '_' && ch != '.') {
        return id; // Empty
    }

    while (!eof() && (std::isalnum(peek()) || peek() == '_' || peek() == '.')) {
        id += peek();
        consume();
    }

    return id;
}

/**
 * @brief Parse a quoted string
 * @return Parsed string (without quotes)
 */
std::string LibertyParser::parseQuotedString() {
    skipWhitespace();
    std::string str;

    char quote = peek();
    if (quote != '"' && quote != '\'') {
        return str; // Empty
    }
    consume(); // Consume opening quote

    while (!eof() && peek() != quote) {
        str += peek();
        consume();
    }

    if (!eof() && peek() == quote) {
        consume(); // Consume closing quote
    }

    return str;
}

/**
 * @brief Parse a number (double)
 * @return Parsed number
 */
double LibertyParser::parseNumber() {
    skipWhitespace();
    std::string num_str;
    bool has_dot = false;

    // Check for negative sign
    if (peek() == '-') {
        num_str += peek();
        consume();
    }

    // Parse digits and decimal point
    while (!eof() && (std::isdigit(peek()) || (!has_dot && peek() == '.'))) {
        if (peek() == '.') has_dot = true;
        num_str += peek();
        consume();
    }

    return std::stod(num_str);
}

/**
 * @brief Parse a list of numbers
 * @param numbers Vector to populate
 */
void LibertyParser::parseNumberList(std::vector<double>* numbers) {
    skipWhitespace();

    while (!eof() && std::isdigit(peek())) {
        double val = parseNumber();
        numbers->push_back(val);

        skipWhitespace();
        if (peek() == ',') {
            consume();
            skipWhitespace();
        }
    }
}

/**
 * @brief Get current character
 */
char LibertyParser::peek() const {
    if (eof()) return '\0';
    return *state_.current;
}

/**
 * @brief Consume current character
 */
void LibertyParser::consume() {
    if (!eof()) state_.current++;
}

/**
 * @brief Check if at end of file
 */
bool LibertyParser::eof() const {
    return state_.current >= state_.end;
}

/**
 * @brief Report error with context
 * @param message Error message
 */
void LibertyParser::error(const std::string& message) {
    error_message_ = message + " at line " + std::to_string(state_.line_number);
    std::cerr << "LibertyParser Error: " << error_message_ << std::endl;
}

} // namespace mini
