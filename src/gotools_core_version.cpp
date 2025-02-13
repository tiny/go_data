/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gotools_core_version.cpp
  @brief      simple library version function

  @author     rmcinnis@solidice.com
  @date       july 17, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <gotools_core_version.h>
#include <cstring>

namespace go {

#ifdef __cplusplus
extern "C" {
#endif

const char *gbl_copyright = "GOTools - Guarded, Observable, object-oriented library.\n"
                            "copyright (c) 2008 Solid ICE Technologies, Inc.  All rights reserved." ;
const char *gbl_developers = "Robert McInnis - rmcinnis@solidice.com\n" ;
const char *gbl_gotools_version = "GOTools v0.0.2.0" ;

uint32_t gotools_core_version()
{
  static char buf[512] ;
  strncpy( buf, gbl_copyright      , 511 ) ;
  strncpy( buf, gbl_developers     , 511 ) ;
  strncpy( buf, gbl_gotools_version, 511 ) ;
  return 0x00000200 ; // 0.0.1.0
} // :: gotools_core_version

#ifdef __cplusplus
} ;
#endif

} // namespace go 
