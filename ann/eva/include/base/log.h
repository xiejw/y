// copyright: see license_rxi-mit file.

#ifndef EVA_LOG_H
#define EVA_LOG_H

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

// =============================================================================
// public macros and apis
// =============================================================================

// -----------------------------------------------------------------------------
// public helper macros
// -----------------------------------------------------------------------------
#define logTrace( ... ) \
    loge( LOG_TRACE, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logDebug( ... ) \
    loge( LOG_DEBUG, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logInfo( ... ) \
    loge( LOG_INFO, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logWarn( ... ) \
    loge( LOG_WARN, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logError( ... ) \
    loge( LOG_ERROR, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logFatal( ... ) \
    loge( LOG_FATAL, /*dump=*/0, __FILE__, __LINE__, __VA_ARGS__ )
#define logFatalAndDump( ... ) \
    loge( LOG_FATAL, /*dump=*/1, __FILE__, __LINE__, __VA_ARGS__ )

// -----------------------------------------------------------------------------
// public low-level apis
// -----------------------------------------------------------------------------
enum {
    LOG_TRACE,
    LOG_DEBUG,
    LOG_INFO,
    LOG_WARN,
    LOG_ERROR,
    LOG_FATAL,
};

// logIsLevelOn returns 1 if the logging for given level is on.
extern int logIsLevelOn( int level );

// logSetLevel sets the current minimum logging level.
extern int logSetLevel( int level );
extern int logSetQuiet( int enable );

// =============================================================================
// private macros
// =============================================================================
#ifndef _SKIP_ASAN
#define loge( ... ) loge_( __VA_ARGS__ )
#else  // _SKIP_ASAN
#define loge( ... ) _simple_loge_( __VA_ARGS__ )
#endif  // _SKIP_ASAN

extern void loge_( int level, int dump, const char *file, int line,
                   const char *fmt, ... );
extern void _simple_loge_( int level, const char *file, int line,
                           const char *fmt, ... );

#endif  // EVA_LOG_H
