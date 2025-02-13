/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*
  enum.h
  some useful definitions

  Defines used:

    __MEMORYDEBUGGER__        Used to find memory leaks
    __BORLANDC__
    UNIX
    _HPUX_
    LINUX
    _MSC_VER
    __WIN32__
    __DLL__
    RWSTD_NO_BOOL
	_USE_MFC				When using MFC
*/
#ifndef  ENUM_H
#define  ENUM_H

#include <cstddef>
#include <stdint.h>


#ifdef LINUX
#  include <arpa/inet.h>
#  if !defined(NULL)
#    define NULL  0
#  endif
#  if !defined(byte)
     typedef unsigned char byte ;
#  endif
#endif

namespace go {

/*
   Define the size_t type in the std namespace if in C++ or globally if in C.
   If we're in C++, make the _SIZE_T macro expand to std::size_t
*/
#if !defined(_SIZE_T) && !defined(_SIZE_T_DEFINED)
#  define _SIZE_T_DEFINED
#if defined(_WIN64)
   typedef unsigned __int64 size_t;
#else
   typedef unsigned int size_t;
#endif
#  if defined(__cplusplus)
#    define _SIZE_T std::size_t
#  else
#    define _SIZE_T size_t
#  endif
#endif

// useful macros 
#ifndef USE_STL
#  define  setbit(b,i)         b |= (1 << i)
#  define  clearbit(b,i)       b &= ~(1 << i)
#  define  togglebit(b,i)      b = (b & (1 << i)) ? (b & ~(1 << i)) : (b | (1 << i))
#  define  getbit(b,i)         (b & (1 << i))
#  define  bitset(b,i)         ((b & (1 << i)) != 0)
#endif

#ifdef   _MSC_VER
# ifndef __LITTLE_ENDIAN
#   define __LITTLE_ENDIAN 1234
# endif

# if !defined(__GNUC__)
#  define TYPENAME  typename
# else
#  define TYPENAME  
# endif

// completely turning this message off
// warning C4237: nonstandard extension used : 'bool' keyword is reserved for future use
# pragma warning(disable:4237)
// warning C4786: identifier was truncated to '255' characters in the debug information
# ifdef _DEBUG
#  pragma warning(disable:4786)
# endif
// warning C4355: 'this' : used in base member initializer list
# pragma warning(disable:4355)
# if !defined(__WIN32__) && defined(WIN32)
#   define __WIN32__   1
# endif
# if defined(WIN32) && !defined(WINVER)
  //#define WINVER 0x0400
#   define WINVER 0x0500   /* windows 2000 or beyond */
# endif
#  if defined(_WINDOWS) && !defined(_AFXDLL)
#    ifndef _INC_WINDOWS
#      include <windows.h>
#    endif /* _INC_WINDOWS */
#    include <tchar.h>
#  elif defined(_AFXDLL)
//#    include <afxwin.h>
#      include <windows.h>
#      include <tchar.h>
#  endif
#  if (_MSC_VER >= 1000) && defined(USE_STL)
#   pragma warning(disable:4666) // get rid of STL nuisance warnings.4786
#   pragma warning(disable:4786)
#   include <stl_config.h>
#  endif
#endif

#define AfxGetInstanceHandle()    GetModuleHandle(nullptr)

#if defined(SUN) && !defined(__SUNPRO_CC)
#  if !defined(__GNUC__)
#    define bool char
#  endif
#elif defined(__SUNPRO_CC)
#  include <stl_config.h>
#endif

#if !defined(byte) && !defined(__RPCNDR_H_VERSION__)
  typedef unsigned char byte ;
#endif

#if defined(__BORANDC__)
#  define bool byte
#endif

#ifdef LINUX
#  include <sys/types.h>
   typedef unsigned char byte ;
#endif

#if ! defined( bool ) && ! defined (__BORLANDC__)
#  ifndef BOOL
     typedef int32_t        BOOL ;
#  endif

#  if !defined(UNIX) && (_MSC_VER < 1000) && !defined(__GNUC__)
     typedef char bool ;
#  endif
#endif

#if !defined(boolean)
  typedef unsigned char boolean ;
# if !defined(__BORLANDC__) && !defined(LINUX) && (_MSC_VER < 1000) && !defined(__GNUC__)
    enum boolean_flags_tag { false = 0, true = 1 } boolean_flags ;
# endif
#endif

#ifdef ONLY_USE_BIT_SPECIFIED_LENGTH_NUMERICS

#if !defined(_MSC_VER) && !defined(UINT)
  typedef uint32_t   UINT ;
#endif
#endif

#if !defined(LPSTR)
  typedef char *   LPSTR ;
#endif

#if !defined(BYTE)
  typedef unsigned char   BYTE ;
#endif

#if !defined(fileaddr)
  typedef int32_t   fileaddr ;
#endif

#ifdef  _MSC_VER
#  define DllExport __declspec( dllexport )
#  define  _EXPORT
#elif defined(__DLL__)
#  define  _EXPORT   _export
#else
#  ifdef __WIN32__
#    define  _EXPORT
#  elif defined(UNIX)
#    define  _EXPORT
#  else
#    define  _EXPORT   huge
#  endif
#endif

#if defined(LINUX)
//#pragma message "defining min & max"
#  define __MINMAX_DEFINED
//#  ifndef max
//#    define max(a,b)   (((a) > (b)) ? (a) : (b))
//#  endif

//#  ifndef min
//#    define min(a,b)   (((a) < (b)) ? (a) : (b))
//#  endif
#endif

#ifdef SUN
# ifndef __GNUC__
#    define max(a,b)   (((a) > (b)) ? (a) : (b))
#    define min(a,b)   (((a) < (b)) ? (a) : (b))
# endif
#endif

//#if !defined(FIX_THIS_FOR_ALL_SYSTEMS_BEFORE_BLINDLY_COMMENTING_IT_OUT) && !defined(WIN32)
#ifdef _MSC_VER
#  define gMaximum(a,b)   (((a) > (b)) ? (a) : (b))
#  define gMinimum(a,b)   (((a) < (b)) ? (a) : (b))
//#else
//#  define gMaximum(a,b)   max(a,b)
//#  define gMinimum(a,b)   min(a,b)
#endif
//#endif

//#ifndef nullptr
//#  define nullptr  0L
//#endif

#ifdef __LITTLE_ENDIAN
#  define ENCODENETWORK(x) (x->encodeNetwork())
#  define DECODEHOST(x) (x->decodeHost())
#else
#  define ENCODENETWORK(x)
#  define DECODEHOST(x)
#endif

#ifdef LINUX
# define sprintf_s   snprintf
# define sscanf_s    sscanf
#endif

#define  INT64   int64_t

// 64 bit int32_t support
#ifdef __GNUC__
#  include <sys/types.h>
//#  define int64_t   int64_t
#elif defined(__WIN32__) || defined(WIN32)
//#  define int64_t   __int32_t64
#else
#  error   not implemented for this compiler/operating system
#endif

} // namespace go

#endif // ENUM_H



