// vim: ft=cpp
#pragma once

// Error provides facilities to define, propagate and dump errors.
//
// A default constructed Error represents no error (isOk()==true). Once any
// diagnosis error is emitted, its state switches to error state
// (isOk()==false). The newError() factory method can create an Error with
// error message.
//
// - To propagate an error, return the Error and optionally emit another note
//   via emitNote().
// - To get the error message, call getMsg().
// - To dump the error message, call dump() or dumpAndPanic().
//
// Two macros provide to improve readability.
//   - panic( fmt, ... )
//   - unimplemented( fmt, ... )

#include <eve/adt/sds.h>
#include <eve/base/base.h>

#include <iostream>
#include <list>

namespace eve::base {

struct [[nodiscard]] Error {
    //
    // nested private data structure
    //
  public:
    enum ErrorKind {
        ERROR     = -1,
        EMALLOC   = -2,
        ENOTEXIST = -3,
        ENOTIMPL  = -4,
    };

    //
    // nested private data structure
    //
  private:
    // The invariant is mKind must be set and mMsg is not empty. Caller is
    // responsible for this.
    struct ErrorHolder {
      private:
        ErrorKind                mKind{ ERROR };
        std::list<eve::adt::Sds> mMsg{ };

      public:
        ErrorKind     getKind( ) const noexcept { return this->mKind; }
        eve::adt::Sds getMsg( ) const noexcept;

        ErrorHolder &emitNote( eve::adt::Sds &&msg ) noexcept;

        template <typename... Args>
        ErrorHolder &emitNote( const char *fmt, Args &&...args ) noexcept
        {
            eve::adt::Sds sds{ };
            sds.catPrintf( fmt, std::forward<Args>( args )... );
            mMsg.push_back( std::move( sds ) );
            return *this;
        }
    };

    //
    // internal data structure
    //
    // should be as cheap as one pointer.
    //
  private:
    ErrorHolder *mHolder{ nullptr };

  public:
    Error( ) = default;
    ~Error( ) { delete mHolder; }

    EVE_DISALBE_COPY_COSTRUCTOR( Error );
    EVE_DECLARE_MOVE_COSTRUCTOR( Error );

    template <typename... Args>
    Error( const char *fmt, Args &&...args )
    {
        mHolder = new ErrorHolder{ };
        mHolder->emitNote( fmt, std::forward<Args>( args )... );
    }

  private:
    using Sds = eve::adt::Sds;

  public:
    auto isOk( ) const noexcept -> bool { return mHolder == nullptr; }
    auto isError( ) const noexcept -> bool { return !isOk( ); }
    auto getKind( ) const noexcept -> ErrorKind { return mHolder->getKind( ); };
    auto getMsg( ) const noexcept -> Sds { return mHolder->getMsg( ); };
    auto unwrap( ) -> void { isError( ) ? dumpAndPanic( ) : void( 0 ); }
    auto dump( ) const noexcept { std::cerr << mHolder->getMsg( ); };

    void dumpAndPanic( ) const noexcept
    {
        std::cerr << "panic error:\n" << mHolder->getMsg( ) << std::flush;
        std::exit( 1 );
    };

    template <typename... Args>
    Error &emitNote( const char *fmt, Args &&...args )
    {
        mHolder->emitNote( fmt, std::forward<Args>( args )... );
        return *this;
    }
};

void panic_impl_( const char *msg, const char *file, int line_no,
                  const char *fmt, ... );
}  // namespace eve::base

namespace eve {

using Error = eve::base::Error;

template <typename... Args>
auto
newError( const char *fmt, Args &&...args ) -> Error
{
    return eve::base::Error{ fmt, std::forward<Args>( args )... };
}

#define panic( fmt, ... )                                \
    eve::base::panic_impl_( "panic", __FILE__, __LINE__, \
                            fmt __VA_OPT__(, ) __VA_ARGS__ )
#define unimplemented( fmt, ... )                                \
    eve::base::panic_impl_( "unimplemented", __FILE__, __LINE__, \
                            fmt __VA_OPT__(, ) __VA_ARGS__ )
}  // namespace eve
