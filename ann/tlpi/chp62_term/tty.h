// vim: ft=c
#pragma once

#include <base.h>

// -----------------------------------------------------------------------------
// The Run sets terminal state correct for the cbreak mode (non-canonical)
// and calls the CallbackFn for each key event.
//
// Upon return, it will always restore the terminal state. Run never returns
// OK.
//
// Protocol for the CallbackFn:
//
//   - Return OK, Run will wait and continue handling the next key event.
//   - Return non-OK, Run will return immediately. Conventionally, EEOF is the
//     special code to indicate the CallbackFn considers the processing is over
//     rather than an error.
//
typedef enum {
    Key_Na,
    Key_ArrowLeft,
    Key_ArrowRight,
    Key_Enter,
    Key_Esc,
    Key_Other,
} tty_key_kind_e;

typedef struct {
    tty_key_kind_e kind;  // The Kind.
    const char    *str;   // Lifetime: Valid in CallbackFn invocation period.
} tty_key_info_s;

typedef error_t ( *tty_callback_fnt )( void *udp, const tty_key_info_s * );
error_t tty_run( void *udp, tty_callback_fnt fn );
