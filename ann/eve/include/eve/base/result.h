// vim: ft=cpp
//
// Result provides facilities to return either value or Error.
//
// - Use ofValue() to create a Result with value. Then isOk() returns true, and
//   getValue() returns ref.
// - Use ofError() to create a Result with error. Then isOk() returns false, and
//   getError() returns ref.
// - Use releaseValue() or releaseError() to get move values.
//
#pragma once

#include <eve/base/error.h>

#include <variant>

namespace eve::base {

template <class T>
    requires( not std::is_same_v<T, eve::Error> )
struct [[nodiscard]] Result {
  private:
    std::variant<T, Error> mValue;

  public:
    Result( ) = delete;

    static Result ofValue( T &&v ) { return Result( std::move( v ) ); }
    static Result ofError( Error &&e ) { return Result( std::move( e ) ); }

    bool isOk( ) const noexcept { return mValue.index( ) == 0; }
    bool isError( ) const noexcept { return !isOk( ); }

    T     &getValue( ) noexcept { return std::get<0>( mValue ); }
    Error &getError( ) noexcept { return std::get<1>( mValue ); }

    T &&releaseValue( ) noexcept { return std::move( std::get<0>( mValue ) ); }
    Error &&releaseError( ) noexcept
    {
        return std::move( std::get<1>( mValue ) );
    }

  private:
    Result( std::variant<T, Error> &&v ) : mValue{ std::move( v ) } {};
};
}  // namespace eve::base

namespace eve {

template <class T>
using Result = eve::base::Result<T>;

}  // namespace eve
