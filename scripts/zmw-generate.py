#!/usr/bin/python

"""
This program generate automaticaly C files.
This is done by program to be sure that all is coherent.
"""

 
 
import os
import sys

if not os.path.isdir("kernel"):
    print "You are not in the ZMW source root"
    sys.exit(1)


intro = """
/* This file is automaticaly generated by the script zmw-generate.py
 * DO NOT EDIT
*/
"""

c       = open("kernel/zmw_generated.c", "w")
private = open("include/zmw/zmw_generated_private.h", "w")
public  = open("include/zmw/zmw_generated_public.h", "w")
files   = (c, private, public)

for f in files:
    f.write(intro)

c.write("""
#include <zmw/zmw.h>
#include <zmw/zmw_private.h>

Zmw zmw ;

""")

private.write("""
extern Zmw zmw ;
""")


class Definition:
    def __init__(self, name, value, type,
                 take_pointer=False, is_a_table=False, condition=""):
        self.name = name
        self.value = value
        self.type = type
        self.take_pointer = take_pointer
        self.is_a_table = is_a_table
        self.condition = condition

    def generate(self, files, text):
        if self.condition:
            for f in files:
                f.write(text)
            return True
        return False

    def generate_if(self, files=files):
        return self.generate(files, "#if " + self.condition + "\n")
    def generate_else(self, files=files):
        return self.generate(files, "#else\n")
    def generate_endif(self, files=files):
        return self.generate(files, "#endif\n")

    def star(self):
        return ("","*")[self.take_pointer]
    def ampersand(self):
        return ("","&")[self.take_pointer]
    def const(self):
        return ("const ", "")[self.is_a_function() or self.is_a_pointer()]

    def is_a_function(self):
        return self.type.find("(*") >= 0
    def is_a_pointer(self):
        return self.type.find("*") >= 0 and not self.is_a_table
    def is_a_boolean(self):
        return self.type.startswith("Zmw_Boolean")
    def can_get_ptr(self):        
        return not (self.is_a_boolean()
                    or self.take_pointer
                    or self.is_a_table
                    or self.name == "size"
                    )
    def in_stack(self):
        return self.value.startswith("zmw.ptr->")    
    def in_child(self):
        return self.value.startswith("zmw.ptr->u.size")    

def generate_get(d, prefix="", old="", new="", index="void", i="", ptr=""):

    if ptr:
        star = "*"
        ampersand = "&"
    else:
        star = ""
        ampersand = ""
    name = "zmw_" + prefix + d.name + "_get" + ptr
    value = d.ampersand() + d.value.replace(old, new)
    proto = d.type % (d.star() + star +  "%s(" + index + ")")
    cast = "(" + (d.type % d.star()) + ")"
    define = "#define %s(" + i + ") "
            
    d.generate_if()
    private.write((define + "(%s%s)\n")         % (name, ampersand, value))
    public.write("%s ;\n"                       % (proto % name))
    c.write("#undef %s\n"                       % name)
    c.write("%s { return %s%s ; }\n"        % (proto % name, ampersand, value))

    if d.generate_else():
        public.write((define + "(%s%s)NULL\n")        % (name, cast, star))

    d.generate_endif()


def generate_get_ptr(d, prefix="", old="", new="", index="void", i=""):
    if d.can_get_ptr():
        generate_get(d, prefix, old, new, index, i, ptr="_ptr")


def generate_set(d, prefix="", old="", new="", index="", i=""):

    if d.is_a_table:
        return

    if index:
        index += ", "
        i += ", "

    name = "zmw_" + prefix + d.name + "_set"
    parameter = d.const() + "%s" % (d.type % (d.star() + "x"))
    proto = ("void %%s(" + index + "%s)") % parameter
    value = d.value.replace(old, new)
    define = "#define %s(" + i + "X) "
    
    d.generate_if()
    if d.name == "size":
        private.write((define + "(*%s = *%s(X))\n") % (name, value, d.star()))
    else:
        private.write((define + "(%s = %s(X))\n") % (name, value, d.star()))
    public.write("%s ;\n"                        % (proto % name))
    c.write("#undef %s\n"                        % name)
    if d.name == "size":
        c.write("%s { *%s = *%sx ; }\n"     % (proto % name, value, d.star()))
    else:
        c.write("%s { %s = %sx ; }\n"       % (proto % name, value, d.star()))

    if d.generate_else():
        public.write((define + "while(0){}\n")         % name)

    d.generate_endif()


def generate_parent(d):

    if not d.in_stack():
        return

    p = {"prefix":"parent__", "old":"zmw.ptr" ,"new":"(zmw.ptr-1)"}
    
    generate_get    (d, **p)
    generate_get_ptr(d, **p)
    generate_set    (d, **p)

def generate_child(d):

    if not d.in_child():
        return

    p = {"prefix":"child__"
         , "old":"zmw.ptr->u.size"
         , "new":"(&zmw.ptr->u.children[i])"
         , "index": "int i"
         , "i": "i"
         }
    
    generate_get    (d, **p)
    generate_get_ptr(d, **p)
    generate_set    (d, **p)
    


for d in (
    Definition("action"      , "zmw.ptr->i.action"      , "int (*%s)(void)"            ),
    Definition("debug"       , "zmw.ptr->i.debug"       , "int %s"                     ),
    Definition("colors"      , "zmw.ptr->i.colors"      , "int* %s"                 , is_a_table=True),
    Definition("font_family" , "zmw.ptr->i.font.family" , "Zmw_Font_Family %s"         ),
    Definition("font_size"   , "zmw.ptr->i.font.size"   , "Zmw_Font_Size %s"           ),
    Definition("font_weight" , "zmw.ptr->i.font.weight" , "Zmw_Font_Weight %s"         ),
    Definition("font_style"  , "zmw.ptr->i.font.style"  , "Zmw_Font_Style %s"          ),
    Definition("font"        , "zmw.ptr->i.font"        , "Zmw_Font_Description %s" , take_pointer=True),
    Definition("focus_value" , "zmw.ptr->i.focus->value", "void* %s"                   ),
    Definition("focus"       , "zmw.ptr->i.focus"       , "Zmw_Name* %s"               ),
    Definition("border_width", "zmw.ptr->i.border_width", "Zmw_Width %s"               ),
    Definition("focus_width" , "zmw.ptr->i.focus_width" , "Zmw_Width %s"               ),
    Definition("auto_resize" , "zmw.ptr->i.auto_resize" , "Zmw_Boolean %s"             ),
    Definition("sensitive"   , "zmw.ptr->i.sensitive"   , "Zmw_Boolean %s"             ),

    Definition("call_number"       , "zmw.ptr->u.call_number"                    , "int %s"                     ),
    Definition("name"              , "zmw.ptr->u.name"                           , "char* %s"                   ),
    Definition("name_index"        , "zmw.ptr->u.name_index"                     , "char* %s"                   ),
    Definition("name_separator"    , "zmw.ptr->u.name_separator"                 , "int %s"                     ),
    Definition("nb_of_children"    , "zmw.ptr->u.nb_of_children"                 , "int %s"                     ),
    Definition("nb_of_children_max", "zmw.ptr->u.nb_of_children_max"             , "int %s"                     ),
    Definition("children"          , "zmw.ptr->u.children"                       , "Zmw_Child* %s"              ),
    Definition("do_not_execute_pop", "zmw.ptr->u.do_not_execute_pop"             , "Zmw_Boolean %s"             ),
    Definition("external_state"    , "zmw.ptr->u.external_state"                 , "Zmw_External_State %s"      ),
    Definition("asked_width"       , "zmw.ptr->u.asked.width"                    , "Zmw_Size %s"                ),
    Definition("asked_height"      , "zmw.ptr->u.asked.height"                   , "Zmw_Size %s"                ),
    Definition("asked_x"           , "zmw.ptr->u.asked.x"                        , "Zmw_Position %s"            ),
    Definition("asked_y"           , "zmw.ptr->u.asked.y"                        , "Zmw_Position %s"            ),
    Definition("asked"             , "zmw.ptr->u.asked"                          , "Zmw_Rectangle %s"       , take_pointer=True),
    Definition("type"              , "zmw.ptr->u.type"                           , "const char* %s"             ),
    Definition("file"              , "zmw.ptr->u.file"                           , "const char* %s"             ),
    Definition("line"              , "zmw.ptr->u.line"                           , "int %s"                     ),
    Definition("subaction"         , "zmw.ptr->u.subaction"                      , "Zmw_Subaction %s"           ),
    Definition("menu_state"        , "zmw.ptr->u.menu_state"                     , "int* %s"                    ),
    Definition("child_number"      , "zmw.ptr->u.child_number"                   , "int %s"                     ),
    Definition("window"            , "zmw.ptr->u.parent_to_child.window"         , "GdkWindow** %s"             ),
    Definition("gc"                , "zmw.ptr->u.parent_to_child.gc"             , "GdkGC* %s"                  ),
    Definition("clipping_x"        , "zmw.ptr->u.parent_to_child.clipping.x"     , "Zmw_Position %s"            ),
    Definition("clipping_y"        , "zmw.ptr->u.parent_to_child.clipping.y"     , "Zmw_Position %s"            ),
    Definition("clipping_width"    , "zmw.ptr->u.parent_to_child.clipping.width" , "Zmw_Size %s"                ),
    Definition("clipping_height"   , "zmw.ptr->u.parent_to_child.clipping.height", "Zmw_Size %s"                ),
    Definition("clipping"          , "zmw.ptr->u.parent_to_child.clipping"       , "Zmw_Rectangle %s"      , take_pointer=True),
    Definition("parent_to_child"   , "zmw.ptr->u.parent_to_child"                , "Zmw_Parent_To_Child %s", take_pointer=True),

    
    Definition("size"                       , "zmw.ptr->u.size"                      , "Zmw_Child *%s"              ),
    Definition("invisible"                  , "zmw.ptr->u.size->invisible"           , "Zmw_Boolean %s"             ),
    Definition("sensitived"                 , "zmw.ptr->u.size->sensitived"          , "Zmw_Boolean %s"             ),
    Definition("pass_through"               , "zmw.ptr->u.size->pass_through"        , "Zmw_Boolean %s"             ),
    Definition("event_in_rectangle"         , "zmw.ptr->u.size->event_in_rectangle"  , "Zmw_Boolean %s"             ),
    Definition("event_in_children"          , "zmw.ptr->u.size->event_in_children"   , "Zmw_Boolean %s"             ),
    Definition("allocated_x"                , "zmw.ptr->u.size->allocated.x"         , "Zmw_Position %s"            ),
    Definition("allocated_y"                , "zmw.ptr->u.size->allocated.y"         , "Zmw_Position %s"            ),
    Definition("allocated_width"            , "zmw.ptr->u.size->allocated.width"     , "Zmw_Size %s"                ),
    Definition("allocated_height"           , "zmw.ptr->u.size->allocated.height"    , "Zmw_Size %s"                ),
    Definition("allocated"                  , "zmw.ptr->u.size->allocated"           , "Zmw_Rectangle %s"           ,take_pointer=True),
    Definition("required_x"                 , "zmw.ptr->u.size->required.x"          , "Zmw_Position %s"            ),
    Definition("required_y"                 , "zmw.ptr->u.size->required.y"          , "Zmw_Position %s"            ),
    Definition("required_width"             , "zmw.ptr->u.size->required.width"      , "Zmw_Size %s"                ),
    Definition("required_height"            , "zmw.ptr->u.size->required.height"     , "Zmw_Size %s"                ),
    Definition("required"                   , "zmw.ptr->u.size->required"            , "Zmw_Rectangle %s"           ,take_pointer=True),
    Definition("min_x"                      , "zmw.ptr->u.size->min.x"               , "Zmw_Position %s"            ),
    Definition("min_y"                      , "zmw.ptr->u.size->min.y"               , "Zmw_Position %s"            ),
    Definition("min_width"                  , "zmw.ptr->u.size->min.width"           , "Zmw_Size %s"                ),
    Definition("min_height"                 , "zmw.ptr->u.size->min.height"          , "Zmw_Size %s"                ),
    Definition("min"                        , "zmw.ptr->u.size->min"                 , "Zmw_Rectangle %s"           ,take_pointer=True),
    Definition("focused"                    , "zmw.ptr->u.size->focused"             , "Zmw_Boolean %s"             ),
    Definition("activated"                  , "zmw.ptr->u.size->activated"           , "Zmw_Boolean %s"             ),
    Definition("children_activated"         , "zmw.ptr->u.size->children_activated"  , "Zmw_Boolean %s"             ),
    Definition("changed"                    , "zmw.ptr->u.size->changed"             , "Zmw_Boolean %s"             ),
    Definition("tip_visible"                , "zmw.ptr->u.size->tip_visible"         , "Zmw_Boolean %s"             ),
    Definition("hash_key"                   , "zmw.ptr->u.size->hash_key"            , "Zmw_Hash %s"                ),
    Definition("do_not_map_window"          , "zmw.ptr->u.size->do_not_map_window"   , "Zmw_Boolean %s"             ),
    Definition("used_by_parent"             , "zmw.ptr->u.size->used_by_parent"      , "Zmw_Boolean %s"             ),
    Definition("horizontaly_expanded"       , "zmw.ptr->u.size->horizontaly_expanded", "Zmw_Boolean %s"             ),
    Definition("verticaly_expanded"         , "zmw.ptr->u.size->verticaly_expanded"  , "Zmw_Boolean %s"             ),
    Definition("horizontal_expand"   , "zmw.ptr->u.size->current_state.horizontal_expand"   , "Zmw_Boolean %s"             ),
    Definition("vertical_expand"     , "zmw.ptr->u.size->current_state.vertical_expand"     , "Zmw_Boolean %s"             ),
    Definition("padding_width"       , "zmw.ptr->u.size->current_state.padding_width"       , "Zmw_Width %s"               ),
    Definition("horizontal_alignment", "zmw.ptr->u.size->current_state.horizontal_alignment", "Zmw_Alignment %s"           ),
    Definition("vertical_alignment"  , "zmw.ptr->u.size->current_state.vertical_alignment"  , "Zmw_Alignment %s"           ),
    Definition("current_state"       , "zmw.ptr->u.size->current_state"                     , "Zmw_Child_Current_State %s", take_pointer=True),
    Definition("cs_horizontal_expand"          , "zmw.ptr->u.size[1].current_state.horizontal_expand"   , "Zmw_Boolean %s"             ),
    Definition("cs_vertical_expand"            , "zmw.ptr->u.size[1].current_state.vertical_expand"     , "Zmw_Boolean %s"             ),
    Definition("cs_padding_width"              , "zmw.ptr->u.size[1].current_state.padding_width"       , "Zmw_Width %s"               ),
    Definition("cs_horizontal_alignment"       , "zmw.ptr->u.size[1].current_state.horizontal_alignment", "Zmw_Alignment %s"           ),
    Definition("cs_vertical_alignment"         , "zmw.ptr->u.size[1].current_state.vertical_alignment"  , "Zmw_Alignment %s"           ),
    Definition("cs_current_state"              , "zmw.ptr->u.size[1].current_state"                     , "Zmw_Child_Current_State %s", take_pointer=True),
    Definition("state"                      , "zmw.ptr"                                              , "Zmw_State* %s"              ),


    Definition("zmw_full_name"               , "zmw.full_name"                 , "char* %s"        , is_a_table=True),
    Definition("zmw_table"                   , "zmw.table"                     , "Zmw_State* %s"   ),
    Definition("zmw_inside_zmw_parameter"    , "zmw.inside_zmw_parameter"      , "Zmw_Boolean %s"  , condition="ZMW_DEBUG_INSIDE_ZMW_PARAMETER"),
    Definition("zmw_profiling_displayed"     , "zmw.profiling_displayed"       , "Zmw_Boolean %s"  , condition="ZMW_PROFILING"),
    Definition("zmw_event"                   , "zmw.event"                     , "GdkEvent* %s"    ),
    Definition("zmw_x"                       , "zmw.x"                         , "Zmw_Position %s" ),
    Definition("zmw_y"                       , "zmw.y"                         , "Zmw_Position %s" ),
    Definition("zmw_x_root"                  , "zmw.x_root"                    , "Zmw_Position %s" ),
    Definition("zmw_y_root"                  , "zmw.y_root"                    , "Zmw_Position %s" ),
    Definition("zmw_found"                   , "zmw.found"                     , "Zmw_Name %s"     ,take_pointer=True),
    Definition("zmw_widget_to_trace"         , "zmw.widget_to_trace"           , "Zmw_Name %s"     ,take_pointer=True),
    Definition("zmw_windows"                 , "zmw.windows"                   , "GdkWindow **%s"  ),
    Definition("zmw_nb_windows"              , "zmw.nb_windows"                , "int %s"          ),
    Definition("zmw_window"                  , "zmw.window"                    , "GdkWindow *%s"   ),
    Definition("zmw_raised"                  , "zmw.raised"                    , "GdkWindow *%s"   ),
    Definition("zmw_event_removed"           , "zmw.event_removed"             , "Zmw_Boolean %s"  ),
    Definition("zmw_focus_with_cursor_value" , "zmw.focus_with_cursor->value"  , "void *%s"        ),
    Definition("zmw_focus_with_cursor"       , "zmw.focus_with_cursor"         , "Zmw_Name *%s"    ),
    Definition("zmw_focused_x"               , "zmw.focused.x"                 , "Zmw_Position %s" ),
    Definition("zmw_focused_y"               , "zmw.focused.y"                 , "Zmw_Position %s" ),
    Definition("zmw_focused_width"           , "zmw.focused.width"             , "Zmw_Size %s"     ),
    Definition("zmw_focused_height"          , "zmw.focused.height"            , "Zmw_Size %s"     ),
    Definition("zmw_focused"                 , "zmw.focused"                   , "Zmw_Rectangle %s",take_pointer=True),
    Definition("zmw_still_yet_displayed"     , "zmw.still_yet_displayed"       , "Zmw_Boolean %s"  ),
    Definition("zmw_button_pressed"          , "zmw.button_pressed"            , "Zmw_Boolean %s"  ),
    Definition("zmw_key_pressed"             , "zmw.key_pressed"               , "Zmw_Boolean %s"  ),
    Definition("zmw_tip_displayed"           , "zmw.tip_displayed"             , "Zmw_Name %s"     ,take_pointer=True),
    Definition("zmw_tips_yet_displayed"      , "zmw.tips_yet_displayed"        , "Zmw_Boolean %s"  ),
    Definition("zmw_external_do_not_make_init","zmw.external_do_not_make_init" , "Zmw_Boolean %s"  ),
    ):

    generate_get(d)
    generate_get_ptr(d)
    generate_set(d)
    generate_parent(d)
    generate_child(d)

for f in files:
    f.close()

