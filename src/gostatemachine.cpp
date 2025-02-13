/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostatemachine.cpp
  @brief      GOStateMachine class implementation

  @author     rmcinnis@solidice.com
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <gostatemachine.h>
#include <cstddef>

namespace go {

GOStateNode *GOStateMachine :: find_node( const char *node_name ) 
{
  StateMap_iter    iter = _states.find( node_name ) ;
  return (iter == _states.end()) ? nullptr : (*iter).second ;
} // GOStateMachine :: find_node

int16_t GOStateMachine :: onStateEnterCB( const ArgList *args )
{
  if (args == nullptr)
    return 0 ;

  // trigger _newStateCB
  // args: new-state-name, new-state-node, this
  const char      *name = (const char*)args->asVoid(0) ;
  GOStateNode     *node = (GOStateNode*)args->asVoid(1) ;
  int32_t             payload_sz = args->asLong(2) ;
  void            *payload = args->asVoid(3) ;

  _active = node ;
  _newStateCB.invoke( ArgList() << (void*)name << (void*)node << (void*)this << payload_sz << payload ) ;
  return 0 ;
} // GOStateMachine :: onStateEnterCB

int16_t GOStateMachine :: onStateLeaveCB( const ArgList *args )
{
  if (args == nullptr)
    return 0 ; 
  //
  // no need to do anything here
  //
//  const char      *old_node_name = (const char*)args->asVoid(0) ;
  const char      *new_node_name = (const char*)args->asVoid(1) ;
//  GOStateNode     *old_node = (GOStateNode*)args->asVoid(2) ;
  int32_t             payload_sz = args->asLong(3) ;
  void            *payload = args->asVoid(4) ;

  _active = find_node( new_node_name ) ;
  if (_active != nullptr)
  {
    _active->set_active( true, payload_sz, payload ) ;
  }
  return 0 ;
} // GOStateMachine :: onStateLeaveCB

GOStateMachine :: GOStateMachine( const char *name )
: _name( name )
, _invalidEventCB( this )
, _newStateCB( this )
{
  _active = nullptr ;
} // GOStateMachine [constructor]

GOStateMachine :: GOStateMachine( const GOStateMachine &i )
: _name( i._name )
, _invalidEventCB( i._invalidEventCB )
, _newStateCB( i._newStateCB )
{
  _active = i._active ;
  _invalidEventCB.srcObj( this ) ;
  _newStateCB.srcObj( this ) ;
} // GOStateMachine [constructor]

GOStateMachine :: ~GOStateMachine()
{
  // clean up
  MutexScope       sc( _states.gate() ) ;
  StateMap_iter    iter ;
  for (iter = _states.begin(); iter != _states.end(); iter++)
  {
    delete (*iter).second->enterCB().remove( this ) ;
    delete (*iter).second->leaveCB().remove( this ) ;
    delete (*iter).second ;
  }
  _states.clear() ;
} // GOStateMachine [destructor]

int16_t GOStateMachine :: add_state( const char *state_name )
{
  if (state_name == nullptr)
    return -1 ;

  MutexScope       sc( _states.gate() ) ;
  GOStateNode     *node = find_node( state_name ) ;
  if (node != nullptr)
    return 0 ;
  _states.insert( StateMap_pair( state_name, (node = new GOStateNode( state_name )) )) ;

  if (node != nullptr)
  {
    node->enterCB().install( new Observer<GOStateMachine>( this, &GOStateMachine::onStateEnterCB )) ;
    node->leaveCB().install( new Observer<GOStateMachine>( this, &GOStateMachine::onStateLeaveCB )) ;
  }
  return 0 ;
} // GOStateMachine :: add_state

int16_t GOStateMachine :: add_state_action( const char *state_name, uint32_t event_id, Callback *cb )
{
  if ((state_name == nullptr) || (cb == nullptr))
    return -1 ;

  get_node( state_name )->inStateEvents().event( event_id )->install( cb ) ;
  return 0 ;
} // GOStateMachine :: add_state_action

int16_t GOStateMachine :: add_timeout( const char *src, int32_t timeout, const char *dest )
{
  if (src == nullptr)
    return -1 ;

  GOStateNode     *node = get_node( src ) ;
  if (node == nullptr)
    return -2 ;

  node->add_timeout_transition( timeout, dest ) ;
  return 0 ;
} // GOStateMachine :: add_timeout

HTIMER GOStateMachine :: add_timeout_action( const char *src, int32_t timeout, Callback *cb, bool one_shot ) 
{
  if ((src == nullptr) || (cb == nullptr))
    return -1 ;

  GOStateNode     *node = get_node( src ) ;
  if (node == nullptr)
    return -2 ;

  return node->timer().notify( timeout, cb, one_shot ) ;
} // GOStateMachine :: add_timeout_action

int16_t GOStateMachine :: add_transition( const char *src, int32_t event_id, const char *dest )
{
  if ((src == nullptr) || (dest == nullptr))
    return -1 ;

  MutexScope       sc( _states.gate() ) ;
  GOStateNode     *src_node = get_node( src ) ;
  if (src_node == nullptr)
  {
    return -2 ; // unable to find src-node
  }

  GOStateNode     *dest_node = get_node( dest ) ;
  if (dest_node == nullptr)
  {
    return -3 ; // unable to find dest-node
  }

  src_node->add_transition( event_id, dest ) ;
  return 0 ;
} // GOStateMachine :: add_transition

GOStateNode *GOStateMachine :: current()
{
  return _active ; 
} // GOStateMachine :: current

MsgCB *GOStateMachine :: event( const char *state_name, uint32_t event_id ) 
{
  if (state_name == nullptr)
    return nullptr ;
  return get_node( state_name )->inStateEvents().event( event_id ) ;
} // GOStateMachine :: event

GOStateNode *GOStateMachine :: get_node( const char *node_name ) 
{
  if (node_name == nullptr)
    return nullptr ;

  MutexScope       sc( _states.gate() ) ;
  GOStateNode     *node = find_node( node_name ) ;
  if (node != nullptr)
    return node ;
  add_state( node_name ) ;
  return find_node( node_name ) ;
} // GOStateMachine :: get_node

void GOStateMachine :: on_enter( const char *state_name, Callback *cb ) 
{
  if ((state_name == nullptr) || (cb == nullptr))
    return ;

  get_node( state_name )->enterCB().install( cb ) ;
} // GOStateMachine :: on_enter

void GOStateMachine :: on_event( const char *state_name, uint32_t event_id, Callback *cb ) 
{
  if ((state_name == nullptr) || (cb == nullptr))
    return ;

  get_node( state_name )->inStateEvents().event( event_id )->install( cb ) ;
} // GOStateMachine :: on_event

void GOStateMachine :: on_leave( const char *state_name, Callback *cb ) 
{
  if ((state_name == nullptr) || (cb == nullptr))
    return ;
  get_node( state_name )->leaveCB().install( cb ) ;
} // GOStateMachine :: on_leave

int16_t GOStateMachine :: process( int32_t event_id, int32_t payload_sz, void *payload )
{
  if (_active == nullptr)
    return -1 ;
  int16_t rc = _active->process( event_id, payload_sz, payload ) ;
  if (rc == -2)
  {
    _invalidEventCB.invoke( ArgList() << event_id << (void*)_active << (void*)this << payload_sz << payload ) ;
  }
  return rc ;
} // GOStateMachine :: process

int16_t GOStateMachine :: remove_state( const char *state_name )
{
  if (state_name == nullptr)
    return -1 ;

  MutexScope       sc( _states.gate() ) ;
  StateMap_iter    iter = _states.find( state_name ) ;
  if (iter == _states.end())
    return 0 ; // not found - equivalent to removed

  // found. disconnect from diagram and clean up
  GOStateNode     *node = (*iter).second ;
  delete node->enterCB().remove( this ) ;
  delete node->leaveCB().remove( this ) ;
  _states.erase( iter ) ;
  delete node ;

  return 0 ;
} // GOStateMachine :: remove_state

int16_t GOStateMachine :: remove_timeout_action( const char *state_name, HTIMER hTimer ) 
{
  if ((state_name == nullptr) || (hTimer == NULL))
    return -1 ;

  GOStateNode     *node = get_node( state_name ) ;
  if (node == nullptr)
    return -2 ;

  return node->timer().unnotify( hTimer ) ;
} // GOStateMachine :: remove_timeout_action

void GOStateMachine :: remove_timeout_transition( const char *src ) 
{
  if (src == nullptr)
    return ;

  GOStateNode     *node = find_node( src ) ;
  if (node != nullptr)
    node->remove_timeout_transition() ;
} // GOStateMachine :: remove_timeout_transition

int16_t GOStateMachine :: remove_transition( const char *src, int32_t event_id, const char *dest )
{
  if ((src == nullptr) || (dest == nullptr))
    return -1 ;

  GOStateNode  *node = find_node( src ) ;
  if (node == nullptr)
    return -2 ;

  return node->remove_transition( event_id, dest ) ;
} // GOStateMachine :: remove_transition

int16_t GOStateMachine :: set_active( const char *state_name, int32_t payload_sz, void *payload ) 
{
  if (_active != nullptr)
  {
    _active->set_active( false, payload_sz, payload ) ;
    _active = nullptr ;
  }

  _active = find_node( state_name ) ;
  if (_active != nullptr)
  {
    _active->set_active( true, payload_sz, payload ) ;
  }
  return (_active == nullptr) ? -1 : 0 ;
} // GOStateMachine :: set_active

} // namespace go 
