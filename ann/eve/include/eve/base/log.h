// copyright: see license_rxi-mit file.
//
// vim: ft=cpp
#pragma once

// -----------------------------------------------------------------------------
// public helper macros
// -----------------------------------------------------------------------------
#define logTrace( ... ) \
    eve::base::loge_( eve::base::LOG_TRACE, __FILE__, __LINE__, __VA_ARGS__ )
#define logDebug( ... ) \
    eve::base::loge_( eve::base::LOG_DEBUG, __FILE__, __LINE__, __VA_ARGS__ )
#define logInfo( ... ) \
    eve::base::loge_( eve::base::LOG_INFO, __FILE__, __LINE__, __VA_ARGS__ )
#define logWarn( ... ) \
    eve::base::loge_( eve::base::LOG_WARN, __FILE__, __LINE__, __VA_ARGS__ )
#define logError( ... ) \
    eve::base::loge_( eve::base::LOG_ERROR, __FILE__, __LINE__, __VA_ARGS__ )
#define logFatal( ... ) \
    eve::base::loge_( eve::base::LOG_FATAL, __FILE__, __LINE__, __VA_ARGS__ )

// -----------------------------------------------------------------------------
// public low-level apis
// -----------------------------------------------------------------------------
namespace eve::base {

enum Logger {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
};

// isLogLevelOn returns 1 if the logging for given level is on.
int isLogLevelOn( int level );

// setLogLevel sets the current minimum logging level.
int setLogLevel( int level );
int setLogQuiet( int enable );

void loge_( int level, const char *file, int line, const char *fmt, ... );
}  // namespace eve::base
