/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       grset.h
  @brief      GRSet template definition

  @author     rmcinnis@solidice.com
  @date       april 18, 2007
  @par        copyright (c) 2007 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GRSET_H
#define GRSET_H

#include <set>
#include <msgcb.h>
#include <mutexscope.h>
#include <go_semaphoregate.h>

#if !defined(__STL_NO_NAMESPACES)
using namespace std;
#endif

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
template <class Value, class Compare = less<Value> >
class GRSet : public set< Value, Compare >
{
  protected:
  typedef set< Value, Compare > _Parent;
//  typedef typename set< Value, Compare > this_container;
  typedef typename set< Value, Compare >::iterator set_iter ;
  typedef typename std::set<Value, Compare>::size_type size_type;
  typedef Value key_type ;  
  MsgCB                 _preEraseCB;
  MsgCB                 _postInsertCB;
  MutexGate             _gate;
  set_iter              _current;
  SemaphoreGate         _resources;

  public:
                        GRSet() 
                        : _preEraseCB(this), _postInsertCB(this), _resources(0) 
                        {}
                       ~GRSet() 
                        {}
  MutexGate            &gate() { return _gate ; }
  int32_t                  cnt() { return _resources.cnt() ; }
  MsgCB                &preEraseCB() { return _preEraseCB ; }
  MsgCB                &postInsertCB() { return _postInsertCB ; }
  set_iter             &current() { return _current ; }
  
  pair<set_iter, bool>  insert(const Value &obj) 
                        {
                          MutexScope  sc(_gate) ;
                          pair<set_iter, bool> insert_result = _Parent::insert(obj);
                          _current = insert_result.first;
                          if( insert_result.second ) 
                          {
                            _postInsertCB.invoke() ;
                          } 
                          _resources.post() ;
                          return insert_result ;
                        }

  size_type             erase(const key_type &key) 
                        {
                          MutexScope  sc(_gate) ;
                          _current = find(key);
                          if( _Parent::end() == _current )
                            return( 0 );
                          _preEraseCB.invoke() ;
                          _Parent::erase(_current);
                          if (_resources.cnt() > _Parent::size())
                            _resources.Wait() ;
                          return 1 ;
                        }
  void                  erase(set_iter it)
                        {
                          MutexScope  sc(_gate) ;
                          if( _Parent::end() == it ) return;
                          _current = it; // con
                          _preEraseCB.invoke() ;
                          _Parent::erase(it); 
                          if (_resources.cnt() > _Parent::size())
                            _resources.Wait() ;
                        }

  void                  erase(set_iter f, set_iter l)
                        {
                          MutexScope  sc(_gate) ;
                          for( _current = f; _current != l; _current++ )
                          {
                            _preEraseCB.invoke() ;
                            _Parent::erase(_current); 
                          }
                          _resources.Wait( _resources.cnt() - _Parent::size() ) ;
                        }

  void                  pop( Value & _X)
                        {
                          _resources.Wait() ;
                          MutexScope sc(_gate) ;
                          _X = *_Parent::begin() ;
                          erase( _Parent::begin() ) ;
                        }

} ; // template GRSet

} // namespace go 

#if defined(_MSC_VER)
#  pragma warning( default : 4355 )
#endif

#endif
