/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inisection.h
  IniSection class definition

  programmer  rmcinnis
  date        april 11, 1997
*/
#ifndef INISECTION_H
#define INISECTION_H

#include <inifield.h>
#include <grdeque.h>

namespace go {

typedef grdeque<IniField*>                 INIFieldDeque ;
typedef grdeque<IniField*>::iterator       INIFieldDequeIter ;
typedef grdeque<IniField*>::const_iterator INIFieldDequeIter_const ;

class IniSection
{
  private :
    GOString             _name ;
    INIFieldDeque        _fields ;

  public  :
                         IniSection( const char *name ) ;
                         IniSection( const IniSection &c ) ;
    virtual             ~IniSection() ;
    virtual void         clear() ;
    IniField            *find( const char *fldname ) ;
    virtual void         remove( const char *fldname ) ;

    // import/export methods
    virtual bool         import( DAR &db ) ;
    virtual bool         save_as( DAR &db ) ;

    // overloaded operators
    IniSection          &operator= ( const IniSection &c ) ;
    IniSection          &operator<< ( IniField *fld ) ;

    // access methods
    GOString            &name() { return _name ; }
    INIFieldDeque       &fields() { return _fields ; }
} ; // class IniSection

} // namespace go 

#endif
