// vim: ft=cpp
#pragma once

#include "base.h"

namespace eve::tty {

// -----------------------------------------------------------------------------
// The Run sets terminal state correct for the cbreak mode (non-canonical)
// and calls the CallbackFn for each key event..
//
// Upon return, it will always restore the terminal state.
//
// Protocol for the CallbackFn:
//
//   - Return OK, Run will wait and continue handling the next key event.
//   - Return non-OK, Run will return immediately. Conventionally, EEOF is the
//     special code to indicate the CallbackFn considers the processing is over
//     rather than an error.
//
enum class KeyKind {
    Na,
    ArrowLeft,
    ArrowRight,
    Enter,
    Esc,
    Other,
};

struct KeyInfo {
    KeyKind     Kind;
    const char *Str;
};

using CallbackFn = auto ( * )( void *data, const KeyInfo *Info ) -> error_t;

auto Run( CallbackFn fn, void *data ) -> error_t;

}  // namespace eve::tty
