/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_msg.h
  @brief      ReportMsg class definition

  @author     rmcinnis@solidice.com
  @date       august 08, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef REPORT_MSG_H
#define REPORT_MSG_H

#include <gostring.h>
#include <pointintime.h>
#include <report_flags.h>
#include <gomap.h>
#include <iostream>

namespace go {

typedef GOMap<int64_t,int32_t>               int32_map ;
typedef GOMap<int64_t,int32_t>::iterator     int32_map_iter ;
typedef GOMap<int64_t,int32_t>::value_type   int32_map_pair ;

//ostream &operator<<( ostream &out, const lngint32_tmap::value_type &v ) ;

//#include <stdio.h>

/*!
  @class      ReportMsg
   
  <b>Description:</b>

  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class ReportMsg : public GOString
{
  private  :
    int32_t               _userdata ;
    int32_t               _flags ;
    int32_t                _level ;
    PointInTime        _tm ;
    int32_map          _debuglevels ;
    
  public   :
                       ReportMsg() ;
    bool               isWatched() ;
    int32_t                maxlevel() ;
    void               register_level( int64_t id_or_this, int32_t level ) ;

    int32_t              &userdata() { return _userdata ; }
    int32_t              &flags() { return _flags ; }
    int32_t               &level() { return _level ; }
    int32_t                nRegistered() { return _debuglevels.size() ; }
    PointInTime       &tm() { return _tm ; }
} ; // class ReportMsg

ReportMsg  &report_output_str() ;

} // namespace go 

#endif
