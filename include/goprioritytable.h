/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goprioritytable.h
  @brief      GOPriorityTable class definition

  @author     rmcinnis@solidice.com
  @date       december 30, 1993
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOPRIORITYTABLE_H
#define GOPRIORITYTABLE_H

#include <goset.h>

namespace go {

/*!
  @class CBNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class CBNode
{
  private :
    int16_t     _lvl ;
    MsgCB     _cb ;

  public  : 
              CBNode( int16_t lvl, void *src ) : _cb( src ) { _lvl = lvl ; }
              CBNode( const CBNode &c ) : _cb( c._cb ) { _lvl = c._lvl ; }
    CBNode   &operator=( const CBNode &c ) { _lvl = c._lvl; _cb = c._cb ; return *this ; }
    bool      operator<( const CBNode &c ) { return (_lvl < c._lvl) ; }

    // access methods
    int16_t    &lvl() { return _lvl ; }
    int16_t     lvl() const { return _lvl ; }
    MsgCB    &cb() { return _cb ; }
} ; // class CBNode

// sort by display level
struct CBNodeSort : std::binary_function< CBNode *, CBNode *, bool >
{
  bool operator()( const CBNode *lhs, const CBNode *rhs ) const 
       { if (lhs == nullptr) return true ;
         if (rhs == nullptr) return false ;
         return (lhs->lvl() < rhs->lvl()) ;
       }
} ; // struct CBNode

typedef GOSet< CBNode*, CBNodeSort >              CBNodeSet ;
typedef GOSet< CBNode*, CBNodeSort >::iterator    CBNodeSet_iter ;

/*!
  @class GOPriorityTable
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class _EXPORT GOPriorityTable
{
  private  :
    CBNodeSet      _mlmap ;
    bool           _enabled ;
    void          *_originator ;

    CBNode        *findValue( int16_t lvl ) ;

  public   :      
                   GOPriorityTable (void *origin) ; // creates a default level (0)
                   GOPriorityTable ( const GOPriorityTable &m ) { *this = m ; }
                  ~GOPriorityTable () ;

    bool           changeLevel ( void *who, int32_t lvl ) ;
    bool           changeLevelRel ( void *who, int32_t dLevel ) ;
    MsgCB         *findLevel ( int32_t lvl ) ;
    Callback      *install ( void *who, MsgFunc msg, int32_t lvl = 0 ) ;
    Callback      *install ( Callback *c, int32_t lvl = 0 ) ;
    void           invoke () ; // invokes all levels in their numeric order
    void           invoke ( const ArgList &args ) ; // invokes all levels in their numeric order
    MsgCB         *level ( int32_t lvl ) ; // finds & returns.  If not found, creates node
    void           remove ( void *who ) ; // removes any callback at any level attached to "who"

    // operator overloads
    GOPriorityTable  &operator=( const GOPriorityTable &m ) 
                   { _mlmap = m._mlmap ; _enabled = m._enabled ; _originator = m._originator ; return *this ; }

    // access methods
    bool          &enabled() { return _enabled ; }
} ; // class GOPriorityTable

} // namespace go 

#endif
