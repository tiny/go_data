/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gomean.h
  @brief      GOMean template definition

  @author     rmcinnis@solidice.com
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <msgcb.h>
#include <gonumeric.h>

namespace go {

/*!
  @class GOMean<T>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class VEC, class ELT, class ELTTYPE>
class GOMean : public GODouble
{
  protected :
    VEC           *_vec ;
    double         _sum ;

    int16_t          onDataChange( const ArgList *args )
                   {
                     PBVParameter  *arg = args->getArg(1) ; // old value

                     _sum -= (ELTTYPE)*arg ;
                     arg = args->getArg(0) ; // new value
                     _sum += (ELTTYPE)*arg ;

                     GODouble::operator=( (_vec->size() == 0) ? 0.0 : (_sum / _vec->size()) ) ;
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
                     _sum += elt ;
                     GODouble::operator=( _sum / _vec->size() ) ;
                     elt.valueCB().install( new Observer<GOMean>( this, GOMean::onDataChange )) ;
                   }

    void           hookup() 
                   { 
                     _sum = 0.0 ;
                     if (_vec == nullptr) return ;

                      // hook int32_to vector updates
                     _vec->postInsertCB().install( new Observer<GOMean>( this, GOMean::onVectorAdd ) ) ;
                     _vec->preEraseCB  ().install( new Observer<GOMean>( this, GOMean::onVectorRemove ) ) ;

                     // hook int32_to elements
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       hookup( (*iter) ) ;
                     }
                   }

    void           unhook( ELT &elt )
                   {
                     _sum -= elt ;
                     GODouble::operator=( (_vec->size() <= 1) ? 0.0 : (_sum / (_vec->size()-1)) ) ;
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
                   GOMean() { _vec = nullptr ; _sum = 0.0 ; }
                   GOMean( VEC &v ) { _vec = &v ; hookup() ; }
                  ~GOMean() { unhook() ; }
    void           set_data( VEC &v ) { unhook() ; _vec = &v ; hookup() ; }
} ; // class GOMean

} // namespace go 

