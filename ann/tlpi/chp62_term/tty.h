// vim: ft=cpp
#pragma once

#include "base.h"

namespace eve::tty {

// -----------------------------------------------------------------------------
// Interface for the Run.
//
// The Run will set terminal state correct for the cbreak mode (non-cannonical)
// and call the CallbackFn for each key.
//
// Upon return, it will always restore the terminal state.
//
// Protocol for the
// EOF i stop.
using CallbackFn = error_t ( * )( void *data, char *in );

error_t Run( CallbackFn fn, void *data );

}  // namespace eve::tty
