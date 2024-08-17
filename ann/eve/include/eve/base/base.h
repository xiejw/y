// vim: ft=cpp

#pragma once

#define EVE_DISALBE_COPY_COSTRUCTOR( T ) \
    T( T const & )            = delete;  \
    T &operator=( T const & ) = delete

#define EVE_DECLARE_MOVE_COSTRUCTOR( T ) \
    T( T && );                           \
    T &operator=( T && )
