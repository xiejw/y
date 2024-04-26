## LLVM


### Projects

- compiler-rt is needed for asan https://compiler-rt.llvm.org
- clang-format is moved under clang (not confirmed yet)

### Clang on macOS

```
SDKROOT=`xcrun --show-sdk-path`
man xcrun
```

### Clang

```
# Print the header file included
clang -H
```

### Clang-format

https://clang.llvm.org/docs/ClangFormatStyleOptions.html

```
clang-format -style=Google -dump-config
```

### Clang Cross Compile

```
# check all targets (both can work)
clang --print-targets
~/Workspace/llvm-project/build/bin/llvm-objdump --version

clang --target=aarch64 -O1 -o h.o -c h.c
 ~/Workspace/llvm-project/build/bin/llvm-objdump -d h.o

clang --target=riscv64 -O1 -o h.o -c h.c
```
