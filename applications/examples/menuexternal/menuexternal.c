/* DO NOT DISPLAY */
#include "zmw/zmw.h"
/* DO NOT DISPLAY */

void popup(char *title)
{
  ZMW_EXTERNAL_RESTART ;

  ZMW(zmw_window_popup_right_with_title(title))
    {
      ZMW(zmw_box_vertical())
	{
	  zmw_tearoff() ;
	  ZMW_EXTERNAL ;
	}
    }
  ZMW_EXTERNAL_STOP ;
}

void menuexternal(void)
{
  ZMW(zmw_window("Menu"))
    {
      zmw_button("Menu") ;
      if ( zmw_window_is_popped() )
	{
	  ZMW(popup("Menu1"))
	    {
	      zmw_text(zmw_name_full) ;
	      zmw_button("Action2") ;
	    }
	}
    }
}
/* DO NOT DISPLAY */
int main(int argc, char *argv[])
{
  zmw_init(&argc, &argv) ;
  zmw_run(menuexternal) ;
  return 0 ;
}
/* DO NOT DISPLAY */
/* REGRESSION TEST

zmw_move_cursor_to 26 36
zmw_button_click
zmw_move_cursor_to 98 38 # Detach
zmw_button_click
zmw_dump_screen 0




REGRESSION TEST */
