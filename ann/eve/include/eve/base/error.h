// vim: ft=cpp
#pragma once

#include <eve/adt/sds.h>

#include <iostream>
#include <list>

#define errFatalAndExit( fmt, ... )                  \
    eve::base::errFatalAndExit_( __FILE__, __LINE__, \
                                 fmt __VA_OPT__(, ) __VA_ARGS__ )

namespace eve::base {
enum class ErrorKind {
    ERROR     = -1,
    EMALLOC   = -2,
    ENOTEXIST = -3,
    ENOTIMPL  = -4,
};

struct ErrorHolder {
  private:
    ErrorKind                m_kind;
    std::list<eve::adt::Sds> m_msg;

  public:
    ErrorHolder( eve::adt::Sds &&msg );

    template <typename... Args>
    ErrorHolder( const char *fmt, Args &&...args )
        : m_kind{ ErrorKind::ERROR }, m_msg{ }
    {
        EmitNote( fmt, std::forward<Args>( args )... );
    }

    ErrorKind     Kind( ) const { return this->m_kind; }
    eve::adt::Sds Msg( ) const noexcept;

    ErrorHolder &EmitNote( eve::adt::Sds &&msg );

    template <typename... Args>
    ErrorHolder &EmitNote( const char *fmt, Args &&...args )
    {
        eve::adt::Sds sds{ };
        sds.CatPrintf( fmt, std::forward<Args>( args )... );
        m_msg.push_back( std::move( sds ) );
        return *this;
    }
};

struct Error {
  private:
    ErrorHolder *m_holder{ nullptr };

  public:
    Error( ) = default;
    ~Error( ) { delete m_holder; }

    template <typename... Args>
    Error( const char *fmt, Args &&...args )
    {
        this->m_holder = new ErrorHolder{ fmt, std::forward<Args>( args )... };
    }

    bool          Ok( ) const noexcept { return this->m_holder == nullptr; }
    eve::adt::Sds Msg( ) const noexcept { return this->m_holder->Msg( ); };

    void Dump( ) const noexcept { std::cerr << this->m_holder->Msg( ); };
    void FatalAndExit( ) const noexcept
    {
        std::cerr << "fatal error:\n" << this->m_holder->Msg( );
        std::exit( 1 );
    };

    template <typename... Args>
    Error &EmitNote( const char *fmt, Args &&...args )
    {
        this->m_holder->EmitNote( fmt, std::forward<Args>( args )... );
        return *this;
    }
};

void errFatalAndExit_( const char *file, int line_no, const char *fmt, ... );
}  // namespace eve::base
