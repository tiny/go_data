/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goeventtree.cpp
  @brief      GOEventTree class implementation

  @author     rmcinnis@solidice.com
  @date       december 30, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <goeventtree.h>

namespace go {

GOEventNode *GOEventTree :: findNode ( uint32_t evt )
{
  GOEventNodeMap_iter  iter = _evtmap.find( evt ) ;
  return (iter == _evtmap.end()) ? nullptr : &(*iter).second ;
} // GOEventTree :: findNode
      
GOEventTree :: GOEventTree (void *origin)
: _postEveryCB( origin )
, _preEveryCB( origin )
{
  _originator = origin ;
} // GOEventTree [constructor]

GOEventTree :: ~GOEventTree ()
{
} // GOEventTree [destructor]

MsgCB *GOEventTree :: event ( uint32_t evt )
{
  MsgCB *m = findEvent ( evt ) ;

  if (m != nullptr)
    return m ;

  MutexScope  sc( gate() ) ;
  _evtmap.insert( GOEventNodeMap_pair( evt, GOEventNode( _originator, evt )) ) ;
  return findEvent ( evt ) ;
} // GOEventTree :: event

MsgCB *GOEventTree :: findEvent ( uint32_t evt )
{
  GOEventNode *eN = findNode( evt ) ;

  return ((eN == nullptr) ? (MsgCB*)nullptr : &eN->cb()) ;
} // GOEventTree :: findEvent

int16_t GOEventTree :: invoke( uint32_t evt, ArgList *args ) 
{
  if (_preEveryCB.nWatchers() != 0)
    _preEveryCB.invoke( ArgList() << (int32_t)evt << (void*)args ) ;

  MsgCB  *cb = findEvent( evt ) ;
  if ((cb != nullptr) && (args != nullptr))
    return (int16_t)cb->invoke( *args ) ;
  else if (cb != nullptr)
    return (int16_t)cb->invoke() ;

  if (_postEveryCB.nWatchers() != 0)
    _postEveryCB.invoke( ArgList() << (int32_t)evt << (void*)args ) ;

  return -1 ;
} // GOEventTree :: invoke

bool GOEventTree :: ok2invoke( uint32_t evt )
{
  GOEventNode *eN = findNode( evt ) ;

  return (eN == nullptr) ? false : (eN->verify().invoke() == 0) ;
} // GOEventTree :: ok2invoke

MsgCB *GOEventTree :: verify ( uint32_t evt )
{
  GOEventNode *node = findNode( evt ) ;

  if (node == nullptr)
  {
    MutexScope  sc( gate() ) ;
    _evtmap.insert( GOEventNodeMap_pair( evt, GOEventNode( _originator, evt )) ) ;
    node = findNode( evt ) ;
  }
  return (node == nullptr) ? nullptr : &node->verify() ;
} // GOEventTree :: verify

} // namespace go 
