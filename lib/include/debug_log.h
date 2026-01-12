/**
 * @file debug_log.h
 * @brief Unified Debug Logging System
 * @details Provides compile-time controlled debug logging with zero overhead in production
 */

#ifndef MINI_DEBUG_LOG_H
#define MINI_DEBUG_LOG_H

#include <iostream>
#include <string>

// ============================================================================
// Debug Log Configuration
// ============================================================================

// Global debug switch (defined at compile time)
// -DDEBUG_LOG_ENABLED: Enable debug logging
// -DDEBUG_LOG_LEVEL=0: Disable all logs
// -DDEBUG_LOG_LEVEL=1: Show INFO only
// -DDEBUG_LOG_LEVEL=2: Show INFO + DEBUG

#ifndef DEBUG_LOG_ENABLED
#define DEBUG_LOG_ENABLED 1
#endif

#ifndef DEBUG_LOG_LEVEL
#define DEBUG_LOG_LEVEL 2  // Default: show all logs
#endif

// ============================================================================
// Debug Log Macros
// ============================================================================

#if DEBUG_LOG_ENABLED == 0
    // Completely disable logging (zero overhead)
    #define DEBUG_LOG(tag, msg)
    #define DEBUG_LOG_IF(cond, tag, msg)
    
#else
    // Enable logging
    #if DEBUG_LOG_LEVEL >= 2
        // DEBUG level
        #define DEBUG_LOG(tag, msg) \
            do { \
                std::cout << "[" << tag << "] " << msg << std::endl; \
            } while(0)
        
        #define DEBUG_LOG_IF(cond, tag, msg) \
            do { \
                if (cond) { \
                    std::cout << "[" << tag << "] " << msg << std::endl; \
                } \
            } while(0)
    #else
        // Only show INFO level, hide DEBUG
        #define DEBUG_LOG(tag, msg)
        #define DEBUG_LOG_IF(cond, tag, msg)
    #endif
#endif

// ============================================================================
// Convenience Macros
// ============================================================================

// Use function name as tag
#define DEBUG_LOG_FUNC(msg) DEBUG_LOG(__func__, msg)

// Use class name as tag (requires __PRETTY_FUNCTION__ or manual tag)
// Usage: DEBUG_LOG_CLASS("CellMapper", "message")
#define DEBUG_LOG_CLASS(tag, msg) DEBUG_LOG(tag, msg)

// Conditional logging with function name
#define DEBUG_LOG_FUNC_IF(cond, msg) DEBUG_LOG_IF(cond, __func__, msg)

// ============================================================================
// Routing-Specific Logging
// ============================================================================

// Routing log level (separate control for routing verbosity)
#ifndef ROUTING_LOG_LEVEL
#define ROUTING_LOG_LEVEL 1  // Default: show only important routing messages
#endif

#if DEBUG_LOG_ENABLED == 0
    // Disable all routing logs when debug is disabled
    #define ROUTING_LOG(tag, msg)
    #define ROUTING_LOG_IF(cond, tag, msg)
#else
    // Routing-specific logging
    #if ROUTING_LOG_LEVEL >= 2
        // Show all routing details
        #define ROUTING_LOG(tag, msg) \
            do { \
                std::cout << "[ROUTING] " << msg << std::endl; \
            } while(0)
        
        #define ROUTING_LOG_IF(cond, tag, msg) \
            do { \
                if (cond) { \
                    std::cout << "[ROUTING] " << msg << std::endl; \
                } \
            } while(0)
    #elif ROUTING_LOG_LEVEL >= 1
        // Show only important routing messages (milestones + iteration time)
        #define ROUTING_LOG(tag, msg) \
            do { \
                std::cout << "[ROUTING] " << msg << std::endl; \
            } while(0)
        
        #define ROUTING_LOG_IF(cond, tag, msg) \
            do { \
                if (cond) { \
                    std::cout << "[ROUTING] " << msg << std::endl; \
                } \
            } while(0)
    #else
        // Disable all routing logs
        #define ROUTING_LOG(tag, msg)
        #define ROUTING_LOG_IF(cond, tag, msg)
    #endif
#endif

#endif // MINI_DEBUG_LOG_H