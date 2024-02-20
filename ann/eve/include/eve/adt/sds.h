// vim: ft=cpp
#pragma once

#include <memory>
#include <string_view>

namespace eve::adt {

struct Sds {
  private:
    std::size_t                                    len;
    std::size_t                                    alloc;
    std::unique_ptr<void, decltype( std::free ) *> ptr = { nullptr, std::free };

  public:
    Sds( std::string_view v );
    Sds( ) { Init( nullptr, 0, 0 ); };

    char *data( ) const { return (char *)( ptr.get( ) ); };

  private:
    // create a raw sds without filling len.
    //
    // - the space is at least as large as `cap`. cap will be set.
    // - allocate `cap` size but only initialize `len` size.
    // - the final `\0` is set.
    void Init( const void *init, std::size_t len, std::size_t cap );
};

}  // namespace eve::adt
