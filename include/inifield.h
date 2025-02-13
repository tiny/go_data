/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file common/solidicelicense.txt for more details      */
/*                                                                            */

/*
  inifield.h
  IniField class definition

  programmer  rmcinnis
  date        april 11, 1997
*/
#ifndef INIFIELD_H
#define INIFIELD_H

#include <dar.h>

namespace go {

class IniField
{
  private :
    GOString       _name ;
    GOString       _value ;

  public  :
                   IniField( const char *name, const char *value ) ;
                   IniField( const IniField &c ) ; 
    virtual       ~IniField() ;

    // cast overload operators
                   operator char*() { return (char*)_value ; }
                   operator const char*() { return (char*)_value ; }
    IniField      &operator= ( const IniField &c ) ;

    // import/export methods
    virtual bool   import( DAR &db ) ;
    virtual bool   save_as( DAR &db ) const ;

    // access methods
    GOString      &name() { return _name ; }
    GOString      &value() { return _value ; }
} ; // class IniField

} // namespace go 

#endif

