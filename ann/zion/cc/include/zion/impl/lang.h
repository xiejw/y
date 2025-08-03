// vim: ft=cpp
#pragma once

#include <stdint.h>

// === --- Macros for Class ------------------------------------------------ ===
#define ZION_DISABLE_COPY( T )              \
        T( const T & )            = delete; \
        T &operator=( const T & ) = delete;

#define ZION_DISABLE_MOVE( T )         \
        T( T && )            = delete; \
        T &operator=( T && ) = delete;

#define ZION_DISABLE_COPY_AND_MOVE( T ) \
        ZION_DISABLE_COPY( T ) ZION_DISABLE_MOVE( T )

// === --- Macros for Type ------------------------------------------------- ===
typedef uint64_t u64;
typedef int64_t  i64;
typedef uint32_t u32;
typedef int32_t  i32;
typedef uint8_t  u8;
typedef float    f32;
typedef double   f64;
