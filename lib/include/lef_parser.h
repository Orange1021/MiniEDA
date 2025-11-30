/**
 * @file lef_parser.h
 * @brief LEF (Library Exchange Format) Parser for MiniPlacement
 * @details Extracts physical dimensions and pin locations for Standard Cells
 */

#ifndef MINI_LEF_PARSER_H
#define MINI_LEF_PARSER_H

#include "geometry.h"
#include <string>
#include <vector>
#include <unordered_map>
#include <memory>
#include <fstream>

namespace mini {

/**
 * @class LefPort
 * @brief Stores a list of Rect (the metal shapes of a pin)
 */
class LefPort {
public:
    std::vector<Rect> rects;  // Metal shapes for this pin
    
    LefPort() = default;
    
    /**
     * @brief Add a rectangle to this port
     * @param rect Rectangle to add (relative coordinates)
     */
    void addRect(const Rect& rect) {
        rects.push_back(rect);
    }
    
    /**
     * @brief Get all rectangles for this port
     * @return Vector of rectangles
     */
    const std::vector<Rect>& getRects() const {
        return rects;
    }
    
    /**
     * @brief Get bounding box of all rectangles
     * @return Bounding rectangle
     */
    Rect getBoundingBox() const {
        if (rects.empty()) {
            return Rect{0, 0, 0, 0};
        }
        
        Rect bbox = rects[0];
        for (const auto& rect : rects) {
            bbox.x_min = std::min(bbox.x_min, rect.x_min);
            bbox.y_min = std::min(bbox.y_min, rect.y_min);
            bbox.x_max = std::max(bbox.x_max, rect.x_max);
            bbox.y_max = std::max(bbox.y_max, rect.y_max);
        }
        return bbox;
    }
};

/**
 * @class LefMacro
 * @brief Stores macro information including size and pins
 */
class LefMacro {
public:
    std::string name;
    double width;
    double height;
    std::unordered_map<std::string, LefPort> pins;
    
    LefMacro() : width(0.0), height(0.0) {}
    LefMacro(const std::string& macro_name, double w, double h) 
        : name(macro_name), width(w), height(h) {}
    
    /**
     * @brief Add a pin to this macro
     * @param pin_name Pin name
     * @param port Port information
     */
    void addPin(const std::string& pin_name, const LefPort& port) {
        pins[pin_name] = port;
    }
    
    /**
     * @brief Get pin by name
     * @param pin_name Pin name
     * @return Pointer to port, nullptr if not found
     */
    const LefPort* getPin(const std::string& pin_name) const {
        auto it = pins.find(pin_name);
        return (it != pins.end()) ? &it->second : nullptr;
    }
    
    /**
     * @brief Get all pin names
     * @return Vector of pin names
     */
    std::vector<std::string> getPinNames() const {
        std::vector<std::string> names;
        for (const auto& pair : pins) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Get macro area
     * @return Area in square micrometers
     */
    double getArea() const {
        return width * height;
    }
};

/**
 * @class LefLibrary
 * @brief Container for all LEF macros
 */
class LefLibrary {
public:
    std::unordered_map<std::string, LefMacro> macros;
    
    /**
     * @brief Add a macro to the library
     * @param macro Macro to add
     */
    void addMacro(const LefMacro& macro) {
        macros[macro.name] = macro;
    }
    
    /**
     * @brief Get macro by name
     * @param macro_name Macro name
     * @return Pointer to macro, nullptr if not found
     */
    const LefMacro* getMacro(const std::string& macro_name) const {
        auto it = macros.find(macro_name);
        return (it != macros.end()) ? &it->second : nullptr;
    }
    
    /**
     * @brief Get all macro names
     * @return Vector of macro names
     */
    std::vector<std::string> getMacroNames() const {
        std::vector<std::string> names;
        for (const auto& pair : macros) {
            names.push_back(pair.first);
        }
        return names;
    }
    
    /**
     * @brief Get number of macros
     * @return Macro count
     */
    size_t getMacroCount() const {
        return macros.size();
    }
    
    /**
     * @brief Clear all macros
     */
    void clear() {
        macros.clear();
    }
};

/**
 * @class LefParser
 * @brief LEF file parser with robust error handling
 */
class LefParser {
private:
    std::ifstream file_;
    std::string filename_;
    int line_number_;
    int column_number_;
    bool verbose_;
    
    // Token buffer for lookahead
    std::string current_token_;
    bool has_token_;
    
    // Error handling
    void error(const std::string& message);
    void warning(const std::string& message);
    
    // Token management
    bool nextToken(std::string& token);
    bool peekToken(std::string& token);
    void consumeToken();
    void consumeKeyword(const std::string& expected);
    void consumeSemicolon();
    
    // Parsing utilities
    void skipStatement();
    void skipToEndKeyword(const std::string& keyword);
    double parseNumber();
    Rect parseRect();
    
    // LEF parsing methods
    void parseLefLibrary();
    void parseMacro(LefLibrary& library);
    void parseSize(double& width, double& height);
    void parsePin(LefMacro& macro);
    void parsePort(LefPort& port);
    
public:
    /**
     * @brief Constructor
     * @param filename LEF file path
     * @param verbose Enable verbose output
     */
    explicit LefParser(const std::string& filename, bool verbose = false);
    
    /**
     * @brief Destructor
     */
    ~LefParser();
    
    /**
     * @brief Parse LEF file and return library
     * @return Parsed LEF library
     */
    LefLibrary parse();
    
    /**
     * @brief Check if file is open
     * @return True if file is open and readable
     */
    bool isOpen() const;
    
    /**
     * @brief Get current line number
     * @return Line number in LEF file
     */
    int getLineNumber() const { return line_number_; }
    
    /**
     * @brief Get current column number
     * @return Column number in LEF file
     */
    int getColumnNumber() const { return column_number_; }
};

} // namespace mini

#endif // MINI_LEF_PARSER_H