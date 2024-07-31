## Flag Parsing Libraries

C++ does not provide a built-in flag parsing libraries. This proposal aims to
address this for simple use caese. It follows the golang flag library.

### Use Cases

- User can register flags in different compilation units.
- User can register flag with default values for string, int, and bool.
- User can retreive the remaining flags.
- User can have built-in support for version and help usage.

### Proposed APIs
