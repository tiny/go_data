/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       fastgate.h
  @brief      FastGate class definition

  @author     rmcinnis@solidice.com
  @date       april 16, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef FASTGATE_H
#define FASTGATE_H

#if defined(UNIX) | defined(LINUX)
#  include <pthread.h>
#  define SECTION    pthread_mutex_t
#  define ENTER      ::pthread_mutex_lock
#  define EXIT       ::pthread_mutex_unlock
#else
#  include <windows.h>
#  define SECTION    CRITICAL_SECTION
#  define ENTER      ::EnterCriticalSection
#  define EXIT       ::LeaveCriticalSection
#endif

namespace go {

/*!
  @class FastGate
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       april 16, 1997
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class FastGate 
{
  private :
    bool                _isCreated ; // support for late creation
    SECTION             _section ;

    void                create() ;

  public  :
                        FastGate() ;
                       ~FastGate() ;

    inline void         enter() 
                        {
                          if (!_isCreated) create() ;
                          ENTER( &_section ) ;
                        }
    inline void         exit() 
                        {
                          // commented out creation for performance
                          // we should have gone thru enter & therefore have created the gate
                          // if (!_isCreated) create() ; 
                          EXIT( &_section ) ;
                        }

} ; // class FastGate

} // namespace go 

#endif


