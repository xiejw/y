// vim: ft=cpp
#pragma once

namespace {
// Functions to generate type names for supported c types.
template <class T>
const inline char *
c_flags_type_name_internal( )
{
    return "unknown";
}

template <>
const inline char *
c_flags_type_name_internal<int>( )
{
    return "int";
}
}  // namespace

namespace {
template <class T>
[[nodiscard]] static inline int
c_flags_load_signed_tp( CFlag *flag, bool is_flag_long, char *value )
{
    char *end_ptr;
    errno = 0;

    long long number = strtoll( value, &end_ptr, 10 );
    bool      value_fully_parsed =
        (size_t)( end_ptr - ( value ) ) == strlen( value );

    /*
     * Getting signed number limits
     *  - min: b'10...0
     *  - max: b'01...1
     */
    T max   = 1;
    T min   = T( ( max ) << ( sizeof( T ) * 8 - 1 ) );
    ( max ) = ~( min );

    if ( errno != 0 || !value_fully_parsed || number < ( min ) ||
         number > ( max ) ) {
        printf( "ERROR: invalid value %s for %s flag %s%s\n", ( value ),
                c_flags_type_name_internal<T>( ), ( is_flag_long ) ? "--" : "-",
                ( is_flag_long ) ? ( flag )->long_name : ( flag )->short_name );

        return -1;
    }

    *C_FLAG_DATA_AS_PTR( flag, T ) = (T)number;
    return 0;
}

template <class T>
[[nodiscard]] static int
c_flags_load_unsigned_tp( CFlag *flag, bool is_flag_long, char *value )
{
    char *end_ptr;
    errno = 0;

    unsigned long long number = strtoull( value, &end_ptr, 10 );
    bool               value_fully_parsed =
        (size_t)( end_ptr - ( value ) ) == strlen( value );

    /*
     * Getting unsigned number limits
     *  - max: b'1...1
     */
    T max   = 0;
    ( max ) = ~( max );

    if ( errno != 0 || ( value )[0] == '-' || !value_fully_parsed ||
         number > ( max ) ) {
        printf( "ERROR: invalid value %s for %s flag %s%s\n", ( value ),
                c_flags_type_name_internal<T>( ), ( is_flag_long ) ? "--" : "-",
                ( is_flag_long ) ? ( flag )->long_name : ( flag )->short_name );
        return -1;
    }

    *C_FLAG_DATA_AS_PTR( flag, T ) = (T)number;
    return 0;
}

#define C_FLAG_LOAD_FLOATING_VALUE( flag, is_flag_long, ptr_type, value,     \
                                    strtox_fun )                             \
    {                                                                        \
        char *end_ptr;                                                       \
        errno = 0;                                                           \
                                                                             \
        ptr_type number = strtox_fun( value, &end_ptr );                     \
        bool     value_fully_parsed =                                        \
            (size_t)( end_ptr - ( value ) ) == strlen( value );              \
                                                                             \
        if ( errno != 0 || !value_fully_parsed ) {                           \
            printf( "ERROR: invalid value %s for " #ptr_type " flag %s%s\n", \
                    ( value ), ( is_flag_long ) ? "--" : "-",                \
                    ( is_flag_long ) ? ( flag )->long_name                   \
                                     : ( flag )->short_name );               \
            return -1;                                                       \
        }                                                                    \
                                                                             \
        *C_FLAG_DATA_AS_PTR( flag, ptr_type ) = number;                      \
        return 0;                                                            \
    }

}  // namespace

[[nodiscard]] static inline int
c_flags_parse_internal( int *argc_ptr, char ***argv_ptr )
{
    int    argc = *argc_ptr;
    char **argv = *argv_ptr;

    assert( argc > 0 && "argc must be grater then 0" );

    int arg = 1;
    while ( arg < argc ) {
        StringView token = sv_from_string( argv[arg] );
        assert( token.data != NULL && "argv cannot be NULL" );

        CFlag     *flag      = NULL;
        bool       flag_long = false;
        StringView sv_value  = sv_from_string( NULL );

        // `--flag value` or `--flag=value`
        if ( sv_starts_with( token, sv_from_string( "--" ) ) ) {
            flag_long = true;
            if ( !sv_contains( token,
                               sv_from_string( "=" ) ) ) {  // `--flag value`
                StringView sv_long_name = sv_chop_left( token, strlen( "--" ) );

                flag = find_c_flag_by_long_name( sv_long_name );
                if ( flag == NULL ) {
                    printf( "ERROR: unknown flag --" SVFMT "\n",
                            SVARG( sv_long_name ) );
                    goto error;
                }

                if ( arg + 1 >= argc ) {
                    printf( "ERROR: no value for flag --" SVFMT "\n",
                            SVARG( sv_long_name ) );
                    goto error;
                }

                sv_value = sv_from_string( argv[++arg] );
            } else {  // `--flag=value`
                size_t index_of_eq =
                    (size_t)sv_index_of( token, sv_from_string( "=" ) );
                StringView sv_long_name = sv_chop_left(
                    sv_slice_left( token, index_of_eq ), strlen( "--" ) );

                sv_value = sv_chop_left( token, index_of_eq + 1 );

                if ( sv_value.data == NULL ) {  // `--flag=`
                    printf( "ERROR: no value for flag --" SVFMT "\n",
                            SVARG( sv_long_name ) );
                    goto error;
                }

                flag = find_c_flag_by_long_name( sv_long_name );
                if ( flag == NULL ) {
                    printf( "ERROR: unknown flag --" SVFMT "\n",
                            SVARG( sv_long_name ) );
                    goto error;
                }
            }

        } else if ( sv_starts_with( token,
                                    sv_from_string( "-" ) ) ) {  // `-f value`
            StringView sv_short_name = sv_chop_left( token, strlen( "-" ) );

            flag = find_c_flag_by_short_name( sv_short_name );
            if ( flag == NULL ) {
                printf( "ERROR: unknown flag -" SVFMT "\n",
                        SVARG( sv_short_name ) );
                goto error;
            }

            if ( arg + 1 >= argc ) {
                printf( "ERROR: no value for flag -" SVFMT "\n",
                        SVARG( sv_short_name ) );
                goto error;
            }

            sv_value = sv_from_string( argv[++arg] );
        } else {  // positional arguments
            break;
        }

        char *value = (char *)sv_value.data;

        switch ( flag->type ) {
        case C_FLAG_INT:
            if ( c_flags_load_signed_tp<int>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_8:
            if ( c_flags_load_signed_tp<int8_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_16:
            if ( c_flags_load_signed_tp<int16_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_32:
            if ( c_flags_load_signed_tp<int32_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_64:
            if ( c_flags_load_signed_tp<int64_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UNSIGNED:
            if ( c_flags_load_unsigned_tp<unsigned>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_8:
            if ( c_flags_load_unsigned_tp<uint8_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_16:
            if ( c_flags_load_unsigned_tp<uint16_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_32:
            if ( c_flags_load_unsigned_tp<uint32_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_64:
            if ( c_flags_load_unsigned_tp<uint64_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_SIZE_T:
            if ( c_flags_load_unsigned_tp<size_t>( flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_BOOL:
            *C_FLAG_DATA_AS_PTR( flag, bool ) = true;
            break;
        case C_FLAG_STRING:
            *C_FLAG_DATA_AS_PTR( flag, char * ) = value;
            break;
        case C_FLAG_FLOAT:
            C_FLAG_LOAD_FLOATING_VALUE( flag, flag_long, float, value, strtof )
            break;
        case C_FLAG_DOUBLE:
            C_FLAG_LOAD_FLOATING_VALUE( flag, flag_long, double, value, strtod )
            break;
        default:
            assert( false && "not all flag types implements c_flags_parse()" );
        }

        arg += 1;
    }

    *argc_ptr = argc - arg;
    *argv_ptr = argv + arg;

    return 0;

error:
    return -1;
}

static inline void
c_flags_parse( int *argc_ptr, char ***argv_ptr, bool usage_on_error )
{
    int result_code = c_flags_parse_internal( argc_ptr, argv_ptr );
    if ( result_code != 0 ) {
        if ( usage_on_error ) {
            printf( "\n" );
            c_flags_usage( );
        }

        exit( 1 );
    }
}
