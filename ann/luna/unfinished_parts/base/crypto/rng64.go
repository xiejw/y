package crypo

// -----------------------------------------------------------------------------
// public data structure
// -----------------------------------------------------------------------------

// Rng64 is based the paper "Fast Splittable Pseudorandom Number Generators".
//
// The first two fields are needed to recreate the stat of Rng64. The field
// gamma can be generated on the fly.
type Rng64 struct {
	seed          uint64 // state
	nextGammaSeed uint64 // state
	gamma         uint64 // internal state, generated on the fly
}

// -----------------------------------------------------------------------------
// internal constants
// -----------------------------------------------------------------------------
const (
	gammaPrime uint64  = (1 << 56) - 5      // Percy.
	gammaGamma uint64  = 0x00281E2DBA6606F3 // The coefficient to randomize gamma
	doubleUlp  float32 = 1.0 / (1 << 53)
)

// -----------------------------------------------------------------------------
// public constructor
// -----------------------------------------------------------------------------

func NewRng64(seed uint64) *Rng64 {
	return newRng64(seed, 0 /*gammaSeed*/)
}

// -----------------------------------------------------------------------------
// internal constructor
// -----------------------------------------------------------------------------

func newRng64(seed uint64, gammaSeed uint64) *Rng64 {
	if gammaSeed >= gammaPrime {
		panic("Internal error: gammaSeed passed to new srng is too large")
	}

	// advance the gamma seed
	gammaSeed += gammaGamma
	if gammaSeed >= gammaPrime {
		gammaSeed -= gammaPrime // Constrain the range for gamma seed.
	}

	srng := &Rng64{
		seed:          seed,
		gamma:         rng64Mix56(gammaSeed) + 13,
		nextGammaSeed: gammaSeed,
	}
	return srng

}

// -----------------------------------------------------------------------------
// conform Rng interface
// -----------------------------------------------------------------------------

func (srng *Rng64) Split() Rng {
	seed := srng.advanceSeed()
	gammaSeed := srng.nextGammaSeed
	return newRng64(seed, gammaSeed)
}

func (srng *Rng64) U64() uint64 {
	return rng64Mix64(srng.advanceSeed())
}

func (srng *Rng64) F32() float32 {
	return float32(srng.U64()>>11) * doubleUlp
}

func (rng *Rng64) To() [2]uint64 {
	return [2]uint64{rng.seed, rng.nextGammaSeed}
}

func (rng *Rng64) From(s [2]uint64) {
	rng.seed = s[0]
	rng.nextGammaSeed = s[1]
	rng.gamma = rng64Mix56(s[1]) + 13
}
