// vim: ft=cpp
#pragma once

// errFatalAndExit( fmt, ... )
// err.emitNote()
// err.isOk()
// err.getMsg()
// err.dump()
// err.dumpAndExit()

#include <eve/adt/sds.h>

#include <iostream>
#include <list>

#define errFatalAndExit( fmt, ... )                  \
    eve::base::errFatalAndExit_( __FILE__, __LINE__, \
                                 fmt __VA_OPT__(, ) __VA_ARGS__ )

namespace eve::base {

struct Error {
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
            sds.CatPrintf( fmt, std::forward<Args>( args )... );
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

    Error( Error const & )            = delete;
    Error &operator=( Error const & ) = delete;
    Error( Error && );
    Error &operator=( Error && );

    template <typename... Args>
    Error( const char *fmt, Args &&...args )
    {
        mHolder = new ErrorHolder{ };
        mHolder->emitNote( fmt, std::forward<Args>( args )... );
    }

  public:
    bool          isOk( ) const noexcept { return mHolder == nullptr; }
    eve::adt::Sds getMsg( ) const noexcept { return mHolder->getMsg( ); };
    ErrorKind     getKind( ) const noexcept { return mHolder->getKind( ); };

    template <typename... Args>
    Error &emitNote( const char *fmt, Args &&...args )
    {
        mHolder->emitNote( fmt, std::forward<Args>( args )... );
        return *this;
    }

    void dump( ) const noexcept { std::cerr << mHolder->getMsg( ); };
    void dumpAndExit( ) const noexcept
    {
        std::cerr << "fatal error:\n" << mHolder->getMsg( );
        std::exit( 1 );
    };
};

void errFatalAndExit_( const char *file, int line_no, const char *fmt, ... );
}  // namespace eve::base

namespace eve {
template <typename... Args>
eve::base::Error
Error( const char *fmt, Args &&...args )
{
    return eve::base::Error{ fmt, std::forward<Args>( args )... };
}
}  // namespace eve
