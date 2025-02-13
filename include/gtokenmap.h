/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       GTokenMap.h
  @brief      GTokenMap template definition

  @author     rmcinnis@solidice.com
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <map>
#include <msgcb.h>
#include <mutexscope.h>

#ifdef _MSC_VER
#  pragma warning(disable: 4355)
#endif

#if !defined(__STL_NO_NAMESPACES)
using namespace std;
#endif

namespace go {

/*!
  @class GTokenMap< ELTTYPE >
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class ELTTYPE>
class GTokenMap
{
  public    :
    typedef typename pair<uint32_t, ELTTYPE>                  member ;
    typedef typename map<uint32_t,member>                     tokenmap ;
    typedef typename map<uint32_t,member>::iterator           tokenmap_iter ;
    typedef typename map<uint32_t,member>::value_type         tokenmap_pair ;

  protected :
    MutexGate           _gate ;
    tokenmap            _s2d ;
    tokenmap            _d2s ;
    uint32_t               _ticker ; // used to create new d_tokens
    tokenmap_iter       _current ;
    MsgCB               _preEraseCB ;
    MsgCB               _postInsertCB ;

  public    :
                        GTokenMap( uint32_t start_ticker = 100L ) 
                        : _preEraseCB( this ), _postInsertCB( this ) 
                        { _ticker = 0L ; }
                       ~GTokenMap() {}

    tokenmap_iter       find_dest( uint32_t d_token ) 
    {
      MutexScope        sc( _gate ) ;
      tokenmap_iter     it = _d2s.find( d_token ) ;
      return (it != _d2s.end()) ? it : _s2d.end() ; // return s2d.end for a consistent comparison point32_t
    }
    tokenmap_iter       find_src ( uint32_t s_token )
    {
      MutexScope        sc( _gate ) ;
      tokenmap_iter     it = _s2d.find( s_token ) ;
      return (it != _s2d.end()) ? it : _s2d.end() ;
    }
    uint32_t               insert   ( uint32_t s_token, ELTTYPE data ) // returns d_token
    {
      MutexScope                  sc( _gate ) ;
      tokenmap_iter               it = _s2d.find( s_token ) ;
      uint32_t                       d_token = -1L ;
      pair<tokenmap_iter, bool>   insert_result ;
      insert_result.second = false ;
      if (it != _s2d.end())
      {
        // replacement?
        (*it).second.second = data ;
        d_token = (*it).second.first ;
      }
      else
      {
        d_token = ++_ticker ;
        if (d_token == -1) // wrap around??
          d_token = _ticker = 1 ; // jump over -1 and 0 - what about overrunning existing data?
        insert_result = _s2d.insert( tokenmap_pair( s_token, member( d_token, data ))) ;
      }
      _d2s[ d_token ] = member( s_token, data ) ;
      if (insert_result.second)
      {
        _current = insert_result.first ;
        _postInsertCB.invoke( ArgList() << (void*)this ) ;
      }
      return d_token ;
    }
    int16_t               erase    ( uint32_t s_token ) // 0 == erased; != 0 means error
    {
      MutexScope        sc( _gate ) ;
      tokenmap_iter     it = _s2d.find( s_token ) ;
      if (it == _s2d.end())
        return 0 ;
      // trigger pre-erase-cb
      _current = it ;
      _preEraseCB.invoke( ArgList() << (void*)this ) ;

      // actually remove it
      uint32_t   d_token = (*it).second.first ;
      _s2d.erase( it ) ;
      // clean up d2s
      it = _d2s.find( d_token ) ;
      if (it == _d2s.end())
        return 0 ;
      _d2s.erase( it ) ;
      return 0 ;
    }
    MutexGate          &gate() { return _gate ; }
    tokenmap_iter       begin() { return _s2d.begin() ; }
    tokenmap_iter       current() { return _current ; } // not valid unless retrieved while responding to a callback
    tokenmap_iter       end  () { return _s2d.end() ; }
    MsgCB              &preEraseCB() { return( _preEraseCB ); }
    MsgCB              &postInsertCB() { return( _postInsertCB ); }
} ; // class GTokenMap

} // namespace go 
