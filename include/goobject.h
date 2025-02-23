/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goobject.h
  @brief      GOObject class definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOOBJECT_H
#define GOOBJECT_H

#include <msgcb.h>

namespace go {

/*!
  @class GOObject
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOObject 
{
  protected :
    MsgCB          _valueCB ;

  public   :
                   GOObject() ; // default constructor
                   GOObject( const GOObject &i ) ;
    virtual       ~GOObject() ;

    virtual bool   interpret( const char * /* buf */ ) { return false ; }
    inline bool    is_watched() const { return (_valueCB.nWatchers() > 0) ; }
    virtual bool   represent( int32_t /* max */, char * /* buf */ ) { return false ; }
    virtual Callback *operator>> ( Callback *cb ) { return _valueCB.install( cb ) ; }

    // access methods
    MsgCB         &valueCB() { return _valueCB ; }
} ; // class GOObject

} // namespace go 

#endif
