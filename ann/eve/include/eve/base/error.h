// vim: ft=cpp
#pragma once

#define errFatalAndExit( fmt, ... )                  \
    eve::base::errFatalAndExit_( __FILE__, __LINE__, \
                                 fmt __VA_OPT__(, ) __VA_ARGS__ )

namespace eve::base {
void errFatalAndExit_( const char *file, int line_no, const char *fmt, ... );
}
