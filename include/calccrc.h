/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  calccrc.h
  calculate CRC value from a buffer

  programmer  rmcinnis
  date        august 31, 1996
*/
#ifndef CALCCRC_H
#define CALCCRC_H

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
#include <enum.h>

#ifndef UNIX
#include <ctype.h>
#else
#include <stdio.h>
#endif

namespace go {

uint32_t calcCRC( unsigned char *s, size_t n, bool initialize = true ) ;
uint32_t calcCRC_t( unsigned char *s, size_t n, uint32_t &crc ) ; // thread-safe version; init crc to -1L

} // namespace go 

#endif

