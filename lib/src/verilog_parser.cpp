/**
 * @file verilog_parser.cpp
 * @brief Verilog Gate-Level Netlist Parser Implementation
 */

#include "verilog_parser.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <algorithm>

namespace mini {

// ============================================================================
// Constructor & Destructor
// ============================================================================

VerilogParser::VerilogParser()
    : strict_mode_(false)
    , verbose_(false)
{
    // Initialize keyword set
    keywords_ = {
        "module", "endmodule", "input", "output", "inout", "wire", "reg",
        "assign", "always", "begin", "end", "if", "else", "case", "endcase"
    };

    // Initialize gate type mapping table
    gate_type_map_ = {
        {"IV", CellType::NOT},
        {"BUF", CellType::BUF},
        {"AN2", CellType::AND},
        {"AN3", CellType::AND},
        {"AN4", CellType::AND},
        {"OR2", CellType::OR},
        {"OR3", CellType::OR},
        {"OR4", CellType::OR},
        {"ND2", CellType::NAND},
        {"ND3", CellType::NAND},
        {"ND4", CellType::NAND},
        {"NR2", CellType::NOR},
        {"NR3", CellType::NOR},
        {"NR4", CellType::NOR},
        {"XO2", CellType::XOR},
        {"XOR", CellType::XOR},
        {"XN2", CellType::XNOR},
        {"XNOR", CellType::XNOR},
        {"FD1", CellType::DFF},
        {"FD2", CellType::DFF},
        {"DFF", CellType::DFF}
    };
}

VerilogParser::~VerilogParser() {
}

// ============================================================================
// Main parsing interfaces
// ============================================================================

bool VerilogParser::parseFile(const std::string& filename, NetlistDB& db) {
    // Clear previous errors
    clearErrors();

    // Read file
    std::ifstream file(filename);
    if (!file.is_open()) {
        reportError(ParseErrorType::FILE_NOT_FOUND, 0, 0,
                   "Cannot open file: " + filename);
        return false;
    }

    // Read file content
    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string content = buffer.str();
    file.close();

    if (verbose_) {
        std::cout << "Parsing file: " << filename << std::endl;
        std::cout << "File size: " << content.size() << " bytes" << std::endl;
    }

    // Parse content
    return parseString(content, db);
}

bool VerilogParser::parseString(const std::string& content, NetlistDB& db) {
    // Clear parsing state
    wire_set_.clear();
    port_set_.clear();
    current_module_.clear();

    // 1. Remove comments
    std::string clean_content = removeComments(content);

    // 2. Lexical analysis
    std::vector<Token> tokens = tokenize(clean_content);

    if (verbose_) {
        std::cout << "Tokens generated: " << tokens.size() << std::endl;
    }

    // 3. Syntax analysis
    bool success = parseModule(tokens, db);

    // 4. Print errors (if any)
    if (hasErrors()) {
        printErrors();
        return false;
    }

    if (verbose_) {
        std::cout << "Parsing completed successfully!" << std::endl;
    }

    return success;
}

// ============================================================================
// Error reporting
// ============================================================================

void VerilogParser::printErrors() const {
    if (errors_.empty()) {
        return;
    }

    std::cerr << "========================================" << std::endl;
    std::cerr << "Verilog Parser Errors (" << errors_.size() << ")" << std::endl;
    std::cerr << "========================================" << std::endl;

    for (const auto& error : errors_) {
        std::cerr << "Line " << error.line << ", Col " << error.column << ": ";

        switch (error.type) {
            case ParseErrorType::SYNTAX_ERROR:
                std::cerr << "[SYNTAX ERROR] ";
                break;
            case ParseErrorType::UNEXPECTED_TOKEN:
                std::cerr << "[UNEXPECTED TOKEN] ";
                break;
            case ParseErrorType::MISSING_SEMICOLON:
                std::cerr << "[MISSING SEMICOLON] ";
                break;
            case ParseErrorType::UNKNOWN_GATE_TYPE:
                std::cerr << "[UNKNOWN GATE] ";
                break;
            case ParseErrorType::DUPLICATE_DECLARATION:
                std::cerr << "[DUPLICATE] ";
                break;
            case ParseErrorType::UNDEFINED_SIGNAL:
                std::cerr << "[UNDEFINED] ";
                break;
            case ParseErrorType::FILE_NOT_FOUND:
                std::cerr << "[FILE NOT FOUND] ";
                break;
        }

        std::cerr << error.message << std::endl;
    }

    std::cerr << "========================================" << std::endl;
}

// ============================================================================
// Lexical analysis
// ============================================================================

std::string VerilogParser::removeComments(const std::string& content) {
    std::string result;
    result.reserve(content.size());

    size_t i = 0;
    while (i < content.size()) {
        // Single-line comment //
        if (i + 1 < content.size() && content[i] == '/' && content[i + 1] == '/') {
            // Skip to end of line
            while (i < content.size() && content[i] != '\n') {
                i++;
            }
            if (i < content.size()) {
                result += '\n'; // Keep newline
                i++;
            }
        }
        // Multi-line comment /* */
        else if (i + 1 < content.size() && content[i] == '/' && content[i + 1] == '*') {
            i += 2;
            // Skip to */
            while (i + 1 < content.size()) {
                if (content[i] == '*' && content[i + 1] == '/') {
                    i += 2;
                    break;
                }
                if (content[i] == '\n') {
                    result += '\n'; // Keep newline to maintain line numbers
                }
                i++;
            }
        }
        else {
            result += content[i];
            i++;
        }
    }

    return result;
}

bool VerilogParser::isKeyword(const std::string& word) const {
    return keywords_.find(word) != keywords_.end();
}

std::vector<VerilogParser::Token> VerilogParser::tokenize(const std::string& content) {
    std::vector<Token> tokens;
    size_t line = 1;
    size_t column = 1;
    size_t i = 0;

    while (i < content.size()) {
        char ch = content[i];

        // Skip whitespace
        if (std::isspace(ch)) {
            if (ch == '\n') {
                line++;
                column = 1;
            } else {
                column++;
            }
            i++;
            continue;
        }

        // Separators
        if (ch == '(' || ch == ')' || ch == ',' || ch == ';') {
            tokens.emplace_back(TokenType::SEPARATOR, std::string(1, ch), line, column);
            i++;
            column++;
            continue;
        }

        // Escaped identifier \xxx
        if (ch == '\\') {
            size_t start = i;
            i++; // Skip '\'
            // Read until whitespace
            while (i < content.size() && !std::isspace(content[i]) &&
                   content[i] != '(' && content[i] != ')' &&
                   content[i] != ',' && content[i] != ';') {
                i++;
            }
            std::string identifier = content.substr(start, i - start);
            tokens.emplace_back(TokenType::IDENTIFIER, identifier, line, column);
            column += (i - start);
            continue;
        }

        // Identifier or keyword
        if (std::isalpha(ch) || ch == '_') {
            size_t start = i;
            while (i < content.size() &&
                   (std::isalnum(content[i]) || content[i] == '_')) {
                i++;
            }
            std::string word = content.substr(start, i - start);

            if (isKeyword(word)) {
                tokens.emplace_back(TokenType::KEYWORD, word, line, column);
            } else {
                tokens.emplace_back(TokenType::IDENTIFIER, word, line, column);
            }

            column += (i - start);
            continue;
        }

        // Numbers
        if (std::isdigit(ch)) {
            size_t start = i;
            while (i < content.size() && std::isdigit(content[i])) {
                i++;
            }
            std::string number = content.substr(start, i - start);
            tokens.emplace_back(TokenType::IDENTIFIER, number, line, column);
            column += (i - start);
            continue;
        }

        // Unrecognized character - skip
        i++;
        column++;
    }

    // Add EOF token
    tokens.emplace_back(TokenType::ENDOFFILE, "", line, column);

    return tokens;
}

// ============================================================================
// Syntax analysis
// ============================================================================

bool VerilogParser::parseModule(const std::vector<Token>& tokens, NetlistDB& db) {
    size_t pos = 0;

    // Skip empty tokens
    while (pos < tokens.size() && tokens[pos].type == TokenType::ENDOFFILE) {
        pos++;
    }

    if (pos >= tokens.size()) {
        reportError(ParseErrorType::SYNTAX_ERROR, 1, 1, "Empty file");
        return false;
    }

    // Parse module header
    std::string module_name;
    std::vector<std::string> port_list;

    if (!parseModuleHeader(pos, tokens, module_name, port_list)) {
        return false;
    }

    db.setDesignName(module_name);
    current_module_ = module_name;

    if (verbose_) {
        std::cout << "Module: " << module_name << std::endl;
        std::cout << "Ports: " << port_list.size() << std::endl;
    }

    // Parse module body
    while (pos < tokens.size()) {
        const Token& token = tokens[pos];

        if (token.type == TokenType::ENDOFFILE) {
            break;
        }

        if (token.type == TokenType::KEYWORD) {
            if (token.value == "endmodule") {
                pos++;
                break;
            }
            else if (token.value == "input") {
                if (!parsePortDeclaration(pos, tokens, db, true)) {
                    return false;
                }
            }
            else if (token.value == "output") {
                if (!parsePortDeclaration(pos, tokens, db, false)) {
                    return false;
                }
            }
            else if (token.value == "wire") {
                if (!parseWireDeclaration(pos, tokens)) {
                    return false;
                }
            }
            else if (token.value == "inout") {
                // Skip inout declaration
                while (pos < tokens.size() && tokens[pos].value != ";") {
                    pos++;
                }
                if (pos < tokens.size()) pos++; // Skip semicolon
            }
            else if (token.value == "reg" || token.value == "assign" ||
                     token.value == "always" || token.value == "initial") {
                // Skip unsupported statements
                int depth = 0;
                while (pos < tokens.size()) {
                    if (tokens[pos].value == "begin") depth++;
                    if (tokens[pos].value == "end") {
                        depth--;
                        if (depth == 0) {
                            pos++;
                            break;
                        }
                    }
                    if (depth == 0 && tokens[pos].value == ";") {
                        pos++;
                        break;
                    }
                    pos++;
                }
            }
            else {
                reportError(ParseErrorType::UNEXPECTED_TOKEN, token.line, token.column,
                           "Unexpected keyword: " + token.value);
                pos++;
            }
        }
        else if (token.type == TokenType::IDENTIFIER) {
            // Instantiation statement
            if (!parseInstance(pos, tokens, db)) {
                // Try to recover: skip to next semicolon
                while (pos < tokens.size() && tokens[pos].value != ";") {
                    pos++;
                }
                if (pos < tokens.size()) pos++;
            }
        }
        else {
            pos++;
        }
    }

    return !hasErrors();
}

bool VerilogParser::parseModuleHeader(size_t& pos,
                                     const std::vector<Token>& tokens,
                                     std::string& module_name,
                                     std::vector<std::string>& port_list) {
    // Expect "module"
    if (!expect(pos, tokens, "module")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected 'module'");
        return false;
    }

    // Get module name
    if (pos >= tokens.size() || tokens[pos].type != TokenType::IDENTIFIER) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected module name");
        return false;
    }

    module_name = cleanIdentifier(tokens[pos].value);
    pos++;

    // Expect "("
    if (!expect(pos, tokens, "(")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected '(' after module name");
        return false;
    }

    // Parse port list
    while (pos < tokens.size() && tokens[pos].value != ")") {
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            port_list.push_back(cleanIdentifier(tokens[pos].value));
            port_set_.insert(cleanIdentifier(tokens[pos].value));
            pos++;

            // Skip comma
            if (pos < tokens.size() && tokens[pos].value == ",") {
                pos++;
            }
        }
        else {
            pos++;
        }
    }

    // Expect ")"
    if (!expect(pos, tokens, ")")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected ')' after port list");
        return false;
    }

    // Expect ";"
    if (!expect(pos, tokens, ";")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected ';' after module header");
        return false;
    }

    return true;
}

bool VerilogParser::parsePortDeclaration(size_t& pos,
                                        const std::vector<Token>& tokens,
                                        NetlistDB& db,
                                        bool is_input) {
    // Skip "input" or "output"
    pos++;

    // Read port name list
    std::vector<std::string> port_names;

    while (pos < tokens.size() && tokens[pos].value != ";") {
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            port_names.push_back(cleanIdentifier(tokens[pos].value));
            pos++;

            // Skip comma
            if (pos < tokens.size() && tokens[pos].value == ",") {
                pos++;
            }
        }
        else {
            pos++;
        }
    }

    // Expect semicolon
    if (pos >= tokens.size() || tokens[pos].value != ";") {
        reportError(ParseErrorType::MISSING_SEMICOLON,
                   tokens[pos-1].line, tokens[pos-1].column,
                   "Expected ';' after port declaration");
        return false;
    }
    pos++; // Skip semicolon

    // Create port cells
    for (const auto& port_name : port_names) {
        CellType type = is_input ? CellType::INPUT : CellType::OUTPUT;
        Cell* cell = db.createCell(port_name, type);

        if (cell != nullptr) {
            // Add pin to port
            if (is_input) {
                cell->addPin("Y", PinDirection::OUTPUT); // INPUT port has output pin
            } else {
                cell->addPin("A", PinDirection::INPUT);  // OUTPUT port has input pin
            }

            if (verbose_) {
                std::cout << "  Port: " << port_name
                         << " (" << (is_input ? "input" : "output") << ")" << std::endl;
            }
        }
    }

    return true;
}

bool VerilogParser::parseWireDeclaration(size_t& pos, const std::vector<Token>& tokens) {
    // Skip "wire"
    pos++;

    // Read wire name list
    while (pos < tokens.size() && tokens[pos].value != ";") {
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            wire_set_.insert(cleanIdentifier(tokens[pos].value));
            pos++;

            // Skip comma
            if (pos < tokens.size() && tokens[pos].value == ",") {
                pos++;
            }
        }
        else {
            pos++;
        }
    }

    // Expect semicolon
    if (pos >= tokens.size() || tokens[pos].value != ";") {
        reportError(ParseErrorType::MISSING_SEMICOLON,
                   tokens[pos-1].line, tokens[pos-1].column,
                   "Expected ';' after wire declaration");
        return false;
    }
    pos++;

    return true;
}

bool VerilogParser::parseInstance(size_t& pos,
                                  const std::vector<Token>& tokens,
                                  NetlistDB& db) {
    if (pos + 2 >= tokens.size()) {
        return false;
    }

    // Get gate type
    std::string gate_type = tokens[pos].value;
    size_t gate_line = tokens[pos].line;
    size_t gate_col = tokens[pos].column;
    pos++;

    // Get instance name
    if (tokens[pos].type != TokenType::IDENTIFIER) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected instance name");
        return false;
    }

    std::string instance_name = cleanIdentifier(tokens[pos].value);
    pos++;

    // Expect "("
    if (!expect(pos, tokens, "(")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected '(' after instance name");
        return false;
    }

    // Get connection list
    std::vector<std::string> connections;
    while (pos < tokens.size() && tokens[pos].value != ")") {
        if (tokens[pos].type == TokenType::IDENTIFIER) {
            connections.push_back(cleanIdentifier(tokens[pos].value));
            pos++;

            if (pos < tokens.size() && tokens[pos].value == ",") {
                pos++;
            }
        }
        else {
            pos++;
        }
    }

    // Expect ")"
    if (!expect(pos, tokens, ")")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected ')' after connections");
        return false;
    }

    // Expect ";"
    if (!expect(pos, tokens, ";")) {
        reportError(ParseErrorType::SYNTAX_ERROR, tokens[pos].line, tokens[pos].column,
                   "Expected ';' after instance");
        return false;
    }

    // Map gate type
    CellType cell_type = mapGateType(gate_type);
    if (cell_type == CellType::UNKNOWN) {
        reportError(ParseErrorType::UNKNOWN_GATE_TYPE, gate_line, gate_col,
                   "Unknown gate type: " + gate_type);
        return false;
    }

    // Create cell
    Cell* cell = db.createCell(instance_name, cell_type);
    if (cell == nullptr) {
        reportError(ParseErrorType::DUPLICATE_DECLARATION, gate_line, gate_col,
                   "Duplicate instance: " + instance_name);
        return false;
    }

    // Get pin names
    std::vector<std::string> pin_names = getGatePins(gate_type);

    // Validate connection count
    if (connections.size() != pin_names.size()) {
        if (verbose_) {
            std::cerr << "Warning: Pin count mismatch for " << instance_name
                     << " (expected " << pin_names.size()
                     << ", got " << connections.size() << ")" << std::endl;
        }
        // Adjust pin count
        if (connections.size() < pin_names.size()) {
            pin_names.resize(connections.size());
        }
    }

    // Create pins and connect
    for (size_t i = 0; i < std::min(connections.size(), pin_names.size()); i++) {
        // First pin is usually output
        PinDirection dir = (i == 0) ? PinDirection::OUTPUT : PinDirection::INPUT;

        // Special handling for DFF: CK(INPUT), D(INPUT), Q(OUTPUT)
        if (cell_type == CellType::DFF) {
            if (i == 0) dir = PinDirection::INPUT;  // CK
            else if (i == 1) dir = PinDirection::INPUT;  // D
            else if (i == 2) dir = PinDirection::OUTPUT; // Q
        }

        // Add pin
        Pin* pin = cell->addPin(pin_names[i], dir);

        // Create or get Net
        Net* net = getOrCreateNet(db, connections[i]);

        // Connect
        if (pin != nullptr && net != nullptr) {
            db.connect(pin, net);
        }
    }

    if (verbose_) {
        std::cout << "  Instance: " << instance_name
                 << " (" << gate_type << ")" << std::endl;
    }

    return true;
}

// ============================================================================
// Helper methods
// ============================================================================

bool VerilogParser::expect(size_t& pos,
                          const std::vector<Token>& tokens,
                          const std::string& expected) {
    if (pos >= tokens.size()) {
        return false;
    }

    if (tokens[pos].value == expected) {
        pos++;
        return true;
    }

    return false;
}

CellType VerilogParser::mapGateType(const std::string& gate_type) const {
    auto it = gate_type_map_.find(gate_type);
    if (it != gate_type_map_.end()) {
        return it->second;
    }
    return CellType::UNKNOWN;
}

std::vector<std::string> VerilogParser::getGatePins(const std::string& gate_type) const {
    // Basic gates: first is output, rest are inputs
    if (gate_type == "IV" || gate_type == "BUF") {
        return {"Y", "A"};
    }
    else if (gate_type == "AN2" || gate_type == "OR2" || gate_type == "ND2" ||
             gate_type == "NR2" || gate_type == "XO2" || gate_type == "XOR" ||
             gate_type == "XN2" || gate_type == "XNOR") {
        return {"Y", "A", "B"};
    }
    else if (gate_type == "AN3" || gate_type == "OR3" || gate_type == "ND3" ||
             gate_type == "NR3") {
        return {"Y", "A", "B", "C"};
    }
    else if (gate_type == "AN4" || gate_type == "OR4" || gate_type == "ND4" ||
             gate_type == "NR4") {
        return {"Y", "A", "B", "C", "D"};
    }
    else if (gate_type == "FD1" || gate_type == "FD2" || gate_type == "DFF") {
        return {"CK", "D", "Q"};
    }

    // Default: assume first is output, rest are inputs
    return {"Y", "A"};
}

Net* VerilogParser::getOrCreateNet(NetlistDB& db, const std::string& net_name) {
    Net* net = db.getNet(net_name);
    if (net == nullptr) {
        net = db.createNet(net_name);
    }
    return net;
}

void VerilogParser::reportError(ParseErrorType type,
                               size_t line,
                               size_t column,
                               const std::string& message) {
    ParseError error;
    error.type = type;
    error.line = line;
    error.column = column;
    error.message = message;
    errors_.push_back(error);
}

std::string VerilogParser::cleanIdentifier(const std::string& name) const {
    // Remove escape character \ and trailing spaces
    std::string result = name;

    // Remove leading backslash
    if (!result.empty() && result[0] == '\\') {
        result = result.substr(1);
    }

    // Remove trailing spaces
    while (!result.empty() && std::isspace(result.back())) {
        result.pop_back();
    }

    return result;
}

} // namespace mini
