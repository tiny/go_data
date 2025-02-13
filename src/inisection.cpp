/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inisection.cpp
  IniSection class implementation

  programmer  rmcinnis
  date        april 11, 1997
*/
#include <inisection.h>

namespace go {

IniSection :: IniSection( const char *name ) 
: _name( name )
{
} // IniSection [constructor]

IniSection :: IniSection( const IniSection &c ) 
{
  *this = c ;
} // IniSection [constructor]

IniSection :: ~IniSection()
{
  clear() ;
} // IniSection [destructor]

void IniSection :: clear()
{
  INIFieldDequeIter  iter ;
  LockScope          sc( _fields.mtx() ) ;

  for (iter = _fields.begin(); iter != _fields.end(); iter++)
    delete (*iter) ;
  _fields.clear() ;
} // IniSection :: clear

IniField *IniSection :: find( const char *fldname )
{
  if ((fldname == nullptr) || (_fields.size() == 0))
    return nullptr ;
  
  INIFieldDequeIter  iter ;
  LockScope          sc( _fields.mtx() ) ;

  for (iter = _fields.begin(); iter != _fields.end(); iter++)
    if ((*iter)->name() == fldname)
      return (*iter) ;
  return nullptr ;
} // IniSection :: find

void IniSection :: remove( const char *fldname )
{
  if ((fldname == nullptr) || (_fields.size() == 0))
    return ;
  
  INIFieldDequeIter  iter ;
  LockScope          sc( _fields.mtx() ) ;

  for (iter = _fields.begin(); iter != _fields.end(); iter++)
    if ((*iter)->name() == fldname)
    {
      delete (*iter) ;
      _fields.erase( iter ) ;
      return ;
    }
} // IniSection :: remove

bool IniSection :: import( DAR &db )
{
  if (!db.isOpen())
    return false ;

  GOString    buf ;
  bool        done, inName ;
  char        ch ;

  _name  = "" ;
  done   = false ;
  inName = false ;
  while (!done && !db.eof())
  {
    ch = db.getch() ;

    switch (ch)
    {
      case  '\n'  :
      case  '\r'  :
        inName = false ;
        buf.empty() ;
        break ;
      case  '['   :
        inName = true ;
        break ;
      case  ']'   :
        done = true ;
        buf += '\0' ;
        _name = buf ;
        break ;
      default     :
        if (inName)
          buf += ch ;
        break ;
    }
  }

  if (db.eof())
    return false ;

  done = false ;
  while (!done && !db.eof())
  {
    ch = db.getch() ;
    if (ch == '[')
      done = true ;
    db.seek( -1, SEEK_CUR ) ; // back up 1
    if (!done)
    {
      IniField    *ini = new IniField( "", "" ) ;
      if (!ini->import( db ))
      {
        delete ini ;
        done = true ;
      }
      else
        *this << ini ;
    }
  }

  return true ;
} // IniSection :: import

bool IniSection :: save_as( DAR &db )
{
  if (!db.isOpen())
    return false ;

  INIFieldDequeIter  iter ;
  LockScope          sc( _fields.mtx() ) ;

  fprintf( db, "[%s]\n", (const char*)_name ) ;
  
  for (iter = _fields.begin(); iter != _fields.end(); iter++)
    (*iter)->save_as( db ) ;

  fprintf( db, "\n" ) ;
  return true ;
} // IniSection :: save_as

IniSection &IniSection :: operator= ( const IniSection &c ) 
{
  if (this != &c)
  {
    _name = c._name ;
    _fields.clear() ;

    INIFieldDequeIter_const  it ;
    for (it = c._fields.begin(); it != c._fields.end(); it++)
    {
      _fields.push_back( new IniField( *(*it) )) ;
    }
  } 
  return *this ;
} // IniSection :: operator=

IniSection &IniSection :: operator<< ( IniField *fld )
{
  if (fld == nullptr)
    return *this ;

  _fields.push_back( fld ) ;
  
  return *this ;
} // IniSection :: operator<<

} // namespace go 
