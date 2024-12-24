### Use Case

- Imagine we are doing a board game in CLI. Rather than asking users to type the
  `x` and `y` coordinates from `stdion`, it could be a great UX to allow user to
  use left and right arrows to select a position with real-time visual feedback.
- The user can also to use `Ctrl-C` to quit, `Enter` ot confirm the selection,
  and `Esc` to cancel the selection.
- After the confirmation, the terminal state will be restored.

### The Proposed API

```
// -----------------------------------------------------------------------------
// The Run sets terminal state correct for the cbreak mode (non-canonical)
// and calls the CallbackFn for each key event.
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
namespace tty {
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
}  // namespace

```

Code Example [main.cc](main.cc).

### Two Problems to Solve
- How to set the terminal state (no echo, read without line buffering) with
  interruption (`Ctrl-C`) still present.
- How to get the control sequence? Is that unique to xterm? Is that ANSI control
  seq?


```
Further question is TERM=linux make a difference for ctrl-arrow?
od -c
https://github.com/atomicgo/keyboard/blob/main/input.go
https://notes.burke.libbey.me/ansi-escape-codes
https://www.xfree86.org/current/ctlseqs.html
```
