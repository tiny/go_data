/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostring.cpp
  @brief      GOString class implementation

  @author     rmcinnis@solidice.com
  @date       january 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include "gostring.h"
//#include <cstring>
#include <string.h>
//#include <string>
#include <stdio.h>

#ifdef LINUX
#  define  strncpy_s(a,b,c,d)            strncpy(a,c,b)
#  define  vsnprintf_s(a,b,c,d,...)      vsnprintf(a,b,d,__VA_ARGS__)
#endif

namespace go {

void GOString :: cleanup()
{
  try 
  {
    MutexScope   sc( _gate ) ;
    delete _str  ; _str  = nullptr ; _sz  = 0 ; 
#ifdef USE_WIDE_CHARS
    delete _wstr ; _wstr = nullptr ; _wsz = 0 ;
#endif
  } catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
} // GOString :: cleanup


#ifdef USE_WIDE_CHARS
void GOString :: convert() 
{ 
  try {
  if (_wsz < _sz)
  {
    delete _wstr ;
    _wsz = _sz ;
    _wstr = new WCHAR[_sz+1] ;
  }
  else if (_str == nullptr)
  {
      delete _wstr ;
      _wstr = nullptr ;
      _wsz = 0 ;
      return ;
  }
  mbstowcs( _wstr, _str, (_wsz+1)*sizeof(WCHAR) ) ;
  } catch (...)
  {
    _wstr = nullptr ; _wsz = 0 ;
  }
} // GOString :: convert

void GOString :: import( const wchar_t *wcs )
{
  cleanup() ;
  if (wcs == nullptr)
      return ;
  _wsz = (int16_t)wcslen( wcs ) ;
  _wstr = new WCHAR[ _wsz + 1 ] ;
  memcpy( _wstr, wcs, (_wsz+1)*sizeof(WCHAR) ) ;

  _sz = _wsz ;
  _str = new char[ _sz + 1 ] ;
  wcstombs( _str, _wstr, _sz+1 ) ;
} // GOString :: import

#endif

void GOString :: init() 
{ 
  _str = nullptr ; 
  _sz = 0 ; 
#ifdef USE_WIDE_CHARS
  _wstr = nullptr ; 
  _wsz = 0 ; 
#endif
  _use_hex = false ; 
} // GOString :: init

void GOString :: trigger() 
{
  if (_valueCB.nWatchers() > 0) // don't want to create args if noone is watching; string args are copied
  {
    _valueCB.invoke( ArgList() << (void*)_str << (void*)_str << this ) ; // 3rd arg MUST be GOObject*
  }
} // GOString :: trigger

GOString :: GOString( const GOString &c ) 
{ 
  if (this != &c) { init() ; *this = c ; } 
} // GOString [constructor]

GOString :: GOString( int32_t n, bool use_hex ) 
{ 
  init() ; 
  _use_hex = use_hex ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( int16_t n, bool use_hex ) 
{ 
  init() ; 
  _use_hex = use_hex ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( uint16_t n, bool use_hex ) 
{ 
  init() ; 
  _use_hex = use_hex ; 
  *this = n ; 
} // GOString [constructor]

// GOString :: GOString( int32_t n, bool use_hex ) 
// { 
//   init() ; 
//   _use_hex = use_hex ; 
//   *this = n ; 
// } // GOString [constructor]

GOString :: GOString( uint32_t n, bool use_hex ) 
{ 
  init() ; 
  _use_hex = use_hex ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( int64_t n, bool use_hex ) 
{ 
  init() ; 
  _use_hex = use_hex ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( double n ) 
{ 
  init() ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( float n ) 
{ 
  init() ; 
  *this = n ; 
} // GOString [constructor]

GOString :: GOString( bool n ) 
{ 
  init() ; 
  *this = (n ? "T" : "F") ; 
} // GOString [constructor]

GOString :: GOString( const char *s ) 
{ 
  init() ; 
  *this = s ; 
} // GOString [constructor]

#ifdef USE_WIDE_CHARS

GOString :: GOString( const wchar_t *wcs ) 
{ 
  init() ; 
  import( wcs ) ; 
} // GOString [constructor]

#endif

GOString :: ~GOString() 
{ 
  cleanup() ; 
} // GOString [destructor]

// -1,-1 == chop 1 char from end; 0,-1 == chop entire; n,-1 == chop from n to end; -1,n == chop up to n, shifting left
void GOString :: chop( int32_t p1, int32_t p2 ) 
{
  if (_str[0] == '\0') // empty, nothing to chop
    return ;
  if ((p1 < -1) || (p2 < -1) || (p1 > _sz) || (p2 > _sz)) // invalid range
    return ; 

  MutexScope   sc( _gate ) ;
  if ((p1 == -1) && (p2 == -1)) // chop 1 char from end
  {
    _str[ (--_sz) ] = '\0' ;
  }
  else if (p2 == -1) // chop from start to end
  {
    _str[p1] = '\0' ;
    _sz = p1 ;
  }
  else if (p1 == -1) // chop up to end, shifting left
  {
    _sz -= p2 ;
    memcpy( _str, &_str[p2], (_sz + 1) ) ; // +1 to catch the null
  }
  else // chop between p1 and p2, squeezing string; hello chop(1,4) -> ho; hello chop(0,3) -> lo
  {
    memcpy( &_str[p1], &_str[p2], (_sz - p2 + 1) ) ; // +1 to catch the null
    _sz -= (p2 - p1) ;
  }

#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  // alert observers to change
  trigger() ;
} // GOString :: chop

// contents @ pos will be shifted right; pos == 0, prepend; pos == -1, append
// hello insert(3, 's') -> helslo
//
void GOString :: insert( int32_t pos, char c )
{
  if ((pos < -1) || (pos > _sz) || (c == '\0'))
    return ;

  MutexScope   sc( _gate ) ;
  char *buf = new char[ _sz + 2 ] ;

  if (pos == 0) // prepend
  {
    buf[0] = c ;
    memcpy( &buf[1], _str, _sz + 1 ) ; // +1 to catch the null
  }
  else if (pos == -1) // append
  {
    memcpy( buf, _str, _sz ) ;
    buf[_sz] = c ;
    buf[_sz+1] = '\0' ;
  }
  else
  {
    memcpy( buf, _str, pos ) ;
    buf[pos] = c ;
    memcpy( &buf[pos+1], &_str[pos], (_sz - pos + 1) ) ; // +1 to catch the null
  }
  delete _str ;
  _str = buf ;
  _sz++ ;

#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  // alert observers to change
  trigger() ;
} // GOString :: insert

// contents @ pos will be shifted right; pos == 0, prepend; pos == -1, append
void GOString :: insert( int32_t pos, const char *s ) 
{
  if ((pos < -1) || (pos > _sz) || (*s == '\0') || (_str == s))
    return ;

  MutexScope   sc( _gate ) ;
  int32_t  n = strlen(s) ;
  char *buf = new char[ _sz + n + 1 ] ;

  if (pos == 0) // prepend
  {
    memcpy( buf, s, n ) ;
    memcpy( &buf[n], _str, _sz + 1 ) ; // +1 to catch the null
  }
  else if (pos == -1) // append
  {
    memcpy( buf, _str, _sz ) ;
    memcpy( &buf[_sz], s, n + 1 ) ; // +1 to catch the null
  }
  else
  { // hello insert(2,'cat') -> hecatllo
    memcpy( buf, _str, pos ) ;
    memcpy( &buf[pos], s, n ) ; 
    memcpy( &buf[pos+n], &_str[pos], (_sz - pos + 1) ) ; // +1 to catch the null
  }
  delete _str ;
  _str = buf ;
  _sz += n ; ;

#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  // alert observers to change
  trigger() ;
} // GOString :: insert

bool GOString :: interpret( const char *buf )
{ 
  if (buf == nullptr) return false ;
  *this = buf ; return true ; 
} // GOString :: interpret

bool GOString :: represent( int32_t max, char *buf )
{ 
  if (buf == nullptr) return false ;
  if (_str == nullptr) 
    buf[0] = '\0' ; 
  else
  {
    MutexScope   sc( _gate ) ;
    strncpy_s( buf, max-1, _str, max-1 ) ; 
  }
  return true ; 
} // GOString :: represent

void GOString :: resize( int32_t sz ) 
{ 
  cleanup() ; 
  MutexScope   sc( _gate ) ;
  _str = new char[ (_sz = sz) + 1 ] ; 
  _str[0] = '\0' ;
  _str[_sz] = '\0' ; // insure a nullptr at the end
} // GOString :: resize

void GOString :: shift( int32_t sz )
{ 
  if (sz > _sz)
  { _sz = 0 ; _str[0] = '\0' ;
    return ;
  }
  MutexScope   sc( _gate ) ;
  char *p1 = _str, *p2 ;
  p2 = &_str[sz] ;
  while (*p2 != '\0')
  {
    *p1++ = *p2++ ;
  }
  *p1 = '\0' ;
#ifdef USE_WIDE_CHARS
  convert() ; // sync mbs
#endif
  trigger() ;
} // GOString :: shift

const char *GOString :: sprint( const char *format, ... )
{
  va_list     ap ;
  int32_t         rc = 0 ;
  va_start( ap, format ) ;
  rc = vsprint( format, ap ) ;
  va_end( ap ) ;  

//  return rc ;
  return _str ;
} // GOString :: sprint

int32_t GOString :: vsprint( const char *format, va_list ap ) 
{
  int32_t          rc = -1 ;
  MutexScope   sc( _gate ) ;
  do
  {
    if ((_str != nullptr) && (_sz > 0))
    {
      rc = vsnprintf_s( _str, _sz, _TRUNCATE, format, ap ) ;
    }
    if ((rc == -1) || (rc > _sz))
    {
      resize( (rc < 0) ? (int32_t)_sz + 256 : (int32_t)rc ) ;
      rc = -1 ; 
    }
    else
    {
      _sz = rc ;
    }
  } while (rc == -1) ;
  
#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  trigger() ;
  return rc ;
} // GOString :: vsprint

bool GOString :: operator==( const char *s ) const
{ 
  return ((s == nullptr) || (_str == nullptr)) ? (s == _str) : (_stricmp( s, _str ) == 0) ; 
} // GOString :: operator==

bool GOString :: operator==( const GOString &c ) const
{ 
  return ((c._str == nullptr) || (_str == nullptr)) ? (c._str == _str) : (_stricmp( c._str, _str ) == 0) ; 
} // GOString :: operator==

bool GOString :: operator!=( const GOString &c ) const
{ 
  return !(*this == c) ; 
} // GOString :: operator!=

bool GOString :: operator!=( const char *s ) const
{ 
  return !(*this == s) ; 
} // GOString :: operator!=

GOString &GOString :: operator= ( const GOString &c )
{
  if ((this == &c) || (c._str == _str))  return *this ;
  try {
  MutexScope   sc( _gate ) ;
  delete _str ;
  _sz = c._sz ;
  _str = (c._str == nullptr) ? nullptr : new char[_sz+1] ;
  if (_str != nullptr)
  {
      memcpy( _str, c._str, _sz ) ;
      _str[_sz] = '\0' ;
  }
#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  trigger() ;
  } catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
  return *this ;
} // GOString :: operator=

GOString &GOString :: operator= ( const char *s )
{
  if (s == _str)  return *this ;
  try {
  MutexScope   sc( _gate ) ;
  delete _str ;
  _sz = (s == nullptr) ? 0 : (int32_t)strlen(s) ;
  _str = (s == nullptr) ? nullptr : new char[_sz+1] ;
  if (_str != nullptr)
  {
      memcpy( _str, s, _sz ) ;
      _str[_sz] = '\0' ;
  }
#ifdef USE_WIDE_CHARS
  convert() ;
#endif
  trigger() ;
  } catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
  return *this ;
} // GOString :: operator=

#ifdef USE_WIDE_CHARS
GOString &GOString :: operator= ( const wchar_t *s )
{
  if (s == _wstr)  
    return *this ;

  try 
  {
    MutexScope   sc( _gate ) ;

    size_t  sz = wcslen(s) ;
    delete _str ;
    _sz = (int16_t)sz ;
    _str = new char[ _sz+1 ] ;
    wcstombs( _str, s, _sz+1 ) ;
    _str[_sz] = '\0' ;
    convert() ;
    trigger() ;
  } 
  catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
  return *this ;
} // GOString :: operator=
#endif

GOString &GOString :: operator= ( int32_t n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, (_use_hex ? "%x" : "%d"), n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString &GOString :: operator= ( int16_t n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, (_use_hex ? "%x" : "%d"), n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString &GOString :: operator= ( uint16_t n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, (_use_hex ? "%x" : "%u"), n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

// GOString &GOString :: operator= ( int32_t n ) 
// { 
//   char  buf[32] ; 
//   sprintf_s( buf, 32, (_use_hex ? "%lx" : "%ld"), n ) ; 
//   return (*this = buf) ; 
// } // GOString :: operator=

GOString &GOString :: operator= ( uint32_t n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, (_use_hex ? "%x" : "%u"), n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString &GOString :: operator= ( int64_t n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, (_use_hex ? "%I64lx" : "%I64lu"), n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString &GOString :: operator= ( float n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, "%5.2f", n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString &GOString :: operator= ( double n ) 
{ 
  char  buf[32] ; 
  sprintf_s( buf, 32, "%5.4lf", n ) ; 
  return (*this = buf) ; 
} // GOString :: operator=

GOString GOString :: operator+ ( GOString &c ) const 
{ 
  return operator+( c.c_str() ) ; 
} // GOString :: operator+

GOString GOString :: operator+ ( const GOString &c ) const 
{ 
  return operator+( c.c_str() ) ; 
} // GOString :: operator+

GOString GOString :: operator+ ( const char *str ) const
{ 
  if ((str == nullptr) || (*str == '\0'))
      return *this ;
  GOString  dest( *this ) ;
  dest += str ;
  return dest ;
} // GOString :: operator+

#ifdef USE_WIDE_CHARS
GOString GOString :: operator+ ( const wchar_t *str ) const
{ 
  if (str == nullptr)  
    return *this ;
  GOString  dest( *this ) ;
  GOString  wstr( str ) ;
  dest += wstr ;
  return dest ;
} // GOString :: operator+
#endif

GOString &GOString :: operator+= ( const GOString &c ) 
{ 
  return operator+=( c.c_str() ) ; 
} // GOString :: operator+=

#ifdef USE_WIDE_CHARS
GOString &GOString :: operator+= ( const wchar_t *c ) 
{ 
  return operator+=( GOString(c) ) ; 
} // GOString :: operator+=
#endif

GOString &GOString :: operator+= ( const char *str )
{
  if ((str == nullptr) || (*str == '\0'))
      return *this ;
  try 
  {
    MutexScope      sc( _gate ) ;
    int32_t   s = (int32_t)strlen(str) ;
    char  *ch = new char[ _sz + s + 1 ] ;
    memcpy( ch, _str, _sz ) ;
    memcpy( &ch[_sz], str, s+1 ) ; // the s+1 will grab and place the terminating null
    delete _str ;
    _str = ch ;
    _sz  = _sz + s ;
#ifdef USE_WIDE_CHARS
    convert() ;
#endif
    trigger() ;
  } 
  catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
  return *this ;
} // GOString :: operator+

GOString &GOString :: operator+= ( char ch ) 
{
  if (ch == '\0')
    return *this ;
  try 
  {
    MutexScope   sc( _gate ) ;
    char *buf = new char[_sz+2] ;
    memcpy( buf, _str, _sz ) ;
    buf[_sz] = ch ;
    buf[_sz+1] = '\0' ;
    _sz++ ;
    delete _str ;
    _str = buf ;
#ifdef USE_WIDE_CHARS
    convert() ;
#endif
    trigger() ;
  } 
  catch (...)
  {
    _str = nullptr ; _sz = 0 ;
#ifdef USE_WIDE_CHARS
    _wstr = nullptr ; _wsz = 0 ;
#endif
  }
  return *this ;
} // GOString :: operator+=

GOString &GOString :: operator<< ( const GOString &c )
{ 
  return operator+=(c) ; 
} // GOString :: operator<<

GOString &GOString :: operator<< ( const char *str )
{ 
  return operator+=(str) ; 
} // GOString :: operator<<

#ifdef USE_WIDE_CHARS
GOString &GOString :: operator<< ( const wchar_t *str )
{ 
  return operator+=(str) ; 
} // GOString :: operator<<
#endif

bool GOString :: operator< ( const GOString &c ) const 
{ 
  if (c.isEmpty()) return (!isEmpty()) ;
  if (isEmpty()) return true ;
  return (strcmp( c_str(), c.c_str() ) < 0) ;
} // GOString :: operator<

char GOString :: operator[] ( int32_t i ) const 
{ 
  return _str[i] ;
} // GOString :: operator[]

void GOString :: empty() 
{ 
  cleanup() ; 
} // GOString :: empty

bool GOString :: isEmpty() const 
{ 
  return ((_str == nullptr) || (*_str == '\0')) ; 
} // GOString :: isEmpty

bool GOString :: isNull() const 
{ 
  return (_str == nullptr) ; 
} // GOString :: isNull

char *GOString :: c_str() 
{ 
  return _str ; 
} // GOString :: c_str

const char *GOString :: c_str() const 
{ 
  return _str ; 
} // GOString :: c_str

BYTE *GOString :: b_str() const 
{ 
  return (BYTE*)_str ; 
} // GOString :: b_str

#ifdef USE_WIDE_CHARS
const wchar_t *GOString :: w_str() const 
{ 
  return _wstr ; 
} // GOString :: w_str
#endif

int32_t GOString :: size() const 
{ 
  return _sz ; 
} // GOString :: size

} // namespace go 
