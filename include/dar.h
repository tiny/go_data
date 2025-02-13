/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       dar.h
  @brief      Disk Access Routines class definition

  @author     rmcinnis@solidice.com
  @date       june 10, 1992
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef  DAR_H
#define  DAR_H

#include <gostring.h>
#include <stdio.h>
#include <string.h>

namespace go {

#define  FNULL     -1

/*!
  @class DAR
   
  <b>Description:</b>
   
  <b>Notes:</b>
  Disk Access Routines for random access files on many platforms
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class _EXPORT DAR
{
  private  :
    GOString            _fileName ; // absolute path to file
    GOString            _fileMode ; // mode the file was opened in
    FILE               *_stream ;
    int32_t                 _error ; // file error
    char                _error_buf[256] ;
    size_t              _amt ;
    int32_t                _len ;

  public   :
                        DAR ( const char *fname = "", const char *mode="r+b" ) ;
                        DAR ( const DAR &d ) ;
    virtual            ~DAR () ;
    virtual bool        open ( const char *fname, const char *mode="r+b" ) ;
    virtual bool        Open ( const char *fname, const char *mode="r+b" ) ;
    virtual bool        open_tmp() ; // creates a temp file in current directory and opens it
    virtual GOString    tmpname() ; // creates temp file name, does not open
    virtual bool        create( const char *fname ) { return open( fname, "w+b" ) ; }
    virtual void        close () ;
    virtual bool        unlink () ; // remove current file from disk
    virtual bool        read ( size_t amt, void *dest ) ;
    virtual bool        read ( byte &dest ) { return read( sizeof(byte), &dest ) ; }
    virtual bool        ignoreUntil( const char *stopSet ) ; // ignore all characters until anything in stopSet found
    virtual bool        readWhile( const char *stopSet ) ; // keep reading until a character comes up that is not in the set
    virtual bool        readUntil( const char *stopSet, char *dest = nullptr, int16_t max = -1 ) ; 
    virtual int32_t         getch () ;
    virtual bool        write ( size_t amt, const void *src ) ;
    virtual bool        write ( byte src )
                        {
                          byte i = src ;
                          return write( sizeof(byte), &i ) ;
                        }
    virtual bool        seek ( fileaddr pos, int32_t whence = SEEK_SET ) ;
    virtual bool        flush() ;
    virtual bool        rewind() { return seek ( 0L ) ; }
    virtual fileaddr    tell () ;
    virtual fileaddr    length () ; // length of file
    virtual bool        eof () ;
    virtual bool        lock ( fileaddr offset, int32_t length ) ;
    virtual bool        unlock ( fileaddr offset, int32_t length ) ;
    virtual bool        isOpen () const { return (_stream != nullptr) ; }

    // overloaded operators
    DAR                &operator=(const DAR &d ) ;
                        operator FILE*() { return _stream ; }

    // access methods
    GOString           &fileName() { return _fileName ; }
    GOString           &fileMode() { return _fileMode ; }
    FILE              *&stream () { return _stream ; }
    int32_t                &error() { return _error ; }
    size_t             &amt() { return _amt ; }
    const char         *errMsg() ;

    // statics
    static uint32_t        nReads ;
    static uint32_t        nSeeks ;
    static uint32_t        nWrites ;
    static uint32_t        nFlushes ;
} ; // class DAR

} // namespace go 

#endif


