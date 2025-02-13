/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       dar.cpp
  @brief      Disk Access Routines class implementation

  @author     rmcinnis@solidice.com
  @date       june 10, 1992
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <errno.h>
#include <dar.h>
#include <stdlib.h>

#ifdef _MSC_VER 
#  include <sys/locking.h>
#  include <io.h>
#else
# if defined(__BORLANDC__)
#  include <io.h>
# else
#  if defined(UNIX)
#   include <unistd.h>
#  endif
# endif
#endif

#include <string.h>
#include <time.h>

namespace go {

// statics
uint32_t DAR :: nReads   = 0L ;
uint32_t DAR :: nWrites  = 0L ;
uint32_t DAR :: nSeeks   = 0L ;
uint32_t DAR :: nFlushes = 0L ;

DAR :: DAR ( const char *fname, const char *mode )
: _fileName( fname )
, _fileMode( mode ) 
{
  _stream = nullptr ;
  _error = 0 ;
  _amt = 0 ;
  _len = -1L ;
  _error_buf[0] = '\0' ;
  
  if ((fname != nullptr) && (fname[0] != '\0'))
    open ( fname, mode ) ;
} // DAR [constructor]

DAR :: DAR ( const DAR &d )
: _fileName( d._fileName ) 
, _fileMode( d._fileMode )
{
  _stream = nullptr ;
  _error = 0 ;
  _amt = 0 ;
  _len = -1L ;
} // DAR [constructor]

DAR :: ~DAR ()
{
  close () ;
} // DAR [destructor]

bool DAR :: open ( const char *fname, const char *mode )
{
  close() ;
  if ((fname == nullptr) || (fname[0] == '\0'))
    return false ;

  _stream = ::fopen ( fname, mode ) ;
  if (_stream == nullptr)
  {
    _error = errno ;
    return false ;
  }

  _fileName = fname ;
  _fileMode = mode ;
  return true ;
} // DAR :: open

bool DAR :: Open ( const char *fname, const char *mode )
{
  return DAR::open( fname, mode ) ;
} // DAR :: Open

bool DAR :: open_tmp ()
{
  close() ;

  // get temporary file name
  GOString fname = tmpname() ;

  _stream = ::fopen ( fname, "w+b" ) ;
  if (_stream == nullptr)
  {
    _error = errno ;
    return false ;
  }

  _fileName = fname ;
  _fileMode = "w+b" ;
  return true ;
} // DAR :: open_tmp

GOString DAR :: tmpname() 
{
  // get temporary file name
  char  fname[64] ;
  FILE *tmp = nullptr ;
  do {
    sprintf_s( fname, 64, "vtmp%06ld%03d.tmp", ::time(nullptr), rand() % 1000 ) ;
    if (tmp != nullptr)  ::fclose(tmp) ;
    tmp = fopen( fname, "rb" ) ;
  } while (tmp != nullptr) ;
  if (tmp != nullptr)  ::fclose(tmp) ;
  return fname ;
} // DAR :: tmpname

void DAR :: close ()
{
  if (_stream != nullptr)
    ::fclose ( _stream ) ;

  _stream   = nullptr ;
  _error    = 0 ;
  _fileName = (const char*)nullptr ;
  _fileMode = (const char*)nullptr ;
  _len      = -1L ;
} // DAR :: close

bool DAR :: unlink ()
{
  if (_fileName == (const char*)nullptr)
    return false ;

  // close file if open
  if (_stream != nullptr)
  {
    ::fclose ( _stream ) ;
    _stream = nullptr ;
  }

  // delete file
  if (::unlink ( _fileName ) == -1)
    return false ;

  close() ; // clean up variables
  return true ;
} // DAR :: unlink

bool DAR :: read ( size_t amt, void *dest )
{
  if (!isOpen())
    return false ;

//  int32_t  fpos = ::ftell( _stream ) ;
//  _amt = ::read ( fileno(_stream), dest, amt ) ;

  // since MS are such morons, the fread call won't read over
  // 64k in one shot, even tho the size_t type is an uint32_t (4 bytes).
  // Just more of the same from the guys who brought us windows.

  // loop reads, in chunks of 30k, until we have it all, or hit EOF
  size_t  sz, orig_amt, max_chunk, rc ;

  _amt      = 0 ;
  orig_amt  = amt ;
  max_chunk = 30 * 1024 ;
  while ((amt > 0) && !eof())
  {
    sz = min( amt, max_chunk ) ;
    amt -= sz ;

    rc = ::fread ( dest, 1, sz, _stream ) ;
    nReads++ ;
    if (rc != sz)
      break ;
    _amt += rc ;
    dest = &((byte*)dest)[rc] ;
  }

//  fpos = ::ftell( _stream ) ;

  if (_amt != orig_amt)
  {
    _error = errno ;
    return false ;
  }
  return true ;
} // DAR :: read

bool DAR :: ignoreUntil( const char *stopSet ) 
{
  return readUntil( stopSet, nullptr, -1 ) ;
} // DAR :: ignoreUntil

bool DAR :: readWhile( const char *stopSet )
{
  if (!isOpen())
    return false ;

  char   ch ;

  while (!eof())
  {
    ch = (char)getch() ;
    nReads++ ;
    if (::strchr( stopSet, ch ) == nullptr)
    {
      // put it back so it can be the next thing read
      ::fputc ( ch, _stream ) ;
      return true ;
    }
  }

  return false ;
} // DAR :: readWhile

bool DAR :: readUntil( const char *stopSet, char *dest, int16_t max ) 
{
  if (!isOpen())
    return false ;

  char   ch, *d ;

  d = dest ;
  while (!eof())
  {
    ch = (char)getch() ;
    nReads++ ;
    if (::strchr( stopSet, ch ) != nullptr)
    {
      // put it back so it can be the next thing read
//      ::fputc ( ch, _stream ) ;
      return true ;
    }

    if ((dest != nullptr) && (max > 0))
    {
      *d++ = ch ;
      *d = '\0' ; // null terminate for fun
      max-- ;
    }
  }

  return false ;
} // DAR :: readUntil

int32_t DAR :: getch ()
{
  if (!isOpen())
    return false ;

  int32_t c = ::fgetc ( _stream ) ;

  if (c == EOF)
  {
    _error = errno ;
  }
  return c ;
} // DAR :: getch

bool DAR :: write ( size_t amt, const void *src )
{
  if (!isOpen())
    return false ;

  fileaddr pos = ::ftell( _stream ) ; // should be fast

  _amt = ::fwrite ( src, 1, amt, _stream ) ;
  nWrites++ ;

  if (_amt != amt)
  {
    _error = errno ;
    return false ;
  }
  else
    if (_len != -1) 
      _len = max( _len, (int32_t)(pos+amt) ) ;

  return true ;
} // DAR :: write

bool DAR :: seek ( fileaddr pos, int32_t whence )
{
  if (!isOpen() || ((pos == FNULL) && (whence == SEEK_SET)))
    return false ;

  if (::fseek ( _stream, pos, whence ))
  {
    _error = errno ;
    return false ;
  }
  nSeeks++ ;
  return true ;
} // DAR :: seek

bool DAR :: flush()
{
  if (!isOpen())
  {
    _error = EBADF ;
    return false ;
  }

  int32_t rc = ::fflush( _stream ) ;
  if (rc != 0)
  {
    _error = errno ;
    return false ;
  }
  
  nFlushes++ ;
  return true ;
} // :: flush

fileaddr DAR :: tell ()
{
  if (!isOpen())
    return FNULL ;

  fileaddr pos = ::ftell ( _stream ) ;
  if (pos == FNULL)
  {
    _error = errno ;
    return FNULL ;
  }
  return pos ;
} // DAR :: tell

fileaddr DAR :: length ()
{
  if (!isOpen())
    return FNULL ;

  if (_len != -1L)
    return _len ;
  
  fileaddr  cpos = ::ftell(_stream), pos ;
  ::fseek ( _stream, 0, SEEK_END ) ;
  nSeeks++ ;
  pos = ::ftell ( _stream ) ;
  seek ( cpos ) ;
  return (_len = pos) ;
} // DAR :: length

bool DAR :: eof ()
{
  if (!isOpen())
    return false ;

  return ( feof ( _stream ) != 0 ? true : false );
} // DAR :: eof

bool DAR :: lock ( fileaddr offset, int32_t length )
{
  if (!isOpen() || (offset == FNULL))
    return false ;

  int32_t  rv ;
#ifdef __BORLANDC__
   rv = ::lock ( fileno(_stream), offset, length ) ;
#elif _MSC_VER
  seek( offset ) ;
  rv = ::_locking( fileno(_stream), _LK_LOCK, length ) ;
#elif _HPUX_ || LINUX || SUN
  rv = -1 ;
  // Get rid of warnings
  offset=offset; length=length;
#else
   rv = ::lock ( fileno(_stream), offset, length ) ;
#endif
  if (rv == -1)
  {
    _error = errno ;
    return false ;
  }
  return true ;
} // DAR :: lock

bool DAR :: unlock ( fileaddr offset, int32_t length )
{
  if (!isOpen() || (offset == FNULL))
    return false ;

  int32_t  rv ;
#ifdef __BORLANDC__
  rv = ::unlock ( fileno(_stream), offset, length ) ;
#elif _MSC_VER
  seek( offset ) ;
  rv = ::_locking( fileno(_stream), _LK_UNLCK, length ) ;
#elif _HPUX_ || LINUX || SUN
  rv = -1 ;
  // Get rid of warnings
  offset=offset; length=length;
#else
  rv = ::unlock ( fileno(_stream), offset, length ) ;
#endif
  if (rv == -1)
  {
    _error = errno ;
    return false ;
  }
  return true ;
} // DAR :: unlock

DAR &DAR :: operator=(const DAR &d ) 
{
  _fileName = d._fileName ; 
  _fileMode = d._fileMode ;
  _stream = nullptr ;
  _error = 0 ;
  _amt = 0 ;
  return *this ;
} // DAR :: operator=

const char *DAR :: errMsg()
{
#ifdef _MSC_VER
  strerror_s( _error_buf, 255, _error ) ; // (_error) ; // _sys_errlist[_error] ;
  return _error_buf ;
#elif  _HPUX_
  return "Unknown error" ; 
#else
  return strerror(_error) ; 
#endif
} // DAR :: errMsg

} // namespace go 
