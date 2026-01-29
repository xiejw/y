### Use Case

- Imagine we are doing a board game in CLI. Rather than asking users to type the
  `x` and `y` coordinates from `stdin`, it could be a great UX to allow user to
  use left and right arrows to select a position with real-time visual feedback.
- The user can also to use `Ctrl-C` to quit, `Enter` to confirm the selection,
  and `Esc` to cancel the selection.
- After the confirmation, the terminal state will be restored.

### The Proposed API

```
// -----------------------------------------------------------------------------
// The tty_run sets terminal state correct for the cbreak mode (non-canonical)
// and calls the callback fn for each key event.
//
// Upon return, it will always restore the terminal state. tty_run never
// returns OK.
//
// Protocol for the callback fn:
//
//   - Return OK, tty_run will wait and continue handling the next key event.
//   - Return non-OK, tty_run will return immediately. Conventionally, ERR_EOF
//   is the special code to indicate the callback fn considers the processing is
//   over rather than an error.
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
    const char    *str;   // Lifetime: Valid in callback fn invocation period.
} tty_key_info_s;

typedef error_t ( *tty_callback_fnt )( void *udp, const tty_key_info_s * );
error_t tty_run( void *udp, tty_callback_fnt fn );
```

Code Example [main.c](main.c).

### Two Problems to Solve
- How to set the terminal state (no echo, read without line buffering) with
  interruption (`Ctrl-C`) still present. _This is the cbreak mode_.
- How to get the control sequence? Is that unique to xterm? Is that ANSI control
  seq? _Read the following links_.

```
https://github.com/atomicgo/keyboard/blob/main/input.go
https://notes.burke.libbey.me/ansi-escape-codes
https://www.xfree86.org/current/ctlseqs.html
```
