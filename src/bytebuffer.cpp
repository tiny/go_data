/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       bytebuffer.cpp
  @brief      ByteBuffer class implementation

  @author     rmcinnis@solidice.com
  @date       june 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <bytebuffer.h>
#include <algorithm>
#ifdef UNIX
#  include <string.h>
#endif

namespace go {

int16_t ByteBuffer :: increase() 
{
  if ((_buff_sz + _buff_incsz) > _buff_maxsz)
    return -1 ;
  byte   *tmp = new byte[ (_buff_sz += _buff_incsz) ] ;
  if (tmp == nullptr)
    return -2 ;
  memcpy( tmp, _buffer, _buff_used ) ;
  delete _buffer ;
  _buffer = tmp ;
  return 0 ;
} // ByteBuffer :: increase

ByteBuffer :: ByteBuffer( int32_t init_sz, int32_t inc_sz, int32_t max_sz ) 
{
  _buffer     = (init_sz <= 0) ? nullptr : new byte[ init_sz ] ;
  _buff_maxsz = max_sz ;
  _buff_incsz = inc_sz ;
  _buff_sz    = max( 0, init_sz ) ;
  _buff_used  = 0L ;
} // ByteBuffer [constructor]

ByteBuffer :: ~ByteBuffer() 
{
  delete _buffer ;
  _buffer = nullptr ;
} // ByteBuffer [destructor]

int16_t ByteBuffer :: append( int32_t nbytes, const void *data ) 
{
  if (data == nullptr)
    return -1 ;
  if (nbytes <= 0)
    return 0 ;

  int16_t rc ;

  // increase until we have a big enough buffer or we overflow
  while ((_buff_used + nbytes) >= _buff_sz)
  {
    if ((rc = increase()) != 0)
      return rc ;
  }

  MutexScope  sc( _gate ) ;
  memcpy( &_buffer[ _buff_used ], data, nbytes ) ;
  _buff_used += nbytes ;
  return 0 ;
} // ByteBuffer :: append

int16_t ByteBuffer :: reserve( int32_t nbytes ) 
{
  if (nbytes <= 0)
    return 0 ;

  MutexScope  sc( _gate ) ;
  int16_t       rc = 0 ;
  while ((_buff_used + nbytes) >= _buff_sz)
  {
    if ((rc = increase()) != 0)
      return rc ;
  }

  return rc ;
} // ByteBuffer :: reserve

int16_t ByteBuffer :: shift( int32_t nbytes ) 
{
  if (nbytes <= 0)
    return 0 ;

  MutexScope  sc( _gate ) ;

  nbytes = min( nbytes, _buff_used ) ;
  memcpy( _buffer, &_buffer[ nbytes ], (_buff_used - nbytes) ) ;
  _buff_used -= nbytes ;
  return 0 ;
} // ByteBuffer :: shift

} // namespace go 
