package errors

import (
	"bytes"
	"fmt"
)

// =============================================================================
// design
// =============================================================================

// DError is a special diagnosis error structure, which allows downstream call
// site to emit more notes after creation.
type DError struct {
	notes     []string // Reverse order. The tail is the outer/recent.
	rootCause error
}

// -----------------------------------------------------------------------------
// conform error interface
// -----------------------------------------------------------------------------

// Error returns the string representation.
func (de *DError) Error() string { return de.String() }

// -----------------------------------------------------------------------------
// conform string
// -----------------------------------------------------------------------------

// String formats the error stack into string.
func (de *DError) String() string {
	var buf bytes.Buffer

	fmt.Fprint(&buf, "error stack:\n")
	for index := len(de.notes) - 1; index >= 0; index-- {
		fmt.Fprintf(&buf, "  > %v\n", de.notes[index])
	}

	fmt.Fprintf(&buf, "  @ %v\n", de.rootCause)
	return buf.String()
}

// -----------------------------------------------------------------------------
// factory Methods
// -----------------------------------------------------------------------------

// WrapNote wraps a note to the diagnosis error.
//
// This is (almost) same as From(error).EmitNote(...).
func WrapNote(err error, sfmt string, args ...interface{}) *DError {
	if de, ok := err.(*DError); ok { // fast path
		note := fmt.Sprintf(sfmt, args...)
		de.notes = append(de.notes, note)
		return de
	}

	return From(err).EmitNote(sfmt, args...)
}

// New creates a DError with root cause specified by the message.
func New(sfmt string, args ...interface{}) *DError {
	return &DError{
		rootCause: fmt.Errorf(sfmt, args...),
	}
}

// From creates a DError with root cause specified by err.
//
// This is no-op if err is DError.
func From(err error) *DError {
	if de, ok := err.(*DError); ok {
		return de
	}

	return &DError{
		rootCause: err,
	}
}

// -----------------------------------------------------------------------------
// public apis
// -----------------------------------------------------------------------------

// EmitNote emits one more note to the DError.
func (de *DError) EmitNote(sfmt string, args ...interface{}) *DError {
	note := fmt.Sprintf(sfmt, args...)
	de.notes = append(de.notes, note)
	return de
}
