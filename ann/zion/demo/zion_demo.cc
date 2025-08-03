#include <zion/zion.h>

#include <print>

namespace {
auto
emit_root_note( zion::Context *ctx )
{
        EMIT_DIAG_NOTE( ctx, "try to emit diag note: {}", 123 );
}

auto
emit_note( zion::Context *ctx )
{
        emit_root_note( ctx );
        EMIT_DIAG_NOTE( ctx, "more context: {}", 456 );
}

}  // namespace

int
main( )
{
        zion::Context ctx{ };
        u64           a = 345;
        INFO( "Logging       from zion: {}", a );
        emit_note( &ctx );
        WARN( "Diagnose note from zion:\n{}", ctx.get_diag_notes( ) );

        return 0;
}
