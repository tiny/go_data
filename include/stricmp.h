/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       stricmp.h
  @brief      case insensitive string comparisons

  @author     rmcinnis@solidice.com
  @date       may 29, 1991
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.

  value   meaning
  --------------------------------
  < 0     s1 less than s2
  = 0     s1 identical to s2
  > 0     s1 greater than s2
*/
#ifndef STRICMP_H
#define STRICMP_H

#ifdef __cplusplus
extern "C" {
#endif

namespace go {

int32_t stricmp ( const char *s1, const char *s2 ) ;
int32_t strnicmp ( const char *s1, const char *s2, int32_t n ) ;

} // namespace go 

#ifdef __cplusplus
} ;
#endif

#endif // STRICMP_H

