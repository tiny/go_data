/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gosum.h
  @brief      GOSum template definition

  @author     rmcinnis@solidice.com
  @date       january 12, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <msgcb.h>

namespace go {

/*!
  @class GOSum< T, VEC, ELT, ELTTYPE >
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class T, class VEC, class ELT, class ELTTYPE>
class GOSum : public T
{
  protected :
    VEC           *_vec ;

    int16_t          onDataChange( const ArgList *args )
                   {
                     PBVParameter  *arg = args->getArg(1) ; // old value
                     valueCB().block() ; 

                     *this -= (ELTTYPE)*arg ;
                     arg = args->getArg(0) ; // new value
                     *this += (ELTTYPE)*arg ;

                     valueCB().unblock() ; 
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
                     *this += (T)elt ;
                     elt.valueCB().install( new Observer<GOSum>( this, GOSum::onDataChange )) ;
                   }

    void           hookup() 
                   { 
                     *((T*)this) = 0L ;
                     if (_vec == nullptr) return ;

                      // hook int32_to vector updates
                     _vec->postInsertCB().install( new Observer<GOSum>( this, GOSum::onVectorAdd ) ) ;
                     _vec->preEraseCB  ().install( new Observer<GOSum>( this, GOSum::onVectorRemove ) ) ;

                     // hook int32_to elements
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       hookup( (*iter) ) ;
                     }
                   }

    void           unhook( ELT &elt )
                   {
                     *this -= (T)elt ;
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
                   GOSum() { _vec = nullptr ; }
                   GOSum( VEC &v ) { _vec = &v ; hookup() ; }
                  ~GOSum() { unhook() ; }
    void           set_data( VEC &v ) { unhook() ; _vec = &v ; hookup() ; }
} ; // class GOSum

} // namespace go 

