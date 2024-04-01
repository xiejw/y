// vim: ft=cpp
#pragma once

#include <eve/base/error.h>

#include <variant>

namespace eve::base {

template <class T>
struct Result {
  private:
    std::variant<T, Error> mValue;

  public:
    Result( ) = delete;

    static Result ofValue( T &&v ) { return Result( std::move( v ) ); }
    static Result ofError( Error &&e ) { return Result( std::move( e ) ); }

    bool isOk( ) const noexcept { return mValue.index( ) == 0; }

    T &getValue( ) noexcept { return std::get<0>( this->mValue ); }
    T &getError( ) noexcept { return std::get<1>( this->mValue ); }

    T &&releaseValue( ) noexcept
    {
        return std::move( std::get<0>( this->mValue ) );
    }
    Error &&releaseErr( ) noexcept
    {
        return std::move( std::get<1>( this->mValue ) );
    }

  private:
    Result( std::variant<T, Error> &&v ) : mValue{ std::move( v ) } {};
};
}  // namespace eve::base
