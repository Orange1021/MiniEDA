/**
 * @file liberty_parser.cpp
 * @brief Liberty File Parser Implementation - Robust Version
 * @details Robust recursive descent parser with proper Group/Attribute handling
 */

#include "../include/liberty_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>
#include <cstring>

namespace mini {

/**
 * @brief Constructor
 */
LibertyParser::LibertyParser() {
    state_ = ParserState();
}

/**
 * @brief Destructor
 */
LibertyParser::~LibertyParser() = default;

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

    if (verbose_) {
        std::cout << "Starting to parse Liberty file: " << filename << std::endl;
    }

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

    if (verbose_) {
        std::cout << "  Parsing library: " << library->getName() << std::endl;
    }

    // Parse library body using new hierarchical approach
    parseLibraryBody(library.get());

    if (peek() == '}') {
        consume(); // End of library
        std::cout << "  Successfully parsed library: " << lib_name << std::endl;
        std::cout << "  Cell count: " << library->getCellCount() << std::endl;
        return library;
    }

    error("Unexpected end of file in library");
    return nullptr;
}

/**
 * @brief Parse library body with robust hierarchical logic
 * @param lib Library to populate
 */
void LibertyParser::parseLibraryBody(Library* lib) {
    while (!eof()) {
        skipWhitespace();
        
        if (peek() == '}') {
            return; // End of library
        }

        // Try to read the next keyword
        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        

        if (keyword == "cell") {
            // Parse cell definition
            skipWhitespace();
            if (peek() != '(') {
                error("Expected '(' after cell");
                return;
            }
            consume();

            std::string cell_name = readIdentifier();
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

            // Create and parse cell
            LibCell cell;
            cell.name = cell_name;
            if (verbose_) {
            std::cout << "    [DEBUG] Parsing cell #" << (lib->getCellCount() + 1) 
                      << ": " << cell_name << " at line " << state_.line_number << std::endl;
        }
            
            parseCellBody(&cell);
            
            lib->addCell(cell_name, cell);
            if (verbose_) {
            std::cout << "    [DEBUG] Completed cell: " << cell_name 
                      << " (total: " << lib->getCellCount() << ")" << std::endl;
        }
        }
        else if (keyword == "lu_table_template") {
                    // Skip lu_table_template for now
                    if (verbose_) {
            std::cout << "    [DEBUG] Skipping lu_table_template at line " << state_.line_number << std::endl;
        }
                    skipGroup();
                }
                else {
                    // Check if this is a group or an attribute
                    skipWhitespace();
                    if (peek() == '(') {
                        // This is a group we don't recognize
                        if (verbose_) {
                std::cout << "    [DEBUG] Skipping unknown group: " << keyword 
                                  << " at line " << state_.line_number << std::endl;
            }
                        skipGroup();
                    }
            else if (peek() == ':') {
                // This is an attribute - check if it's a unit definition
                if (keyword == "time_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    lib->time_unit = readString();
                    std::cout << "  Parsed time_unit: " << lib->time_unit << std::endl;
                    skipUntil(';');
                }
                else if (keyword == "capacitive_load_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    if (peek() == '(') {
                        consume(); // '('
                        skipWhitespace();
                        double value = readNumber();
                        std::string unit = readIdentifier();
                        lib->capacitive_unit = std::to_string(value) + unit; // e.g., "1" + "ff"
                        skipWhitespace();
                        if (peek() == ')') consume();
                    }
                    std::cout << "  Parsed capacitive_unit: " << lib->capacitive_unit << std::endl;
                    skipUntil(';');
                }
                else if (keyword == "voltage_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    lib->voltage_unit = readString();
                    std::cout << "  Parsed voltage_unit: " << lib->voltage_unit << std::endl;
                    skipUntil(';');
                }
                else if (keyword == "current_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    lib->current_unit = readString();
                    std::cout << "  Parsed current_unit: " << lib->current_unit << std::endl;
                    skipUntil(';');
                }
                else if (keyword == "pulling_resistance_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    lib->resistance_unit = readString();
                    std::cout << "  Parsed resistance_unit: " << lib->resistance_unit << std::endl;
                    skipUntil(';');
                }
                else if (keyword == "leakage_power_unit") {
                    consume(); // ':'
                    skipWhitespace();
                    lib->power_unit = readString();
                    std::cout << "  Parsed power_unit: " << lib->power_unit << std::endl;
                    skipUntil(';');
                }
                else {
                    // This is an attribute we don't need
                    
                    skipAttribute();
                }
            }
            else {
                // Unknown construct, skip it
                std::cout << "  Skipping unknown construct: " << keyword << std::endl;
                skipAttribute();
            }
        }
    }
}

/**
 * @brief Parse cell body with robust logic
 * @param cell Cell to populate
 */
void LibertyParser::parseCellBody(LibCell* cell) {
    int cell_depth = 1; // We're already inside the cell {
    
    while (!eof() && cell_depth > 0) {
        skipWhitespace();
        
        if (peek() == '}') {
            consume();
            cell_depth--;
            if (cell_depth == 0) {
                return; // End of cell
            }
        }
        else if (peek() == '{') {
            consume();
            cell_depth++;
        }

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        if (keyword == "area") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            cell->area = readNumber();
            skipUntil(';');
        }
        else if (keyword == "pin") {
            // Parse pin definition
            skipWhitespace();
            if (peek() != '(') {
                error("Expected '(' after pin");
                return;
            }
            consume();

            std::string pin_name = readIdentifier();
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

            // Create and parse pin
            LibPin pin;
            pin.name = pin_name;
            if (verbose_) {
                std::cout << "      Parsing pin: " << pin_name << std::endl;
            }
            
            parsePinBody(&pin);
            
            cell->addPin(pin_name, pin);
        }
        else if (keyword == "internal_power" || keyword == "leakage_power" || keyword == "statetable" || 
                 keyword == "clock_gating_integrated_cell") {
            // Skip complex power-related constructs with proper depth tracking
            if (verbose_) {
                            std::cout << "    [DEBUG] Skipping complex cell attribute: " << keyword 
                                              << " at line " << state_.line_number << std::endl;
                        }            skipWhitespace();
            if (peek() == '(') {
                consume(); // Skip arguments
                skipUntil(')');
            }
            if (peek() == '{') {
                consume();
                int depth = 1;
                while (!eof() && depth > 0) {
                    char ch = peek();
                    if (ch == '{') {
                        depth++;
                    }
                    else if (ch == '}') {
                        depth--;
                    }
                    else if (ch == '\n') {
                        state_.line_number++;
                    }
                    consume();
                }
            }
        }
        else {
            // Skip other unknown cell-level constructs
            if (verbose_) {
                            std::cout << "    [DEBUG] Skipping unknown cell attribute: " << keyword 
                                              << " at line " << state_.line_number << std::endl;
                        }            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief Parse pin body with robust logic
 * @param pin Pin to populate
 */
void LibertyParser::parsePinBody(LibPin* pin) {
    while (!eof()) {
        skipWhitespace();
        
        if (peek() == '}') {
            consume(); // End of pin
            return;
        }

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        if (keyword == "direction") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            pin->direction = readIdentifier();
            skipUntil(';');
        }
        else if (keyword == "capacitance") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            pin->capacitance = readNumber();
            skipUntil(';');
        }
        else if (keyword == "timing") {
            // Parse timing definition with state machine for constraint parsing
            skipWhitespace();
            if (peek() == '(') {
                consume(); // timing()
                skipWhitespace();
                if (peek() == ')') consume();
            }

            skipWhitespace();
            if (peek() != '{') {
                error("Expected '{' to start timing body");
                return;
            }
            consume();

            // Parse timing body with unified parser
            if (verbose_) {
                std::cout << "        [DEBUG] Parsing timing block for pin " << pin->name << std::endl;
            }
            
            parseTimingBody(pin);
            
            if (verbose_) {
                std::cout << "        [DEBUG] Finished parsing timing block" << std::endl;
            }
        }
        else if (keyword == "constraint" || keyword == "setup" || keyword == "hold") {
            // Parse constraint definition (setup/hold)
            skipWhitespace();
            if (peek() == '(') {
                consume(); // constraint()
                skipWhitespace();
                if (peek() == ')') consume();
            }

            skipWhitespace();
            if (peek() != '{') {
                error("Expected '{' to start constraint body");
                return;
            }
            consume();

            // Create and parse constraint
            LibConstraint constraint;
            if (verbose_) {
                std::cout << "        Parsing constraint: " << keyword << std::endl;
            }
            
            parseConstraintBody(&constraint, keyword);
            
            if (constraint.isValid()) {
                pin->constraint_arcs.push_back(constraint);
            }
        }
        else if (keyword == "function") {
            // Skip function declarations
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            readString(); // Skip function string
            skipUntil(';');
        }
        else {
            // Skip unknown pin-level constructs (e.g., internal_node, clock_gate_enable_pin)
            if (verbose_) {
                std::cout << "        Skipping unknown pin attribute: " << keyword << std::endl;
            }
            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief Parse timing body with robust logic
 * @param timing Timing to populate
 */
void LibertyParser::parseTimingBody(LibTiming* timing) {
    while (!eof()) {
        skipWhitespace();
        
        if (peek() == '}') {
            consume(); // End of timing
            return;
        }

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        if (keyword == "related_pin") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            timing->related_pin = readString();
            skipUntil(';');
        }
        else if (keyword == "timing_sense") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            timing->timing_sense = readIdentifier();
            skipUntil(';');
        }
        else if (keyword == "cell_rise" || keyword == "cell_fall") {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            parseValuesTable(&timing->cell_delay);
        }
        else if (keyword == "rise_transition" || keyword == "fall_transition") {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            
            // Parse the appropriate transition table
            LookupTable* transition_table = (keyword == "rise_transition") ? 
                &timing->rise_transition : &timing->fall_transition;
            parseValuesTable(transition_table);
        }
        else {
            // Skip unknown timing-level constructs
            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief Parse values table with robust logic
 * @param table Table to populate
 */
void LibertyParser::parseValuesTable(LookupTable* table) {
    skipWhitespace();
    
    // Check if we have a starting brace (some formats have it, some don't)
    bool has_starting_brace = false;
    if (peek() == '{') {
        has_starting_brace = true;
        consume();
        skipWhitespace();
    }

    // Parse index_1 and index_2 if present (Nangate format)
    while (peek() != 'v' && peek() != '}' && !eof()) {
        std::string keyword = readIdentifier();
        skipWhitespace();
        
        if (keyword == "index_1" || keyword == "index_2") {
            if (peek() == '(') {
                consume();
                std::string index_str = readString();
                std::vector<double> index_values;
                std::istringstream iss(index_str);
                double val;
                while (iss >> val) {
                    index_values.push_back(val);
                    if (iss.peek() == ',') iss.ignore();
                }
                
                if (keyword == "index_1") {
                    table->index_1 = index_values;
                } else {
                    table->index_2 = index_values;
                }
                
                skipUntil(';');
            }
        } else {
            // Skip unknown attributes
            skipUntil(';');
        }
        skipWhitespace();
    }

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
        std::string row_str = readString();
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
    if (has_starting_brace && peek() == '}') {
        consume(); // End of block (only if we had a starting brace)
    }

    // Populate LookupTable with parsed values
    if (!rows.empty()) {
        // Use the parsed index values if available, otherwise use defaults
        if (table->index_1.empty()) {
            table->index_1 = {0.01, 0.05, 0.1}; // Default slew values
        }
        if (table->index_2.empty()) {
            table->index_2 = {0.01, 0.05, 0.1}; // Default cap values
        }
        table->values = rows;

        // Note: Size validation removed to handle Nangate 45nm library with larger lookup tables
        // The actual values are parsed correctly, only index sizes may differ from defaults
    }

    
}

/**
 * @brief Parse constraint body (setup/hold) with robust logic
 * @param constraint Constraint to populate
 * @param constraint_type Initial constraint type (setup/hold)
 */
void LibertyParser::parseConstraintBody(LibConstraint* constraint, const std::string& constraint_type) {
    // Set initial constraint type based on keyword
    constraint->constraint_type = constraint_type;
    
    while (!eof()) {
        skipWhitespace();
        
        if (peek() == '}') {
            consume(); // End of constraint
            return;
        }

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        if (keyword == "related_pin") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            constraint->related_pin = readString();
            skipUntil(';');
        }
        else if (keyword == "timing_type") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            std::string timing_type = readIdentifier();
            
            // Update constraint type based on timing_type
            if (timing_type == "setup_rising" || timing_type == "setup_falling" ||
                timing_type == "hold_rising" || timing_type == "hold_falling") {
                constraint->constraint_type = timing_type;
            }
            skipUntil(';');
        }
        else if (keyword == "constraint_rising" || keyword == "constraint_falling" ||
                 keyword == "setup_rising" || keyword == "setup_falling" ||
                 keyword == "hold_rising" || keyword == "hold_falling") {
            skipWhitespace();
            if (peek() == '(') {
                consume(); // template name
                skipUntil(')');
            }
            
            // Parse the constraint table
            parseValuesTable(&constraint->constraint_table);
            
            // Update constraint type if not already set
            if (constraint->constraint_type == "setup" || constraint->constraint_type == "hold") {
                constraint->constraint_type = keyword;
            }
        }
        else {
            // Skip unknown constraint-level constructs
            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief Skip a group block with proper nesting handling
 */
void LibertyParser::skipGroup() {
    skipWhitespace();
    if (peek() == '(') {
        // Skip group arguments
        consume();
        skipUntil(')');
    }
    
    skipWhitespace();
    if (peek() == '{') {
        consume();
        int depth = 1;
        
        while (!eof() && depth > 0) {
            char ch = peek();
            if (ch == '{') {
                depth++;
            }
            else if (ch == '}') {
                depth--;
            }
            else if (ch == '\n') {
                state_.line_number++;
            }
            consume();
        }
    }
}

/**
 * @brief Skip an attribute (ends with semicolon)
 */
void LibertyParser::skipAttribute() {
    skipWhitespace();
    if (peek() == ':') {
        consume();
    }
    skipUntil(';');
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
 * @brief Read identifier without consuming too much
 */
std::string LibertyParser::readIdentifier() {
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
 * @brief Read quoted string
 */
std::string LibertyParser::readString() {
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
 * @brief Read number
 */
double LibertyParser::readNumber() {
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
 * @brief Match expected string
 */
bool LibertyParser::match(const char* expected) {
    skipWhitespace();
    size_t len = strlen(expected);
    if (state_.current + len <= state_.end &&
        strncmp(state_.current, expected, len) == 0) {
        state_.current += len;
        return true;
    }
    return false;
}

/**
 * @brief Check character
 */
bool LibertyParser::check(char c) {
    return !eof() && peek() == c;
}

// --- Backward compatibility methods ---

/**
 * @brief Skip until a specific character
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
 */
std::string LibertyParser::parseIdentifier() {
    return readIdentifier();
}

/**
 * @brief Parse a quoted string
 */
std::string LibertyParser::parseQuotedString() {
    return readString();
}

/**
 * @brief Parse a number (double)
 */
double LibertyParser::parseNumber() {
    return readNumber();
}

/**
 * @brief Parse values matrix into LookupTable
 */
void LibertyParser::parseValues(LookupTable* table) {
    parseValuesTable(table);
}

/**
 * @brief Parse a list of numbers
 */
void LibertyParser::parseNumberList(std::vector<double>* numbers) {
    skipWhitespace();

    while (!eof() && std::isdigit(peek())) {
        double val = readNumber();
        numbers->push_back(val);

        skipWhitespace();
        if (peek() == ',') {
            consume();
            skipWhitespace();
        }
    }
}

/**
 * @brief Parse cell definition (backward compatibility)
 */
void LibertyParser::parseCell(Library* /* lib */) {
    // This method is no longer used in the new implementation
    // parseLibraryBody handles cell parsing directly
}

/**
 * @brief Parse pin definition (backward compatibility)
 */
void LibertyParser::parsePin(LibCell* /* cell */) {
    // This method is no longer used in the new implementation
    // parseCellBody handles pin parsing directly
}

/**
 * @brief Parse timing definition (backward compatibility)
 */
void LibertyParser::parseTiming(LibPin* /* pin */) {
    // This method is no longer used in the new implementation
    // parsePinBody handles timing parsing directly
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
 */
void LibertyParser::error(const std::string& message) {
    error_message_ = message + " at line " + std::to_string(state_.line_number);
    std::cerr << "LibertyParser Error: " << error_message_ << std::endl;
}

/**
 * @brief [NEW] Parse timing body with constraint state machine
 * @details Implements the state machine logic for parsing rise_constraint/fall_constraint
 * based on timing_type (setup_rising/hold_rising/etc.)
 * 
 * State Machine Logic:
 * 1. Read timing_type -> Set current_constraint_mode
 * 2. Read rise_constraint/fall_constraint -> Map to appropriate constraint table
 * 3. Parse values table -> Store in LibPin constraint_arcs
 */
void LibertyParser::parseTimingBodyWithConstraints(LibPin* pin) {
    // State machine variables
    std::string current_timing_type = "";
    std::string related_pin = "";
    std::string current_when_condition = "";  // [NEW] Store when condition
    
    while (!eof()) {
        skipWhitespace();
        
        if (peek() == '}') {
            consume(); // End of timing group
            std::cout << "          [DEBUG] End of timing group (found })" << std::endl;
            return;
        }

        std::string keyword = readIdentifier();
        if (keyword.empty()) {
            if (!eof()) consume();
            continue;
        }

        if (keyword == "timing_type") {
            // Step 1: Read timing_type to set constraint mode
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            current_timing_type = readIdentifier();
            current_when_condition = "";  // [NEW] Reset when condition for new timing type
            skipUntil(';');
            
            // Always show timing_type (important for debugging)
            std::cout << "          [DEBUG] Timing type: " << current_timing_type << std::endl;
        }
        else if (keyword == "related_pin") {
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            related_pin = readString();
            skipUntil(';');
            
            // Always show related_pin (important for debugging)
            std::cout << "          [DEBUG] Related pin: " << related_pin << std::endl;
        }
        else if (keyword == "when") {
            // [NEW] Parse when condition
            skipWhitespace();
            if (peek() == ':') consume();
            skipWhitespace();
            current_when_condition = readString();
            skipUntil(';');
            
            // Always show when condition (important for debugging)
            std::cout << "          Found when condition: " << current_when_condition << std::endl;
        }
        else if (keyword == "rise_constraint" || keyword == "fall_constraint") {
            // Step 2: Parse constraint based on current timing_type
            std::string constraint_edge = keyword; // "rise_constraint" or "fall_constraint"
            
            skipWhitespace();
            if (peek() == '(') {
                consume(); // constraint(template_name)
                std::string template_name = readString();
                skipUntil(')');
                
                // Always show constraint parsing (important for debugging)
                            std::cout << "          Parsing " << constraint_edge << "(" << template_name << ")" 
                                      << " for timing_type: " << current_timing_type << std::endl;                
                skipWhitespace();
                if (peek() != '{') {
                    error("Expected '{' to start constraint body");
                    return;
                }
                consume();

                // Step 3: Create constraint with proper type mapping
                LibConstraint constraint;
                constraint.related_pin = related_pin;
                constraint.when_condition = current_when_condition;  // [NEW] Store when condition
                
                // Map timing_type + edge to constraint_type
                            // Note: timing_type defines the clock edge, constraint_edge defines data transition
                            if (current_timing_type == "setup_rising") {
                                // Setup check for rising clock edge
                                constraint.constraint_type = "setup_rising";
                            }
                            else if (current_timing_type == "hold_rising") {
                                // Hold check for rising clock edge  
                                constraint.constraint_type = "hold_rising";
                            }
                            else if (current_timing_type == "setup_falling") {
                                // Setup check for falling clock edge
                                constraint.constraint_type = "setup_falling";
                            }
                            else if (current_timing_type == "hold_falling") {
                                // Hold check for falling clock edge
                                constraint.constraint_type = "hold_falling";
                            }                else if (current_timing_type == "setup_falling") {
                    if (constraint_edge == "rise_constraint") {
                        constraint.constraint_type = "setup_rising_falling";
                    } else if (constraint_edge == "fall_constraint") {
                        constraint.constraint_type = "setup_falling_falling";
                    }
                }
                else if (current_timing_type == "hold_falling") {
                    if (constraint_edge == "rise_constraint") {
                        constraint.constraint_type = "hold_rising_falling";
                    } else if (constraint_edge == "fall_constraint") {
                        constraint.constraint_type = "hold_falling_falling";
                    }
                }
                
                // Parse the constraint values table
                parseValuesTable(&constraint.constraint_table);
                
                // Add to pin's constraint arcs if valid
                if (constraint.isValid()) {
                    pin->constraint_arcs.push_back(constraint);
                    // Always show constraint addition (important for debugging)
                    std::cout << "            Added constraint: " << constraint.constraint_type 
                              << " with " << constraint.constraint_table.getDim1Size() 
                              << "x" << constraint.constraint_table.getDim2Size() << " table" << std::endl;
                }
            }
        }
        else {
            // Skip other timing-level attributes
            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief [NEW] Unified Timing Parser
 * @details Handles BOTH standard delays (cell_rise) and constraints (setup_rising)
 * @param pin Pin to store timing arcs and constraint arcs
 */
void LibertyParser::parseTimingBody(LibPin* pin) {
    LibTiming timing_arc;      // For delay arcs (combinational logic)
    
    // State tracking
    bool is_constraint = false;
    std::string current_timing_type = "";
    std::string current_when = "";
    std::string current_related_pin = "";
    
    while (!eof()) {
        skipWhitespace();
        
        char next_char = peek();
        if (next_char == '}') {
            consume(); // End of timing group
            
            // End of group: decide where to save
            if (!is_constraint) {
                // It's a delay arc (combinational)
                timing_arc.related_pin = current_related_pin;
                timing_arc.timing_sense = current_timing_type;
                
                // Only add if it has valid data
                if (timing_arc.cell_delay.isValid() || 
                    timing_arc.rise_transition.isValid() || 
                    timing_arc.fall_transition.isValid()) {
                    pin->timing_arcs.push_back(timing_arc);
                }
            }
            
            return;
        }
        
        std::string keyword = readIdentifier();
        if (keyword.empty()) { 
            if (!eof()) consume(); 
            continue; 
        }
        
        // --- Common Attributes ---
        if (keyword == "related_pin") {
            skipWhitespace(); 
            if (peek() == ':') consume();
            current_related_pin = readValue();
            skipUntil(';');
            
            if (verbose_) {
                std::cout << "          [DEBUG] Related pin: " << current_related_pin << std::endl;
            }
        }
        else if (keyword == "timing_type") {
            skipWhitespace(); 
            if (peek() == ':') consume();
            current_timing_type = readValue();
            skipUntil(';');
            
            // Detect if this is a constraint block
            if (current_timing_type.find("setup") != std::string::npos || 
                current_timing_type.find("hold") != std::string::npos ||
                current_timing_type.find("recovery") != std::string::npos ||
                current_timing_type.find("removal") != std::string::npos) {
                is_constraint = true;
            }
            
            if (verbose_) {
                std::cout << "          [DEBUG] Timing type: " << current_timing_type 
                          << (is_constraint ? " (constraint)" : " (delay)") << std::endl;
            }
        }
        else if (keyword == "when") {
            skipWhitespace(); 
            if (peek() == ':') consume();
            std::string condition;
            while (!eof() && peek() != ';') { 
                condition += peek(); 
                consume(); 
            }
            current_when = condition;
            if (peek() == ';') consume();
            
            if (verbose_) {
                std::cout << "          [DEBUG] When condition: " << current_when << std::endl;
            }
        }
        else if (keyword == "timing_sense") {
            skipWhitespace(); 
            if (peek() == ':') consume();
            timing_arc.timing_sense = readValue();
            skipUntil(';');
        }
        
        // --- Standard Delay Tables (Combinational) ---
        else if (keyword == "cell_rise" || keyword == "cell_fall") {
            skipWhitespace(); 
            if (peek() == '(') { 
                consume(); 
                skipUntil(')'); 
            }
            
            skipWhitespace();
            if (peek() == '{') {
                // Don't consume { here - parseValuesTable will handle it
                parseValuesTable(&timing_arc.cell_delay);
            } else {
                // Skip this table if format is unexpected
                skipGroup();
            }
        }
        else if (keyword == "rise_transition" || keyword == "fall_transition") {
            skipWhitespace(); 
            if (peek() == '(') { 
                consume(); 
                skipUntil(')'); 
            }
            
            skipWhitespace();
            if (peek() == '{') {
                // Don't consume { here - parseValuesTable will handle it
                if (keyword == "rise_transition") {
                    parseValuesTable(&timing_arc.rise_transition);
                } else {
                    parseValuesTable(&timing_arc.fall_transition);
                }
            } else {
                // Skip this table if format is unexpected
                skipGroup();
            }
        }
        
        // --- Constraint Tables (Setup/Hold) ---
        else if (keyword == "rise_constraint" || keyword == "fall_constraint") {
            is_constraint = true;
            
            skipWhitespace(); 
            if (peek() == '(') { 
                consume(); 
                skipUntil(')'); 
            }
            
            skipWhitespace();
            if (peek() == '{') { 
                // Don't consume { here - parseValuesTable will handle it
                
                // Create a specific constraint entry
                LibConstraint new_const;
                new_const.related_pin = current_related_pin;
                new_const.when_condition = current_when;
                new_const.constraint_type = current_timing_type;
                
                parseValuesTable(&new_const.constraint_table);
                
                if (new_const.isValid()) {
                
                                pin->constraint_arcs.push_back(new_const);
                
                                if (verbose_) {
                
                                    std::cout << "            [DEBUG] Added constraint: " << new_const.constraint_type 
                
                                              << " with " << new_const.constraint_table.getDim1Size() 
                
                                              << "x" << new_const.constraint_table.getDim2Size() << " table" << std::endl;
                
                                }
                
                            }            } else {
                // Skip this constraint if format is unexpected
                skipGroup();
            }
        }
        else {
            // Skip other timing-level attributes
            skipWhitespace();
            if (peek() == ':') {
                skipAttribute();
            } else {
                skipGroup();
            }
        }
    }
}

/**
 * @brief [NEW] Read a value that might be quoted or unquoted
 * @details Solves the issue where timing_type : setup_rising; (unquoted) fails with readString()
 * @return The value as a string
 */
std::string LibertyParser::readValue() {
    skipWhitespace();
    if (peek() == '"' || peek() == '\'') {
        return readString();
    }
    return readIdentifier();
}

} // namespace mini