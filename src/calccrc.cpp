/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  calccrc.cpp
  calculate CRC value from a buffer

  programmer  rmcinnis
  date        august 31, 1996
*/
#include <stdlib.h>
#ifdef LINUX
# ifdef FREEBSD
#  include <stddef.h>
# else
#  include <linux/stddef.h>
# endif
#endif
#include <calccrc.h>
#include <crc32tab.h>

#define CRC32(c, b) (crc_32_tab[((int32_t)(c) ^ (b)) & 0xff] ^ ((c) >> 8))

namespace go {

/*
  Run a set of bytes through the crc shift register.  If s is a nullptr
  pointr, then initialize the crc shift register contents instead.
  Return the current crc in either case.

  return value
    uint32_t          calculated CRC value

  parameters:
    char   *s      pointr to bytes to pump through
    size_t  n      number of bytes in s[]
*/
uint32_t calcCRC( unsigned char *s, size_t n, bool initialize )
{
  uint32_t c ;                 // temporary variable
  static   uint32_t crc = 0xffffffffL ; // shift register contents

  if (s == nullptr)
    c = 0xffffffffL ;
  else
  {
    if (initialize)
      crc = 0xffffffffL ;

    c = crc ;
    while (n--)
      c = CRC32(c, *s++) ;
  }
  crc = c ;
  return c ^ 0xffffffffL ; // (instead of ~c for 64-bit machines) 
} // :: calcCRC

uint32_t calcCRC_t( unsigned char *s, size_t n, uint32_t &crc )
{
  uint32_t c ;                 // temporary variable

  if (s == nullptr)
    c = 0xffffffffL ;
  else
  {
    c = (crc ^= 0xffffffffL) ;
    while (n--)
      c = CRC32(c, *s++) ;
  }
  return (crc = (c ^ 0xffffffffL)) ; // (instead of ~c for 64-bit machines) 
} // :: calcCRC_t

} // namespace go 

