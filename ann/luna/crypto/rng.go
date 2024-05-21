// Package crypo provides implementation of splittable rng 64.
package crypo

// Rng represents a family of splittable rng.
//
// Example:
//
//	r1 := NewRng64(456)
//	r2 := r1.Split()
//
//	size := 4
//	v1 := make([]uint64, size)
//	v2 := make([]uint64, size)
//	for i := 0; i < size; i++ {
//	    v1[i] = r1.U64()
//	    v2[i] = r2.U64()
//	}
//
// It also supports serialization and deserialization. The From() call will
// replace internal state to be identical to the state obtained by To(). The
// invariance is:
//
//	var a: Rng = NewXXX
//	var b: Rng = NewXXX (this might not be initialized)
//	var b.From(a.To())
//
//	if a != b {  // Here `!=`` means struct comparision. Not a valid go Op.
//	    panic("a should be identical to b")
//	}
type Rng interface {
	Split() Rng

	U64() uint64
	F32() float32

	To() [2]uint64
	From([2]uint64)
}
