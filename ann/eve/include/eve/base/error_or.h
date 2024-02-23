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
    explicit ErrorOr( T &&v ) : m_v( std::move( v ) ) {}
    explicit ErrorOr( Error &&e ) : m_v( std::move( e ) ) {}

    bool Ok( ) const noexcept { return m_v.index( ) == 0; }

    T &Value( ) noexcept { return std::get<0>( this->m_v ); }
    T &Err( ) noexcept { return std::get<1>( this->m_v ); }

    T &&ReleaseValue( ) noexcept
    {
        return std::move( std::get<0>( this->m_v ) );
    }
    Error &&ReleaseErr( ) noexcept
    {
        return std::move( std::get<1>( this->m_v ) );
    }
};
}  // namespace eve::base
