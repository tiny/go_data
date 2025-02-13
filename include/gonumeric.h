/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gonumeric.h
  @brief      GONumeric template definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GONUMERIC_H
#define GONUMERIC_H

#include <goobject.h>
#include <stdio.h>

namespace go {

/*!
  @class GONumeric< T, FMT >::DivByZero
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T, class FMT>
class GONumeric : public GOObject
{
  protected :
    T              _x ;
    ArgList        _args ;

  public    :
                        GONumeric( T x = 0 ) : GOObject(), _x( x ) {}
                        GONumeric( const GONumeric<T,FMT> &i ) : GOObject(i), _x( i._x ) {}
                       ~GONumeric() {}

    inline bool         is_watched() const { return (_valueCB.nWatchers() > 0) ; }

    bool                interpret( const char *buf )
                        { if (buf == nullptr) return false ;
                          T  tmp ;
                          if (sscanf_s( buf, FMT().get(), &tmp ) != 0)
                          {
                            *this = tmp ;
                          }
                          return true ; 
                        }
    bool                represent( int32_t max, char *buf )
                        { if (buf == nullptr) return false ;
                          sprintf_s( buf, max, FMT().get(), _x ) ; return true ; 
                        }

    // comparison operators
    bool                operator== ( T x ) const { return (_x == x) ; }
    bool                operator== ( const GONumeric<T,FMT> &i ) const { return (_x == i._x) ; }
    bool                operator!= ( T x ) const { return (_x != x) ; }
    bool                operator!= ( const GONumeric<T,FMT> &i ) const { return (_x != i._x) ; }
    bool                operator< ( T x ) const { return (_x < x) ; }
    bool                operator< ( const GONumeric<T,FMT> &i ) const { return (_x < i._x) ; }
    bool                operator<=( T x ) const { return (_x <= x) ; }
    bool                operator<=( const GONumeric<T,FMT> &i ) const { return (_x <= i._x) ; }
    bool                operator> ( T x ) const { return (_x > x) ; }
    bool                operator> ( const GONumeric<T,FMT> &i ) const { return (_x > i._x) ; }
    bool                operator>=( T x ) const { return (_x >= x) ; }
    bool                operator>=( const GONumeric<T,FMT> &i ) const { return (_x >= i._x) ; }

    // assignment operators
    GONumeric<T,FMT>   &operator= ( T x ) 
                        {
                          if (_x == x)  return *this ;
                          if (_valueCB.nWatchers() > 0)
                          {
                            _args.init() ;
                            _args << x << _x << this ;

                            _x = x ;
                            _valueCB.invoke(_args) ;
                          }
                          else
                            _x = x ;
                          return *this ;
                        } 
    GONumeric<T,FMT>   &operator= ( const GONumeric<T,FMT> &i ) { return (*this = i._x) ; }
    GONumeric<T,FMT>   &operator+= ( T x ) 
                        {
                          if (x == 0)  return *this ;
                          if (_valueCB.nWatchers() > 0)
                          {
                            _args.init() ;
                            _args << (_x + x) << _x << this ;

                            _x += x ;
                            _valueCB.invoke(_args) ;
                          }
                          else
                            _x += x ;
                          return *this ;
                        } 
    GONumeric<T,FMT>   &operator+= ( const GONumeric<T,FMT> &i ) { return (*this += i._x) ; }
    GONumeric<T,FMT>   &operator-= ( T x ) 
                        {
                          if (x == 0)  return *this ;
                          if (_valueCB.nWatchers() > 0)
                          {
                            _args.init() ;
                            _args << (_x - x) << _x << this ;

                            _x -= x ;
                            _valueCB.invoke(_args) ;
                          }
                          else
                            _x -= x ;
                          return *this ;
                        } 
    GONumeric<T,FMT>   &operator-= ( const GONumeric<T,FMT>  &i ) { return (*this -= i._x) ; }
    GONumeric<T,FMT>   &operator*= ( T x ) 
                        {
                          if (x == 1)  return *this ;
                          if (_valueCB.nWatchers() > 0)
                          {
                            _args.init() ;
                            _args << (_x - x) << _x << this ;

                            _x *= x ;
                            _valueCB.invoke(_args) ;
                          }
                          else
                            _x *= x ;
                          return *this ;
                        } 
    GONumeric<T,FMT>   &operator*= ( const GONumeric<T,FMT> &i ) { return (*this *= i._x) ; }
    GONumeric<T,FMT>   &operator/= ( T x ) 
                        {
                          if (x == 0)  throw GONumeric::DivByZero() ;
                          if (x == 1)  return *this ;
                          if (_valueCB.nWatchers() > 0)
                          {
                            _args.init() ;
                            _args << (_x / x) << _x << this ;

                            _x /= x ;
                            _valueCB.invoke(_args) ;
                          }
                          else
                            _x /= x ;
                          return *this ;
                        } 
    GONumeric<T,FMT>   &operator/= ( const GONumeric<T,FMT> &i ) { return (*this /= i._x) ; }
    GONumeric<T,FMT>   &operator++ () { return (*this += 1) ; }
    GONumeric<T,FMT>   &operator++ ( int32_t junk ) { return (*this += 1) ; }
    GONumeric<T,FMT>   &operator-- () { return (*this -= 1) ; }
    GONumeric<T,FMT>   &operator-- ( int32_t junk ) { return (*this -= 1) ; }

    // pass thru operators
    T                   operator+ ( T x ) const { return (_x + x) ; }
    T                   operator+ ( const GONumeric<T,FMT> &i ) const { return (i._x + _x) ; }
    T                   operator- ( T x ) const { return (_x - x) ; }
    T                   operator- ( const GONumeric<T,FMT> &i ) const { return (_x - i._x) ; }
    T                   operator* ( T x ) const { return (_x * x) ; }
    T                   operator* ( const GONumeric<T,FMT> &i ) const { return (i._x * _x) ; }
    T                   operator/ ( T x ) const { if (x == 0) throw DivByZero() ; return (_x / x) ; }
    T                   operator/ ( const GONumeric<T,FMT> &i ) const { if (i._x == 0) throw DivByZero() ; return (_x / i._x) ; }

    // cast operators
                        operator T() const { return _x ; }

    // exception objects
    class DivByZero
    {
      public :
         DivByZero() {}
    } ; // GONumeric Exception class
} ; // class GONumeric

/*!
  @class GOInteger
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtInteger { const char *get() { static const char *p = "%d"; return p; } };
typedef GONumeric<int32_t   , FmtInteger>   GOInteger ;

/*!
  @class GOShort
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtShort { const char *get() { static const char *p = "%d"; return p; } };
typedef GONumeric<int16_t , FmtShort>   GOShort ;

/*!
  @class GOLong
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtLong { const char *get() { static const char *p = "%ld"; return p; } };
typedef GONumeric<int32_t  , FmtLong>   GOLong ;

/*!
  @class GOHexLong
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtHexLong { const char *get() { static const char *p = "%lX"; return p; } };
typedef GONumeric<int32_t , FmtHexLong>   GOHexLong ;

/*!
  @class GODouble
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtDouble { const char *get() { static const char *p = "%6.2lf"; return p; } };
typedef GONumeric<double, FmtDouble>   GODouble ;

/*!
  @class GOFloat
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtFloat { const char *get() { static const char *p = "%6.2f"; return p; } };
typedef GONumeric<float , FmtFloat>   GOFloat ;

/*!
  @class GOInt64
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
struct FmtInt64 { const char *get() { static const char *p = "%I64d"; return p; } };
typedef GONumeric<int64_t, FmtInt64>   GOInt64 ;

} // namespace go 

#endif
