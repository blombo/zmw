/*
    ZMW: A Zero Memory Widget Library
    Copyright (C) 2002-2003 Thierry EXCOFFIER, Université Claude Bernard, LIRIS

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

    Contact: Thierry.EXCOFFIER@liris.univ-lyon1.fr
*/


void test_color(const char *title)
{
  static char *strings[] = { "Red", "Green" } ;
  static int color[] = { 0x00FF0000, 0x0000FF00 } ;

  int j ;

  ZMW(zmw_window(title))
    {
      zmw_border_relief_out_draw() ;
      zmw_name("Row") ;
      ZMW(zmw_hbox())
	{
	  zmw_name("SubRow") ;
	  ZMW(zmw_hbox())
	    {
	      for(j=0; j<ZMW_TABLE_SIZE(strings); j++)
		{
		  zmw_color(Zmw_Color_Foreground, color[j]) ;
		  zmw_name(strings[j]) ;
		  zmw_label(strings[j]) ;
		  zmw_border_relief_out_draw() ;
		}
	    }
	  zmw_name("Black") ;
	  zmw_label("Black") ;
	}
      zmw_border_relief_out_draw() ;
    }
}
