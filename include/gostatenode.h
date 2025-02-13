/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gostatenode.h
  @brief      GOStateNode class definition

  @author     rmcinnis@solidice.com
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSTATENODE_H
#define GOSTATENODE_H

#include <gostring.h>
#include <gomap.h>
#include <gotimer.h>
#include <goeventtree.h>

namespace go {

typedef GOMap< int32_t, GOString >             TransitionMap ;
typedef GOMap< int32_t, GOString >::iterator   TransitionMap_iter ;
typedef GOMap< int32_t, GOString >::value_type TransitionMap_pair ;

/*!
  @class GOStateNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       september 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOStateNode
{
  protected :
    GOTimer             _timer ;
    HTIMER              _hTimer ;
    GOString            _name ;
    bool                _isActive ;
    TransitionMap       _transitions ;
    bool                _hasTimeout ;
    int32_t                _timeOut ; // timeout in milliseconds
    int32_t                _token ;   // on timeout, token must match - could leave state then return before 1st timeout happens
    GOString            _timeout_dest ; // name of state to move to when timeout expires
    ArgList             _args ;
    GOEventTree         _inStateEvents ; // args: this-state-name, this, payload-sz, payload, result_msgid
    MsgCB               _enterCB ; // args: this-state-name, this, payload-sz, payload
    MsgCB               _leaveCB ; // args: this-state-name, new-state-name, this, payload-sz, payload

    virtual void        onTimeout( int32_t token ) ;

  public    :
                        GOStateNode( const char *name ) ;
                        GOStateNode( const GOStateNode &i ) ;
    virtual            ~GOStateNode() ;

    virtual int16_t       add_timeout_transition( int32_t timeout, const char *dest ) ; // timeout in milliseconds - only 1 timeout event
    virtual int16_t       add_transition( int32_t event_id, const char *dest ) ;
    virtual int16_t       process( int32_t event_id, int32_t payload_sz, void *payload ) ;
    virtual void        remove_timeout_transition() ;
    virtual int16_t       remove_transition( int32_t event_id, const char *dest ) ;
    virtual void        set_active( bool isActive, int32_t payload_sz, void *payload ) ; // will trigger callbacks on state transition

    // access methods
    GOTimer            &timer() { return _timer ; }
    GOString           &name() { return _name ; }
    bool                isActive() const { return _isActive ; }
    GOEventTree        &inStateEvents() { return _inStateEvents ; }
    MsgCB              &enterCB() { return _enterCB ; }
    MsgCB              &leaveCB() { return _leaveCB ; }
} ; // class GOStateNode

typedef GOMap< GOString, GOStateNode* >             StateMap ;
typedef GOMap< GOString, GOStateNode* >::iterator   StateMap_iter ;
typedef GOMap< GOString, GOStateNode* >::value_type StateMap_pair ;

} // namespace go 

#endif

