/* DO NOT DISPLAY */
#include "zmw/zmw.h"

void event()
{
/* DO NOT DISPLAY */
zmw_button("Quit") ;
if ( zmw_activated() )
   {
     printf("END\n") ;
     exit(0) ;
   }
/* DO NOT DISPLAY */
}

void top()
{
  ZMW(zmw_window("Event"))
    event() ;
}

int main(int argc, char *argv[])
{
  zmw_init(&argc, &argv) ; // Take ZMW parameters
  zmw_main(top) ; // Launch the infinite loop
  return 0 ;
}
/* DO NOT DISPLAY */
/* REGRESSION TEST

zmw_move_cursor_to 26 35
zmw_dump_screen 0

REGRESSION TEST */
