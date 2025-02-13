/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       report_msg.cpp
  @brief      ReportMsg class definition

  @author     rmcinnis@solidice.com
  @date       august 08, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <report_msg.h>

namespace go {

ReportMsg  *gbl_report_output_str = nullptr ;

ReportMsg &report_output_str()
{
  if (gbl_report_output_str == nullptr)
  {
    gbl_report_output_str = new ReportMsg ;
  }
  return *gbl_report_output_str ;
} // :: report_output_str

ostream &operator<<( ostream &out, const int32_map::value_type &v )
{
 out << "(" << v.first << ":" << v.second << ")" << endl;
 return( out );
}

//------------------------------------------------------------------------
ReportMsg :: ReportMsg() 
{ 
  _userdata = 0L ; 
  _flags = 0L ; 
  _level = 0 ; 
} // ReportMsg [constructor]

bool ReportMsg :: isWatched()
{ 
  return (!_debuglevels.empty() && (_level <= maxlevel() )) ; 
} // ReportMsg :: isWatched

int32_t ReportMsg :: maxlevel()
{ 
  int32_map_iter it ;
  int32_t        max_level = (*_debuglevels.begin()).second ;

  for (it = _debuglevels.begin(); it != _debuglevels.end(); it++)
  {
    max_level = max( max_level, (*it).second ) ;
  }
  return max_level ;
} // ReportMsg :: maxlevel

void ReportMsg :: register_level( int64_t id_or_this, int32_t level )
{
  _debuglevels.update( int32_map_pair( id_or_this, level )) ;
} // ReportMsg :: register_level

} // namespace go 
