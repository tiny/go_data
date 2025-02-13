/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gotools_core_version.h
  @brief      simple library version function

  @author     rmcinnis@solidice.com
  @date       july 17, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOTOOLS_CORE_VERSION_H
#define GOTOOLS_CORE_VERSION_H

#include <enum.h>

namespace go {

#ifdef __cplusplus
extern "C" {
#endif

uint32_t gotools_core_version() ;

#ifdef __cplusplus
} ;
#endif

} // namespace go 

#endif
