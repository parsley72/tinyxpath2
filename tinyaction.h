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
#ifndef __ACTIONSTACK_H
#define __ACTIONSTACK_H

#include <string.h>

class action_item
{
protected :
   unsigned u_ref, u_sub, u_var;
   char * cp_label;
   action_item * aip_next;
public :
   action_item (unsigned u_in_ref, unsigned u_in_sub, unsigned u_in_var, const char * cp_in_lab)
   {
      cp_label = new char [strlen (cp_in_lab) + 1];
      strcpy (cp_label, cp_in_lab);
      u_ref = u_in_ref;
      u_sub = u_in_sub;
      u_var = u_in_var;
      aip_next = NULL;
   }
   action_item ()
   {
      cp_label = NULL;
      aip_next = NULL;
   }
   operator = (const action_item & copy)
   {
      if (cp_label)
         delete [] cp_label;
      cp_label = new char [strlen (copy . cp_label) + 1];
      strcpy (cp_label, copy . cp_label);
      u_ref = copy . u_ref;
      u_sub = copy . u_sub;
      u_var = copy . u_var;
      aip_next = NULL;
   }
   ~ action_item ()
   {
      if (cp_label)
         delete [] cp_label;
   }
   void v_cleanup ()
   {
      if (aip_next)
      {
         aip_next -> v_cleanup ();
         delete aip_next;
         aip_next = NULL;
      }
   }
   void v_set_next (action_item * aip_in_next)
   {
      aip_next = aip_in_next;
   }
   action_item * aip_get_next ()
   {
      return aip_next;
   }
   unsigned u_get_ref () const
   {
      return u_ref;
   }
   unsigned u_get_sub () const
   {
      return u_sub;
   }
   unsigned u_get_var () const
   {
      return u_var;
   }
   const char * cp_get_label () const
   {
      return cp_label;
   }
} ;

class action_list
{
protected :
   action_item * aip_first, * aip_last, * aip_current;
public :
   action_list ()
   {
      aip_first = NULL;
      aip_last = NULL;
      aip_current = NULL;
   }
   ~ action_list ()
   {
      v_cleanup ();
   }
   void v_cleanup ()
   {
      if (aip_first)
      {
         aip_first -> v_cleanup ();
         delete aip_first;
         aip_first = NULL;
         aip_last = NULL;
      }
   }
   void v_add (action_item * aip_new)
   {
      if (aip_last)
         aip_last -> v_set_next (aip_new);
      else
      {
         aip_first = aip_new;
         aip_current = aip_new;
      }
      aip_last = aip_new;
   }
   const action_item * aip_get_current ()
   {
      return aip_current;
   }
   void v_skip ()
   {
      aip_current = aip_current -> aip_get_next ();
   }
   bool o_is_valid ()
   {
      return ! (aip_current == NULL);
   }
} ;

#endif
