/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       mutexgate.cpp
  @brief      MutexGate class implementation

  @author     rmcinnis@solidice.com
  @date       april 16, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <mutexgate.h>
#ifdef UNIX
#  include <stdio.h>
#  include <string.h>
#endif

namespace go {

int32_t MutexGate :: nActive = 0L ;
int32_t MutexGate :: nCreated = 0L ;
int32_t MutexGate :: nUsed = 0L ;

void MutexGate :: create() 
{
  if (_isCreated)
    return ;
#ifdef UNIX
  pthread_mutexattr_t  attr ;
  pthread_mutexattr_init( &attr ) ;
  pthread_mutexattr_settype( &attr, PTHREAD_MUTEX_RECURSIVE ) ;
  
  if (::pthread_mutex_init( &_section, &attr ) != 0)
    fprintf( stderr, "Error initializing mutex!\n" ) ;
#else
  SECURITY_ATTRIBUTES attributes;
  ZeroMemory( &attributes, sizeof( SECURITY_ATTRIBUTES ) );
  attributes.nLength = sizeof( SECURITY_ATTRIBUTES );
  if( 0 == _handle ) 
  {
    _handle = ::CreateMutex( &attributes, FALSE, (LPCWSTR)_name ); 
  }
#endif
  nCreated++ ;
  _isCreated = true ;
} // MutexGate :: create

// !!!!NOTE!!!!
// Race condition may cause redundant destruction.
void MutexGate :: destroy()
{
  if (!_isCreated)
    return ;
#ifdef UNIX
  ::pthread_mutex_destroy( &_section ) ;
#else
  ::ReleaseMutex( _handle );
  CloseHandle( _handle );
  _handle    = nullptr ;
  _owner     = 0 ;
#endif
  _isCreated = false ;
  nActive-- ;
} // MutexGate :: destroy

void MutexGate :: init()
{
  _isCreated = false ;
#ifdef UNIX
  memset( &_section , 0, sizeof(_section) );
#else
  _count  = 0 ;
  _owner  = 0 ;
  _handle = nullptr ;
#endif
  nActive++ ;
  nUsed++ ;
} // MutexGate :: init

MutexGate :: MutexGate() 
{
#ifndef UNIX
  _name = nullptr ;
#endif
  init() ;
} // MutexGate [constructor]

MutexGate :: MutexGate( const MutexGate &m ) 
{
#ifndef UNIX
  _name = nullptr ;
  set_name( m._name ) ;
#endif
  init() ;
} // MutexGate [constructor]

MutexGate :: ~MutexGate() 
{
  destroy() ;
#ifndef UNIX
  delete _name ;
  _name = nullptr ;
#endif
} // MutexGate [destructor]

void MutexGate :: enter() 
{
  create() ;
#ifdef UNIX
  ::pthread_mutex_lock( &_section ) ;
#else
// !!!!NOTE!!!!
// Race condition may cause handle leak.
  if( 0 == _handle ) 
    create();

  uint32_t myId = GetCurrentThreadId();
  if ( _owner == myId ) 
  {
	  _count++;
	  return; // already own the mutex
  }

  // If the mutex is available, this is optimal
  if ( WaitForSingleObject( _handle, 50L ) == WAIT_TIMEOUT ) {
	  //
	  // Need to wait for the mutex.
	  // Perform a blocking wait, processing Messages sent
	  // from other threads (see ::SendMessage()).
	  // Prevents deadlocks otherwise int32_troduced by SendMessage().
	  //
	  
    uint32_t rc;
	  MSG msg;
	  while ( true ) {
		  rc = MsgWaitForMultipleObjects( 1, &_handle, FALSE, 5000 /* INFINITE */, QS_SENDMESSAGE ) ;
		  if ( rc == WAIT_OBJECT_0 )
			  break; // locked the semaphore
		  else if ( rc == WAIT_OBJECT_0 + 1 )
			  // process a pending sent message
			  PeekMessage( &msg, nullptr, 0, 0, PM_NOREMOVE );
	  }
  }
  // Have the mutex; record that we own it
  _count++;
  _owner = myId;
#endif
} // MutexGate :: enter

void MutexGate :: exit() 
{
  create() ;
#ifdef UNIX
  ::pthread_mutex_unlock( &_section ) ;
#else
  if ( isOwner() ) 
  {
    _count--;
    if ( _count == 0 ) 
    {
      _owner = 0;
      ReleaseMutex( _handle );
    }
  }
  else
    _count += 0 ; // big problem - who would release a mutex that didn't have it to start with?
#endif
} // MutexGate :: exit

} // namespace go 
