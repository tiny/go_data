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
*/
#include <stdlib.h>
#include <stricmp.h>

namespace go {

int32_t stricmp ( const char *s1, const char *s2 )
{
    char  result ;
    if ((s1 == NULL) || (s2 == NULL))
      return (s1 == s2) ;
  
    while (*s1 && *s2)
    {
	result = *s1 ^ *s2 ;
	if ((result == 0) || (result == 0x20))
	    s1++, s2++ ;
	else
	    break ;
    }
    return ((*s1 == 0) && (*s2 == 0)) ? 0
	: ((*s1 > *s2) ? 1 : -1) ;
} /* stricmp */
  
int32_t strnicmp ( const char *s1, const char *s2, int32_t n )
{
    char  result ;
    if ((s1 == NULL) || (s2 == NULL))
      return (s1 == s2) ;

    while (*s1 && *s2 && n)
    {
	result = *s1 ^ *s2 ;
	if ((result == 0) || (result == 0x20))
	    s1++, s2++, n-- ;
	else
	    break ;
    }
    return ((n == 0) || ((*s1 == 0) && (*s2 == 0)))
	? 0
	    : ((*s1 > *s2) ? 1 : -1) ;
} /* strnicmp */

} // namespace go 

