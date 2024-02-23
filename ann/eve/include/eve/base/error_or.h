// vim: ft=cpp
#pragma once

#include <eve/base/error.h>

#include <variant>

namespace eve::base {

template <class T>
struct ErrorOr {
  private:
    std::variant<T, Error> m_v;

  public:
    ErrorOr( T &&v ) : m_v( std::move( v ) ) {}
    ErrorOr( Error &&e ) : m_v( std::move( e ) ) {}
    bool Ok( ) const noexcept { return m_v.index( ) == 0; }
    bool Error( ) const noexcept { return m_v.index( ) == 1; }
};
}  // namespace eve::base
