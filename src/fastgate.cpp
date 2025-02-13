/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       fastgate.cpp
  @brief      FastGate class implementation

  @author     rmcinnis@solidice.com
  @date       april 16, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <fastgate.h>
#ifdef UNIX
#  include <stdio.h>
#endif

namespace go {

void FastGate :: create() 
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
  ::InitializeCriticalSection( &_section ) ;
#endif
  _isCreated = true ;
} // FastGate :: create

FastGate :: FastGate()  
{
  _isCreated = false ;
  //memset( &_section, 0x00, sizeof(SECTION) ) ;
} // FastGate [constructor]

FastGate :: ~FastGate() 
{
  if (_isCreated)
  {
#ifdef UNIX
    ::pthread_mutex_destroy( &_section ) ;
#else
    ::DeleteCriticalSection( &_section ) ;
#endif
  }
} // FastGate [destructor]

} // namespace go 
