// copyright: see license_rxi-mit file.
//
// vim: ft=cpp
#pragma once

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// =============================================================================
// public macros and apis
// =============================================================================

// -----------------------------------------------------------------------------
// public helper macros
// -----------------------------------------------------------------------------
#define logTrace( ... )                                             \
    loge( eve::base::Logger::TRACE, /*dump=*/0, __FILE__, __LINE__, \
          __VA_ARGS__ )
#define logDebug( ... )                                             \
    loge( eve::base::Logger::DEBUG, /*dump=*/0, __FILE__, __LINE__, \
          __VA_ARGS__ )
#define logInfo( ... ) \
    loge( eve::base::Logger::INFO, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logWarn( ... ) \
    loge( eve::base::Logger::WARN, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logError( ... )                                             \
    loge( eve::base::Logger::ERROR, /*dump=*/0, __FILE__, __LINE__, \
          __VA_ARGS__ )
#define logFatal( ... )                                             \
    loge( eve::base::Logger::FATAL, /*dump=*/0, __FILE__, __LINE__, \
          __VA_ARGS__ )
#define logFatalAndDump( ... )                                      \
    loge( eve::base::Logger::FATAL, /*dump=*/1, __FILE__, __LINE__, \
          __VA_ARGS__ )

// -----------------------------------------------------------------------------
// public low-level apis
// -----------------------------------------------------------------------------
namespace eve::base {

enum Logger {
    TRACE,
    DEBUG,
    INFO,
    WARN,
    ERROR,
    FATAL,
};

// logIsLevelOn returns 1 if the logging for given level is on.
int logIsLevelOn( int level );

// logSetLevel sets the current minimum logging level.
int logSetLevel( int level );
int logSetQuiet( int enable );

void loge( int level, int dump, const char *file, int line, const char *fmt,
           ... );
}  // namespace eve::base
