/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_flags.h
  @brief      various report flag bitfields

  @author     rmcinnis@solidice.com
  @date       august 05, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef REPORT_FLAG_H
#define REPORT_FLAG_H

#define  rmREPORTNONE                  ( 0x00000000 )
#define  rmREPORT_DEBUG                ( 0x00000002 )
#define  rmREPORTALL                   ( 0xFFFFFFFF )

#define isReported(f,v)                ((f & v) != 0)

#endif
