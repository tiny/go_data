/*
  si_sleep.cpp
  si_sleep functional implementation

  programmer  rmcinnis
  date        august 03, 1999
*/
#include <si_sleep.h>
#ifdef _MSC_VER
#  include <windows.h>
#  define timevalue  uint32_t
#else
#  include <unistd.h>
#  include <time.h>
#endif

namespace go {

void si_sleep_sec( int16_t sec )
{
  if (sec < 0)
    return ;
#ifdef UNIX
  ::sleep( sec ) ;
#else
  ::Sleep( 1000 * sec ) ; // arg is microseconds
#endif
} // :: si_sleep_sec

void si_sleep_milli( int32_t ms )
{
  if (ms < 0)
    return ;
#ifdef UNIX
  ::usleep( ms*1000 ) ; // arg is microseconds
#else
  ::Sleep( ms ) ;
#endif
} // :: si_sleep_milli

void si_sleep_nano( int32_t ns )
{
  if (ns < 0)
    return ;
#ifdef UNIX
  struct timespec  t1, t2 ;
  t1.tv_sec  = (ns / (1000L*1000L*1000L)) ;
  t1.tv_nsec = (ns % (1000L*1000L*1000L)) ;
  while (::nanosleep( &t1, &t2 ) < 0)
    t1 = t2 ;
#else
// warning nanosleep not implemented for win32
#endif
} // :: si_sleep_nano

} // namespace go 

