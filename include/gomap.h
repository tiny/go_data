/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gomap.h
  @brief      GOMap template definition

  @author     rmcinnis@solidice.com
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <map>
#include <msgcb.h>
#include <mutexscope.h>

#ifdef _MSC_VER
#  pragma warning(disable: 4355)
#endif

#if !defined(__STL_NO_NAMESPACES)
using namespace std;
#endif

namespace go {

/*!
  @class GOMap<Key, Value, _Pr>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       january 12, 1998
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class Key, class Value, class _Pr = less<Key> >
class GOMap : public map< Key, Value, _Pr >
{
  protected:
  //    typedef typename GOMap< Key, Value, _Pr >             gomap_type ;
    typedef typename GOMap<Key,Value,_Pr>::iterator       gomap_iter ;
    typedef typename GOMap< Key, Value, _Pr >::value_type gomap_pair ;
    MsgCB               _preEraseCB;
    MsgCB               _postInsertCB;
    MutexGate           _gate;
    gomap_iter          _current;

  public:
                        GOMap() 
                        : _preEraseCB(this), _postInsertCB(this) 
                        {}
                        GOMap( const GOMap &other_ )
                        : _preEraseCB(this), _postInsertCB(this) 
                        {
                          *this = other_;
                        }
                       ~GOMap() 
                        {}
  GOMap                &operator=( const GOMap &rhs_ ) 
                        { GOMap &other = const_cast<GOMap&>(rhs_);
                          MutexScope sc(other._gate);
                          MutexScope sc2(_gate);
                          this->clear();
                          typename map<Key,Value>::const_iterator  iter ;
                          for (iter = rhs_.begin(); iter != rhs_.end(); iter++)
                            map<Key,Value>::insert( pair<Key, Value>( (*iter).first, (*iter).second )) ;
                          return( *this );
                        }

  MutexGate            &gate() { return( _gate ); }
  MsgCB                &preEraseCB() { return( _preEraseCB ); }
  MsgCB                &postInsertCB() { return( _postInsertCB ); }
  gomap_iter           &current() { return( _current ); }
  
  pair<gomap_iter, bool>  update(const gomap_pair &obj)
                        {
                          MutexScope  sc( _gate ) ;

                          _current = this->find(obj.first);
                          // if not already in the map, then only insert
                          if( this->end() == _current ) 
                          {
                            pair<gomap_iter, bool> insert_result = map<Key,Value>::insert(obj);
                            _current = insert_result.first;
// how did this ever work?
//                            _postInsertCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                            _postInsertCB.invoke( ArgList() << (void*)this ) ;
                            return( insert_result );
                          }
                          // erase current iter in preparation for insert.
                          ArgList args ;
                          args << (void *)this;
                          _preEraseCB.invoke(args);
                          map<Key,Value>::erase(_current);
                          pair<gomap_iter, bool> insert_result = map<Key,Value>::insert(obj);
                          _current = insert_result.first;
// how did this ever work?
//                          _postInsertCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                          _postInsertCB.invoke( ArgList() << (void*)this ) ;
                          return insert_result ;
                        }

  pair<gomap_iter, bool>  insert(const gomap_pair& obj)
                        {
                          MutexScope  sc( _gate ) ;
                          pair<gomap_iter, bool> insert_result = map<Key,Value>::insert(obj);
                          _current = insert_result.first;
                          if( insert_result.second ) 
                          {
// how did this ever work?
//                            _postInsertCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                            _postInsertCB.invoke( ArgList() << (void*)this ) ;
                          }
                          return insert_result ;
                        }

  pair<gomap_iter, bool>  insert( gomap_iter pos, const gomap_pair& obj)
                        {
                          MutexScope  sc( _gate ) ;
                          pair<gomap_iter, bool> insert_result = map<Key,Value>::insert(pos,obj);
                          _current = insert_result.first;
                          if( insert_result.second ) 
                          {
// how did this ever work?
//                            _postInsertCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                          _postInsertCB.invoke( ArgList() << (void*)this ) ;
                          } 
                          return insert_result ;
                        }

  size_t                erase(const Key & key) 
                        {
                          MutexScope  sc( _gate ) ;
                          _current = find(key);
                          if( this->end() == _current )
                          { 
                            return( 0 ); 
                          }
// how did this ever work?
//                          _preEraseCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                          _preEraseCB.invoke( ArgList() << (void*)this ) ;
                          map<Key,Value>::erase(_current);
                          return( 1 );
                        }

  void                  erase(gomap_iter it)
                        {
                          MutexScope  sc( _gate ) ;
                          if( this->end() == it )
                          { 
                            return; 
                          }
                          _current = it; // con
                          ArgList args ;
                          args << (void *)this;
// how did this ever work?
//                          _preEraseCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                          _preEraseCB.invoke( args ) ;
                          map<Key,Value>::erase(it); 
                        }

  void                  erase(gomap_iter f, gomap_iter l)
                        {
                          MutexScope  sc( _gate ) ;
                          for( _current = f; _current != l; _current++ )
                          {
// how did this ever work?
//                            _preEraseCB.invoke( ArgList() << (void*)(*_current).first << (void*)this ) ;
                            _preEraseCB.invoke( ArgList() << (void*)this ) ;
                            map<Key,Value>::erase(_current); 
                          }
                        }
} ; // template GOMap

} // namespace go 
