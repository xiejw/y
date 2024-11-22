// vim: ft=cpp
//
// Not intended for normal include. Use flags.h instead.
//
#pragma once
#include "flags_string_view.h"

#include <variant>

#ifndef C_FLAGS_CAPACITY
#define C_FLAGS_CAPACITY 64
#endif

/* Declares all supported types in an enum. */
typedef enum {
#define C_FLAG_INCLUDE_TYPE( CFlagTypeEnum, tp, postfix ) CFlagTypeEnum,
#include "flags_types.h"
#undef C_FLAG_INCLUDE_TYPE
} CFlagType;

using CFlagDataType = std::variant<
#define C_FLAG_INCLUDE_TYPE( CFlagTypeEnum, tp, postfix )      tp,
#define C_FLAG_INCLUDE_TYPE_LAST( CFlagTypeEnum, tp, postfix ) tp
#include "flags_types.h"
#undef C_FLAG_INCLUDE_TYPE
#undef C_FLAG_INCLUDE_TYPE_LAST
    >;

typedef struct {
    const char   *long_name;
    const char   *short_name;
    const char   *desc;
    CFlagDataType default_data;
    CFlagDataType data;
} CFlag;

static CFlag  flags[C_FLAGS_CAPACITY] = { };
static size_t flags_size              = 0;

static char *c_flags_appname_message     = NULL;
static char *c_flags_pos_args_desc       = NULL;
static char *c_flags_description_message = NULL;

static inline void
c_flags_reset_internal( )
{
    for ( size_t idx = 0; idx < flags_size; idx++ ) {
        flags[idx].~CFlag( );
    }
    flags_size                  = 0;
    c_flags_appname_message     = NULL;
    c_flags_pos_args_desc       = NULL;
    c_flags_description_message = NULL;
}

static inline bool
c_flags_names_unique_internal( const char *long_name, const char *short_name )
{
    for ( size_t i = 0; i < flags_size; i++ ) {
        CFlag *flag = &flags[i];

        if ( !strcmp( long_name, flag->long_name ) ) return false;

        if ( short_name && flag->short_name &&
             !strcmp( short_name, flag->short_name ) )
            return false;
    }

    return true;
}
#define C_FLAG_FILL( flag, _long_name, _short_name, _desc ) \
    {                                                       \
        ( flag )->long_name  = ( _long_name );              \
        ( flag )->short_name = ( _short_name );             \
        ( flag )->desc       = ( _desc );                   \
    }

#define DECLARE_C_FLAG_IMPL( CFlagTypeIndex, ptr_type, postfix )               \
    ptr_type &c_flag_##postfix( const char *long_name, const char *short_name, \
                                const char *desc, const ptr_type default_val ) \
    {                                                                          \
        assert( flags_size < C_FLAGS_CAPACITY &&                               \
                "exceeding the maximum number of flags, "                      \
                "please define C_FLAGS_CAPACITY according "                    \
                "to your needs when compile" );                                \
        assert( long_name != NULL &&                                           \
                "the long name is required and cannot be NULL" );              \
        assert( c_flags_names_unique_internal( long_name, short_name ) &&      \
                "flag names must be unique" );                                 \
                                                                               \
        CFlag *flag = &flags[flags_size++];                                    \
        new ( flag ) CFlag( );                                                 \
                                                                               \
        C_FLAG_FILL( flag, long_name, short_name, desc )                       \
                                                                               \
        flag->default_data = CFlagDataType{                                    \
            std::in_place_index<CFlagTypeIndex>, (ptr_type)default_val };      \
                                                                               \
        flag->data = CFlagDataType{ std::in_place_index<CFlagTypeIndex>,       \
                                    (ptr_type)default_val };                   \
                                                                               \
        return std::get<CFlagTypeIndex>( flag->data );                         \
    }

#define C_FLAG_INCLUDE_TYPE( CFlagTypeEnum, tp, postfix ) \
    DECLARE_C_FLAG_IMPL( CFlagTypeEnum, tp, postfix )
#include "flags_types.h"
#undef C_FLAG_INCLUDE_TYPE

static inline void
c_flags_set_application_name( const char *appname )
{
    c_flags_appname_message = (char *)appname;
}

static inline void
c_flags_set_positional_args_description( const char *description )
{
    c_flags_pos_args_desc = (char *)description;
}

static inline void
c_flags_set_description( const char *description )
{
    c_flags_description_message = (char *)description;
}

static inline CFlag *
find_c_flag_by_long_name( StringView long_name )
{
    for ( size_t i = 0; i < flags_size; i++ ) {
        CFlag *flag = &flags[i];

        if ( sv_equal( sv_from_string( flag->long_name ), long_name ) )
            return flag;
    }

    return NULL;
}

static inline CFlag *
find_c_flag_by_short_name( StringView short_name )
{
    for ( size_t i = 0; i < flags_size; i++ ) {
        CFlag *flag = &flags[i];

        if ( sv_equal( sv_from_string( flag->short_name ), short_name ) )
            return flag;
    }

    return NULL;
}

static inline const char *
c_flag_default_to_str( const CFlag *flag )
{
    static char buff[32] = { 0 };

    switch ( CFlagType( flag->data.index( ) ) ) {
#define SPRINT_CFLAG_TYPE( EnumName, printf_str )             \
    case EnumName:                                            \
        snprintf( buff, sizeof( buff ), "%" printf_str,       \
                  std::get<EnumName>( flag->default_data ) ); \
        return buff;

        SPRINT_CFLAG_TYPE( C_FLAG_INT, "d" )
        SPRINT_CFLAG_TYPE( C_FLAG_INT_8, PRId8 )
        SPRINT_CFLAG_TYPE( C_FLAG_INT_16, PRId16 )
        SPRINT_CFLAG_TYPE( C_FLAG_INT_32, PRId32 )
        SPRINT_CFLAG_TYPE( C_FLAG_INT_64, PRId64 )
        SPRINT_CFLAG_TYPE( C_FLAG_UNSIGNED, "u" )
        SPRINT_CFLAG_TYPE( C_FLAG_UINT_8, PRIu8 )
        SPRINT_CFLAG_TYPE( C_FLAG_UINT_16, PRIu16 )
        SPRINT_CFLAG_TYPE( C_FLAG_UINT_32, PRIu32 )
        SPRINT_CFLAG_TYPE( C_FLAG_UINT_64, PRIu64 )
        SPRINT_CFLAG_TYPE( C_FLAG_SIZE_T, "zu" )
        SPRINT_CFLAG_TYPE( C_FLAG_FLOAT, "f" )
        SPRINT_CFLAG_TYPE( C_FLAG_DOUBLE, "lf" )
#undef SPRINT_CFLAG_TYPE

    case C_FLAG_BOOL:
        return std::get<C_FLAG_BOOL>( flag->default_data ) ? "true" : "false";
    case C_FLAG_STRING:
        return std::get<C_FLAG_STRING>( flag->default_data );
    default:
        assert( false &&
                "not all flag types implements c_flag_default_to_str()" );
    }

    return "unreachable";
}

static inline void
c_flags_usage( void )
{
    if ( c_flags_appname_message )
        printf( "USAGE:\n   %s%s%s\n\n", c_flags_appname_message,
                ( flags_size > 0 ) ? " [OPTIONS] " : " ",
                c_flags_pos_args_desc ? c_flags_pos_args_desc : "" );

    if ( c_flags_description_message )
        printf( "DESCRIPTION:\n   %s\n\n", c_flags_description_message );

    if ( flags_size > 0 ) printf( "OPTIONS:" );

    for ( size_t i = 0; i < flags_size; i++ ) {
        const CFlag *flag = &flags[i];

        printf( "\n   --%s", flag->long_name );
        if ( flag->short_name != NULL ) printf( ", -%s", flag->short_name );
        printf( "\n" );

        if ( flag->desc != NULL )
            printf( "       Description: %s\n", flag->desc );

        const char *default_val = c_flag_default_to_str( flag );
        if ( default_val != NULL )
            printf( "       Default: %s\n", c_flag_default_to_str( flag ) );
    }
}
