// vim: ft=cpp
//
// Not intended for normal include. Use flags.h instead.
//
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
template <class T, int CFlagTypeIndex>
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

    std::get<CFlagTypeIndex>( flag->data ) = (T)number;
    return 0;
}

template <class T, int CFlagTypeIndex>
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

    std::get<CFlagTypeIndex>( flag->data ) = (T)number;
    return 0;
}

template <class T>
[[nodiscard]] static int
c_flags_load_ft( CFlag *flag, bool is_flag_long, char *value,
                 T ( *strtox_fun )( const char *str, char **endptr ) )
{
    char *end_ptr;
    errno = 0;

    T    number = strtox_fun( value, &end_ptr );
    bool value_fully_parsed =
        (size_t)( end_ptr - ( value ) ) == strlen( value );

    if ( errno != 0 || !value_fully_parsed ) {
        printf( "ERROR: invalid value %s for %s flag %s%s\n", ( value ),
                c_flags_type_name_internal<T>( ), ( is_flag_long ) ? "--" : "-",
                ( is_flag_long ) ? ( flag )->long_name : ( flag )->short_name );
        return -1;
    }

    std::get<T>( flag->data ) = (T)number;
    return 0;
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

        switch ( CFlagType( flag->data.index( ) ) ) {
        case C_FLAG_INT:
            if ( c_flags_load_signed_tp<int, C_FLAG_INT>( flag, flag_long,
                                                          value ) )
                goto error;
            break;
        case C_FLAG_INT_8:
            if ( c_flags_load_signed_tp<int8_t, C_FLAG_INT_8>( flag, flag_long,
                                                               value ) )
                goto error;
            break;
        case C_FLAG_INT_16:
            if ( c_flags_load_signed_tp<int16_t, C_FLAG_INT_16>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_32:
            if ( c_flags_load_signed_tp<int32_t, C_FLAG_INT_32>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_INT_64:
            if ( c_flags_load_signed_tp<int64_t, C_FLAG_INT_64>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UNSIGNED:
            if ( c_flags_load_unsigned_tp<unsigned, C_FLAG_UNSIGNED>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_8:
            if ( c_flags_load_unsigned_tp<uint8_t, C_FLAG_UINT_8>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_16:
            if ( c_flags_load_unsigned_tp<uint16_t, C_FLAG_UINT_16>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_32:
            if ( c_flags_load_unsigned_tp<uint32_t, C_FLAG_UINT_32>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_UINT_64:
            if ( c_flags_load_unsigned_tp<uint64_t, C_FLAG_UINT_64>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_SIZE_T:
            if ( c_flags_load_unsigned_tp<size_t, C_FLAG_SIZE_T>(
                     flag, flag_long, value ) )
                goto error;
            break;
        case C_FLAG_BOOL:
            std::get<C_FLAG_BOOL>( flag->data ) = true;
            break;
        case C_FLAG_STRING:
            std::get<C_FLAG_STRING>( flag->data ) = value;
            break;
        case C_FLAG_FLOAT:
            if ( c_flags_load_ft<float>( flag, flag_long, value, strtof ) )
                goto error;
            break;
        case C_FLAG_DOUBLE:
            if ( c_flags_load_ft<double>( flag, flag_long, value, strtod ) )
                goto error;
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
