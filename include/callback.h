/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       callback.h
  @brief      Callback node class definition

  @author     rmcinnis@solidice.com
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef CALLBACK_H
#define CALLBACK_H

#include "arglist.h"

namespace go {

//------------------------------------------------------------------------
/*!
  @class Callback
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class Callback
{
  protected :
    bool           _inComa ;
    void          *_srcObj ;

    void           throwExceptionIfNull( void *f ) { if (f == nullptr) throw InvalidArguments() ; }

  public    :
                   Callback()
                   { _inComa = false ; _srcObj = nullptr ; _useageMeter++ ; _nActive++ ; }
                   Callback( const Callback &c )
                   { _inComa = c._inComa ; _srcObj = c._srcObj ; _useageMeter++ ; _nActive++ ; }
    virtual       ~Callback() { comatose(); _nActive-- ; }

    // operator overloads
    bool           operator<( const Callback &c ) { return (this < &c) ; }

    // core methods
    virtual int32_t    invoke() = 0 ;
    virtual int32_t    invoke( const ArgList &args ) = 0 ;
    void           comatose() { _inComa = true ; }
    void           revive() { _inComa = false ; }
    void           srcObj( void *sObj ) { _srcObj = sObj ; }

    // access methods
    void          *srcObj() const { return _srcObj ; }
    virtual void  *destObj() const { return nullptr ; }
    inline bool    inComa() const { return _inComa ; }
    static uint32_t   _nInvokes ; // invoke counter
    static int32_t    _useageMeter ;
    static int32_t    _nActive ;
    static int32_t    _version ; // library version id

    // exception objects
    class InvalidArguments
    {
      public :
         InvalidArguments() {}
    } ; // GONumeric Exception class
} ; // class Callback

//------------------------------------------------------------------------
/*!
  @class Observer<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T>
class Observer : public Callback
{
  private :
    T             *_obj ;
    int16_t      (T::*_func)( const ArgList * ) ;

  public  :
                   Observer( T *o, int16_t (T::*func)( const ArgList * ) )
                   : Callback(), _obj( o ), _func( func )
                   {
                     if (func == nullptr) throw InvalidArguments() ; 
                     throwExceptionIfNull( o ) ;
                   }
    int32_t            invoke()
                   {
                     return _inComa ? 0 : (_obj->*_func)(nullptr) ;
                   }
    int32_t            invoke( const ArgList &args )
                   {
                     return _inComa ? 0 : (_obj->*_func)(&args) ;
                   }

    // access methods
    virtual void  *destObj() const { return _obj ; }
} ; // template class Observer

//------------------------------------------------------------------------
/*!
  @class PokeObserver<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T>
class PokeObserver : public Callback
{
  private :
    T             *_obj ;
    void      (T::*_func)() ;

  public  :
                   PokeObserver( T *o, void (T::*func)() )
                   : Callback(), _obj( o ), _func( func )
                   {
                     if (func == nullptr) throw InvalidArguments() ; 
                     throwExceptionIfNull( o ) ;
                   }
    int32_t            invoke()
                   {
                     if (!_inComa) (_obj->*_func)() ;
                     return 0 ;
                   }
    int32_t            invoke( const ArgList & /* args */ )
                   {
                     if (!_inComa) (_obj->*_func)() ;
                     return 0 ;
                   }

    // access methods
    virtual void  *destObj() const { return _obj ; }
} ; // template class PokeObserver

//------------------------------------------------------------------------
#ifdef _WINDOWS
/*!
  @class WinObserver<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T>
class WinObserver : public Callback
{
  private :
    T             *_obj ;
    int32_t      (T::*_func)( HWND hwnd, UINT iMsg, WPARAM wP, LPARAM lP ) ;

  public  :
                   WinObserver( T *o, int32_t (T::*func)( HWND hwnd, UINT iMsg, WPARAM wP, LPARAM lP ))
                   : Callback(), _obj( o ), _func( func )
                   {
                     if (func == nullptr) throw InvalidArguments() ; 
                     throwExceptionIfNull( o ) ;
                   }
    int32_t            invoke()
                   {
                     return _inComa ? 0 : (_obj->*_func)(nullptr,0,0,0) ;
                   }
    int32_t            invoke( const ArgList &args )
                   {
                     return _inComa ? 0 : (*(int32_t*)args.asVoid(4) = (_obj->*_func)((HWND)args.asLong(0), args.asLong(1), args.asLong(2), args.asLong(3) )) ;
                   }

    // access methods
    virtual void  *destObj() const { return _obj ; }
} ; // template class WinObserver

//------------------------------------------------------------------------
/*!
  @class PaintObserver<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T>
class PaintObserver : public Callback
{
  private :
    T             *_obj ;
    void      (T::*_func)( HDC hdc, const RECT &dirty ) ;

  public  :
                   PaintObserver( T *o, void (T::*func)( HDC hdc, const RECT &dirty ))
                   : Callback(), _obj( o ), _func( func )
                   {
                     if (func == nullptr) throw InvalidArguments() ; 
                     throwExceptionIfNull( o ) ;
                   }
    int32_t            invoke()
                   {
                     RECT  empty ;  
                     empty.left = empty.top = empty.bottom = empty.right = 0 ;
                     if (!_inComa) (_obj->*_func)(nullptr,empty) ;
                     return 0 ;
                   }
    int32_t            invoke( const ArgList &args )
                   {
                     if (!_inComa)
                       (_obj->*_func)( *(HDC*)args.asVoid(0), *(RECT*)args.asVoid(1) ) ;
                     return 0 ;
                   }

    // access methods
    virtual void  *destObj() const { return _obj ; }
} ; // template class PaintObserver

#endif /* _WINDOWS */

//------------------------------------------------------------------------
/*!
  @class CNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
typedef void (*MsgFunc)(void *srcObj, void *destObj, const ArgList *args ) ;
class CNode : public Callback
{
  private  :
    void     *_destObj ;
    MsgFunc   _f ;

  public   :
              CNode( void *srcObj, void *destObj, MsgFunc f )
              { _srcObj = srcObj ; _destObj = destObj ; _f = f ; 
                if (f == nullptr) throw InvalidArguments() ; 
              }
              CNode( const CNode &c )
              { _srcObj = c._srcObj ; _destObj = c._destObj ; _f = c._f ; 
                if (_f == nullptr) throw InvalidArguments() ; 
              }

    // core methods
    int32_t       invoke()
              {
                 if (!_inComa) _f( _srcObj, _destObj, nullptr ) ;
                 return 0 ;
              }
    int32_t       invoke( const ArgList &args )
              {
                 if (!_inComa) _f( _srcObj, _destObj, &args ) ;
                 return 0 ;
              }

    // access methods
    void     *destObj() const { return _destObj ; }
} ; // class CNode

//------------------------------------------------------------------------
/*!
  @class CPokeNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
typedef void (*PokeFunc)() ;
class CPokeNode : public Callback
{
  private  :
    PokeFunc   _f ;

  public   :
              CPokeNode( PokeFunc f )
              { _srcObj = nullptr ; _f = f ; 
                if (_f == nullptr) throw InvalidArguments() ; 
              }
              CPokeNode( const CPokeNode &c )
              { _srcObj = c._srcObj ; _f = c._f ; 
                if (_f == nullptr) throw InvalidArguments() ; 
              }
    int32_t       invoke()
              {
                 if (!_inComa) _f() ; 
                 return 0 ;
              }
    int32_t       invoke( const ArgList & )
              {
                 if (!_inComa) _f() ; 
                 return 0 ;
              }
} ; // class CPokeNode

} // namespace go 

#endif



