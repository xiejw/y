// vim: ft=cpp

#pragma once

#define EVE_DISABLE_COPY_CONSTRUCTOR( T ) \
    T( T const & )            = delete;   \
    T &operator=( T const & ) = delete

#define EVE_DECLARE_MOVE_CONSTRUCTOR( T ) \
    T( T && );                            \
    T &operator=( T && )

#define EVE_DEFAULT_MOVE_CONSTRUCTOR( T ) \
    T( T && )            = default;       \
    T &operator=( T && ) = default
