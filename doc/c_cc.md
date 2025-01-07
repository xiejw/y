## C and CC

### Language Linkage

Mixing c and cc is fun. See [Language Linkage](https://en.cppreference.com/w/cpp/language/language_linkage) for some
details about the specification. Few important things are

```c
extern "C" int f();
extern "C++" int f(); // Error: different language linkages

extern "C" int g();
int g(); // OK, has C language linkage

int h(); // has C++ language linkage by default
extern "C" int h(); // Error: different language linkages
```
