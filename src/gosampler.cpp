/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       gosampler.cpp
  @brief      GOSampler class implementation

  @author     rmcinnis@solidice.com
  @date       july 08, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#include <gosampler.h>

namespace go {

GOLong GOSampler :: _base ; // used for returning 'null' references

//------------------------------------------------------------------------
void GOSampler :: recalc()
{
  if (_cnt == (int32_t)0)
    return ;
  
  _lo.valueCB().block() ;
  _hi.valueCB().block() ;
  _ave.valueCB().block() ;

  int32_t  a, b, c ;
  a = _lo ;
  b = _hi ; 
  c = _ave ;

  _lo = _hi = _ave = _last[0] ;
  for (int32_t i = 1; i < _cnt; i++)
  {
    if (_last[i] < _lo)
      _lo = _last[i] ;
    if (_last[i] > _hi)
      _hi = _last[i] ;
    _ave += _last[i] ;
  }

  _ave /= _cnt ;
  // trigger callbacks
  _lo.valueCB().unblock( ArgList() << (int32_t)_lo << a ) ; // args must be: new, old
  _hi.valueCB().unblock( ArgList() << (int32_t)_hi << b ) ; 
  _ave.valueCB().unblock( ArgList() << (int32_t)_ave << c ) ; 
} // GOSampler :: recalc

GOSampler :: GOSampler( uint16_t sampleSz )
: _newSampleCB( this )
{
  _sampleSz = (sampleSz == 0) ? 10 : sampleSz ;
  _last = new GOLong[ _sampleSz ] ;
} // GOSampler [constructor]

GOSampler :: ~GOSampler()
{
  delete [] _last ;
} // GOSampler [destructor]

GOLong &GOSampler :: operator[](int32_t i) 
{
  if ((i < 0) || (i >= _cnt) || (_last == nullptr))
    return _base ;

  return _last[i] ;
} // GOSampler :: operator[]

void GOSampler :: data( int32_t d )
{
  if (_last == nullptr)
    return ;

  ++_cnt ;

  if (_cnt > (int32_t)_sampleSz)
    _cnt = _sampleSz ; 

  if (_cnt > 1)
  {
    for (int32_t i = ((int32_t)_cnt) - 1L; i > 0; i--)
      _last[i] = _last[i-1] ;
  }
  _last[0] = d ;
  
  recalc() ;
  _newSampleCB.invoke() ;
} // GOSampler :: data

} // namespace go 
