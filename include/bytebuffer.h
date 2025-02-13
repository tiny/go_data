/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       bytebuffer.h
  @brief      ByteBuffer class definition

  @author     rmcinnis@solidice.com
  @date       june 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef BYTEBUFFER_H
#define BYTEBUFFER_H

#include <enum.h>
#include <mutexscope.h>

/*!
  @class ByteBuffer
   
  <b>Description:</b>
   
  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#define BUFFER_INITSZ   ( 1*1024)
#define BUFFER_INCSZ    ( 2*1024)
#define BUFFER_MAXSZ    (30*1024)

namespace go {

class ByteBuffer
{
  private :
    MutexGate           _gate ;
    byte               *_buffer ;
    int32_t                _buff_maxsz ; // max size buffer is allowed to grow to
    int32_t                _buff_incsz ; // increment size for buffer size bumps 
    int32_t                _buff_sz ; // total size of buffer in memory (not to exceed _buff_maxsz)
    int32_t                _buff_used ; // nbytes in buffer used 

    int16_t               increase() ;

  public  :
                        ByteBuffer( int32_t init_sz = BUFFER_INITSZ, int32_t inc_sz = BUFFER_INCSZ, int32_t max_sz = BUFFER_MAXSZ ) ;
    virtual            ~ByteBuffer() ;

    int16_t               append( int32_t nbytes, const void *data ) ; // appends data to back of buffer; 0 == ok, <0 == error
    int16_t               reserve( int32_t nbytes ) ; // insures there are nbytes available in buffer
    void                set_used( int32_t nused ) { _buff_used = nused ; } // careful: can be risky - int16_t circuiting some code
    int16_t               shift( int32_t nbytes ) ; // 'shifts out' nbytes from front of buffer

    // access methods
    MutexGate          &gate() { return _gate ; }
    byte               *data() { return _buffer ; }
    int32_t                nbytes() { return _buff_used ; }
    int32_t                available() const { return (_buff_sz - _buff_used) ; }
} ; // class ByteBuffer

} // namespace go 

#endif
