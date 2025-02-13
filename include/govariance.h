/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       govariance.h
  @brief      GOVariance template definition

  @author     rmcinnis@solidice.com
  @date       april 07, 2004
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#pragma once

#include <msgcb.h>
#include <gonumeric.h>

namespace go {

/*!
  @class GOVariance<T,E,ET>
   
  <b>Description:</b>
   
  <b>Notes:</b>
  
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template <class VEC, class ELT, class ELTTYPE>
class GOVariance : public GODouble
{
  protected :
    VEC           *_vec ;
    double         _varsum ;
    double         _meansum ;
    double         _mean ;

    void           calc_variance( ELT *elt = nullptr )
                   {
                     _varsum = 0.0 ;
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       if ((elt != nullptr) && (elt == &(*iter)))
                         continue ;
                       _varsum += ((ELTTYPE)(*iter) - _mean) * ((ELTTYPE)(*iter) - _mean) ;
                     }
                     GODouble::operator=( (_vec->size() <= 1) ? 0.0 : (_varsum / (_vec->size() - 1))) ;
                   }
    int16_t          onDataChange( const ArgList *args )
                   {
                     ELTTYPE   oldv = (ELTTYPE)*args->getArg(1) ;  // old value 
                     ELTTYPE   newv = (ELTTYPE)*args->getArg(0) ;  // new value 

                     // find new mean
                     _meansum -= oldv ;
                     _meansum += newv ;
                     _mean = (_vec->size() == 0) ? 0.0 : (_meansum / _vec->size()) ;

                     // force recalc of var-sum due to new mean
                     calc_variance() ;

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
                     elt.valueCB().install( new Observer<GOVariance>( this, GOVariance::onDataChange )) ;

                     // determine new mean
                     _meansum += elt ;
                     _mean     = _meansum / _vec->size() ;
                     // force recalc of var-sum due to new mean
                     calc_variance() ;
                   }

    void           hookup() 
                   { 
                     _meansum = 0.0 ;
                     _varsum  = 0.0 ;
                     _mean    = 0.0 ;
                     if (_vec == nullptr) return ;

                      // hook int32_to vector updates
                     _vec->postInsertCB().install( new Observer<GOVariance>( this, GOVariance::onVectorAdd ) ) ;
                     _vec->preEraseCB  ().install( new Observer<GOVariance>( this, GOVariance::onVectorRemove ) ) ;

                     // hook int32_to elements
                     VEC::iterator  iter ;
                     for (iter = _vec->begin(); iter != _vec->end(); iter++)
                     {
                       hookup( (*iter) ) ;
                     }
                   }

    void           unhook( ELT &elt )
                   {
                     _meansum -= elt ;
                     _mean     = (_vec->size() <= 1) ? 0.0 : (_meansum / (_vec->size()-1)) ;
                     calc_variance( &elt ) ;
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
                   GOVariance() { _vec = nullptr ; _varsum = 0.0 ; _meansum = 0.0 ; _mean = 0.0 ; }
                   GOVariance( VEC &v ) { _vec = &v ; hookup() ; }
                  ~GOVariance() { unhook() ; }
    void           set_data( VEC &v ) { unhook() ; _vec = &v ; hookup() ; }
} ; // class GOVariance

} // namespace go 
