/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gomax.h
  @brief      GOMax template definition

  @author     rmcinnis@solidice.com
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <msgcb.h>

namespace go {

/*!
  @class GOMax< VEC, ELT, ELTTYPE >
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class VEC, class ELT, class ELTTYPE>
class GOMax : public ELT
{
  protected :
    VEC           *_vec ;

    void           check_all( ELT *elt = nullptr ) // elt == element not to compare
                   {
                     ELTTYPE  m = 0 ;
                     if ((_vec == nullptr) || (_vec->size() == 0))
                     {
                       ELT::operator=( 0 ) ;
                       return ;
                     }
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       if ((elt != nullptr) && (elt == &(*iter)))
                         continue ;

                       if (iter == _vec->begin())
                         m = (*iter) ;
                       else
                         m = ((*iter) > m) ? (*iter) : m ;
                     }
                     ELT::operator=( m ) ;
                   }
    int16_t          onDataChange( const ArgList *args )
                   {
                     PBVParameter  *arg = args->getArg(1) ; // old value

                     if (*this == (ELTTYPE)*arg) // if old_min == oldvalue, force check
                     {
                       check_all() ;
                     }
                     else
                     {
                       arg = args->getArg(0) ; // new value
                       if (*this < (ELTTYPE)*arg)
                       {
                         ELT::operator=( (ELTTYPE)*arg ) ;
                       }
                     }
                     return 0 ;
                   }
    int16_t          onVectorAdd( const ArgList *args )
                   {
                     hookup( (*_vec->current()) ) ;
                     return 0 ;
                   }
    int16_t          onVectorRemove( const ArgList *args )
                   {
                     unhook( (*_vec->current()) ) ;
                     return 0 ;
                   }
    void           hookup( ELT &elt )
                   {
                     elt.valueCB().install( new Observer<GOMax>( this, GOMax::onDataChange )) ;
                     if (*this < (ELTTYPE)elt)
                     {
                       ELT::operator=( (ELTTYPE)elt ) ;
                     }
                   }

    void           hookup() 
                   { 
                     if (_vec == nullptr) return ;

                      // hook int32_to vector updates
                     _vec->postInsertCB().install( new Observer<GOMax>( this, GOMax::onVectorAdd ) ) ;
                     _vec->preEraseCB  ().install( new Observer<GOMax>( this, GOMax::onVectorRemove ) ) ;

                     // hook int32_to elements
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       hookup( (*iter) ) ;
                     }
                   }

    void           unhook( ELT &elt )
                   {
                     if (*this == elt)
                     {
                       check_all( &elt ) ; 
                     }
                     delete elt.valueCB().remove( this ) ;
                   }

    void           unhook() 
                   { 
                     if (_vec == nullptr) return ;
                     // unhook vector updates
                     delete _vec->postInsertCB().remove( this ) ;
                     delete _vec->preEraseCB  ().remove( this ) ;

                     // unhook elements
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       unhook( (*iter) ) ;
                     }
                   }

  public    :
                   GOMax() { _vec = nullptr ; }
                   GOMax( VEC &v ) { _vec = &v ; hookup() ; }
                  ~GOMax() { unhook() ; }
    void           set_data( VEC &v ) { unhook() ; _vec = &v ; hookup() ; }
} ; // class GOMax

} // namespace go 

