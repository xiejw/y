#pragma once

#include "base.h"
// -----------------------------------------------------------------------------
// Interface for the TtyRun.
// EOF is stop.
typedef error_t ( *TtyCallbackFn )( void *data, char *in );
error_t TtyRun( TtyCallbackFn fn, void *data );
