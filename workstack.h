/*
www.sourceforge.net/projects/tinyxpath
Copyright (c) 2002 Yves Berquin (yvesb@users.sourceforge.net)

This software is provided 'as-is', without any express or implied
warranty. In no event will the authors be held liable for any
damages arising from the use of this software.

Permission is granted to anyone to use this software for any
purpose, including commercial applications, and to alter it and
redistribute it freely, subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must
not claim that you wrote the original software. If you use this
software in a product, an acknowledgment in the product documentation
would be appreciated but is not required.

2. Altered source versions must be plainly marked as such, and
must not be misrepresented as being the original software.

3. This notice may not be removed or altered from any source
distribution.
*/

#ifndef __WORKSTACK_H
#define __WORKSTACK_H

#include "workitem.h"

/// Work stack : list of work_items
class work_stack 
{
   work_item * wip_top_stack;
public :
   work_stack ()
   {
      wip_top_stack = NULL;
   }
   ~ work_stack ()
   {
      while (wip_top ())
         v_pop ();
   }
   void v_push (work_item * wip_new)
   {
      wip_new -> v_set_next (wip_top_stack);
      wip_top_stack = wip_new;
   }
   void v_pop ()
   {
      work_item * wip_temp;
      wip_temp = wip_top_stack;
      wip_top_stack = wip_top_stack -> wip_get_next ();
      delete wip_temp;
   }
   void v_pop (unsigned u_nb)
   {
      unsigned u_pop;
      for (u_pop = 0; u_pop < u_nb; u_pop++)
         v_pop ();
   }
   work_item * wip_top ()
   {
      return wip_top_stack;
   }
   work_item * wip_top (unsigned u_forward)
   {
      work_item * wip_ret;
      unsigned u_loop;

      wip_ret = wip_top_stack;
      for (u_loop = 0; u_loop < u_forward;  u_loop++)
         if (wip_ret)
            wip_ret = wip_ret -> wip_get_next ();
      return wip_ret;
   }
   virtual const char * cp_get_top_value ()
   {
      return wip_top () -> cp_get_value ();
   }
   void v_dump ()
   {
      work_item * wip_cur;

      printf ("[Work Stack dump]\n");
      wip_cur = wip_top ();
      while (wip_cur)
      {
         wip_cur -> v_dump (1);
         wip_cur = wip_cur -> wip_get_next ();
      }
      printf ("[end Work Stack dump]\n");
   }
} ;

#endif
