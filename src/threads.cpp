/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       threads.cpp
  @brief      Thread class implementation

  @author     rmcinnis@solidice.com
  @date       december 15, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <threads.h>
#include <stdio.h>
#ifdef UNIX
#  include <string.h>
#  include <errno.h>
#endif

namespace go {

int32_t Thread :: nActive = 0 ;
int32_t Thread :: nUsed   = 0 ;

THREADRETURN WINAPI_STDCALL Thread :: threadProc( void *data ) 
{
  Thread     *t = (Thread*)data ;
  if (t == nullptr)
  {
#if defined(UNIX)
    return (void *)0xFFFFFFFF ;
#else
    return 0xFFFFFFFF ;
#endif
  }

  t->_isActive = true ;
  t->action() ;

// don't set invalid, it may get restarted.  also, need to be able to free it
//  t->setInvalid();

  t->doneCB().invoke( t->args() ) ;
  t->_isActive = false ;
  return 0 ;
} // Thread :: threadProc

void Thread :: action() 
{
  responseCB().invoke( args() ) ;
} // Thread :: action

bool Thread :: isValid()
{ 
  return (thread_isvalid( _handle ) != 0) ;
} // Thread :: isValid

void Thread :: setInvalid()
{
  thread_setinvalid( _handle ) ;
} // Thread :: setInvalid

Thread :: Thread( Callback *response, bool debugExclude )
: _responseCB( this )
, _doneCB( this )
{
  setInvalid();
  _debugExclude = debugExclude ;
  _threadId = 0 ;
  _isActive = false ;
  if (response != nullptr)
    _responseCB.install( response ) ;
  nUsed ++ ;
  nActive++ ;
} // Thread [constructor]

Thread :: ~Thread()
{
  destroy() ;
  nActive-- ;
} // Thread [destructor]

int32_t Thread :: create() 
{
  _handle = thread_create( threadProc, this, _threadId ) ;
  return isValid() ;
} // Thread :: create

int32_t Thread :: destroy() 
{
  if (!isValid())
    return -1 ;

  int32_t rc ;
#ifdef __WIN32__
  rc = thread_destroy( _handle ) ;
#else
  if (((rc = thread_destroy( _handle )) != 0) && (rc != ESRCH))
#endif
  
  setInvalid() ;
  return 0 ;
} // Thread :: destroy

void Thread :: start() 
{
  if (!isActive()) 
  {
    if (!this->create())
    {
      return ;
    }      
  }

  resume() ;
} // Thread :: start

void Thread :: stop() 
{
  thread_stop( _handle ) ;
} // Thread :: stop

int32_t Thread :: suspend() 
{
  return thread_suspend( _handle ) ;
} // Thread :: suspend

int32_t Thread :: resume() 
{
  return thread_resume( _handle ) ;
} // Thread :: resume

int32_t Thread :: join()
{
  if (!_isActive)
  {
#ifdef __WIN32__
    ::CloseHandle( _handle ) ;
    return 0 ;
#elif defined(UNIX)
    return ::pthread_cancel( _handle ) ;
#endif
  }
  return thread_join( _handle, (HANDLE)_threadId ) ;
} // Thread :: join

#ifdef _WINDOWS

int32_t Thread :: priority( THREADPRIORITY priority ) 
{
  return ::thread_setpriority( _handle, priority ) ;
} // Thread :: priority

int32_t Thread :: priority_idle()
{
  return priority( thread_idle ) ;
} // Thread :: priority_idle

int32_t Thread :: priority_low()
{
  return priority( thread_low ) ;
} // Thread :: priority_low

int32_t Thread :: priority_medium()
{
  return priority( thread_normal ) ;
} // Thread :: priority_medium

int32_t Thread :: priority_high()
{
  return priority( thread_high ) ;
} // Thread :: priority_high

int32_t Thread :: priority_realtime()
{
  return priority( thread_realtime ) ;
} // Thread :: priority_realtime

#endif  // _WINDOWS

int32_t Thread :: isCurrent()
{
#ifdef FREEBSD
  return thread_iscurrent( (int32_t)_handle ) ;
#else
  return thread_iscurrent( _handle ) ;
#endif
} // Thread :: isCurrent

int32_t Thread :: isActive()
{
  return _isActive ;
} // Thread :: isActive

} // namespace go 
