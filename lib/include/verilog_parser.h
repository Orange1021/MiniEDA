/**
 * @file verilog_parser.h
 * @brief Verilog Gate-Level Netlist Parser
 * @details Parses structural Verilog files and populates NetlistDB
 */

#ifndef MINI_VERILOG_PARSER_H
#define MINI_VERILOG_PARSER_H

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include "netlist_db.h"

namespace mini {

/**
 * @brief Verilog parse error types
 */
enum class ParseErrorType {
    SYNTAX_ERROR,           ///< Syntax error
    UNEXPECTED_TOKEN,       ///< Unexpected token
    MISSING_SEMICOLON,      ///< Missing semicolon
    UNKNOWN_GATE_TYPE,      ///< Unknown gate type
    DUPLICATE_DECLARATION,  ///< Duplicate declaration
    UNDEFINED_SIGNAL,       ///< Undefined signal
    FILE_NOT_FOUND          ///< File not found
};

/**
 * @brief Parse error information
 */
struct ParseError {
    ParseErrorType type;    ///< Error type
    size_t line;           ///< Line number
    size_t column;         ///< Column number
    std::string message;   ///< Error message
};

/**
 * @brief Verilog Parser class
 * @details Parses gate-level Verilog netlist and populates NetlistDB
 */
class VerilogParser {
public:
    // ========================================================================
    // Constructor & Destructor
    // ========================================================================

    VerilogParser();
    ~VerilogParser();

    // ========================================================================
    // Main parsing interfaces
    // ========================================================================

    /**
     * @brief Parse Verilog file
     * @param filename Verilog file path
     * @param db Target NetlistDB
     * @return true if parsing succeeded, false otherwise
     */
    bool parseFile(const std::string& filename, NetlistDB& db);

    /**
     * @brief Parse Verilog string
     * @param content Verilog source code content
     * @param db Target NetlistDB
     * @return true if parsing succeeded, false otherwise
     */
    bool parseString(const std::string& content, NetlistDB& db);

    // ========================================================================
    // Error reporting
    // ========================================================================

    /**
     * @brief Get all errors
     * @return Error list
     */
    const std::vector<ParseError>& getErrors() const { return errors_; }

    /**
     * @brief Check if there are errors
     * @return true if has errors, false otherwise
     */
    bool hasErrors() const { return !errors_.empty(); }

    /**
     * @brief Print all errors
     */
    void printErrors() const;

    /**
     * @brief Clear error list
     */
    void clearErrors() { errors_.clear(); }

    // ========================================================================
    // Configuration options
    // ========================================================================

    /**
     * @brief Set strict mode (undefined signal reports error)
     * @param strict true for strict mode, false for relaxed mode
     */
    void setStrictMode(bool strict) { strict_mode_ = strict; }

    /**
     * @brief Set verbose output
     * @param verbose true for verbose output
     */
    void setVerbose(bool verbose) { verbose_ = verbose; }

private:
    // ========================================================================
    // Lexical analysis
    // ========================================================================

    /**
     * @brief Token types
     */
    enum class TokenType {
        KEYWORD,        // module, input, output, wire, endmodule
        IDENTIFIER,     // Identifier (signal name, instance name)
        SEPARATOR,      // Separators (, ), ,, ;
        ENDOFFILE       // End of file
    };

    /**
     * @brief Token structure
     */
    struct Token {
        TokenType type;
        std::string value;
        size_t line;
        size_t column;

        Token(TokenType t, const std::string& v, size_t l, size_t c)
            : type(t), value(v), line(l), column(c) {}
    };

    /**
     * @brief Lexical analysis (tokenization)
     * @param content Source code
     * @return Token list
     */
    std::vector<Token> tokenize(const std::string& content);

    /**
     * @brief Remove comments
     * @param content Source code
     * @return Code with comments removed
     */
    std::string removeComments(const std::string& content);

    /**
     * @brief Check if word is a keyword
     * @param word Word to check
     * @return true if it's a keyword
     */
    bool isKeyword(const std::string& word) const;

    // ========================================================================
    // Syntax analysis
    // ========================================================================

    /**
     * @brief Parse module
     * @param tokens Token list
     * @param db Target database
     * @return true if succeeded
     */
    bool parseModule(const std::vector<Token>& tokens, NetlistDB& db);

    /**
     * @brief Parse module header (module declaration)
     * @param pos Current position (will be modified)
     * @param tokens Token list
     * @param module_name Output: module name
     * @param port_list Output: port list
     * @return true if succeeded
     */
    bool parseModuleHeader(size_t& pos,
                          const std::vector<Token>& tokens,
                          std::string& module_name,
                          std::vector<std::string>& port_list);

    /**
     * @brief Parse port declaration (input/output)
     * @param pos Current position
     * @param tokens Token list
     * @param db Database
     * @param is_input true for input, false for output
     * @return true if succeeded
     */
    bool parsePortDeclaration(size_t& pos,
                             const std::vector<Token>& tokens,
                             NetlistDB& db,
                             bool is_input);

    /**
     * @brief Parse wire declaration
     * @param pos Current position
     * @param tokens Token list
     * @return true if succeeded
     */
    bool parseWireDeclaration(size_t& pos,
                             const std::vector<Token>& tokens);

    /**
     * @brief Parse instantiation statement
     * @param pos Current position
     * @param tokens Token list
     * @param db Database
     * @return true if succeeded
     */
    bool parseInstance(size_t& pos,
                      const std::vector<Token>& tokens,
                      NetlistDB& db);

    // ========================================================================
    // Helper methods
    // ========================================================================

    /**
     * @brief Expect specific token
     * @param pos Current position
     * @param tokens Token list
     * @param expected Expected value
     * @return true if matched successfully
     */
    bool expect(size_t& pos,
               const std::vector<Token>& tokens,
               const std::string& expected);

    /**
     * @brief Map Verilog gate type to CellType
     * @param gate_type Verilog gate type (e.g. "IV", "AN2")
     * @return CellType
     */
    CellType mapGateType(const std::string& gate_type) const;

    /**
     * @brief Get gate pin name list
     * @param gate_type Gate type
     * @return Pin name list (in order)
     */
    std::vector<std::string> getGatePins(const std::string& gate_type) const;

    /**
     * @brief Create or get Net
     * @param db Database
     * @param net_name Net name
     * @return Net pointer
     */
    Net* getOrCreateNet(NetlistDB& db, const std::string& net_name);

    /**
     * @brief Report error
     * @param type Error type
     * @param line Line number
     * @param column Column number
     * @param message Error message
     */
    void reportError(ParseErrorType type,
                    size_t line,
                    size_t column,
                    const std::string& message);

    /**
     * @brief Clean escaped identifier (remove backslash and trailing spaces)
     * @param name Identifier
     * @return Cleaned identifier
     */
    std::string cleanIdentifier(const std::string& name) const;

    // ========================================================================
    // Data members
    // ========================================================================

    std::vector<ParseError> errors_;        ///< Error list
    bool strict_mode_;                      ///< Strict mode
    bool verbose_;                          ///< Verbose output

    // Parsing state
    std::unordered_set<std::string> wire_set_;      ///< Wire set
    std::unordered_set<std::string> port_set_;      ///< Port set
    std::string current_module_;                    ///< Current module name

    // Gate type mapping table
    std::unordered_map<std::string, CellType> gate_type_map_;

    // Keyword set
    std::unordered_set<std::string> keywords_;
};

} // namespace mini

#endif // MINI_VERILOG_PARSER_H
