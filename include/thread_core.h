/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*
  @file       thread_core.h
  @brief      core thread functional prototypes

  @author     rmcinnis@solidice.com
  @date       december 15, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef THREAD_CORE_H
#define TRREAD_CORE_H

#include <enum.h>

#ifdef UNIX
#  include <pthread.h>
#  if defined( _HPUX_) || defined (LINUX) || defined(SUN)
#    define  HTHREAD  pthread_t
#  else
#    define  HTHREAD  thread_t
#  endif
#  define  HANDLE   int32_t
#  define  INVALID_HANDLE_VALUE  -1
#  define  THREADRETURN   void *
#  define  WINAPI_STDCALL 
#else
   typedef void *HANDLE;
#  define  HTHREAD  HANDLE
#  define  THREADRETURN   uint32_t 
#  define  WINAPI_STDCALL __stdcall
#endif

namespace go {

typedef THREADRETURN  (WINAPI_STDCALL *ThreadFunc)( void *data ) ;

enum THREADPRIORITY
{
  thread_idle, thread_low, thread_normal, thread_high, thread_realtime
} ;

HTHREAD thread_create( ThreadFunc f, void *data, HANDLE &thread_id ) ;
int32_t thread_isvalid( HTHREAD handle ) ;
void thread_setinvalid( HTHREAD &handle ) ;
int32_t thread_isactive( HTHREAD handle ) ;
int32_t thread_iscurrent( HANDLE thread_id ) ;
int32_t thread_destroy( HTHREAD handle ) ;
int32_t thread_start( HTHREAD handle ) ;
int32_t thread_join( HTHREAD handle, HANDLE thread_id ) ;
HTHREAD thread_self() ;
int32_t thread_stop( HTHREAD &handle ) ;
int32_t thread_suspend( HTHREAD handle ) ;
int32_t thread_resume( HTHREAD handle ) ;
int32_t thread_setpriority( HTHREAD handle, THREADPRIORITY priority ) ;

} // namespace go 

#endif
