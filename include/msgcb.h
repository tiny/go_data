/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       msgcb.h
  @brief      MsgCB class definition

  @author     rmcinnis@solidice.com
  @date       march 23, 1992
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef MSGCB_H
#define MSGCB_H

//------------------------------------------------------------------------
#include "callback.h"
#include "mutexscope.h"

namespace go {

//#define  CBMUTEX   FastGate
#define  CBMUTEX   MutexGate

//------------------------------------------------------------------------------
/*!
  @class MsgCB
   
  <b>Description:</b>
   
  <b>Notes:</b>
  a message callback is made up of multiple message nodes.  A node can be added,
  removed, enabled, or disabled.  The actual node beint32_ts only to the callback, the
  object the node refers to cannot touch it.
  The node chain is a fifo list
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class MsgCB
{
    private  :
      int16_t              _block ;        // count of blocks - trigger when first hits 0
      bool               _needToInvoke ; // true when blocked, then tripped by invoke
      bool               _isCopy ;
      bool               _isDead ;       // useful for globals that go out of scope(protection mechanism)
      void              *_srcObj   ;     // who was the originator of the msgs

      // use variant array instead of template list for speed and portability
      CBMUTEX            _cbGate ;
      Callback         **_cbArray ;
      uint16_t             _cbArraySz ;
      uint16_t             _cbWatchers ;

    protected :
      void               cb_clear() ;
      Callback          *cb_erase( uint16_t ndx ) ;
      void               cb_grow( uint16_t delta ) ;
      Callback          *cb_pushback( Callback *c ) ;
      virtual void       clearList() ;
      virtual Callback  *factoryCopy( Callback *cb ) ;

    public   :
                         MsgCB ( void *srcObj = nullptr ) ;
                         MsgCB ( const MsgCB &cb ) ;
      virtual           ~MsgCB () ;

      void               block() ; // disables callback
      Callback          *get ( void *destObj ) ;
      Callback          *install ( void *destObj, MsgFunc msg ) ;
      Callback          *install ( Callback *c ) ;
      int32_t                invoke ( PBVParameter *arg1, PBVParameter *arg2=nullptr ) ; // makes the list for you 
      int32_t                invoke () ;
      int32_t                invoke ( const ArgList &args ) ;
      inline size_t      nWatchers() const { return _cbWatchers ; }
      Callback          *remove ( void *destObj, int16_t nukeNode=true ) ;
      Callback          *remove ( MsgCB &cb ) ;
      Callback          *remove ( void *srcObj, void *destObj, bool nukeNode=true ) ;
      void               srcObj( void *sObj ) { _srcObj = sObj ; } // be VERY careful with this
      Callback          *trigger( PokeFunc msg ) ;
      Callback          *trigger( Callback *c ) ;
      Callback          *trigger( MsgCB &cb ) ;
      int32_t                unblock() ; // enables callback && triggers
      int32_t                unblock( const ArgList &args ) ;

      // assignment operator
      MsgCB             &operator=( const MsgCB &cb ) ;

      // access methods
      inline bool        enabled() const { return (_block == 0) ; }
      void              *srcObj() { return _srcObj ; }
} ; // class MsgCB

//------------------------------------------------------------------------
/*!
  @class MsgTriggerNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       october 08, 1994
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class MsgTriggerNode : public Callback
{
  private  :
    MsgCB         *_cb ;

  public   :
                   MsgTriggerNode( MsgCB *cb ) ;
    int32_t            invoke() ;
    int32_t            invoke( const ArgList &args ) ;

    // access methods
    virtual void  *destObj() const { return _cb ; }
} ; // class MsgTriggerNode

} // namespace go 

#endif


