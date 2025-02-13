/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goprioritytable.cpp
  @brief      GOPriorityTable class implementation

  @author     rmcinnis@solidice.com
  @date       december 30, 1993
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <goprioritytable.h>

namespace go {

CBNode *GOPriorityTable :: findValue( int16_t lvl ) 
{
  for (auto x : _mlmap)
  {
    if (x->lvl() == lvl)
      return x ;
  }
  return nullptr ;
} // GOPriorityTable :: findValue

GOPriorityTable :: GOPriorityTable (void *origin)
{
  _enabled = true ;
  _originator = origin ;
  _mlmap.insert( new CBNode( (int16_t)0, origin ) ) ;
} // GOPriorityTable [constructor]

GOPriorityTable :: ~GOPriorityTable ()
{
} // GOPriorityTable [destructor]

bool GOPriorityTable :: changeLevel ( void *who, int32_t lvl )
{
  Callback        *node ;
  MutexScope       sc( _mlmap.gate() ) ;

  for (auto x : _mlmap) {
    node = x->cb().get( who ) ;
    if (node != nullptr) {  // found it
      node = x->cb().remove( who ) ;
      install( node, lvl ) ;   // place it at the other level
      return true ;
    }
  }
  return false ;
} // GOPriorityTable :: changeLevel

bool GOPriorityTable :: changeLevelRel ( void *who, int32_t dLevel )
{
  Callback        *node ;
  MutexScope       sc( _mlmap.gate() ) ;

  for (auto x : _mlmap) {
    node = x->cb().get( who ) ;
    if (node != nullptr) {  // found it
      node = x->cb().remove( who ) ;
      install( node, x->lvl() + dLevel ) ;   // place it at the other level
      return true ;
    }
  }
  return false ;
} // GOPriorityTable :: changeLevelRel

MsgCB *GOPriorityTable :: findLevel ( int32_t lvl )
{
  CBNode  *node = findValue( (int16_t)lvl ) ;
  return (node == nullptr) ? nullptr : &node->cb() ;
} // GOPriorityTable :: findLevel

Callback *GOPriorityTable :: install ( void *who, MsgFunc msg, int32_t lvl )
{
  CBNode  *node = findValue( (int16_t)lvl ) ;
  if (node == nullptr) 
  {
    _mlmap.insert( new CBNode( (int16_t)lvl, _originator ) ) ;
    node = findValue( (int16_t)lvl ) ;
  }

  return node->cb().install( who, msg ) ;
} // GOPriorityTable :: install

Callback *GOPriorityTable :: install ( Callback *c, int32_t lvl )
{
  if (c == nullptr)
    return nullptr ;

  CBNode  *node = findValue( (int16_t)lvl ) ;
  if (node == nullptr) 
  {
    _mlmap.insert( new CBNode( (int16_t)lvl, _originator ) ) ;
    node = findValue( (int16_t)lvl ) ;
  }

  return node->cb().install( c ) ;
} // GOPriorityTable :: install

MsgCB *GOPriorityTable :: level ( int32_t lvl )
{
  CBNode  *node = findValue( (int16_t)lvl ) ;
  if (node == nullptr) 
  {
    _mlmap.insert( new CBNode( (int16_t)lvl, _originator ) ) ;
    node = findValue( (int16_t)lvl ) ;
  }

  return &node->cb() ;
} // GOPriorityTable :: level

// invokes all levels in their numeric order
void GOPriorityTable :: invoke ()
{
  if (!enabled())
    return ;

  MutexScope       sc( _mlmap.gate() ) ;
  for (auto x : _mlmap) {
    x->cb().invoke() ;
  }
} // GOPriorityTable :: invoke

void GOPriorityTable :: invoke ( const ArgList &args )
{
  if (!enabled())
    return ;

  MutexScope       sc( _mlmap.gate() ) ;
  for (auto x : _mlmap) {
    x->cb().invoke( args ) ;
  }
} // GOPriorityTable :: invoke

// removes any callback at any level attached to "who"
void GOPriorityTable :: remove ( void *who )
{
  MutexScope       sc( _mlmap.gate() ) ;
  for (auto x : _mlmap) {
    delete x->cb().remove( who ) ;
  }
} // GOPriorityTable :: remove

} // namespace go 
