/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       msgcb.cpp
  @brief      MsgCB class implementation - multi message thread callback class

  @author     rmcinnis@solidice.com
  @date       march 23, 1992
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <stdio.h>
#include <stdlib.h>
#include <msgcb.h>
#include <assert.h>
#include <gotools_core_version.h>
#include <cstring>

#define  MUTEXSCOPE     GateScope<CBMUTEX>

namespace go {

//------------------------------------------------------------------------
bool        _cbSingleThreadSwitch = false ;
int32_t     Callback :: _useageMeter = 0 ;
int32_t     Callback :: _nActive = 0 ;
int32_t     Callback :: _version = gotools_core_version() ;

//------------------------------------------------------------------------
void MsgCB :: cb_clear() 
{
  delete _cbArray ;
  _cbArray    = nullptr ;
  _cbArraySz  = 0 ;
  _cbWatchers = 0 ;
} // MsgCB :: cb_clear

Callback *MsgCB :: cb_erase( uint16_t ndx ) 
{
  if ((_cbWatchers == 0) || (ndx > (_cbWatchers - 1)))
    return nullptr ;

  Callback  *cb = _cbArray[ndx] ;

  if ((ndx > 0) && (ndx < (_cbWatchers-1)))
    memcpy( &_cbArray[ ndx ], &_cbArray[ ndx+1 ], (_cbWatchers - ndx - 1)*sizeof(Callback*) ) ;
  else if ((ndx == 0) && (_cbWatchers > 1))
    memcpy( &_cbArray[ ndx ], &_cbArray[ ndx+1 ], (_cbWatchers - 1)*sizeof(Callback*) ) ;

  _cbWatchers-- ;
  _cbArray[ _cbWatchers ] = nullptr ;
  return cb ;      

} // MsgCB :: cb_erase

void MsgCB :: cb_grow( uint16_t delta ) 
{
  MUTEXSCOPE  sc( _cbGate ) ;

  // allocate enough space for new array
  Callback  **tmp = (Callback**)new byte[ sizeof(Callback*) * (delta + _cbArraySz) ] ;
  // init members
  memset( tmp, 0x00, (delta + _cbArraySz)*sizeof(Callback*) ) ; 
  if (_cbArray != nullptr)
  {
    memcpy( tmp, _cbArray, _cbArraySz*sizeof(Callback*) ) ;
    delete _cbArray ;
  }
  _cbArraySz  = _cbArraySz + delta ;
  _cbArray    = tmp ;
} // MsgCB :: cb_grow

Callback *MsgCB :: cb_pushback( Callback *c ) 
{
  if (c == nullptr)
    return nullptr ;

  if ((_cbWatchers+1) >= _cbArraySz)
    cb_grow( max( 3, _cbWatchers/4 ) ) ;

  MUTEXSCOPE  sc( _cbGate ) ;
  _cbArray[ _cbWatchers++ ] = c ;
  return c ;
} // MsgCB :: cb_pushback

void MsgCB :: clearList()
{
  if (_isDead || (nWatchers() == 0))
    return ;

  if (_isCopy)
  {
    MUTEXSCOPE     sc( _cbGate ) ;
    // don't delete the callbacks - this is a copy
    memset( _cbArray, 0x00, sizeof(Callback*) * _cbWatchers ) ;
    _cbWatchers = 0 ;
    return ;
  }

  MUTEXSCOPE  sc( _cbGate ) ;
  for (uint16_t i = 0; i < _cbWatchers; i++)
    delete _cbArray[i] ;
  memset( _cbArray, 0x00, sizeof(Callback*) * _cbWatchers ) ;
  _cbWatchers = 0 ;
} // MsgCB :: clearList

Callback *MsgCB :: factoryCopy( Callback * /* cb */ )
{
//  if (dynamic_cast
  return nullptr ;
} // MsgCB :: factoryCopy

MsgCB :: MsgCB ( void *srcObj )
{
  _srcObj       = srcObj ;
  _block        = 0 ;
  _needToInvoke = false ;
  _isCopy       = false ;
  _isDead       = false ;

  _cbArraySz    = 0 ;
  _cbArray      = nullptr ;
  _cbWatchers   = 0 ;
} // MsgCB [constructor]

MsgCB :: MsgCB ( const MsgCB &cb )
{
  _cbArraySz    = 0 ;
  _cbArray      = nullptr ;
  _cbWatchers   = 0 ;

  *this = cb ;
} // MsgCB [copy constructor]

MsgCB :: ~MsgCB ()
{
  clearList() ;
  cb_clear() ; 
  _isDead = true ;
} // MsgCB [destructor]

void MsgCB :: block()
{
  _block++ ;
  _needToInvoke = false ; // reset
} // MsgCB :: block

Callback *MsgCB :: get ( void *destObj )
{
  if (_isDead || (_cbWatchers == 0))
    return nullptr ;

  MUTEXSCOPE  sc( _cbGate ) ;
  for (uint16_t i = 0; i < _cbWatchers; i++)
    if (_cbArray[i]->destObj() == destObj)
      return _cbArray[i] ;

  return nullptr ;
} // MsgCB :: get

Callback *MsgCB :: install ( void *destObj, MsgFunc msg )
{
  if (_isDead || (msg == nullptr))
    return nullptr ;

  return cb_pushback( new CNode( _srcObj, destObj, msg ) ) ;
} // MsgCB :: install

Callback *MsgCB :: install ( Callback *c )
{
  if (_isDead || (c == nullptr))
    return nullptr ;

  c->srcObj( _srcObj ) ;
  return cb_pushback( c ) ;
} // MsgCB :: install

int32_t MsgCB :: invoke ( PBVParameter *arg1, PBVParameter *arg2 )
{
  if (arg1 == nullptr)
    return 0 ;

  return invoke( ArgList() << arg1 << arg2 ) ;
} // MsgCB :: invoke

int32_t MsgCB :: invoke ()
{
  if (_isDead || (_cbWatchers == 0))
    return 0 ;

  if (_block)
  {
    _needToInvoke = true ;
    return 0 ;
  }

  MUTEXSCOPE  sc( _cbGate ) ;
  int32_t         i = 0, rc = 0 ;

    for (i = 0; i < _cbWatchers; i++)
    {
      try
      {
        Callback::_nInvokes++ ;
        if ((rc = _cbArray[i]->invoke()) != 0)
          break ;
      }
      catch (...)
      {
        assert(false) ;
//        _cbArray[i]->comatose() ; // disable this callback in the future
      }
    } 

  _needToInvoke = false ;
  return rc ;
} // MsgCB :: invoke

int32_t MsgCB :: invoke ( const ArgList &args )
{
  if (_isDead || (_cbWatchers == 0))
    return 0 ;

  if (_block)
  {
    _needToInvoke = true ;
    return 0 ;
  }

  MUTEXSCOPE  sc( _cbGate ) ;
  int32_t         i = 0, rc = 0 ;

    for (i = 0; i < _cbWatchers; i++)
    {
      try
      {
        Callback::_nInvokes++ ;
        if ((rc = _cbArray[i]->invoke(args)) != 0)
          break ;
      }
      catch (...)
      {
        assert(false) ;
//        _cbArray[i]->comatose() ; // disable this callback in the future
      }
    } 

  _needToInvoke = false ;
  return rc ;
} // MsgCB :: invoke

Callback *MsgCB :: remove ( void *destObj, int16_t )
{
  if (_isDead || (_cbWatchers == 0))
    return nullptr ;

  MUTEXSCOPE  sc( _cbGate ) ;

  for (uint16_t i = 0; i < _cbWatchers; i++)
    if (_cbArray[i]->destObj() == destObj)
      return cb_erase( i ) ;
  return nullptr ;
} // MsgCB :: remove

Callback *MsgCB :: remove ( MsgCB &cb )
{
  return remove( &cb ) ;
} // MsgCB :: remove

Callback *MsgCB :: remove ( void *srcObj, void *destObj, bool )
{
  if (_isDead || (_cbWatchers == 0))
    return nullptr ;

  MUTEXSCOPE  sc( _cbGate ) ;

  for (uint16_t i = 0; i < _cbWatchers; i++)
    if ((_cbArray[i]->destObj() == destObj) && (_cbArray[i]->srcObj() == srcObj))
      return cb_erase( i ) ;

  return nullptr ;
} // MsgCB :: remove

Callback *MsgCB :: trigger( PokeFunc msg )
{
  if (_isDead || (msg == nullptr))
    return nullptr ;

  return cb_pushback( new CPokeNode( msg ) ) ;
} // MsgCB :: trigger

Callback *MsgCB :: trigger( Callback *c )
{
  if (_isDead || (c == nullptr))
    return nullptr ;

  c->srcObj( _srcObj ) ;
  return cb_pushback( c ) ;
} // MsgCB :: trigger

Callback *MsgCB :: trigger( MsgCB &cb )
{
  if (_isDead) return nullptr ;
  
  Callback *c = new MsgTriggerNode( &cb ) ;
  c->srcObj( _srcObj ) ;
  return cb_pushback( c ) ;
} // MsgCB :: trigger

int32_t MsgCB :: unblock()
{
  if (_isDead || enabled())
  {
    // should be a bug - unblocked too many times
    return 0 ;
  }

  _block-- ;
  if ((_needToInvoke) && (_block == 0))
    return invoke() ;
  else
    return 0 ;
} // MsgCB :: unblock

int32_t MsgCB :: unblock( const ArgList &args )
{
  if (_isDead || enabled())
  {
    // should be a bug - unblocked too many times
    return 0 ;
  }

  _block-- ;
  if ((_needToInvoke) && (_block == 0))
    return invoke( args ) ;
  else
    return 0 ;
} // MsgCB :: unblock

MsgCB &MsgCB :: operator=( const MsgCB &cb )
{
  clearList() ;
  cb_clear() ;

  _srcObj       = cb._srcObj ;
  _block        = cb._block ;
  _needToInvoke = cb._needToInvoke ;
  _isCopy       = true ;
  _isDead       = cb._isDead ;

//  _cbList       = cb._cbList ;
  cb_grow( cb._cbArraySz ) ;
  memcpy( _cbArray, cb._cbArray, cb._cbArraySz * sizeof(Callback*) ) ;
  _cbArraySz = cb._cbArraySz ;
  _cbWatchers = cb._cbWatchers ;


  return *this ;
} // MsgCB :: operator=

//------------------------------------------------------------------------
MsgTriggerNode :: MsgTriggerNode( MsgCB *cb )
{ 
  _srcObj = nullptr ; 
  _cb = cb ; 
  throwExceptionIfNull( cb ) ;
} // MsgTriggerNode [constructor]

int32_t MsgTriggerNode :: invoke() 
{ 
  return _cb->invoke() ; 
} // MsgTriggerNode :: invoke

int32_t MsgTriggerNode :: invoke( const ArgList &args ) 
{ 
  return _cb->invoke( args ) ; 
} // MsgTriggerNode :: invoke 

} // namespace go 
