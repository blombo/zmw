/* DO NOT DISPLAY */
#include "zmw/zmw.h"
/* DO NOT DISPLAY */

void swappable_text(char **i, int **current, int **old)
{
  zmw_text(*i) ;
  if ( zmw_drag_swap((int*)i, current, old) )
    ZMW(zmw_window_drag())
    {
      zmw_text((char*)**current) ;
    }
}

void dragswap(void)
{
  static int table1[] = { 1, 2, 3, 4 } ;
  static char *table2[] = { "A","B","C","D","E","F","G","H" } ;
  static int *current1 = NULL, *old1 = NULL, *current2 = NULL, *old2 = NULL ;
  int i ;

  ZMW(zmw_window("DragSwap"))
    {
      ZMW(zmw_box_vertical())
	{
	  // Swap the children of a container
	  ZMW(zmw_box_horizontal())
	    for(i=0; i<ZMW_TABLE_SIZE(table1); i++)
	      {
		zmw_int(table1[i]) ;
		if ( zmw_drag_swap(&table1[i], &current1, &old1) )
		  ZMW(zmw_window_drag())
		  {
		    zmw_int(*current1) ;
		  }
	      }
	  // Swap the children in the TWO next containers
	  ZMW(zmw_box_horizontal())
	    for(i=0; i<ZMW_TABLE_SIZE(table2)/2; i++)
	      swappable_text(&table2[i], &current2, &old2) ;
	  ZMW(zmw_box_horizontal())
	    for(i=ZMW_TABLE_SIZE(table2)/2; i<ZMW_TABLE_SIZE(table2); i++)
	      swappable_text(&table2[i], &current2, &old2) ;
	}
    }
}
/* DO NOT DISPLAY */
int main(int argc, char *argv[])
{
  zmw_init(&argc, &argv) ;
  zmw_run(dragswap) ;
  return 0 ;
}
/* DO NOT DISPLAY */
/* REGRESSION TEST

zmw_move_cursor_to 16 34 # 1
zmw_dump_screen 0

zmw_button_press
zmw_move_cursor_to 16 54 # A
zmw_dump_screen 1

zmw_move_cursor_to 35 51 # C
zmw_move_cursor_to 35 35 # 3
zmw_dump_screen 2

zmw_move_cursor_to 27 35 # 2
zmw_dump_screen 3

zmw_button_release
zmw_move_cursor_to 16 51 # A
zmw_button_press
zmw_dump_screen 4

zmw_move_cursor_to 27 51 # B
zmw_move_cursor_to 27 70 # F
zmw_button_release
zmw_dump_screen 5



REGRESSION TEST */