// Package errors provides toolkit around `error`. The receiver of the `error`
// can emit a note to help to explain the context for the underlying situation.
// In addition upon creation and emiting a note, the caller stack is recorded
// as well.
//
// Usages:
//
//	-- Constructor
//	New(fmt, ...)
//	From(err)
//
//	-- Emit note
//	err.EmitNote(fmt, ...)        // in place recording
//	return err.EmitNote(fmt, ...) // returns itself.
//
// Example ()
//
//	// err could be raw 'error' or 'DError'
//	if err != nil {
//	    return From(err).EmitNote(fmt, ...)
//	}
//
//	// err is known to be 'DError'
//	if err != nil {
//	    return err.EmitNote(fmt, ...)
//	}
package errors

import (
	"bytes"
	"fmt"
	"path/filepath"
	"runtime"
)

var (
	// stub for testing
	printLongPath = true
)

// =============================================================================
// design
// =============================================================================

// DError is a special diagnosis error structure, which allows downstream call
// site to emit more notes after creation.
type DError struct {
	notes     []string // Reverse order. The tail is the outer/recent.
	rootCause error

	// len(filePos) == len(linePos) == len(notes) + 1
	filePos []string // Reverse order. The tail is out/recent.
	linePos []int    //  Reverse order. The tail is out/recent.
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
		fmt.Fprintf(&buf, "  > %v (%v:%v)\n", de.notes[index], de.filePos[index+1], de.linePos[index+1])
	}

	fmt.Fprintf(&buf, "  @ %v (%v:%v)\n", de.rootCause, de.filePos[0], de.linePos[0])
	return buf.String()
}

// -----------------------------------------------------------------------------
// factory Methods
// -----------------------------------------------------------------------------

// New creates a DError with root cause specified by the message.
func New(sfmt string, args ...interface{}) *DError {
	de := &DError{
		rootCause: fmt.Errorf(sfmt, args...),
	}
	file, line := getCaller(1)
	de.filePos = append(de.filePos, file)
	de.linePos = append(de.linePos, line)
	return de
}

// From creates a DError with root cause specified by err.
//
// This is no-op if err is DError.
func From(err error) *DError {
	if de, ok := err.(*DError); ok {
		return de
	}

	de := &DError{
		rootCause: err,
	}
	file, line := getCaller(1)
	de.filePos = append(de.filePos, file)
	de.linePos = append(de.linePos, line)
	return de
}

// -----------------------------------------------------------------------------
// public apis
// -----------------------------------------------------------------------------

// EmitNote emits one more note to the DError.
func (de *DError) EmitNote(sfmt string, args ...interface{}) *DError {
	return de.emitNote(1, sfmt, args...)
}

func (de *DError) emitNote(startLevel int, sfmt string, args ...interface{}) *DError {
	note := fmt.Sprintf(sfmt, args...)
	file, line := getCaller(startLevel + 1)
	de.notes = append(de.notes, note)
	de.filePos = append(de.filePos, file)
	de.linePos = append(de.linePos, line)
	return de
}

func getCaller(startLevel int) (string, int) {
	_, file, line, ok := runtime.Caller(startLevel + 1)
	if !ok {
		return "???", -1
	}
	if printLongPath {
		return file, line
	} else {
		return filepath.Base(file), line
	}
}
