// vim: ft=cpp
#pragma once

#include <functional>

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
    KeyKind     Kind;  // The Kind.
    const char *Str;   // Lifetime: Valid in CallbackFn invocation period.
};

using CallbackFn = std::function<auto( const KeyInfo *Info )->error_t>;
auto Run( CallbackFn fn ) -> error_t;

}  // namespace eve::tty
