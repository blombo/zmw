/*
  ZMW: A Zero Memory Widget Library
  Copyright (C) 2002-2004 Thierry EXCOFFIER, Université Claude Bernard, LIRIS
  
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
/* -*- outline-minor -*- */

#include <ctype.h>
#include "zmw/zmw.h"


static Zmw_Name global_zmw_selected = ZMW_NAME_UNREGISTERED("Selected") ;
static Zmw_Name global_zmw_selection = ZMW_NAME_UNREGISTERED("Selection") ;

#define C(X) if ( (X) == zmw.event->type ) zmw_printf("%s\n", #X)
void zmw_event_dump()
{
	zmw_printf("EVENT_DUMP\n") ;
	zmw_debug_trace() ;
	zmw_printf("%s\n", zmw_size_string(&ZMW_SIZE)) ;
	zmw_printf("zmw.x = %d %d\n", zmw.x, zmw.y) ;
	zmw_printf("ZMW_SIZE_EVENT_IN_RECTANGLE = %d\n", ZMW_SIZE_EVENT_IN_RECTANGLE) ;
	zmw_printf("ZMW_SIZE_EVENT_IN_CHILDREN = %d\n", ZMW_SIZE_EVENT_IN_CHILDREN) ;
	zmw_printf("ZMW_EVENT_IN_MASKED = %d\n", ZMW_EVENT_IN_MASKED) ;
	zmw_printf("ZMW_EVENT_IN_FOCUS = %d\n", ZMW_EVENT_IN_FOCUS) ;
	zmw_printf("ZMW_SIZE_SENSIBLE = %d\n", ZMW_SIZE_SENSIBLE) ;
	zmw_printf("zmw_focus_in() = %d\n", zmw_focus_in()) ;
	zmw_printf("zmw_key_pressed() = %d\n", zmw_key_pressed()) ;
	if ( zmw_key_pressed() )
	  zmw_printf("KEY = %c(%d)\n", zmw.event->key.string[0],
		     zmw.event->key.string[0]) ;
		  C(GDK_KEY_RELEASE) ;
		  C(GDK_KEY_PRESS) ;
		  C(GDK_MOTION_NOTIFY) ;
		  C(GDK_EXPOSE) ;
		  C(GDK_2BUTTON_PRESS) ;
		  C(GDK_3BUTTON_PRESS) ;
		  C(GDK_BUTTON_PRESS) ;
		  C(GDK_BUTTON_RELEASE) ;
	zmw_printf("\n") ;
}
#undef C

/*
 * True if cursor in
 */
Zmw_Boolean zmw_cursor_in()
{
  ZMW_FUNCTION_CALLED_OUTSIDE_ZMW_PARAMETER ;
  return( ZMW_SIZE_EVENT_IN_RECTANGLE && !ZMW_EVENT_IN_MASKED ) ;
  /* zmw.event->type != GDK_NOTHING */
}

/*
 * True if the event should be processed
 */
Zmw_Boolean zmw_event_to_process()
{
  return( ZMW_SUBACTION == Zmw_Input_Event
	  && !zmw.event_removed
	  && ZMW_CALL_NUMBER > 0
	  && ZMW_SIZE_EVENT_IN_RECTANGLE
	  && !ZMW_EVENT_IN_MASKED
	  && ZMW_SIZE_SENSIBLE
	  && ZMW_SENSIBLE
	  ) ;
}

Zmw_Boolean zmw_state_change_allowed()
{
  return( ZMW_SUBACTION == Zmw_Input_Event
	  || ZMW_WINDOW == NULL
	  || *ZMW_WINDOW == NULL
	  ) ;
}
/*
 * True if the last zmw has the focus
 */
Zmw_Boolean zmw_focus_in()
{
  return(
	 ZMW_EVENT_IN_FOCUS
	 && !ZMW_EVENT_IN_MASKED
	 // && *ZMW_WINDOW == zmw.event->any.window // No cross-window focus
	 && ZMW_SENSIBLE
	 && zmw_name_is(ZMW_FOCUS)
	 ) ;
}
Zmw_Boolean zmw_focus_in_by_its_parents()
{
  return(
	 ZMW_EVENT_IN_FOCUS
	 && !ZMW_EVENT_IN_MASKED
	 // && *ZMW_WINDOW == zmw.event->any.window // No cross-window focus
	 && zmw_name_is_inside(ZMW_FOCUS)
	 && ZMW_SENSIBLE
	 ) ;
}
static int zmw_pass_through(int n)
{
  int i ;

  for(i = n ; i>0 && zMw[-1].u.children[i].pass_through ; i--)
    {
    }
  return i>=0 ? i : 0 ;
}

static void zmw_widget_is_tested_(int n)
{
  int i ;

  if ( zMw[-1].u.nb_of_children == 0 )
    return ;

  i = zmw_pass_through(ZMW_CHILD_NUMBER - n) ;

  if ( i >= 0 )
    {
      zMw[-1].u.children[i].sensible = Zmw_True ;
    }
  //  ZMW_SIZE_SENSIBLE = Zmw_True ; // Added 30/4/2004 (menu3.c)
}

void zmw_widget_is_tested()
{
  //  ZMW_FUNCTION_CALLED_OUTSIDE_ZMW_PARAMETER ;
  zmw_widget_is_tested_(0) ;
}

void zmw_widget_previous_is_tested()
{
  ZMW_FUNCTION_CALLED_INSIDE_ZMW_PARAMETER ;
  zmw_widget_is_tested_(1) ;
}

/*
 * True if the key is pressed in the zmw
 * Or if the zmw has the focus.
 */
Zmw_Boolean zmw_key_pressed()
{
  zmw_widget_is_tested() ;
  return( !zmw.event_removed
	  && zmw_focus_in_by_its_parents()
	  && zmw.event->type == GDK_KEY_PRESS
	  ) ;
}
/*
 * True if the NON NUL key is pressed in the zmw
 * Or if the zmw has the focus.
 */
Zmw_Boolean zmw_key_string()
{
  return(  !zmw.event_removed
	  && zmw_focus_in()
	  && zmw.event->type == GDK_KEY_PRESS
	  && zmw.event->key.string[0]
	  ) ;
}
/*
 * True if the cursor is pressed in the zmw
 */
Zmw_Boolean zmw_button_pressed()
{
  zmw_widget_is_tested() ;
  return( zmw_event_to_process()
	  && zmw.event->type == GDK_BUTTON_PRESS
	  ) ;
}

/*
 * True if the cursor is released in the zmw
 */
Zmw_Boolean zmw_button_released()
{
  return( zmw_event_to_process()
	  && zmw.event->type == GDK_BUTTON_RELEASE
	  ) ;
}
Zmw_Boolean zmw_button_released_anywhere()
{
  return(  !zmw.event_removed
	 && ZMW_ACTION == zmw_action_dispatch_event
	 && ZMW_CALL_NUMBER > 0
	 && zmw.event->type == GDK_BUTTON_RELEASE ) ;
}
/*
 * True if the previous widget was activated
 * Or itself if in it is called inside the ZMW parameter.
 */
Zmw_Boolean zmw_activated()
{
  zmw_widget_is_tested() ;

  if( ZMW_SIZE_ACTIVATED
      && ( ZMW_ACTION == zmw_action_dispatch_event
	   || ZMW_ACTION == zmw_action_dispatch_accelerator) )
    {
      zmw_event_remove() ; /* 2004-23-06 in order to no change widget tree */
      return Zmw_True ;
    }

  if ( ZMW_SIZE_PASS_THROUGH )
    return zmw_activated_previous() ;

  return Zmw_False ;
}
/*
 * True if the previous previous widget was activated.
 * Or the previous if it is called inside the ZMW parameter.
 */
Zmw_Boolean zmw_activated_previous()
{
  int i ;

  i = zmw_pass_through(ZMW_CHILD_NUMBER - 1) ;
  return zMw[-1].u.children[i].activated 
    &&  ( ZMW_ACTION == zmw_action_dispatch_event
	  || ZMW_ACTION == zmw_action_dispatch_accelerator) ;
}

/*
 * True if event_in_rectangle is true for a not "pass_through" widget
 */
Zmw_Boolean zmw_event_in_rectangle_previous()
{
  int i ;

  i = zmw_pass_through(ZMW_CHILD_NUMBER - 1) ;
  return zMw[-1].u.children[i].event_in_rectangle  ;
}

/*
 * True if the previous widget was modified (text, scrollbar)
 */
Zmw_Boolean zmw_changed()
{
  zmw_widget_is_tested() ;
  return( (ZMW_SIZE_CHANGED || ZMW_SIZE_ACTIVATED)
	  && ( ZMW_ACTION == zmw_action_dispatch_event
	       || ZMW_ACTION == zmw_action_dispatch_accelerator) ) ;
}
/*
 * True is cursor in the zmw and the button pressed
 */
Zmw_Boolean zmw_cursor_in_and_pressed()
{
  zmw_widget_is_tested() ;
  return( ZMW_SUBACTION == Zmw_Input_Event
	  && global_zmw_selected.name && zmw_cursor_in()
	  && zmw.event->type != GDK_NOTHING
	  ) ;
}
/*
 * Cursor enter/leave the widget.
 *
 * global_zmw_cursor : contains the last registered cursor.
 *
 * The first pass "leave" all the widgets.
 * "zmw_cursor_set" is called by zmw_call_widget.
 * We enter now all the widgets.
 */
 
static Zmw_Name global_zmw_cursor = ZMW_NAME_UNREGISTERED("Cursor") ;

Zmw_Boolean zmw_cursor_leave()
{
  if ( 0 )
    zmw_printf("cursor_in %s %d %d %d %d %s/%d\n"
	       , global_zmw_cursor.name
	       , ZMW_SUBACTION == Zmw_Input_Event
	       , zmw_name_contains(&global_zmw_cursor)
	       , !ZMW_SIZE_EVENT_IN_RECTANGLE
	       , !ZMW_EVENT_IN_MASKED
	       , zmw_action_name_fct()+11
	       , ZMW_CALL_NUMBER
	       ) ;

  if( ZMW_SUBACTION == Zmw_Input_Event
      && zmw_name_contains(&global_zmw_cursor)
      && !ZMW_SIZE_EVENT_IN_RECTANGLE
      && !ZMW_EVENT_IN_MASKED
      )
  	 {
	   if ( 0 )
	     zmw_printf("LEAVE!\n") ;
	   //	zmw.need_dispatch = Zmw_True ;
	   return( Zmw_True ) ;
  	 }

   return( Zmw_False ) ;
}
       
 
Zmw_Boolean zmw_cursor_enter()
{
  if ( 0 )
    zmw_printf("cursor_in %s\n", global_zmw_cursor.name) ;
  
  if( ZMW_SUBACTION == Zmw_Input_Event
      && ZMW_SIZE_EVENT_IN_RECTANGLE
      && !ZMW_EVENT_IN_MASKED
      && !zmw_name_contains(&global_zmw_cursor)
      )
    {
      if ( 0 )
	zmw_printf("ENTER!\n") ;
      return( Zmw_True ) ;
    }

   return( Zmw_False ) ;
}

void zmw_cursor_set(char *name)
{
  if ( (name == NULL && global_zmw_cursor.name != NULL)
       || (name != NULL && global_zmw_cursor.name == NULL)
       || (name && global_zmw_cursor.name && strcmp(name, global_zmw_cursor.name))
       )
       zmw_need_repaint() ;

  zmw_name_register_with_name(&global_zmw_cursor, name) ;
}

/*
 * In a hierarchic menu.
 *  A -> B -> C -> D -> E -> F
 * With D detached. D is always visible
 * If the user asks to see B, then B is visible
 * If the user asks to see C, then B/C is visible
 * If the user asks to see E, then E is visible
 * If the user asks to see F, then E/F is visible
 */


#define ZMW_PRINTF if (0) printf("%25s %25s/%d ", zmw_name_full, zmw_action_name_fct()+11, ZMW_CALL_NUMBER), printf

void zmw_window_update_uppers(int action)
{
  Zmw_State *s ;

  action *= Zmw_Menu_State_New ;
  
  for(s=zMw-1; s >= zmw.zmw_table; s--)
    if ( s->u.menu_state )
      {
	*s->u.menu_state |= action ;

	/* If an upper menu is detached, no need to go upper
	 * because it is yet done
	 */
	if ( *s->u.menu_state & Zmw_Menu_Is_Detached )
	  break ;
	
	ZMW_PRINTF("Update %d\n", zMw - s) ;
      }
}
/*
 * This function is only callable inside a ZMW parameter.
 * the registered name is the zmw_if one.
 */

static Zmw_Name global_inner_visible_menu = ZMW_NAME_UNREGISTERED("Menu") ;

Zmw_Boolean zmw_window_is_popped_with_detached(int *detached)
{
  Zmw_Boolean visible, continue_recursion ;
  int action ;

  /*
   * So a tip can be defined after the popup window
   */
  ZMW_SIZE_PASS_THROUGH = Zmw_True ;

  /*
   * Get a pointer on the menu state.
   * It could be a user pointer or a resource pointer.
   * If a menu is called, a ressource is automaticaly created.
   * ZMW_MENU_STATE is never NULL after a call to this function.
   * This function is the only one to initialize ZMW_MENU_STATE
   *
   * Could be a better idea to store the state in ZMW_SIZE_MENU_STATE
   * to not use resource system so much.
   */
  if ( ZMW_MENU_STATE == NULL )
    {
      if ( detached )
	ZMW_MENU_STATE= detached ;
      else
	ZMW_MENU_STATE= zmw_name_get_pointer_on_int_resource("WindowDetached");
    }
  zmw_widget_previous_is_tested() ;

  /*
   * In some case, the event never come here because
   * the widget tree traversal stopped (button press elsewhere)
   * So you must unpop window if necessary in the very first
   * stage of event dispatching : when computing sizes.
   */
  if ( ZMW_ACTION == zmw_action_compute_required_size
       && ZMW_CALL_NUMBER == 1
       && !zmw_name_contains(&global_inner_visible_menu)
       && ! (*ZMW_MENU_STATE & (Zmw_Menu_Contains_A_Detached|Zmw_Menu_Is_Detached))
       )
    {
      ZMW_PRINTF("Unpop before\n") ;
      *ZMW_MENU_STATE &= ~Zmw_Menu_Is_Poped ;
    }


  if ( ZMW_SUBACTION == Zmw_Input_Event && !ZMW_EVENT_IN_MASKED )
    {
      /*
       * It is here AFTER the event dispatch on children
       */
      action = 0 ;
      /*
       * Update state
       */
      ZMW_PRINTF("COMPUTE STATE current=%d\n", *ZMW_MENU_STATE) ;
      *ZMW_MENU_STATE = *ZMW_MENU_STATE/Zmw_Menu_State_New
	| (*ZMW_MENU_STATE & Zmw_Menu_Is_Detached);
      ZMW_PRINTF("COMPUTE STATE new=%d\n", *ZMW_MENU_STATE) ;
      /*
       * Need to be made only once for all widget.
       * This should not be here.
       */
      if ( zmw.button_pressed
	   && zmw_name_registered(&global_inner_visible_menu))
	{
	  zmw_use_window_from_button_press(Zmw_False) ;
	}
      /*
       *
       */
      if ( *ZMW_MENU_STATE & Zmw_Menu_Is_Detached )
	{
	  ZMW_PRINTF("I am detached\n") ;
	  action = Zmw_Menu_Contains_A_Detached ;
	  zmw_window_update_uppers(action) ;
	}
      /*
       *
       */
      if ( zmw_name_is(&global_inner_visible_menu) )
	{
	  ZMW_PRINTF("poped\n") ;
	  action = Zmw_Menu_Is_Poped ;
	}
      /*
       *
       */
      if ( action )
	{
	  *ZMW_MENU_STATE |= Zmw_Menu_Is_Poped ;
	  zmw_window_update_uppers(action) ;
	}
      else
	if ( !zmw_name_contains(&global_inner_visible_menu))
	  {
	    *ZMW_MENU_STATE &= ~Zmw_Menu_Is_Poped ;
	  }
    }

  if ( ZMW_ACTION == zmw_action_dispatch_event && ZMW_SUBACTION == Zmw_Init)
    {
      /*
       * It is here BEFORE the event dispatch on children
       */
      /*
       *
       */
      if ( *ZMW_MENU_STATE & Zmw_Menu_Is_Detached )
	{
	  ZMW_PRINTF("I am detached\n") ;
	  action = Zmw_Menu_Contains_A_Detached ;
	  zmw_window_update_uppers(action) ;
	}
      /*
       * If the menu button was activated we do not
       * want the unpop.
       */
      if ( zmw_activated_previous() )
	{
	  ZMW_PRINTF("Registered by Activation\n") ;
	  zmw_name_register(&global_inner_visible_menu) ;
	  action = Zmw_Menu_Is_Poped ;
	  zmw_window_update_uppers(action) ;
	  *ZMW_MENU_STATE |= Zmw_Menu_Is_Poped ;
	}
      /*
       * The inner visible menu is modified when the cursor
       * is over a button making it appear.
       */
      if ( zmw_event_in_rectangle_previous() && zmw.button_pressed )
	{
	  ZMW_PRINTF("Register\n") ;
	  zmw_name_register(&global_inner_visible_menu) ;
	  action = Zmw_Menu_Is_Poped ;
	  zmw_window_update_uppers(action) ;
	  *ZMW_MENU_STATE |= Zmw_Menu_Is_Poped ;
	}
    }

  visible = *ZMW_MENU_STATE & ( Zmw_Menu_Is_Poped | Zmw_Menu_Is_Detached ) ;

  continue_recursion = visible
    || ZMW_SIZE_EVENT_IN_CHILDREN
    || ( *ZMW_MENU_STATE & Zmw_Menu_Contains_A_Detached ) ;

  if ( !visible && continue_recursion )
    {
      ZMW_SIZE_DO_NOT_MAP_WINDOW = Zmw_True ;
    }

  ZMW_PRINTF("v=%d c=%d p=%d %s\n"
	     , visible, continue_recursion,zmw_event_in_rectangle_previous()
	     , zmw_name_is(&global_inner_visible_menu) ? "***" : ""
	     ) ;

  return continue_recursion ;
}

Zmw_Boolean zmw_window_is_popped()
{
  return(zmw_window_is_popped_with_detached(NULL) ) ;
}

void zmw_window_unpop_all()
{
  ZMW_PRINTF("Unpop all\n") ;

  if ( zmw_name_registered(&global_inner_visible_menu) )
    {
      ZMW_PRINTF("Unpop all YES %s\n", global_inner_visible_menu.name) ;
      zmw_name_unregister(&global_inner_visible_menu) ;
      zmw_need_repaint() ;
    }
  if ( zmw.debug & Zmw_Debug_Event )
    zmw_printf("Unpop all popup window\n") ;

  zmw_use_window_from_button_press(Zmw_True) ;
}
/*
 * True if the widget is selected (button pressed in the widget
 * but not released
 */
Zmw_Boolean zmw_selected()
{
  return( zmw_name_is(&global_zmw_selected) ) ;
}
Zmw_Boolean zmw_selected_by_its_parents()
{
  return( zmw_name_is_inside(&global_zmw_selected) ) ;
}
/*
 * True if the widget is selected button pressed on itself or an ancestor
 */

Zmw_Boolean zmw_focused_by_its_parents()
{
  return ( ZMW_FOCUS && zmw_name_is_inside(ZMW_FOCUS) ) ;
}
/*
 * Focus remove
 */
void zmw_focus_remove()
{
	zmw_name_unregister(ZMW_FOCUS) ;
}
/*
 * Remove the current event
 */
void zmw_event_remove()
{	
  if ( zmw.debug & Zmw_Debug_Event )
    zmw_printf("**** EVENT **** REMOVE of %s\n", zmw_name_full) ;
  
  zmw.event_removed = Zmw_True ;
}
/*
 * If your zmw can have the focus, call this function
 */
void zmw_focusable()
{
  if ( zmw_event_to_process() )
    {
      if ( zmw.event->type == GDK_BUTTON_PRESS
	   || zmw.event->type == GDK_BUTTON_RELEASE )
	zmw_name_register(ZMW_FOCUS) ;

      if ( zmw.event->type == GDK_BUTTON_PRESS )
	zmw_need_repaint() ;
      // zmw_event_remove() ; // Removed the 2004-1-4
     }
  if ( ZMW_FOCUS && zmw_name_is(ZMW_FOCUS) )
    {
      ZMW_SIZE_FOCUSED = Zmw_True ;
    }
}
/*
 * For copy/paste
 */
Zmw_Boolean zmw_selection_have()
{
  return ( zmw_name_is(&global_zmw_selection) ) ;
}

void zmw_selection_take()
{
  zmw_name_register(&global_zmw_selection) ;
}

void zmw_selection_clear()
{
  zmw_name_unregister(&global_zmw_selection) ;
}


/*
 * If your zmw is activable, call this function
 */

void zmw_event_button_release()
{
  zmw_name_unregister(&global_zmw_selected) ;
}

void zmw_activable()
{
  if ( 0 && zmw.event && zmw.event->type == GDK_BUTTON_PRESS )
      zmw_event_dump() ;

  if (  zmw_event_to_process() && zmw.event->type == GDK_BUTTON_PRESS )
    {
      if ( global_zmw_selected.name )
	return ;
      /*
      if ( zmw_name_registered(&global_zmw_selected) )
	fprintf(stderr, "Button (%s) press 2 times without releasing\n"
		, zmw_name_full) ;
      */
      zmw_name_register(&global_zmw_selected) ;
    }
  else
  {
    
    if( 0 && ZMW_SUBACTION == Zmw_Input_Event &&zmw.event->type == GDK_BUTTON_RELEASE )
      zmw_printf("%s\n%s\n event_in_masked=%d sp=%d s=%d event_in=%d name_is_inside (%d) %s\n"
		 , zmw_name_full
		 , global_zmw_selected.name?global_zmw_selected.name:"???"
		 , ZMW_EVENT_IN_MASKED
		 , zmw_selected_by_its_parents()
		 , zmw_selected()
		 , ZMW_SIZE_EVENT_IN_RECTANGLE
		 , zmw_name_is_inside(&global_inner_visible_menu)
		 , global_inner_visible_menu.name
		 );
    if ( 
	 ZMW_SUBACTION == Zmw_Input_Event
	 && !zmw.event_removed
	 && !ZMW_EVENT_IN_MASKED
	 && zmw.event
	 && zmw.event->type == GDK_BUTTON_RELEASE
	 && ( zmw_name_is_inside(&global_inner_visible_menu)
	      || zmw_selected() )
	 )
      {
	if ( ZMW_SIZE_EVENT_IN_RECTANGLE )
	  {
	    ZMW_SIZE_ACTIVATED = Zmw_True ;
	    zmw_need_repaint() ;
	  }
      }
  }
}
/*
 *
 */

Zmw_Boolean zmw_accelerator(GdkModifierType state, int character)
{
  if (
      ZMW_ACTION == zmw_action_dispatch_accelerator
      && !zmw.event_removed
      && zmw.event->type == GDK_KEY_PRESS
      && toupper(zmw.event->key.keyval) == toupper(character)
      && zmw.event->key.state == state
      )
    {
      zmw_need_repaint() ;
      zmw_event_remove() ;
       return(1) ;
    }
  return(0) ;
}


/*
 * This function should be used as a ZMW() parameter.
 * "tip_displayed" is the name of the "zmw_if(zmw_tip_visible())"
 *
 * If there is two tips on the same widget the second one
 * must search what the first one has done.
 */
Zmw_Boolean zmw_tip_visible()
{
  int v, i ;

  v = 0 ;

  ZMW_SIZE_PASS_THROUGH = Zmw_True ;
  
  /* When searching, the tip_displayed is updated
   * In the other cases, test if the tip is displayed
   */
  
   if ( zmw.zmw_table[1].i.action == zmw_action_search )
    {
    	/* Set tip on the first inner most widget containing "found" */
      if ( ! zmw_name_registered(&zmw.tip_displayed) )
	{
	  if ( zmw_event_in_rectangle_previous() )
	    {
	      zmw_name_register(& zmw.tip_displayed) ;
	    }
	}
    }
    else
    {
      if ( zmw.tips_yet_displayed && zmw_name_registered(&zmw.tip_displayed) )
	{
	  if ( zmw_name_is(&zmw.tip_displayed) )
	    {
	      v = ZMW_SIZE_TIP_VISIBLE = Zmw_True ;
	    }
	  else
	    {
	      /* Search another tip before */
	      for(i = ZMW_CHILD_NUMBER - 1 ;
		  i>0 && zMw[-1].u.children[i].pass_through ; i--)
		{
		  if ( zMw[-1].u.children[i].tip_visible )
		    {
		      v = Zmw_True ;
		      break ;
		    }
		}
	    }
	}
    }

  return(v) ;
}



/*
 *
 */

#define C(X) case X: zmw_text(#X)

void zmw_event_debug_window()
{
  static int display_zmw = 0 ;
  char *found ;
  char buf[9999] ;
  
  found = zmw_name_registered(&zmw.found) ;

  ZMW(zmw_box_vertical())
    {
      zmw_toggle_int_with_label(&display_zmw, "zmw") ;
      if ( display_zmw )
	{
	  sprintf(buf, "event=%p", zmw.event) ;
	  zmw_text(buf) ;	  

	  if ( zmw.event )
	    switch(zmw.event->type)
	      {
		C(GDK_KEY_PRESS) ;
		  C(GDK_KEY_RELEASE) ;
		  sprintf(buf, "state=%d code=%x string=\"%s\""
			  , zmw.event->key.state
			  , zmw.event->key.keyval
			  , zmw.event->key.string
			  ) ;
		  zmw_text(buf) ;
		  break ;

		  C(GDK_MOTION_NOTIFY) ; break ;
		  C(GDK_EXPOSE) ; break ;
		  C(GDK_2BUTTON_PRESS) ; break ;
		  C(GDK_3BUTTON_PRESS) ; break ;
		  C(GDK_BUTTON_PRESS) ; break ;
		  C(GDK_BUTTON_RELEASE) ; break ;

		default:
		  sprintf(buf, "type=%d", zmw.event->type) ;
		  zmw_text(buf) ;
		  
		}

	  sprintf(buf, "x=%d y=%d "
		  , zmw.x
		  , zmw.y
		  ) ;
	  zmw_text(buf) ;

	  if ( zmw_name_registered(&zmw.tip_displayed) )
	    {
	      sprintf(buf, "TIP=%s",  zmw_name_registered(&zmw.tip_displayed));
	      zmw_text(buf) ;
	    }
	  sprintf(buf, "inner_menu = %s", global_inner_visible_menu.name) ;
	  zmw_text(buf) ;
	  sprintf(buf, "zmw.tips_yet_displayed = %d", zmw.tips_yet_displayed) ;
	  zmw_text(buf) ;
	  sprintf(buf, "zmw.still_yet_displayed = %d",zmw.still_yet_displayed);
	  zmw_text(buf) ;

	  sprintf(buf,"SELECTED=%s",zmw_name_registered(&global_zmw_selected));
	  zmw_text(buf) ;
	  sprintf(buf, "FOUND=%s", found) ;
	  zmw_text(buf) ;
	}
    }
  zmw_border_embossed_in_draw() ;
}

