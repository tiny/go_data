/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       cb_statetimeout.h
  @brief      StateTimeoutObserver<T> template definition

  @author     rmcinnis@solidice.com
  @date       september 12, 2004
  @par        copyright (c) 2004 Voipster, BV.  All rights reserved.
*/
#ifndef CB_STATETIMEOUT_H
#define CB_STATETIMEOUT_H

#include <callback.h>

namespace go {

/*!
  @class StateTimeoutObserver<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       september 12, 2004
  @par        copyright (c) 2004 Voipster, BV.  All rights reserved.
*/
template <class T>
class StateTimeoutObserver : public Callback
{
  private :
    int32_t           _token ;
    T             *_obj ;
    void      (T::*_func)( int32_t ) ;

  public  :
                   StateTimeoutObserver( int32_t token, T *o, void (T::*func)( int32_t ) )
                   : Callback(), _token(token), _obj( o ), _func( func )
                   {
                     if (func == nullptr) throw InvalidArguments() ; 
                     throwExceptionIfNull( o ) ;
                   }
    int32_t            invoke()
                   {
                     if (!_inComa) (_obj->*_func)(_token) ;
                     return 0 ;
                   }
    int32_t            invoke( const ArgList & /* args */ )
                   {
                     if (!_inComa)
                       (_obj->*_func)( _token ) ;
                     return 0 ;
                   }

    // access methods
    virtual void  *destObj() const { return _obj ; }
} ; // template class StateTimeoutObserver

} // namespace go 

#endif
