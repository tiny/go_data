/*
  si_sleep.h
  sleep functional prototypes

  programmer  rmcinnis
  date        august 03, 1999
*/
#ifndef SI_SLEEP_H
#define SI_SLEEP_H

#include <stdint.h>

namespace go {

void si_sleep_sec( int16_t sec ) ;
void si_sleep_milli( int32_t ms ) ; // 1 s == 1,000 ms; 1 ms == 1,000 usec
void si_sleep_nano( int32_t ns ) ; // 1 ms == 1,000,000 nsec

} // namespace go 

#endif


