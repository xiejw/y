package errors

import (
	"testing"
)

func TestDiagnosisErrorOutput(t *testing.T) {
	err := New("root")
	err.EmitNote("during stack 1")
	err.EmitNote("during stack 2")

	expected := `error stack:
  > during stack 2
  > during stack 1
  @ root
`

	got := err.String()

	if got != expected {
		t.Errorf("Error() mismatch. expected: %v, got: %v", expected, got)
	}

	if err == nil {
		t.Errorf("error should be nil.")
	}
}
