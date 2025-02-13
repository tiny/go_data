/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       thread_core.cpp
  @brief      core thread functions

  @author     rmcinnis@solidice.com
  @date       december 15, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <thread_core.h>
#ifdef _WIN32
#  include <windows.h>
#  include <process.h>
#endif
#include <stdio.h>
#include <string.h>

#ifdef UNIX
#  include <errno.h>
#elif defined(_WINDOWS) || defined(_AFXDLL)
#  if defined(_AFXDLL) && !defined(_WINSOCK2API_)
#    include <winsock2.h>
#  endif
#endif

namespace go {

HTHREAD thread_create( ThreadFunc f, void *data, HANDLE &thread_id )
{
  HTHREAD  handle ;
#ifdef __WIN32__
  handle = (void*)_beginthreadex( nullptr, 0, f, data, CREATE_SUSPENDED, (uint32_t*)&thread_id ) ;
//  handle = ::CreateThread( nullptr, 0, (LPTHREAD_START_ROUTINE) f, data,CREATE_SUSPENDED, (LPDWORD)&thread_id ) ;
  return handle ;
#else
  int32_t  rc ;

  do {
#  if defined(UNIX)
    rc = pthread_create( &handle, (pthread_attr_t*)nullptr, f, data ) ;
    if (rc != 0)
    {
      fprintf( stderr, "(Threads.create) *** Error: <%d:%s> creating thread\n",rc, strerror(errno));
    }
#  else
    rc = ::thr_create( nullptr, 0, (LPTHREAD_START_ROUTINE)f, data, THR_SUSPENDED, &handle ) ;
#  endif
  } while ((rc != 0) && (errno != EINTR)) ;
  return handle ;
#endif
} // thread_create

int32_t thread_isvalid( HTHREAD handle )
{
// #if defined(_HPUX_)  10.20 only
#if 0
  return (!((_handle.field1 == nullptr) &&
	    (_handle.field2 == (-1)) &&
	    (_handle.field3 == (-1))));
#else
  return (handle != (HTHREAD)INVALID_HANDLE_VALUE) ;
#endif
} // :: thread_isvalid

void thread_setinvalid( HTHREAD &handle ) 
{
// #if defined(_HPUX_)  10.20 only
#if 0
  handle.field1=nullptr;
  handle.field2=-1;
  handle.field3=-1;
#else
  handle = (HTHREAD)INVALID_HANDLE_VALUE ;
#endif
} // :: thread_setinvalid

int32_t thread_isactive( HTHREAD handle )
{ 
  return thread_isvalid( handle ) ;
} // :: thread_isactive

int32_t thread_iscurrent( HANDLE thread_id )
{
  return (thread_self() == (HTHREAD)thread_id) ;
} // :: thread_iscurrent

int32_t thread_destroy( HTHREAD handle )
{
  int32_t    rc = 0 ;

  if (thread_isvalid( handle ))
  {
#ifdef __WIN32__
    // If not the current thread, wait for it to finish before yanking out the rug!
    if ( !thread_iscurrent( handle ) )
    {
      WaitForSingleObject( handle, INFINITE );
      try {
        TerminateThread(handle, 0x00);
//        ::CloseHandle(handle);
      }
      catch (...) {
      }
    }
#else
#  if defined(UNIX)
  rc = ::pthread_cancel( handle ) ;
#  else
  rc = 0 ;
  ::thr_exit( nullptr ) ;
#  endif
#endif
    return rc ;
  }
  else
    return -1 ;
} // :: thread_destroy

int32_t thread_start( HTHREAD handle )
{
#ifdef __WIN32__
  uint32_t result = ::ResumeThread ( handle );
  return ( result != 0xFFFFFFFF ) ;
#else

  // In HPUX the thread is already started.
#  if !defined(_HPUX_) && !defined(LINUX) && !defined(SUN)
  int32_t rc = thr_continue( handle ) ;
  if (rc != 0)
    fprintf( stderr, "Problem resuming thread\n" ) ;
  return (rc == 0) ;

#  else
  // HPUX or linux
  return 0;
#  endif
#endif // ! WIN32
} // :: thread_start

int32_t thread_join( HTHREAD handle, HANDLE thread_id )
{
  if (!thread_isactive( handle ))
    return 0 ;
  
#ifdef __WIN32__
  if ( thread_iscurrent( thread_id ) ) 
  {
    // error - can't join from same thread!
    return 0;
  }

  WaitForSingleObject( handle, INFINITE );
  return 1 ;
#else
#if defined(UNIX)
  return ::pthread_join( handle, (void **)nullptr ) ;
#else
  return ::thr_join( handle, nullptr, nullptr ) ;
#endif
#endif
} // :: thread_join

HTHREAD thread_self()
{
#ifdef __WIN32__
  return (HANDLE)::GetCurrentThreadId() ;
#else
  return ::pthread_self() ;
#endif
} // :: thread_self

int32_t thread_stop( HTHREAD &handle )
{
  if (!thread_isactive( handle )) 
    return 0 ;

  int32_t rc = 0 ;

#ifdef __WIN32__
  if ( GetCurrentThread() == handle ) 
  {
	// commiting suicide
    ::_endthreadex( 0 ) ;
//  ::ExitThread( 0 );
  } 
  else 
  {
	  // NB: this is a NASTY way to kill a thread in win32!
	::TerminateThread( handle, 1 ) ;
  }
  rc = 0 ;
#elif defined(_HPUX_)
  rc = pthread_cancel( handle ) ;
  if (rc != 0)
    fprintf( stderr, "Error while stopping thread.\n" ) ;
#elif defined (LINUX)
  rc = pthread_cancel( handle ) ;
  if (rc != 0)
    fprintf( stderr, "Error while stopping thread.\n" ) ;
#elif defined (SUN)
  int32_t rc = pthread_cancel( _handle ) ;
  if (rc != 0)
    fprintf( stderr, "Error while stopping thread.\n" ) ;
#else
#  error  Not yet ported to targeted platform - Please port
#endif

  thread_setinvalid( handle ) ;
  return rc ;
} // :: thread_stop

int32_t thread_suspend( HTHREAD handle )
{
  if (!thread_isactive( handle ))
    return false ;

#ifdef __WIN32__
  return (::SuspendThread ( handle ) != 0xFFFFFFFF) ;
#else

#  if defined(UNIX)
     return false ;
#  else
     return (::thr_suspend( handle ) == 0) ;
#  endif
#endif
} // :: thread_suspend

int32_t thread_resume( HTHREAD handle )
{
  if (!thread_isactive( handle ))
    return false ;

#ifdef __WIN32__
  uint32_t result = ::ResumeThread ( handle );
  return ( result == 0xFFFFFFFF ) ;
#else

  // In HPUX the thread is already started.
#  if !defined(_HPUX_) && !defined(LINUX) && !defined(SUN)
  int32_t rc = thr_continue( handle ) ;
  if (rc != 0)
    fprintf( stderr, "Problem resuming thread\n" ) ;
  //  else
  //    fprintf( stderr, "Thread resumed...\n" ) ;
  return (rc != 0) ;

#  else // HPUX or linux
  return 0;
#  endif
#endif // ! WIN32
} // :: thread_resume

int32_t thread_setpriority( HTHREAD handle, THREADPRIORITY priority ) 
{
#ifdef __WIN32__
  if (!thread_isactive( handle ))
    return false ;

  switch (priority)
  {
    case thread_idle  :
      return ::SetThreadPriority( handle, THREAD_PRIORITY_IDLE ) ;
    case thread_low  :
      return ::SetThreadPriority( handle, THREAD_PRIORITY_BELOW_NORMAL ) ;
    case thread_normal  :
      return ::SetThreadPriority( handle, THREAD_PRIORITY_NORMAL ) ;
    case thread_high :
      return ::SetThreadPriority( handle, THREAD_PRIORITY_HIGHEST ) ;
    case thread_realtime :
      return ::SetThreadPriority( handle, THREAD_PRIORITY_TIME_CRITICAL ) ;
    default :
      return 0 ;
  }
#else
  return 0 ;
#endif
} // :: thread_setpriority

} // namespace go 
