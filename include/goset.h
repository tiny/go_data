/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       goset.h
  @brief      GOSet template definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSET_H
#define GOSET_H

#include <set>
#include <msgcb.h>
#include <mutexscope.h>

#if defined(_MSC_VER)
#  pragma warning( disable : 4355 )
#endif

namespace go {

/*!
  @class GOSet< Value, Compare >
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T, class Compare = std::less<T> >
class GOSet : public std::set< T, Compare >
{
  public:
  typedef typename std::set<T,Compare>::iterator   iterator ;
  typedef typename std::set<T,Compare>::size_type  size_type ;

  protected:
//  typedef typename set< T, Compare > this_container;
//  typedef typename set< T, Compare >::iterator set_iter ;

  MsgCB                 _preEraseCB;
  MsgCB                 _postInsertCB;
  MutexGate             _gate;

  public:
                        GOSet() 
                        : _preEraseCB(this), _postInsertCB(this) 
                        {}
                       ~GOSet() 
                        {}
  MutexGate            &gate() { return _gate ; }
  MsgCB                &preEraseCB() { return _preEraseCB ; }
  MsgCB                &postInsertCB() { return _postInsertCB ; }
  
  bool                  insert(const T &obj) 
                        {
                          MutexScope  sc(_gate) ;
//                          pair<iterator, bool> insert_result = set<T, Compare>::insert(obj);
                          auto insert_result = std::set<T,Compare>::insert(obj);
                          if( insert_result.second ) 
                          {
                            _postInsertCB.invoke() ;
                          } 
                          return insert_result.second ;
                        }

  size_type             erase(const T & key) 
                        {
                          MutexScope  sc(_gate) ;
//                          iterator it = this->find(key);
                          auto it = this->find(key);
                          if( this->end() == it )
                            return( 0 );
                          _preEraseCB.invoke() ;
                          std::set< T, Compare >::erase( (*it) );
                          return 1 ;
                        }

  void                  erase(iterator it)
                        {
                          MutexScope  sc(_gate) ;
                          if( this->end() == it ) return;
                          _preEraseCB.invoke() ;
                          std::set< T, Compare >::erase(it);
                        }

  void                  erase(iterator f, iterator l)
                        {
                          MutexScope  sc(_gate) ;
                          for(auto it = f; it != l; it++ )
                          {
                            _preEraseCB.invoke() ;
                            std::set< T, Compare >::erase( it );
                          }
                        }
} ; // template GOSet

} // namespace go 

#if defined(_MSC_VER)
#  pragma warning( default : 4355 )
#endif

#endif
