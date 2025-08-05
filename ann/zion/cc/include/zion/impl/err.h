// vim: ft=cpp
//
// A non-thread-safe Error to record diagnose notes when Error occurs. The
// call stacks can record more context, i.e., more notes, to the same
// Error.
//
// ```
// # With std::expected
// ```
//
#pragma once

#include <memory>

#include "lang.h"
#include "log.h"

namespace zion {

/* An Error records diagnose notes when error occurs. This is not thread safe
 * in general.
 */
class Error {
      public:
        enum class Kind {
                /* Result codes */
                EndOfFile,

                /* Error codes */
                IOError,
                LengthError,
                RuntimeError,
        };

      private:
        Kind                         kind_;
        std::unique_ptr<std::string> error_msg_;  // Small mem footprint.

      private:
        static constexpr const char *fmt =
            "[F " ZION_COLOR_CYAN "{:<8}" ZION_COLOR_RESET "/L " ZION_COLOR_YELW
            "{:<4}" ZION_COLOR_RESET "] {}";

      public:
        Error( Kind kind ) : kind_( kind ) {};

        ZION_DISABLE_COPY_AND_MOVE( Error );

      public:
        static auto runtime_error() { return Error{Kind::RuntimeError};}

      public:
        /* Get the diagnose notes. */
        const std::string &get_diag_notes( ) const { return *error_msg_; };

      public:
        /* Low level API to emit a diagnose note.
         *
         * Use ZION_EMIT_ERROR_NOTE helper macro for common use cases.
         */
        void emit_diag_note( std::string msg, const char *file, int line )
        {
                bool first_msg = error_msg_.get( ) == nullptr;
                if ( first_msg ) {
                        error_msg_.reset( new std::string{ } );
                }
                bool end_with_newline =
                    msg.size( ) > 0 && msg.data( )[msg.size( ) - 1] == '\n';
                if ( first_msg )
                        error_msg_->append(
                            std::format( fmt, file, line, "|> " ) );
                else
                        error_msg_->append(
                            std::format( fmt, file, line, "~~> " ) );

                error_msg_->append( std::move( msg ) );
                if ( !end_with_newline ) error_msg_->push_back( '\n' );
        }
};

#define ZION_EMIT_DIAG_NOTE( err, ... )                                \
        ( err ).emit_diag_note( std::format( __VA_ARGS__ ), __FILE__, \
                                 __LINE__ )
