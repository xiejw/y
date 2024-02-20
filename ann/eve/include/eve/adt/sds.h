// vim: ft=cpp
#pragma once

#include <memory>
#include <string_view>

namespace eve::adt {

struct Sds {
  private:
    std::size_t                                    m_len;
    std::size_t                                    m_cap;
    std::unique_ptr<void, decltype( std::free ) *> m_ptr = { nullptr,
                                                             std::free };

  public:
    Sds( std::string_view v ) { Init( v.data( ), v.size( ), v.size( ) ); };
    Sds( ) { Init( nullptr, 0, 0 ); };
    Sds( int cap ) { Init( nullptr, 0, cap ); };

    Sds( Sds const & )            = delete;
    Sds &operator=( Sds const & ) = delete;
    Sds( Sds && )                 = default;
    Sds &operator=( Sds && )      = default;

    char       *data( ) const { return (char *)( m_ptr.get( ) ); };
    std::size_t cap( ) const { return m_cap; };
    std::size_t len( ) const { return m_len; };
    std::size_t size( ) const { return m_len; };

  private:
    // Create a raw sds with correct cap.
    //
    // - the space is at least as large as `cap`. m_cap will be set.
    // - allocate `cap` size but only initialize `len` size.
    // - the final `\0` is set; it is the memory space after m_cap.
    void Init( const void *init, std::size_t len, std::size_t cap );
};

}  // namespace eve::adt
