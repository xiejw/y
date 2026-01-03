# My Core C Library

Configurations
```
platform: amd64/arm64 (Linux, macOS/Darwin), armv7 (Raspberry Pi OS/Linux)
compiler: clang (+ lld on Linux)
```

## Compile

```
make                    # debug mode.
make RELEASE=1          # release mode.
make test               # test
make ASAN=1 test        # asan test mode.

make NO_CLANG_FLAGS=1   # disable Weverything flag for clang
```
## Knob

During comilation, there are few knobs users can tune

```
EVA_LOG_NO_COLOR        # disable log color
EVA_VEC_CHECK_MAGIC     # insert code to check magic num for vec
```

# Projects

- Project _Cron_ ([here](doc/cron/usage.txt)): Print the number of seconds until
  next matching time.
- Project _MLVM_ ([here](doc/mlvm/)): Low-Level Machine Learning VM.

