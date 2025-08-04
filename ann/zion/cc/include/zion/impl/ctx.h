// vim: ft=cpp
//
// A non-thread-safe Context to record diagnose notes when Error occurs. The
// call stacks can record more context, i.e., more notes, to the same Context.
//
#pragma once

#include <memory>

#include "lang.h"
#include "log.h"

namespace zion {
enum class Error {
        /* Result codes */
        OK,
        EndOfFile,

        /* Error codes */
        IOError,
        LengthError,
        RuntimeError,
};

/* A Context records diagnose notes when error occurs. This is not thread safe
 * in general. For each new thread, call fork() to create a new cheap Context.
 */
class Context {
      private:
        Context    *parent_{ };  // Unowned.
        std::string error_msg_;

      private:
        static constexpr const char *fmt =
            "[F " ZION_COLOR_CYAN "{:<8}" ZION_COLOR_RESET "/L " ZION_COLOR_YELW
            "{:<4}" ZION_COLOR_RESET "] {}";

      public:
        Context( )          = default;
        virtual ~Context( ) = default;

        ZION_DISABLE_COPY_AND_MOVE( Context );

      public:
        /* Must be called when the ctx has no error_msg_ set. */
        auto fork( ) -> std::unique_ptr<Context>
        {
                if ( !error_msg_.empty( ) ) {
                        PANIC(
                            "Internal error: Context::fork cannot be called "
                            "when diagnose note is not empty. got: {}",
                            get_diag_notes( ) );
                }

                auto child     = std::make_unique<Context>( );
                child->parent_ = this;
                return child;
        }

        /* Get the diagnose notes. */
        const std::string &get_diag_notes( ) const { return error_msg_; };

      public:
        /* Low level API to emit a diagnose note.
         *
         * Use ZION_EMIT_ERROR_NOTE helper macro for common use cases.
         */
        void emit_diag_note( std::string msg, const char *file, int line )
        {
                bool first_msg = error_msg_.empty( );
                bool end_with_newline =
                    msg.size( ) > 0 && msg.data( )[msg.size( ) - 1] == '\n';
                if ( first_msg )
                        error_msg_.append(
                            std::format( fmt, file, line, "|> " ) );
                else
                        error_msg_.append(
                            std::format( fmt, file, line, "~~> " ) );

                error_msg_.append( std::move( msg ) );
                if ( !end_with_newline ) error_msg_.push_back( '\n' );
        }
};

#define ZION_EMIT_DIAG_NOTE( ctx, ... )                                \
        ( ctx )->emit_diag_note( std::format( __VA_ARGS__ ), __FILE__, \
                                 __LINE__ )
}  // namespace zion
