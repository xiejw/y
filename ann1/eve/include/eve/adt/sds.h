// vim: ft=cpp
//
// Sds provides the string like holder which allows catPrintf() in-place.
#pragma once

#include <cstdarg>  // va_list
#include <memory>
#include <ostream>
#include <string_view>

#include <eve/base/base.h>

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
        Sds( std::size_t cap ) { Init( nullptr, 0, cap ); };

        EVE_DISABLE_COPY_CONSTRUCTOR( Sds );
        EVE_DEFAULT_MOVE_CONSTRUCTOR( Sds );

        char       *getData( ) const { return (char *)( m_ptr.get( ) ); };
        std::size_t getCap( ) const { return m_cap; };
        std::size_t getLen( ) const { return m_len; };
        std::size_t getSize( ) const { return m_len; };

        void reserve( std::size_t new_len );
        void cat( std::string_view s );
        void catPrintf( const char *fmt, ... );

        bool                 operator==( std::string_view s ) const;
        friend std::ostream &operator<<( std::ostream &os, const Sds &dt )
        {
                return os << dt.getData( );
        }

      private:
        // Create a raw sds with correct cap.
        //
        // - the space is at least as large as `cap`. m_cap will be set.
        // - allocate `cap` size but only initialize `len` size.
        // - the final `\0` is set; it is the memory space after m_cap.
        void Init( const void *init, std::size_t len, std::size_t cap );

        void catVprintf( const char *fmt, va_list ap );
};

namespace literals {
Sds operator""_sds( const char *s, std::size_t );
}

}  // namespace eve::adt

namespace eve {

using Sds = eve::adt::Sds;

}  // namespace eve
