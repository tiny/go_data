/*!
  @file       cb_lambda.h
  @brief      LambdaPokeCB<T> template definition

  @author     rmcinnis@solidice.com
  @date       september 17, 2020
  @par        copyright (c) 2020 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include "callback.h"
#include <functional>

namespace go {

/*!
  @class LambdaPokeCB<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com
  @date       october 21, 2015
  @par        copyright (c) 2015 Solid ICE Technologies, Inc.  All rights reserved.
*/
class LambdaPokeCB : public Callback
{
  private :
    std::function<void()>    _func ;

  public  :
                   LambdaPokeCB( std::function<void()> func ) 
                   : Callback(), _func( func )
                   {
                     if (func == NULL) throw InvalidArguments() ; 
//                     throwExceptionIfNull( NULL ) ;
                   }
    int            invoke()
                   {
                     if (!_inComa)
                       _func() ;
                     return 0 ;
                   }
    int            invoke( const ArgList &args )
                   {
                     if (!_inComa)
                       _func() ;
                     return 0 ;
                   }

    // access methods
    virtual void  *destObj() const { return nullptr ; }
} ; // class LambdaPokeCB

#define poke(a)      (new go::LambdaPokeCB(a))

} // namespace go