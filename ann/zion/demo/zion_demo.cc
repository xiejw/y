#include <zion/zion.h>

#include <exception>
#include <expected>
#include <print>

namespace {
std::expected<void, zion::Error>
emit_root_note( )
{
        auto err = zion::Error::runtime_error( );
        ZION_EMIT_DIAG_NOTE( err, "try to emit diag note: {}", 123 );
        return std::unexpected{ std::move( err ) };
}

zion::Expected<void>
emit_note( )
{
        auto rc = emit_root_note( );
        ZION_EMIT_DIAG_NOTE( rc.error( ), "more context:     {}", 456 );
        ZION_EMIT_DIAG_NOTE( rc.error( ), "one more context: {}", 789 );
        ZION_CHECK_OK_OR_RETURN( rc, "should return" );
        return zion::Expected<void>{ };
}

zion::Expected<void>
emit_note_again( )
{
        auto rc = emit_note( );
        ZION_RETURN_ERR( rc, "final context" );
}

}  // namespace

int
main( )
{
        u64 a = 345;
        INFO( "Logging       from zion: {}", a );
        auto rc = emit_note_again( );
        WARN( "Diagnose note from zion:\n{}", rc.error( ).get_diag_notes( ) );
        //        PANIC( "panic point" );

        return 0;
}
