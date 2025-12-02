/**
 * @file lef_parser.cpp
 * @brief LEF (Library Exchange Format) Parser Implementation
 */

#include "lef_parser.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <cctype>
#include <limits>

namespace mini {

// ============================================================================
// LefParser Implementation
// ============================================================================

LefParser::LefParser(const std::string& filename, bool verbose) 
    : filename_(filename), line_number_(1), column_number_(1), verbose_(verbose), has_token_(false) {
    file_.open(filename);
    if (!file_.is_open()) {
        error("Cannot open file: " + filename);
    }
}

LefParser::~LefParser() {
    if (file_.is_open()) {
        file_.close();
    }
}

bool LefParser::isOpen() const {
    return file_.is_open();
}

void LefParser::error(const std::string& message) {
    std::cerr << "LEF Parser Error [" << filename_ << ":" << line_number_ 
              << ":" << column_number_ << "]: " << message << std::endl;
    throw std::runtime_error("LEF Parser Error: " + message);
}

void LefParser::warning(const std::string& message) {
    std::cerr << "LEF Parser Warning [" << filename_ << ":" << line_number_ 
              << ":" << column_number_ << "]: " << message << std::endl;
}

bool LefParser::nextToken(std::string& token) {
    if (has_token_) {
        token = current_token_;
        has_token_ = false;
        return true;
    }
    
    // Skip whitespace and comments
    char c;
    while (file_.get(c)) {
        if (c == '\n') {
            line_number_++;
            column_number_ = 1;
        } else if (c == '#') {
            // Skip comment until end of line
            while (file_.get(c) && c != '\n') {
                // Skip comment content
            }
            line_number_++;
            column_number_ = 1;
        } else if (!isspace(c)) {
            // Found non-whitespace character
            file_.unget();
            break;
        } else {
            column_number_++;
        }
    }
    
    if (!file_.good()) {
        return false;
    }
    
    // Check for special separators first (CRITICAL FIX)
    if (file_.get(c)) {
        if (c == ';' || c == '{' || c == '}') {
            token = std::string(1, c);
            column_number_++;
            return true; // Return separator as token
        }
        file_.unget(); // Not a separator, put it back
    } else {
        return false; // True EOF
    }
    
    // Read regular word token
    token.clear();
    while (file_.get(c)) {
        if (isspace(c) || c == ';' || c == '{' || c == '}') {
            file_.unget();
            break;
        }
        token += c;
        column_number_++;
    }
    
    if (token.empty()) {
        return false;
    }
    
    
    
    return true;
}

bool LefParser::peekToken(std::string& token) {
    if (!has_token_) {
        if (nextToken(current_token_)) {
            has_token_ = true;
            token = current_token_;
            return true;
        }
        return false;
    }
    token = current_token_;
    return true;
}

void LefParser::consumeToken() {
    std::string token;
    if (!nextToken(token)) {
        error("Unexpected end of file");
    }
}

void LefParser::consumeKeyword(const std::string& expected) {
    std::string token;
    if (!nextToken(token)) {
        error("Unexpected end of file, expected: " + expected);
    }
    if (token != expected) {
        error("Expected keyword '" + expected + "', got '" + token + "'");
    }
}

void LefParser::consumeSemicolon() {
    consumeKeyword(";");
}

void LefParser::skipStatement() {
    // Skip tokens until we find a semicolon
    char c;
    int brace_count = 0;
    
    while (file_.get(c)) {
        if (c == '{') {
            brace_count++;
        } else if (c == '}') {
            if (brace_count > 0) {
                brace_count--;
            }
        } else if (c == ';' && brace_count == 0) {
            column_number_++;
            return;
        } else if (c == '\n') {
            line_number_++;
            column_number_ = 1;
        } else {
            column_number_++;
        }
    }
    error("Unexpected end of file while skipping statement");
}

void LefParser::skipToEndKeyword(const std::string& keyword) {
    std::string token;
    while (nextToken(token)) {
        if (token == "END") {
            // Check what comes after END
            std::string end_name;
            if (peekToken(end_name)) {
                if (end_name == keyword) {
                    // Found "END keyword" - consume both
                    consumeToken(); // consume keyword
                    consumeSemicolon();
                    return;
                } else if (end_name == ";") {
                    // Found just "END;" - consume semicolon and return
                    consumeSemicolon();
                    return;
                } else {
                    // Found "END something_else" - not our block, continue
                    continue;
                }
            } else {
                // Found just "END" at end of file or next token is not available
                return;
            }
        }
    }
    error("Could not find END " + keyword);
}

double LefParser::parseNumber() {
    std::string token;
    if (!nextToken(token)) {
        error("Expected number, got end of file");
    }
    
    try {
        return std::stod(token);
    } catch (const std::exception& e) {
        error("Invalid number format: " + token);
        return 0.0;
    }
}

Rect LefParser::parseRect() {
    double x1 = parseNumber();
    double y1 = parseNumber();
    double x2 = parseNumber();
    double y2 = parseNumber();
    consumeSemicolon();
    
    // Ensure proper rectangle orientation
    if (x1 > x2) std::swap(x1, x2);
    if (y1 > y2) std::swap(y1, y2);
    
    return Rect{x1, y1, x2, y2};
}

Rect LefParser::parsePolygon() {
    // Parse polygon coordinates and convert to bounding box
    double min_x = std::numeric_limits<double>::infinity();
    double min_y = std::numeric_limits<double>::infinity();
    double max_x = -std::numeric_limits<double>::infinity();
    double max_y = -std::numeric_limits<double>::infinity();
    
    std::string token;
    int vertex_count = 0;
    
    // Read coordinates until we hit semicolon
    while (nextToken(token)) {
        if (token == ";") {
            break;  // End of polygon definition
        }
        
        // Parse X coordinate
        double x = std::stod(token);
        if (!nextToken(token)) {
            error("Expected Y coordinate after X coordinate in POLYGON");
        }
        
        // Parse Y coordinate  
        double y = std::stod(token);
        vertex_count++;
        
        // Update bounding box
        min_x = std::min(min_x, x);
        min_y = std::min(min_y, y);
        max_x = std::max(max_x, x);
        max_y = std::max(max_y, y);
        
        
    }
    
    if (vertex_count < 3) {
        error("POLYGON must have at least 3 vertices");
    }
    
    
    
    return Rect{min_x, min_y, max_x, max_y};
}

void LefParser::parseMacro(LefLibrary& library) {
    std::string macro_name;
    if (!nextToken(macro_name)) {
        error("Expected macro name after MACRO");
    }
    
    
    
    LefMacro macro(macro_name, 0.0, 0.0);
    
    // Parse macro content until END macro_name
    std::string token;
    while (nextToken(token)) {
        if (verbose_) {
            
        }
        
        if (token == "END") {
            std::string end_name;
            if (nextToken(end_name) && end_name == macro_name) {
                // Check if there's a semicolon after END macro_name (optional in LEF)
                std::string next_tok;
                if (peekToken(next_tok) && next_tok == ";") {
                    consumeSemicolon();
                }
                break;
            }
        } else if (token == "SIZE") {
            parseSize(macro.width, macro.height);
        } else if (token == "PIN") {
            parsePin(macro);
        } else if (token == "OBS") {
            // Skip OBS block - ends with bare END, not END OBS
            
            std::string skip_token;
            while (nextToken(skip_token)) {
                if (skip_token == "END") {
                    break; // Found bare END, OBS block ends
                }
            }
        } else if (token == "CLASS") {
            // Skip CLASS statements
            consumeToken(); // consume class (CORE/PAD/ENDCAP/etc.)
            consumeSemicolon();
        } else if (token == "FOREIGN") {
            // Skip FOREIGN statements
            consumeToken(); // consume foreign name
            consumeToken(); // consume x coordinate
            consumeToken(); // consume y coordinate
            consumeSemicolon();
        } else if (token == "ORIGIN") {
            // Skip ORIGIN statements
            consumeToken(); // consume x coordinate
            consumeToken(); // consume y coordinate
            consumeSemicolon();
        } else if (token == "SYMMETRY") {
            // Skip SYMMETRY statements
            while (peekToken(token) && token != ";") {
                consumeToken(); // consume symmetry tokens (X, Y, R90, etc.)
            }
            consumeSemicolon();
        } else if (token == "SITE") {
            // Skip SITE statements
            consumeToken(); // consume site name
            consumeSemicolon();
        } else {
            // Skip unknown statements
            skipStatement();
        }
    }
    
    library.addMacro(macro);
    
    
}

void LefParser::parseSize(double& width, double& height) {
    width = parseNumber();
    consumeKeyword("BY");
    height = parseNumber();
    consumeSemicolon();
    
    
}

void LefParser::parsePin(LefMacro& macro) {
    std::string pin_name;
    if (!nextToken(pin_name)) {
        error("Expected pin name after PIN");
    }
    
    
    
    LefPort port;
    
    // Parse pin content until END pin_name
    std::string token;
    while (nextToken(token)) {
        if (token == "END") {
            std::string end_name;
            if (nextToken(end_name) && end_name == pin_name) {
                // Check if there's a semicolon after END pin_name (optional in LEF)
                std::string next_tok;
                if (peekToken(next_tok) && next_tok == ";") {
                    consumeSemicolon();
                }
                break;
            }
        } else if (token == "PORT") {
            parsePort(port);
        } else if (token == "DIRECTION") {
            // Skip DIRECTION statements
            consumeToken(); // consume direction (INPUT/OUTPUT/INOUT)
            consumeSemicolon();
        } else if (token == "USE") {
            // Skip USE statements
            consumeToken(); // consume use (SIGNAL/CLOCK/POWER/GROUND)
            consumeSemicolon();
        } else if (token == "ANTENNAPARTIALMETALAREA" || 
                   token == "ANTENNAPARTIALMETALSIDEAREA") {
            // Skip antenna statements with extra parameters
            consumeToken(); // consume value
            consumeKeyword("LAYER");
            consumeToken(); // consume layer name
            consumeSemicolon();
        } else if (token == "ANTENNAGATEAREA") {
            // Skip antenna statements
            consumeToken(); // consume value
            consumeSemicolon();
        } else {
            // Skip unknown statements
            skipStatement();
        }
    }
    
    macro.addPin(pin_name, port);
    
    
}

void LefParser::parsePort(LefPort& port) {
    // Parse port content until END
    std::string token;
    while (nextToken(token)) {
        
        if (token == "END") {
            // Check if next token is PORT (optional)
            std::string end_name;
            if (peekToken(end_name) && end_name == "PORT") {
                consumeToken(); // consume PORT
                consumeSemicolon();
            }
            break;
        } else if (token == "LAYER") {
            // Skip LAYER statements (we already know the layer)
            consumeToken(); // consume layer name
            // Check if there's a semicolon immediately after
            std::string next_tok;
            if (peekToken(next_tok) && next_tok == ";") {
                consumeSemicolon();
            }
        } else if (token == "RECT") {
            // Parse RECT statement
            Rect rect = parseRect();
            port.addRect(rect);
            
            
        } else if (token == "POLYGON") {
            // Parse POLYGON statement and convert to bounding box
            Rect rect = parsePolygon();
            port.addRect(rect);
            
            
        } else {
            // Skip unknown statements
            skipStatement();
        }
    }
}

void LefParser::parseLefLibrary() {
    // This is the main parsing entry point
    LefLibrary library;
    
    std::string token;
    while (nextToken(token)) {
        if (token == "MACRO") {
            parseMacro(library);
        } else if (token == "END") {
            // Check if this is END LIBRARY
            std::string end_name;
            if (peekToken(end_name) && end_name == "LIBRARY") {
                consumeToken(); // consume LIBRARY
                consumeSemicolon();
                break;
            } else {
                // Some other END statement, skip it
                skipStatement();
            }
        } else {
            // Skip unknown top-level statements
            skipStatement();
        }
    }
}

LefLibrary LefParser::parse() {
    if (!isOpen()) {
        error("File is not open: " + filename_);
    }
    
    LefLibrary library;
    
    std::string token;
    while (nextToken(token)) {
        if (token == "MACRO") {
            parseMacro(library);
        } else if (token == "END") {
            // Check if this is END LIBRARY
            std::string end_name;
            if (peekToken(end_name) && end_name == "LIBRARY") {
                consumeToken(); // consume LIBRARY
                // Check for optional semicolon
                std::string next_tok;
                if (peekToken(next_tok) && next_tok == ";") {
                    consumeSemicolon();
                }
                break; // End of file
            } else {
                // Some other END, skip it
                skipStatement();
            }
        } else {
            // Skip unknown statements
            skipStatement();
        }
    }
    
    
    
    return library;
}

} // namespace mini