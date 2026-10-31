# C++ Coding Rules Spec

This document defines mandatory rules for all C++ code in this project.

## A) Language Standard

- The compiler **must** be invoked with `-std=c++17`.
- No other standard version (older or newer) is permitted in build flags.

## B) Fixed-Width Type Aliases

- Code **must not** use built-in/standard integer types directly, including
  (but not limited to): `int`, `unsigned int`, `short`, `long`, `long long`,
  `size_t`, `ssize_t`, `int8_t`, `uint8_t`, `int32_t`, `uint32_t`, `int64_t`,
  `uint64_t`.
- Instead, use the following project-defined type aliases:

  | Alias   | Meaning                          |
  |---------|-----------------------------------|
  | `f32`   | 32-bit floating point             |
  | `f64`   | 64-bit floating point             |
  | `i8`    | 8-bit signed integer              |
  | `u8`    | 8-bit unsigned integer            |
  | `i32`   | 32-bit signed integer             |
  | `u32`   | 32-bit unsigned integer           |
  | `i64`   | 64-bit signed integer             |
  | `u64`   | 64-bit unsigned integer           |
  | `isize` | signed pointer-sized integer      |
  | `usize` | unsigned pointer-sized integer    |

- These aliases must be defined once in a common header and included wherever
  needed.
- `char` is permitted only for raw byte/string literal handling (e.g.
  `const char*` string literals), not as a numeric type.

## C) No Reference Parameters

- No function signature (free function, member function, lambda, etc.) may
  take a parameter by lvalue reference (`T&`) or rvalue reference (`T&&`).
- This applies to all parameter types, including fundamental types, pointers,
  and class/struct types.
- Pass by value or by pointer (`T*` / `const T*`) instead.

## D) No Pass-by-Value for Class Objects

- No function signature may take a `class` (or `struct`) object as a
  by-value parameter.
- Class/struct objects **must** be passed as pointers (`T*` or `const T*`),
  unless the type appears on the **allowed-by-value list** below.
- This rule does not apply to fundamental/alias types from section (B)
  (e.g. `i32`, `f64`, `u8`), raw pointers, or enums — only to class/struct
  types.

### Allowed-by-value list

Types explicitly exempt from rule (D) and permitted to be passed by value:

- *(none yet — add exceptions here as `TypeName — justification`)*

Any addition to this list requires explicit approval and a documented
justification (e.g. the type is a trivial, cheap-to-copy value type intended
to be used like a primitive, such as a small POD struct or a `std::string_view`-like
wrapper).

## Enforcement Notes

- Rules (C) and (D) apply to all function signatures: free functions, class
  methods, static methods, lambdas, and function pointers/`std::function`
  signatures.
- Return types are not restricted by rules (C) or (D) unless stated
  otherwise — these rules govern **parameters** only.
- Violations should be caught via code review and, where feasible, static
  analysis (e.g. a `clang-tidy` custom check or lint script).
