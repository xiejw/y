// vim: ft=c
#pragma once

#include <base.h>

/*
 * -----------------------------------------------------------------------------
 * The tty_run sets terminal state for the cbreak mode (non-canonical) and
 * calls the callback fn for each key event.
 *
 * Upon return, it will always restore the terminal state. tty_run NEVER
 * returns OK.
 *
 * Protocol for the callback fn:
 *
 *   - Return OK, tty_run will wait and continue handling the next key event.
 *   - Return non-OK, tty_run will return immediately. Conventionally, ERR_EOF
 *   is the special code to indicate the callback fn considers the processing is
 *   over rather than an error.
 */
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
        const char *str;  // Lifetime: Valid in callback fn invocation period.
} tty_key_info_s;

typedef rc_t ( *tty_callback_fnt )( void *udp, const tty_key_info_s * );
rc_t tty_run( void *udp, tty_callback_fnt fn );
