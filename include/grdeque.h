/*
  grdeque.h
  guarded resource deque template definition

  © 2024 Robert McInnis <r_mcinnis@solidice.com>
*/
#pragma once
#ifndef GRDEQUE_H
#define GRDEQUE_H

#include <deque>
#include "go_semaphore.h"

namespace go {

 typedef std::lock_guard<std::mutex >   LockScope ;

  template<class _Value >
  class grdeque : public std::deque< _Value > 
  {
  private:
      typedef grdeque< _Value > gdeque_type;
      typedef typename std::deque< _Value >::iterator gdeque_iter;
      typedef typename std::deque<_Value>::size_type  gdeque_size_type;
      
    typedef std::deque< _Value > _Parent;
    typedef grdeque< _Value > _TGDeque;
    std::mutex        _mtx;
    semaphore         _sem;

  public:
    typedef typename std::deque<_Value>::value_type  gdeque_value_type ;
    typedef std::lock_guard<std::mutex > _Scope;

    std::mutex & mtx() { return _mtx ; }
    semaphore &sem() { return _sem ; }

    grdeque() :_sem(0){}
    grdeque(gdeque_size_type _N ) : _Parent( _N ), _sem(0){}
    grdeque( _TGDeque& _X) : _sem(0)
    {
      _TGDeque &other = const_cast<_TGDeque &>(_X);
      _Scope sc1( other._mtx );
      _Parent::insert( this->begin(), other.begin(), other.end() );
      _sem.post( _Parent::size() );
    }
    virtual ~grdeque() {}

    _TGDeque &operator=( const _TGDeque &cother_ ) {
      _TGDeque &other = const_cast<_TGDeque &>(cother_);
      _Scope sc1( other._mtx );
      _Scope sc2( _mtx );
      _Parent::clear();
      _sem.clear();
      _Parent::insert( this->begin(), other.begin(), other.end() );
      _sem.post( _Parent::size() );
      return( *this );
    }
    private:
    void reserve(gdeque_size_type _N) {
      _Scope sc(_mtx);
      _Parent::reserve( _N );
    }
    void resize(gdeque_size_type _N ){
      _Scope sc(_mtx);
      _Parent::resize( _N );
    }
    public:
    void push_front(const _Value& _X){
      _Scope sc(_mtx);
      _Parent::push_front( _X );
      _sem.post();
    }
    bool pop_front( int32_t timeout_ms = 100 ){
      if (_sem.wait( timeout_ms) == -1) return false ;
      _Scope sc(_mtx);
      _Parent::pop_front();
      return true;
    }
    bool pop_front( _Value &_X, int32_t timeout_ms = 100 ){
      if (_sem.wait( timeout_ms) == -1) return false ;
      _Scope sc(_mtx);
      _X = this->front();
      _Parent::pop_front();
      return true ;
    }
    void push_back(const _Value& _X){
      _Scope sc(_mtx);
      _Parent::push_back( _X );
      _sem.post();
    }
    bool pop_back(int32_t timeout_ms = 100 ){
      if (_sem.wait( timeout_ms) == -1) return false ;
      _Scope sc(_mtx);
      _Parent::pop_back();
      return true;
    }
    bool pop_back( _Value &_X, int32_t timeout_ms = 100 ){
      if (_sem.wait( timeout_ms) == -1) return false ;
      _Scope sc(_mtx);
      _X = this->back();
      _Parent::pop_back();
      return true ;
    }
    void assign( gdeque_iter _F, gdeque_iter _L){
      _Scope sc(_mtx);
      _Parent::assign( _F, _L );
    }
    void assign(gdeque_size_type _N, const _Value& _X = _Value() ){
      _Scope sc(_mtx);
      _Parent::assign( _N, _X );
    }
    gdeque_iter insert(gdeque_iter _P, const _Value& _X = _Value() ){
      _Scope sc(_mtx);
      gdeque_iter iter = _Parent::insert( _P, _X );
      _sem.post();
      return( iter );
    }
    void insert(gdeque_iter _P, gdeque_size_type _M, const _Value& _X ){
      _Scope sc(_mtx);
      _Parent::insert( _P, _M, _X );
      _sem.post( _M );
    }
    void insert(gdeque_iter _P, gdeque_iter _F, gdeque_iter _L){
      _Scope sc(_mtx);
      gdeque_size_type size_before = this->size();
      _Parent::insert( _P, _F, _L );
      gdeque_size_type size_after = this->size();
      _sem.post( size_after - size_before );
    }
    gdeque_iter erase(gdeque_iter _P){
      if ((_P == this->end()) || (this->size() < 1))  return this->end() ;
      _sem.wait() ;
      _Scope sc(_mtx);
      gdeque_iter iter = _Parent::erase( _P );
      return( iter );
    }
    void clear(){
      _Scope sc(_mtx);
      _sem.clear();
      _Parent::clear();
    }
  };

} // namespace go 

#endif

