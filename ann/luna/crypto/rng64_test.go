package crypo

import (
	"math/rand/v2"
	"testing"
)

func TestU64(t *testing.T) {
	r := NewRng64(456)

	size := 4
	values := make([]uint64, size)
	for i := 0; i < size; i++ {
		values[i] = r.U64()
	}

	expected := []uint64{
		1843008821991917904,
		15828912964246028322,
		5426006831109020538,
		1221103108877787995,
	}
	assertArrayEqual(t, expected, values)
}

func BenchmarkRngU64(b *testing.B) {
	r := NewRng64(456)
	b.ResetTimer()

	for i := 0; i < b.N; i++ {
		r.U64()
	}
}

func BenchmarkRandV2U64(b *testing.B) {
	for i := 0; i < b.N; i++ {
		rand.Uint64()
	}
}

func BenchmarkRandV2Pcg(b *testing.B) {
	r := rand.NewPCG(1, 2)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		r.Uint64()
	}
}

func BenchmarkRandV2ChaCha8(b *testing.B) {
	r := rand.NewChaCha8(chacha8seed)
	b.ResetTimer()
	for i := 0; i < b.N; i++ {
		r.Uint64()
	}
}

var chacha8seed = [32]byte([]byte("ABCDEFGHIJKLMNOPQRSTUVWXYZ123456"))

func TestU64Split(t *testing.T) {
	r1 := NewRng64(456)
	r2 := r1.Split()

	size := 4
	v1 := make([]uint64, size)
	v2 := make([]uint64, size)
	for i := 0; i < size; i++ {
		v1[i] = r1.U64()
		v2[i] = r2.U64()
	}

	e1 := []uint64{
		15828912964246028322,
		5426006831109020538,
		1221103108877787995,
		4684767104136985954,
	}
	e2 := []uint64{
		13171695844766518471,
		16372514907123123114,
		5062863193646888206,
		5556012337442857035,
	}
	assertArrayEqual(t, e1, v1)
	assertArrayEqual(t, e2, v2)
}

func TestFromAndTo(t *testing.T) {
	// make a chain of calls to mutate the state of a
	r := NewRng64(456).Split().Split()
	r.U64()
	r.U64()
	r.U64()
	a := r.Split().Split().(*Rng64)

	b := new(Rng64) // uninit
	var states [2]uint64 = a.To()
	b.From(states)

	// check internal states
	s1 := [3]uint64{a.seed, a.nextGammaSeed, a.gamma}
	s2 := [3]uint64{b.seed, b.nextGammaSeed, b.gamma}
	assertArrayEqual(t, s1[:], s2[:])

	// check values as well
	size := 8
	v1 := make([]uint64, size)
	v2 := make([]uint64, size)
	for i := 0; i < size; i++ {
		v1[i] = a.U64()
		v2[i] = b.U64()
	}
	assertArrayEqual(t, v1, v2)
}

// -----------------------------------------------------------------------------
// helper
// -----------------------------------------------------------------------------
func assertArrayEqual(t *testing.T, expected, got []uint64) {
	t.Helper()
	size := len(got)
	if len(expected) != size {
		t.Fatalf("size mismatch. expected %v got %v", len(expected), size)
	}

	for i := 0; i < size; i++ {
		if expected[i] != got[i] {
			t.Errorf("ele mismatch (index %v). expected %v got %v", i,
				expected[i], got[i])
		}
	}
}
