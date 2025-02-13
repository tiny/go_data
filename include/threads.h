/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       threads.h
  @brief      Thread class definition

  @author     rmcinnis@solidice.com
  @date       december 15, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef THREADS_H
#define THREADS_H

#include "thread_core.h"
#include "msgcb.h"

namespace go {

/*!
  @class Thread
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class Thread
{
  private   :
    HTHREAD        _handle ;
    bool           _isActive ;
    MsgCB          _responseCB ;
    MsgCB          _doneCB ;
    HANDLE         _threadId ;
    bool           _debugExclude ;
    ArgList        _args ; 

  protected :
    static THREADRETURN WINAPI_STDCALL threadProc( void *data ) ;
    virtual void   action() ;
    bool           isValid();
    void           setInvalid();

  public    :
                   Thread( Callback *response = (Callback *)nullptr, bool debugExclude = false ) ;
    virtual       ~Thread() ;
    virtual int32_t    create() ;
    virtual int32_t    destroy() ;
    virtual void   start() ;
    virtual void   stop() ;
    virtual int32_t    suspend() ;
    virtual int32_t    resume() ;
    virtual int32_t    join() ; // will halt calling thread until this thread is done
    #ifdef _WINDOWS
    virtual int32_t    priority( THREADPRIORITY priority ) ;
    virtual int32_t    priority_idle() ;
    virtual int32_t    priority_low() ;
    virtual int32_t    priority_medium() ;
    virtual int32_t    priority_high() ;
    virtual int32_t    priority_realtime() ;
    #endif

    // access methods
    HTHREAD        handle() { return _handle ; }
    int32_t            isCurrent() ;
    int32_t            isActive() ;
    MsgCB         &responseCB() { return _responseCB ; }
    MsgCB         &doneCB() { return _doneCB ; }
    HANDLE         threadId() const { return _threadId ; }
    ArgList       &args() { return _args ; }
    static int32_t    nUsed ;
    static int32_t    nActive ;
} ; // class Thread

} // namespace go 

#endif
