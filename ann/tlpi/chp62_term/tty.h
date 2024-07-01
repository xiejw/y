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
using CallbackFn = error_t ( * )( void *data, char *in );

error_t Run( CallbackFn fn, void *data );

}  // namespace eve::tty
