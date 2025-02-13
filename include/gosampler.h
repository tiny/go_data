/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gosampler.h
  @brief      GOSampler class definition

  @author     rmcinnis@solidice.com
  @date       july 08, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef GOSAMPLER_H
#define GOSAMPLER_H

// this class should be a series of templates, and the sample size should
// be dynamic.
#include <gonumeric.h>

namespace go {
  
/*!
  @class GOSampler
   
  <b>Description:</b>
   
  <b>Notes:</b>
   
  @author     rmcinnis@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
class GOSampler
{
  private   :
    static GOLong  _base ; // used for returning 'null' references
    GOLong         _cnt ;
    GOLong         _hi ;
    GOLong         _lo ;
    GOLong         _ave ;
    uint16_t         _sampleSz ;
    GOLong        *_last ;
    MsgCB          _newSampleCB ;

  protected :
    void           recalc() ;
    
  public    :
                   GOSampler( uint16_t sampleSz = 10 ) ;
                  ~GOSampler() ;
    
    void           data( int32_t d ) ;
    GOLong        &operator[](int32_t i) ;
    
    // access methods
    GOLong        &cnt() { return _cnt ; }
    GOLong        &hi() { return _hi ; }
    GOLong        &lo() { return _lo ; }
    GOLong        &ave() { return _ave ; }
    uint16_t         sampleSz() { return _sampleSz ; }
    MsgCB         &newSampleCB() { return _newSampleCB ; }
} ; // class GOSampler

} // namespace go 

#endif
