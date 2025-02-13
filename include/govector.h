/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       govector.h
  @brief      GOVector template definition

  @author     rmcinnis@solidice.com
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <msgcb.h>
#include <mutexscope.h>

namespace go {

/*!
  @class GOVector< T, _Gate >
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template<class T, class _Gate = MutexGate >
class GOVector : public vector< T > 
{
  public:
  typedef typename std::vector<T>::iterator   iterator ;
  typedef typename std::vector<T>::size_type  size_type ;

  private:
    typedef vector< T >           _Parent;
    typedef GOVector< T, _Gate >  _TGOVector;

    _Gate          _gate;
    MsgCB          _postInsertCB;
    MsgCB          _preEraseCB;

  public:
    typedef GateScope< _Gate >         _Scope; 

                   GOVector() 
                   : _postInsertCB( this ), _preEraseCB( this )
                   {}
                   GOVector(size_type _N ) 
                   : _Parent( _N ), _postInsertCB( this ), _preEraseCB( this )
                   {}
                   GOVector( const _TGOVector& _X) 
                   : _postInsertCB( this ), _preEraseCB( this )
                   {
                     _TGOVector &other = const_cast<_TGOVector &>(_X);
                     _Scope sc1( other._gate );
                     insert( this->begin(), other.begin(), other.end() );
                   }
    virtual       ~GOVector() {}

    _TGOVector    &operator=( const _TGOVector &cother_ ) 
                   {
                     _TGOVector &other = const_cast<_TGOVector &>(cother_);
                     _Scope sc1( other._gate );
                     _Scope sc2( _gate );
                     for(auto current = this->begin(); current != this->end(); current++ ) 
                     {
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _preEraseCB.invoke( args );
                     }
                     _Parent::clear();
                     for(auto current = other.begin(); current != other.end(); current++ ) 
                     {
                       _Parent::push_back( (*current) );
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _postInsertCB.invoke( args );
                     }
                     return( *this );
                   }
    void           reserve(size_type _N) 
                   {
                     _Scope sc(_gate);
                     _Parent::reserve( _N );
                   }
    void           resize(size_type _N, T x = T() )
                   {
                     _Scope sc(_gate);
                     _Parent::resize( _N, x );
                   }
    void           push_back(const T& _X)
                   {
                     _Scope sc(_gate);
                     _Parent::push_back( _X );
                     auto current = (this->end() - 1);    
                     ArgList  args;
                     args << (void*)&(*current) << (void *)this;
                     _postInsertCB.invoke( args );
                   }
    void           pop_back()
                   {
                     _Scope sc(_gate);
                     if( this->size() < 1 ) 
                     {
                       return;
                     }
                     auto current = (this->end()-1);
                     ArgList  args;
                     args << (void*)&(*current) << (void *)this;
                     _preEraseCB.invoke( args );
                     _Parent::pop_back();
                   }
    void           assign( iterator _F, iterator _L )
                   {
                     _Scope sc(_gate);
                     _Parent::assign( _F, _L );
                   }
    void           assign(size_type _N, const T& _X = T() )
                   {
                     _Scope sc(_gate);
                     _Parent::assign( _N, _X );
                   }
    iterator       insert(iterator _P, size_type n, const T& _X = T() )
                   {
                     _Scope sc(_gate);
                     auto current = _Parent::insert( _P, n, _X );
                     if( this->end() != current ) 
                     {
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _postInsertCB.invoke( args );
                     }
                     return( current );
                   }
    void           insert(iterator _P, iterator _F, iterator _L)
                   {
                     _Scope sc(_gate);
                     if( _F == _L ) 
                     {
                       return;
                     }
                     auto current = _P;
                     for(auto iter = _F; iter != _L; iter++ ) 
                     {
                       insert( _P, (*iter) );
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _postInsertCB.invoke( args );
                     }
                   }

    iterator       erase(iterator _P)
                   {
                     _Scope sc(_gate);
                     if( this->end() != _P ) 
                     {
                       auto current = _P;
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _preEraseCB.invoke( args );
                     }
                     return( _Parent::erase( _P ) );
                   }
    iterator       erase(iterator _F, iterator _L)
                   {
                     _Scope sc(_gate);
                     for(auto current = _F; current != _L; current++ ) 
                     {
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _preEraseCB.invoke( args );
                     }
                     return( _Parent::erase( _F, _L ) );
                   }
    void           clear()
                   {
                     _Scope sc(_gate);
                     for(auto current = this->begin(); current != this->end(); current++ ) 
                     {
                       ArgList  args;
                       args << (void*)&(*current) << (void *)this;
                       _preEraseCB.invoke( args );
                     }
                     _Parent::clear();
                   }

    // access methods
    _Gate         &gate() { return( _gate ); }
    MsgCB         &postInsertCB() { return( _postInsertCB ); }
    MsgCB         &preEraseCB() { return( _preEraseCB ); }
} ; // template GOVector

} // namespace go 
