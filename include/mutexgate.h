/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       mutexgate.h
  @brief      MutexGate class definition

  @author     rmcinnis@solidice.com
  @date       april 16, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef MUTEXGATE_H
#define MUTEXGATE_H

#include <stdint.h>
#if defined(UNIX) | defined(LINUX)
#  include <pthread.h>
#  include <stdint.h>
#else
#  include <string.h>
#  ifndef _WINSOCKAPI_
#   include "socket_interface.h"
//#  include <winsock.h>
#  endif
#endif

namespace go {

/*!
  @class MutexGate
   
  <b>Description:</b>
   
  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class MutexGate
{
  private   :
    bool                _isCreated ;

#ifdef UNIX
  pthread_mutex_t     _section ;
#else
    uint32_t               _owner;
    int16_t               _count;
    HANDLE              _handle;
    char               *_name ; // for named mutexes
#endif
  protected :
    void                create() ;
    void                destroy() ;
    void                init() ;

  public    :
                        MutexGate() ;
                        MutexGate( const MutexGate &m ) ;
                       ~MutexGate() ;

    void                enter() ;
    void                exit() ;

#ifndef UNIX
    bool                isOwner() const { return ( isLocked() && _owner == GetCurrentThreadId() ); }
    bool                isLocked() const { return ( _count != 0 ); }
    int16_t               count() const { return _count; }
    void                set_name( const char *n ) 
                        { delete _name ; _name = nullptr ; if (n == nullptr) return ;
                          size_t size = strlen(n) + 1 ;
                          _name = new char[ size ] ; strcpy_s( _name, size, n ) ;
                        }
#endif

    // static counters
    static int32_t         nActive ;
    static int32_t         nCreated ;
    static int32_t         nUsed ;
} ; // class MutexGate

} // namespace go 

#endif


