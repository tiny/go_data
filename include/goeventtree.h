/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goeventtree.h
  @brief      GOEventTree class definition

  @author     rmcinnis@solidice.com
  @date       december 30, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOEVENTTREE_H
#define GOEVENTTREE_H

#include <msgcb.h>
#include <gomap.h>

namespace go {

/*!
  @class GOEventNode
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       december 30, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOEventNode
{
  private :
    uint32_t          _evt ;
    MsgCB          _verify ;
    MsgCB          _cb ;
                       
  public  :         
                   GOEventNode ( void *from, uint32_t evt )
                   : _verify( from ),  _cb ( from )
                   { _evt = evt ; }
                   GOEventNode ( const GOEventNode &e ) { *this = e ; }
    bool           operator<( const GOEventNode &e ) { return (_evt < e._evt) ; }
    GOEventNode   &operator=( const GOEventNode &e ) 
                   { _evt = e._evt ; _verify = e._verify ; _cb = e._cb ; return *this ; }

    // access methods
    uint32_t         &evt() { return _evt ; }
    MsgCB         &verify() { return _verify ; }
    MsgCB         &cb() { return _cb ; }
} ; // class GOEventNode

typedef GOMap< uint32_t, GOEventNode >              GOEventNodeMap ;
typedef GOMap< uint32_t, GOEventNode >::iterator    GOEventNodeMap_iter ;
typedef GOMap< uint32_t, GOEventNode >::value_type  GOEventNodeMap_pair ;

/*!
  @class GOEventTree
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       december 30, 1996
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOEventTree
{
  private  :
    GOEventNodeMap  _evtmap ;
    MsgCB           _postEveryCB ; // args: evt-id, args
    MsgCB           _preEveryCB  ; // args: evt-id, args
    
    void           *_originator ;

    GOEventNode    *createNode ( uint32_t evt ) ;
    GOEventNode    *findNode ( uint32_t evt ) ;
    GOEventNode    *insert ( GOEventNode *eN, uint32_t evt ) ;
    void            deleteNode ( GOEventNode *eN ) ;

  public   :      
                    GOEventTree (void *origin) ;
                   ~GOEventTree () ;
    MsgCB          *event ( uint32_t evt ) ; // finds & returns.  If not found, creates node
    MsgCB          *findEvent ( uint32_t evt ) ;
    int16_t           invoke( uint32_t evt, ArgList *args = nullptr ) ; // return -1 == evt not found
    bool            ok2invoke( uint32_t evt ) ; // 0 == ok to invoke
    MsgCB          *verify ( uint32_t evt ) ; // finds & returns.  If not found, creates node

    // access methods
    MutexGate      &gate() { return _evtmap.gate() ; }
    MsgCB          &postEveryCB() { return _postEveryCB ; }
    MsgCB          &preEveryCB() { return _preEveryCB ; }
} ; // class GOEventTree

} // namespace go 

#endif

