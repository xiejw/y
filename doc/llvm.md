## LLVM


### Projects

- compiler-rt is needed for asan https://compiler-rt.llvm.org
- clang-format is moved under clang (not confirmed yet)

### Best Practices for Clang Flags

```
${CXX} -std=<PLACEHOLDER> -Wall -Werror -pedantic -Wextra -Wfatal-errors -Wconversion
```

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

### Linker
```
# Find system search path.
# Run the following command line and see "Shared library search path" section.
#
/lib64/ld-linux-x86-64.so.2 --help

# Or
ld --verbose | grep SEARCH_DIR | tr -s ' ;' \\012
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
