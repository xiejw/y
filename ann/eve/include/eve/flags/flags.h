/*
 * vim: ft=cpp
 *
 * SPDX-FileCopyrightText: Copyright 2023 Denis Glazkov <glazzk.off@mail.ru>
 * SPDX-License-Identifier: MIT
 *
 * *****************************************************************************
 * This is a header only c style header file. Modified a little so c++ compiler
 * can compile it. Also uses std::variant to avoid type-unsafe changes.
 *
 * In general, it is expected only one compilation unit includes this,
 * typically the one with `main` function.
 *
 * The flag value can be set in the following ways
 *
 *     --long_flag_name=value
 *     --long_flag_name value
 *     -short_flag_name value
 *
 * Bool flags are a little special here
 *     --long_flag_name=true|false
 *     --long_flag_name true|false
 *     -short_flag_name true|false
 */

#ifndef C_FLAGS_SINGLE_HEADER_H
#define C_FLAGS_SINGLE_HEADER_H

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>

/**
 * Declare `c_flag_*` function definition for any type.
 *
 * @param T       Flag type (size_t, int, bool, ...)
 * @param postfix Function name postfix
 */
#define DECLARE_C_FLAG_DEF( T, postfix )                                \
    T &c_flag_##postfix( const char *long_name, const char *short_name, \
                         const char *desc, const T default_val );

/* Declare the function defitions for all types by using flags_types header. */
#define C_FLAG_INCLUDE_TYPE( CFlagTypeEnum, T, postfix ) \
    DECLARE_C_FLAG_DEF( T, postfix )
#include "flags_types.h"

#undef C_FLAG_INCLUDE_TYPE

/**
 * Customize usage block of help message.
 * The final help message will contain the following block:
 *
 *  USAGE:
 *     <text defined by you> [OPTIONS] ...
 *
 * @param appname Application name of the usage block
 */
static inline void c_flags_set_application_name( const char *appname );

/**
 * Customize usage block of help message.
 * The final help message will contain the following block:
 *
 * In order for the usage block to be printed in the help
 * message, you must set the application name using
 * the `c_flags_set_application_name()` function.
 *
 *  USAGE:
 *     ... [OPTIONS] <text defined by you>
 *
 * @param description Postitional arguments description of the usage block
 */
static inline void c_flags_set_positional_args_description(
    const char *description );

/**
 * Customize description block of help message.
 * The final help message will contain the following block:
 *
 *  DESCRIPTION:
 *     <text defined by you>
 *
 * @param description Text of the usage block
 */
static inline void c_flags_set_description( const char *description );

/**
 * Parse command line arguments into declared arguments.
 * This function mutate `argc` and `argv` for comfortably positional arguments
 * parsing. Upon failure this function will panic.
 *
 * @param argc_ptr Pointer to program argc
 * @param argv_ptr Pointer to program argv
 * @param usage_on_error Show usage on parsing error
 */
static inline void c_flags_parse( int *argc_ptr, char ***argv_ptr,
                                  bool usage_on_error );

/**
 * Show usage based on your declared flags.
 */
static inline void c_flags_usage( void );

/*
 * Splits header only functions into multiple files so it is easier to change.
 */
#include "flags_internal.h"
#include "flags_parser.h"

#endif  // C_FLAGS_SINGLE_HEADER_H
