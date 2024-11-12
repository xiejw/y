// vim: ft=cpp
#pragma once

typedef struct {
    const char *data;
    size_t      size;
} StringView;

#define SVFMT       "%.*s"
#define SVARG( sv ) (int)( sv ).size, ( sv ).data

/**
 * Create string view from string
 *
 * @param string line to create
 * @return string view instance
 */
static inline StringView sv_from_string( const char *string );

/**
 * Create string view from `count` characters of passed string view
 *
 * @param sv string view from which a new one is created
 * @param count number of characters
 * @return string view instance
 */
static inline StringView sv_slice_left( StringView sv, size_t count );

/**
 * Create string view by indenting `count` characters to the left of passed
 * string view
 *
 * @param sv string view from which a new one is created
 * @param count number of characters to indenting
 * @return string view instance
 */
static inline StringView sv_chop_left( StringView sv, size_t count );

/**
 * Check if string view instances are equal
 *
 * @param a first string view instance
 * @param b second string view instance
 * @return true if equal, otherwise false
 */
static inline bool sv_equal( StringView a, StringView b );

/**
 * Check if one string view starts with the second one
 *
 * @param a first string view instance
 * @param b second string view instance
 * @return true if `a` starts with `b`, otherwise false
 */
static inline bool sv_starts_with( StringView a, StringView b );

/**
 * Check if one string view contains the second one
 *
 * @param a first string view instance
 * @param b second string view instance
 * @return true if `a` contains `b`, otherwise false
 */
static inline bool sv_contains( StringView a, StringView b );

/**
 * Find index where the second string view starts in the first
 *
 * @param a first string view instance
 * @param b second string view instance
 * @return index in `a` string if `a` contains `b`, otherwise -1
 */
static inline int sv_index_of( StringView a, StringView b );

static inline StringView
sv_from_string( const char *string )
{
    if ( string == NULL ) {
        auto result = StringView{ .data = NULL, .size = 0 };
        return result;
    }

    auto result = StringView{ .data = string, .size = strlen( string ) };
    return result;
}

static inline StringView
sv_slice_left( StringView sv, size_t count )
{
    if ( count == 0 || sv.size < count ) return sv_from_string( NULL );

    auto result = StringView{ .data = sv.data, .size = count };
    return result;
}

static inline StringView
sv_chop_left( StringView sv, size_t count )
{
    if ( sv.size <= count ) return sv_from_string( NULL );

    auto result =
        StringView{ .data = sv.data + count, .size = sv.size - count };
    return result;
}

static inline bool
sv_equal( StringView a, StringView b )
{
    if ( a.size != b.size ) return false;

    return !memcmp( a.data, b.data, a.size );
}

static inline bool
sv_starts_with( StringView a, StringView b )
{
    if ( b.data == NULL ) return false;

    return sv_equal( sv_slice_left( a, b.size ), b );
}

static inline bool
sv_contains( StringView a, StringView b )
{
    return sv_index_of( a, b ) != -1;
}

static inline int
sv_index_of( StringView a, StringView b )
{
    StringView null = sv_from_string( NULL );

    if ( sv_equal( a, null ) || sv_equal( b, null ) ) return -1;

    const char *pos = strstr( a.data, b.data );
    if ( pos == NULL ) return -1;

    return (int)( pos - a.data );
}
