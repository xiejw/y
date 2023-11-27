### APIs in `sys` Package~

--------------------------------------------------------------------------------
Package [errors](./src/sys/errors)

A helper library to provide stacks for `error`.

A sample usage is
```go
err := New("root")
err.EmitNote("note 1")
err.EmitNote("note 2")
return err
```

--------------------------------------------------------------------------------
Package [crypto](./src/sys/crypto)

Splittable rng, based the paper "Fast Splittable Pseudorandom Number
Generators". It also support serialization and de-serialization.

A sample usage is
```go
r := NewRng64(456)

size := 4
values := make([]uint64, size)
for i := 0; i < size; i++ {
	values[i] = r.U64()
}
```

--------------------------------------------------------------------------------
Package [log](./src/sys/log)

Usage

```go
import "github.com/xiejw/luna/src/base/log"

log.Infof(...)
```

### Third-party APIs

- https://github.com/fatih/color
