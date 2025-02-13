/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inifiledb.h
  IniFileDB class definition

  programmer  rmcinnis
  date        april 11, 1997
*/
#ifndef INIFILEDB_H
#define INIFILEDB_H

#include <inisection.h>
#include <grdeque.h>

namespace go {

typedef grdeque<IniSection*>                  INISectionDeque ;
typedef grdeque<IniSection*>::iterator        INISectionDequeIter ;
typedef grdeque<IniSection*>::const_iterator  INISectionDequeIter_const ;

class IniFileDB
{
  private :
    GOString            _fname ;
    INISectionDeque     _sections ;

  public  :
                        IniFileDB() ;
                        IniFileDB( const IniFileDB &c ) ;
    virtual            ~IniFileDB() ;

    virtual void        clear() ;
    virtual IniSection *find( const char *section ) ;
    virtual IniField   *find( const char *section, const char *fldname ) ;
    virtual const char *get( const char *section, const char *fldname, const char *defaultValue="" ) ;
    virtual bool        set( const char *section, const char *fldname, const char *value ) ;
    virtual void        remove( const char *section, const char *fldname ) ;

    // import/export methods
    virtual bool        import( const char *fname ) ;
    virtual bool        save_as( const char *fname ) ;

    // operators
    IniFileDB          &operator= ( const IniFileDB &c ) ;

    // access methods
    const char         *ini_fname() { return _fname.c_str() ; }
} ; // class IniFileDB

} // namespace go 

#endif

