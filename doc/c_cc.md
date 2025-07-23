## C and CC

### Linux Distribution and Compiler Support

C++23 Support

| Spec  | Linux     | Compiler |
| ----- | --------- | ------------- |
| C++23 | Ubuntu 24 | Gcc 14+  |
| C++23 | Debian 13 | Gcc 14+  |

### Language Linkage

#### `Extern` for Language Linkage

The `extern` is a Language linkage syntax which supports `c` and `c++` for now.
See
[Language Linkage](https://en.cppreference.com/w/cpp/language/language_linkage)
for some details about the specification. Few important things are

```c
extern "C" int f();
extern "C++" int f(); // Error: different language linkages

extern "C" int g();
int g(); // OK, has C language linkage

int h(); // has C++ language linkage by default
extern "C" int h(); // Error: different language linkages
```

#### Special Rules

Let C be a declaration that declares a function or variable with "C" language
linkage. If another declaration D declares an entity with the same name, and it
satisfies any of the following conditions, C and D declare the same entity:

- D declares a variable that belongs to the global scope.
- If C declares a variable, D also declares a variable.
- If C declares a function, D also declares a function.

Unlike regular redeclarations, C and D can have different target scopes:
```
extern "C"
{
    int x;
    int f();
    int g() { return 1; }
}

namespace A
{
    int x;                // Error: redefines "x"
    int f();              // OK, redeclares “f"
    int g() { return 1; } // Error: redefines "g"
}
```

