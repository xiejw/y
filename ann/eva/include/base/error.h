#ifndef EVA_ERROR_H_
#define EVA_ERROR_H_

// =============================================================================
//
// -----------------------------------------------------------------------------
// design
// -----------------------------------------------------------------------------
//
// - All error codes are negative numbers. 0 is success.
//     - This makes the error checking super cheap according to ABIs in most
//       platforms.
//
// - Limited error codes are pre-defined here.
//     - If possible matching to these cannonical codes or just -1 (ERROR)
//
// -----------------------------------------------------------------------------
// behavior
// -----------------------------------------------------------------------------
//
// - Upon success, the control flow should be as fast as checking an int,
//   typically the w/x0 register in calling convention.
//
// - Upon failure,
//   - if the calling API is not `error_t` type, use `errNewWithNote`, or
//     `emitNew`.
//
//   - if the calling API is `error_t` type
//     - use `errEmitNote` to propagate the error to upstream with one more note
//       emitted.
//     - use `errFree` to free all resources if not propagate any further. The
//       resources should be freed.
//
// - Call `errDump` to print the correct error message stack to stderr. Resource
//   related errors will NOT be freed.
//
// - Call `errFatalAndExit` or errFatalAndExit1 to quit the application (error
//   message stack will be printed also.). Alias like errPanic[1] are also
//   provided.
//
// -----------------------------------------------------------------------------
// caveat
// -----------------------------------------------------------------------------
// - As error is the core data structure for eva, to avoid dependency loop, it
//   basically cannot import any other library, e.g., sds.
//
//   errDumpSds's first argument is void* rather than its true type _mut_ sds*
// =============================================================================

// =============================================================================
// data types and predefined error enums.
// =============================================================================
typedef int error_t;

#define OK 0

#define ERROR     -1
#define EMALLOC   -2
#define ENOTEXIST -3
#define ENOTIMPL  -4

// -----------------------------------------------------------------------------
// apis for error handling
// -----------------------------------------------------------------------------

extern error_t errNew( const char *fmt, ... );
extern error_t errNewWithNote( error_t err, const char *fmt, ... );
extern error_t errEmitNote( const char *fmt, ... );
extern void    errFree( void );

extern error_t errNum( void );      // error must be set already
extern error_t errNumOrOK( void );  // ret OK if no error
extern void    errDump( const char *, ... );
extern void    errDumpSds( void *sds, const char *,
                           ... );  // _mut_ sds_t * for sds

#define errFatalAndExit( fmt, ... ) \
        errFatalAndExit_( __FILE__, __LINE__, fmt, __VA_ARGS__ )
#define errFatalAndExit1( fmt ) errFatalAndExit_( __FILE__, __LINE__, fmt )

// alias
#define errPanic( ... ) errFatalAndExit_( __FILE__, __LINE__, __VA_ARGS__ )

// -----------------------------------------------------------------------------
// helper macro.
// -----------------------------------------------------------------------------
#define errMalloc( )                                                        \
        errNewWithNote( EMALLOC, "malloc err. file: %s, loc: %d", __FILE__, \
                        __LINE__ )

// -----------------------------------------------------------------------------
// private prototype.
// -----------------------------------------------------------------------------
extern error_t errFatalAndExit_( char *file, int line_no, const char *fmt,
                                 ... );

#endif  // EVA_ERROR_H_
