/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inifield.cpp
  IniField class implementation

  programmer  rmcinnis
  date        april 11, 1997
*/
#include <inifield.h>
#include <stdio.h>

namespace go {

IniField :: IniField( const char *name, const char *value )
: _name( name )
, _value( value )
{
} // IniField [constructor]

IniField :: IniField( const IniField &c )
{
  *this = c ;
} // IniField [constructor]

IniField :: ~IniField() 
{
} // IniField [destructor]

IniField &IniField :: operator= ( const IniField &c ) 
{
  if (this != &c)
  {
    _name  = c._name ;
    _value = c._value ;
  }
  return *this ;
} // IniField :: operator =

bool IniField :: import( DAR &db ) 
{
  if (!db.isOpen())
    return false ;

  GOString    buf ;
  bool        done, errFound ;
  char        ch ;

  _name    = nullptr ;
  _value   = nullptr ;
  done     = false ;
  errFound = false ;
  while (!done && !db.eof() && !errFound)
  {
    ch = db.getch() ;

    switch (ch)
    {
      case  '='  :
//        buf += (char)'\0' ;
        _name = buf ; 
        done = true ;
        break ;
      case  '\n' :
      case  '\r' :
        buf.empty() ;
        break ;
      case  '['  : // invalid in the name area
      case  ']'  : // invalid in the name area
        errFound = true ;
        db.seek( -1, SEEK_CUR ) ; // back up 1
        break ;
      default    :
        buf += ch ;
        break ;
    }
  }

  if (db.eof() || errFound)
    return false ;

  done = false ;
  buf.empty() ;
  while (!done && !db.eof())
  {
    ch = db.getch() ;

    switch (ch)
    {
      case  '\n' :
      case  '\r' :
//        buf += '\0' ;
        _value = buf ;
        done = true ;
        break ;
      default    :
        buf += ch ;
        break ;
    }
  }

  return true ;
} // IniField :: import 

bool IniField :: save_as( DAR &db ) const
{
  if (!db.isOpen())
    return false ;
  
  if (_name.isNull())
    return true ; // nothing to do

  fprintf( db, "%s", (const char*)_name ) ;
  if (!_value.isNull())
    fprintf( db, "=%s\n", (const char*)_value ) ;
  else
    fprintf( db, "\n" ) ;
  return true ;
} // IniField :: save_as

} // namespace go 
