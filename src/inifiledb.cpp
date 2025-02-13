/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inifiledb.cpp
  IniFileDB class implementation

  programmer  rmcinnis
  date        april 11, 1997
*/
#include <enum.h>
#include <inifiledb.h>

namespace go {

IniFileDB :: IniFileDB()
{
} // IniFileDB [constructor]

IniFileDB :: IniFileDB( const IniFileDB &c )
{
  *this = c ;
} // IniFileDB [constructor]

IniFileDB :: ~IniFileDB() 
{
  clear() ;
} // IniFileDB [destructor]

void IniFileDB :: clear()
{
  INISectionDequeIter  iter ;
  LockScope            sc( _sections.mtx() ) ;

  for (iter = _sections.begin(); iter != _sections.end(); iter++)
    delete (*iter) ;
  _sections.clear() ;  
} // IniFileDB :: clear

IniSection *IniFileDB :: find( const char *section_name )
{
  if ((section_name == nullptr) || (_sections.size() == 0))
    return nullptr ;
  
  INISectionDequeIter  iter ;
  LockScope            sc( _sections.mtx() ) ;

  for (iter = _sections.begin(); iter != _sections.end(); iter++)
    if ((*iter)->name() == section_name)
      return (*iter) ;
  return nullptr ;
} // IniFileDB :: find

IniField *IniFileDB :: find( const char *section_name, const char *fldname )
{
  if ((section_name == nullptr) || (_sections.size() == 0))
    return nullptr ;

  IniSection  *section = find( section_name ) ;
  return (section == nullptr) ? nullptr : section->find( fldname ) ;
} // IniFileDB :: find

const char *IniFileDB :: get( const char *section_name, const char *fldname, const char *defaultValue )
{
  IniField  *fld = find( section_name, fldname ) ;

  if ((fld != nullptr) && !fld->value().isNull()) return (const char *)fld->value() ;
  // set default value in struct and return 
  set( section_name, fldname, defaultValue ) ;
  return defaultValue ;
} // IniFileDB :: get

bool IniFileDB :: set( const char *section_name, const char *fldname, const char *value )
{
  if ((section_name == nullptr) || (fldname == nullptr))
    return false ;

  IniSection  *section = find( section_name ) ;
  if (section == nullptr)
  {
    _sections.push_back( (section = new IniSection( section_name )) ) ;
  }
  IniField   *fld = section->find( fldname ) ;
  if (fld == nullptr)
  {
    *section << new IniField( fldname, value ) ;
  }
  else
  {
    fld->value() = value ;
  }
/*
  IniField   *fld = find( section_name, fldname ) ;

  if (fld == nullptr)
  {
    IniSection  *section = find( section_name ) ;
    if (section == nullptr)
      _sections.push_back( (section = new IniSection( section_name )) ) ;
    *section << (fld = new IniField( fldname, value )) ;
  }
  else
    fld->value() = value ;
*/
  return true ;
} // IniFileDB :: set

void IniFileDB :: remove( const char *section_name, const char *fldname )
{
  IniSection   *section = find( section_name ) ;
  if (section == nullptr)
    return ;
  section->remove( fldname ) ;
} // IniFileDB :: remove

bool IniFileDB :: import( const char *fname )
{
  clear() ;
  
  _fname = fname ;
  if (fname == nullptr)
    return false ;
  DAR  db ;
  if (!db.open( _fname, "r" ))
    return false ;

  LockScope        sc( _sections.mtx() ) ;
  IniSection      *sect ;

  while (!db.eof())
  {
    sect = new IniSection( "" ) ;
    if (!sect->import( db ))
      delete sect ;
    else
      _sections.push_back( sect ) ;
  }

  db.close() ;
  return true ;
} // IniFileDB :: import

bool IniFileDB :: save_as( const char *fname )
{
  _fname = fname ;
  if (fname == nullptr)
    return false ;

  DAR  db ;
  if (!db.open( _fname, "w+" ))
    return false ;

  LockScope sc( _sections.mtx() ) ;
  INISectionDequeIter  iter ;

  for (iter = _sections.begin(); iter != _sections.end(); iter++)
    (*iter)->save_as( db ) ;

  db.close() ;
  return true ;
}  // IniFileDB :: save_as

IniFileDB &IniFileDB :: operator= ( const IniFileDB &c ) 
{
  if (this != &c)
  {
    _fname = c._fname ;
    _sections.clear() ;

    INISectionDequeIter_const     it ;
    for (it = c._sections.begin(); it != c._sections.end(); it++)
    {
      _sections.push_back( new IniSection( *(*it) ) ) ;
    }

  }
  return *this ;
} // IniFileDB :: operator=

} // namespace go {
