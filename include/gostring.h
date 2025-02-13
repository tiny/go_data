/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostring.h
  @brief      GOString class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSTRING_H
#define GOSTRING_H

#include <goobject.h>
#ifdef _WIN32
#  define  vsnprintf  _vsnprintf
#  define  _stricmp   stricmp
#endif
#include <enum.h>
#ifdef USE_WIDE_CHARS
#  include <wchar.h>
#  ifndef WCHAR
#    define WCHAR  wchar_t
#  endif
#endif
#if defined(UNIX) | defined(LINUX)
#  include <stdarg.h>
#  include <stricmp.h>
#  define  _stricmp   stricmp
#endif

namespace go {
    
/*!
  @class GOString
   
  <b>Description:</b>
   
  <b>Notes:</b>
  needs to incorporate a growing buffer capability
  needs to be optimized
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOString : public GOObject
{
  private :
    MutexGate           _gate ;
    char               *_str ;
    int32_t                _sz ;
#ifdef USE_WIDE_CHARS
    wchar_t            *_wstr ;
    int16_t               _wsz ;
#endif
    bool                _use_hex ;

    void                cleanup() ;
#ifdef USE_WIDE_CHARS
    void                convert() ;
    void                import( const wchar_t *wcs ) ;
#endif
    void                init() ;
  public  :
    void                trigger() ;

                        GOString( const GOString &c ) ;
                        GOString( int32_t n, bool use_hex = false ) ;
                        GOString( int16_t n, bool use_hex = false ) ;
                        GOString( uint16_t n, bool use_hex = false ) ;
                        GOString( uint32_t n, bool use_hex = false ) ;
                        GOString( int64_t n, bool use_hex = false ) ;
                        GOString( double n ) ;
                        GOString( float n ) ;
                        GOString( bool n ) ;
                        GOString( const char *s = nullptr ) ;
#ifdef USE_WIDE_CHARS
                        GOString( const wchar_t *wcs ) ;
#endif
    virtual            ~GOString() ;

    void                chop( int32_t p1 = -1, int32_t p2 = -1 ) ; // -1,-1 == chop 1 char from end; 0,-1 == chop entire; n,-1 == chop from n to end; -1,n == chop up to n, shifting left
    void                insert( int32_t pos, char c ) ; // contents @ pos will be shifted right; pos == 0, prepend; pos == -1, append
    void                insert( int32_t pos, const char *s ) ; // contents @ pos will be shifted right; pos == 0, prepend; pos == -1, append
    bool                interpret( const char *buf ) ;
    bool                represent( int32_t max, char *buf ) ;
    void                resize( int32_t sz ) ;
    void                shift( int32_t sz ) ;
    virtual const char *sprint( const char *format, ... ) ;
    virtual int32_t         vsprint( const char *format, va_list ap ) ;
    bool                operator==( const char *s ) const ;
    bool                operator==( const GOString &c ) const ;
    bool                operator!=( const GOString &c ) const ;
    bool                operator!=( const char *s ) const ;
    GOString           &operator= ( const GOString &c ) ;
    GOString           &operator= ( const char *s ) ;
#ifdef USE_WIDE_CHARS
    GOString           &operator= ( const wchar_t *s ) ;
#endif

    GOString &operator= ( int32_t n ) ;
    GOString &operator= ( int16_t n ) ;
    GOString &operator= ( uint16_t n ) ;
//    GOString &operator= ( int32_t n ) ;
    GOString &operator= ( uint32_t n ) ;
    GOString &operator= ( int64_t n ) ;
    GOString &operator= ( float n ) ;
    GOString &operator= ( double n ) ;
    GOString  operator+ ( GOString &c ) const ;
    GOString  operator+ ( const GOString &c ) const ;
    GOString  operator+ ( const char *str ) const ;
#ifdef USE_WIDE_CHARS
    GOString  operator+ ( const wchar_t *str ) const ;
#endif
    GOString &operator+= ( const GOString &c ) ;
#ifdef USE_WIDE_CHARS
    GOString &operator+= ( const wchar_t *c ) ;
#endif
    GOString &operator+= ( const char *str ) ;
    GOString &operator+= ( char ch ) ;
    GOString &operator<< ( const GOString &c ) ;
    GOString &operator<< ( const char *str ) ;
#ifdef USE_WIDE_CHARS
    GOString &operator<< ( const wchar_t *str ) ;
#endif

    bool      operator< ( const GOString &c ) const ;
    char      operator[] ( int32_t i ) const ;

#ifdef UNICODE
    operator const LPCWSTR() const { return (LPCWSTR)_str ; }
#endif

    operator const LPSTR() const { return _str ; }
    operator const char *() const { return _str ; }
#ifdef USE_WIDE_CHARS
    operator const wchar_t *() const ;
#endif

    void           empty() ;
    bool           isEmpty() const ;
    bool           isNull() const ;

    char          *c_str() ;
    const char    *c_str() const ;
    BYTE          *b_str() const ;
#ifdef USE_WIDE_CHARS
    const wchar_t *w_str() const ;
#endif
    int32_t           size() const ;
    MutexGate& gate() { return _gate; }
} ; // class GOString

#include <vector>
using namespace std ;

typedef vector< GOString >            GOStringVector ;
typedef vector< GOString >::iterator  GOStringVector_iter ;

} // namespace go 

#endif
