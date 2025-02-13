/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostatenode.cpp
  @brief      GOStateNode class implementation

  @author     rmcinnis@solidice.com
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <gostatenode.h>
#include <cb_statetimeout.h>

namespace go {

void GOStateNode :: onTimeout( int32_t token ) 
{
  if ((!_hasTimeout && (_timer.nWatchers() == 0)) || !_isActive || (token != _token))
    return ;
  if (_timeout_dest == _name)
  {
    return ;
  }

  // bump to next state
  _leaveCB.invoke( ArgList() << (void*)_name.c_str() << (void*)_timeout_dest.c_str() << (void*)this << (int32_t)0 << (void*)nullptr ) ;
  set_active( false, 0, nullptr ) ;
} // GOStateNode :: onTimeout

GOStateNode :: GOStateNode( const char *name ) 
: _name( name )
, _inStateEvents( this )
, _enterCB ( this ) 
, _leaveCB ( this ) 
{
  _hTimer     = NULL ;
  _isActive   = false ;
  _hasTimeout = false ;
  _timeOut    = 15*1000 ;
  _token      = 10 ;
} // GOStateNode [constructor]

GOStateNode :: GOStateNode( const GOStateNode &i ) 
: _name( i._name )
, _inStateEvents( this ) /* not doing a deep copy */
, _enterCB ( i._enterCB ) 
, _leaveCB ( i._leaveCB ) 
, _timeout_dest( i._timeout_dest )
{
  _hTimer     = NULL ;
  _isActive   = i._isActive ;
  _hasTimeout = i._hasTimeout ;
  _timeOut    = i._timeOut ;
  _token      = i._token ;
  _enterCB.srcObj( this ) ;
  _leaveCB.srcObj( this ) ;
} // GOStateNode [constructor]

GOStateNode :: ~GOStateNode() 
{
  if (_hTimer != NULL)
  {
    _timer.unnotify( _hTimer ) ;
    _hTimer = NULL;
  }
} // GOStateNode [destructor]

int16_t GOStateNode :: add_timeout_transition( int32_t timeout, const char *dest )
{
  if ((_hTimer != NULL) && (_timeOut != timeout))
  {
    _timer.unnotify( _hTimer ) ;
    _hTimer = NULL;
  }

  _hasTimeout   = ((timeout > 0) && (dest != nullptr)) ;
  _timeOut      = timeout ;
  _timeout_dest = dest ;

  if (_hasTimeout)
  {
    _hTimer = _timer.notify( _timeOut, new StateTimeoutObserver<GOStateNode>( _token, this, &GOStateNode::onTimeout ), true ) ;
  }

  return 0 ;
} // GOStateNode :: add_timeout_transition

int16_t GOStateNode :: add_transition( int32_t event_id, const char *dest ) 
{
  if (dest == nullptr)
    return -1 ;

  TransitionMap_iter    iter = _transitions.find( event_id ) ;
  if (iter != _transitions.end())
  {
    return ((*iter).second == dest) ? 0 : -2 ; // 1 event cannot have more then 1 transition
  }
  _transitions.insert( TransitionMap_pair( event_id, dest )) ;
  return 0 ;
} // GOStateNode :: add_transition

int16_t GOStateNode :: process( int32_t event_id, int32_t payload_sz, void *payload ) 
{
  if (!_isActive)
    return -1 ; // not active state .. invalid to process events

  // push to event-tree for this node
//  _args.init() ;
//  _args << payload_sz << payload ;
//  _inStateEvents.invoke( event_id, &_args ) ;
//  _args.init() ;
//  _args << payload_sz << payload ;
  ArgList  args ;
  int32_t     result_msgid = 0 ;
  args << payload_sz << payload << &result_msgid ;
  _inStateEvents.invoke( event_id, &args ) ;

  // check to see if we need to move off this state
  MutexScope            sc( _transitions.gate() ) ;
  TransitionMap_iter    iter = _transitions.find( (result_msgid != 0) ? result_msgid : event_id ) ;

  if (iter == _transitions.end())
    return -2 ; // unhandled event

  //  check name of next state
  if ((*iter).second == _name)
    return 0 ; // state unchanged

  // state change
  // note: leaveCB should be processed by GOStateDiagram and would set active the appropriate state
  set_active( false, payload_sz, payload ) ;
  _leaveCB.invoke( ArgList() << (void*)_name.c_str() << (void*)(*iter).second.c_str() << (void*)this << payload_sz << payload ) ;

  return 0 ;
} // GOStateNode :: process

void GOStateNode :: remove_timeout_transition() 
{
  _isActive   = false ;
  _hasTimeout = false ;
  _timeOut    = 15*1000 ;
  if (_hTimer != NULL)
  {
    _timer.unnotify( _hTimer ) ;
    _hTimer = NULL;
  }

  _timeout_dest.empty() ;
} // GOStateNode :: remove_timeout_transition

int16_t GOStateNode :: remove_transition( int32_t event_id, const char *dest ) 
{
  if (dest == nullptr)
    return -1 ;

  MutexScope            sc( _transitions.gate() ) ;
  TransitionMap_iter    iter = _transitions.find( event_id ) ;

  if (iter == _transitions.end())
    return 0 ;

  _transitions.erase( iter ) ;
  return 0 ;
} // GOStateNode :: remove_transition

void GOStateNode :: set_active( bool isActive, int32_t payload_sz, void *payload ) 
{
  if (_isActive == isActive)
    return ; // no change in state

  // changing state.. trigger associated callback
  // note: enterCB triggered here, leaveCB triggered from process
  _isActive = isActive ;

  if (isActive)
  {
    int32_t     result_msgid = 0 ;
    _enterCB.invoke( ArgList() << (void*)_name.c_str() << (void*)this << payload_sz << payload << &result_msgid ) ;

    // allowing an on-enter callback to push to next state
    if (result_msgid != 0L)
    {
      MutexScope            sc( _transitions.gate() ) ;
      TransitionMap_iter    iter = _transitions.find( result_msgid ) ;

      if ((iter != _transitions.end()) && ((*iter).second != _name))
      {
        // state change
        // note: leaveCB should be processed by GOStateDiagram and would set active the appropriate state
        _leaveCB.invoke( ArgList() << (void*)_name.c_str() << (void*)(*iter).second.c_str() << (void*)this << payload_sz << payload ) ;

//        set_active( false, payload_sz, payload ) ;
        // set inactive as it leaves to next state
        _isActive = false ;
        _timer.stop() ;
        if (_hTimer != NULL)
        {
          _timer.unnotify( _hTimer ) ;
          _hTimer = NULL;
        }
        return ; // nothing more to do
      }
    }

    if (_hasTimeout || (_timer.nWatchers() != 0))
    {
      _token++ ;
      _hTimer = _timer.notify( _timeOut, new StateTimeoutObserver<GOStateNode>( _token, this, &GOStateNode::onTimeout ), true ) ;
      _timer.start() ;
    }
  }
  else
  {
    _timer.stop() ;
    if (_hTimer != NULL)
    {
      _timer.unnotify( _hTimer ) ;
      _hTimer = NULL;
    }
  }
} // GOStateNode :: set_active

} // namespace go 

