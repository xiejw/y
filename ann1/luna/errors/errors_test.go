package errors

import (
	"testing"
)

func TestDiagnosisErrorOutput(t *testing.T) {
	oldV := printLongPath
	printLongPath = false
	defer func() {
		printLongPath = oldV
	}()

	err := New("root")
	err.EmitNote("during stack 1")
	err.EmitNote("during stack 2")

	expected := `error stack:
  > during stack 2 (errors_test.go:16)
  > during stack 1 (errors_test.go:15)
  @ root (errors_test.go:14)
`

	got := err.String()

	if got != expected {
		t.Errorf("Error() mismatch. expected: %v, got: %v", expected, got)
	}

	if err == nil {
		t.Errorf("error should be nil.")
	}
}
