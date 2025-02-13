/*                                                                            */
/* This file is copyright Solid ICE Technologies, Inc., all rights reserved.  */
/* Refer to the license file www.solidice.com/licensing/ for more details     */
/*                                                                            */
/*!
  @file       mutexscope.h
  @brief      GateScope template definition

  @author     elindahl@solidice.com
  @date       june 15, 1999
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
#ifndef MUTEXSCOPE_H
#define MUTEXSCOPE_H

#include "mutexgate.h"
#include "fastgate.h"

#if defined(_MSC_VER) 
#  pragma warning( disable : 4512 )
#endif

namespace go {

/*!
  @class GateScope<_Gate>
   
  <b>Description:</b>
   
  <b>Notes:</b>
   
  @author     elindahl@solidice.com 
  @par        copyright (c) 2004 Solid ICE Technologies, Inc.  All rights reserved.
*/
template< class _Gate >
class GateScope 
{
  private:
    _Gate         &_gate ;

  public:
    inline         GateScope( _Gate &gate ) : _gate( gate ) { _gate.enter() ; }
    inline        ~GateScope() { _gate.exit() ; }
} ; // template GateScope< Gate >

typedef GateScope<MutexGate> MutexScope;
typedef GateScope<FastGate> FastScope;

} // namespace go 

#endif
