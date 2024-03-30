// vim: ft=cpp
#pragma once

#include <eve/base/error.h>

#include <variant>

namespace eve::base {

template <class T>
struct ErrorOr {
  private:
    std::variant<T, Error> mValue;

  public:
    ErrorOr( ) = delete;

    static ErrorOr withValue( T &&v ) { return ErrorOr( std::move( v ) ); }
    static ErrorOr withError( Error &&e ) { return ErrorOr( std::move( e ) ); }

    bool isOk( ) const noexcept { return mValue.index( ) == 0; }

    T &Value( ) noexcept { return std::get<0>( this->mValue ); }
    T &Err( ) noexcept { return std::get<1>( this->mValue ); }

    T &&ReleaseValue( ) noexcept
    {
        return std::move( std::get<0>( this->mValue ) );
    }
    Error &&ReleaseErr( ) noexcept
    {
        return std::move( std::get<1>( this->mValue ) );
    }

  private:
    ErrorOr( std::variant<T, Error> &&v ) : mValue{ std::move( v ) } {};
};
}  // namespace eve::base
