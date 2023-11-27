// Package crypo provides implementation of splittable rng 64.
package crypo

// Rng represents a family of splittable rng.
//
// It also supports serialization and deserialization. The From() call will
// replace internal state to be identical to the state obtained by To(). The
// invariance is:
//
//	var a: Rng = NewXXX
//	var b: Rng = NewXXX (this might not be initialized)
//	var b.From(a.To())
//
//	if a != b {
//	    panic("a should be identical to b")
//	}
type Rng interface {
	Split() Rng

	U64() uint64
	F32() float32

	To() [2]uint64
	From([2]uint64)
}
