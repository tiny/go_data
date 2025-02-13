/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostatemachine.h
  @brief      GOStateMachine class definition

  @author     rmcinnis@solidice.com
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSTATEMACHINE_H
#define GOSTATEMACHINE_H

#include <gostatenode.h>

namespace go {

/*!
  @class GOStateMachine
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOStateMachine
{
  protected :
    GOString                 _name ;
    StateMap                 _states ;
    GOStateNode             *_active ;
    MsgCB                    _invalidEventCB ; // args: event-id, current-state-node, this, payload_sz, payload
    MsgCB                    _newStateCB ; // args: new-state-name, new-state-node, this, payload_sz, payload

    virtual GOStateNode     *find_node( const char *node_name ) ;
    virtual int16_t            onStateEnterCB( const ArgList *args ) ;
    virtual int16_t            onStateLeaveCB( const ArgList *args ) ;

  public    :
                             GOStateMachine( const char *name ) ;
                             GOStateMachine( const GOStateMachine &i ) ;
    virtual                 ~GOStateMachine() ;

    virtual int16_t            add_state( const char *state_name ) ;
    virtual int16_t            add_state_action( const char *state_name, uint32_t event_id, Callback *cb ) ; 
    virtual int16_t            add_timeout( const char *src, int32_t timeout, const char *dest ) ; // timeout in milliseconds - only 1 timeout event
    virtual HTIMER           add_timeout_action( const char *src, int32_t timeout, Callback *cb, bool one_shot = false ) ; // timeout in milliseconds - timeout actions
    virtual int16_t            add_transition( const char *src, int32_t event_id, const char *dest ) ;
    virtual GOStateNode     *current() ;
    virtual MsgCB           *event( const char *state_name, uint32_t event_id ) ;
    virtual GOStateNode     *get_node( const char *node_name ) ; // creates node if not found
    virtual void             on_enter( const char *state_name, Callback *cb ) ;
    virtual void             on_event( const char *state_name, uint32_t event_id, Callback *cb ) ;
    virtual void             on_leave( const char *state_name, Callback *cb ) ;
    virtual int16_t            process( int32_t event_id, int32_t payload_sz, void *payload ) ;
    virtual int16_t            remove_state( const char *state_name ) ;
    virtual int16_t            remove_timeout_action( const char *state_name, HTIMER hTimer ) ;
    virtual void             remove_timeout_transition( const char *src ) ;
    virtual int16_t            remove_transition( const char *src, int32_t event_id, const char *dest ) ;
    virtual int16_t            set_active( const char *state_name, int32_t payload_sz = 0, void *payload = nullptr ) ; 

    // access methods
    GOString                &name() { return _name ; }
    StateMap                &states() { return _states ; }
    MsgCB                   &invalidEventCB() { return _invalidEventCB ; }
    MsgCB                   &newStateCB() { return _newStateCB ; }
} ; // class GOStateMachine

} // namespace go 

#endif
