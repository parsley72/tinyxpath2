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

#include <stdio.h>
#include <string.h>
#include <assert.h>

#include "tinyxml.h"
#include "tinystr.h"
#include "tinyxpstream.h"

#include "xmlutil.h"

#ifdef TEST_SYNTAX
   static void v_decode (const char * cp_in)
   {
      xpath_stream xs (cp_in);

      printf ("Decoding --> %s <--\n", cp_in);
      xs . v_evaluate ();
      printf ("(end)\n");
   }

   static void v_test_syntax ()
   {
      FILE * Fp_in;
      char ca_s [202];

      Fp_in = fopen ("xpath_in.txt", "rt");
      if (! Fp_in)
         return;
      fgets (ca_s, 200, Fp_in);
      while (! feof (Fp_in))
      {
         ca_s [strlen (ca_s) - 1] = 0;
         v_decode (ca_s);
         fgets (ca_s, 200, Fp_in);
      }
      fclose (Fp_in);
   }
#endif

class test_fail {};

enum WORK_ITEM_ENUM {WORK_NONE, WORK_STRING, WORK_QNAME, WORK_AXIS, WORK_NAME_TEST, WORK_NODE_TEST, WORK_STEP,
      WORK_EXPR};

/// Working stack virtual item
class work_item
{
protected :
   /// class : must be one of the WORK_ITEM_ENUM
   unsigned u_class;
   /// pointer to next item on the stack
   work_item * wip_next;
public :
   /// constructor
   work_item () {u_class = WORK_NONE; wip_next = NULL;}
   /// virtual destructor
   virtual ~ work_item () {}
   /// Get the class of this work_item
   unsigned u_get_class () {return u_class;}
   /// Set the next work_item
   void v_set_next (work_item * wip_in_next)
   {
      wip_next = wip_in_next;
   }
   /// Get the next work_item
   work_item * wip_get_next ()
   {
      return wip_next;
   }
   /// Get the useful value of an item. By default, this doesn't exist
   virtual const char * cp_get_value () { assert (false); return ""; }
   /// Prints an item's internal content to stdout
   virtual void v_dump (int = 0) {}
   /// Get an expression's value
   virtual int i_get_expr_value () { assert (false); return 0;}
   /// Apply an XPath predicate
   virtual void v_apply (TiXmlNode * , const char * , long & ) { assert (false); }
} ;

/// Specialized work_item for strings
class work_string : public work_item
{
   TIXML_STRING value;
public :
   work_string (const char * cp_in)
   {
      value = cp_in;
      u_class = WORK_STRING;
   }
   work_string (const work_string & )
   {
      assert (false);
      printf ("\n");
   }
   virtual const char * cp_get_value ()
   {
      return value . c_str ();
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("string \"%s\"\n", cp_get_value ());
   }
} ;

/// Specialized work_item for axis
class work_axis : public work_item
{
   TIXML_STRING value;
   bool o_at;
   bool o_abbrev;
public :
   work_axis (bool o_abbreviated, bool o_in_at, const char * cp_in = "") : work_item ()
   {
      o_at = o_in_at;
      o_abbrev = o_abbreviated;
      value = cp_in;
      u_class = WORK_AXIS;
   }
   work_axis (const work_axis & copy) : work_item ()
   {
      o_at = copy . o_at;
      o_abbrev = copy . o_abbrev;
      value = copy . value;
      u_class = WORK_AXIS;
   }
   virtual const char * cp_get_value ()
   {  
      if (o_abbrev)
         if (o_at)
            return "@";
         else
            return "";
      return value . c_str ();
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("axis \"%s\"\n", cp_get_value ());
   }
	bool o_is_at ()
	{
		return o_at;
	}
} ;

/// Specialized work_item for expressions
class work_expr : public work_item
{
   int i_value;
   TIXML_STRING S_value;
   unsigned u_cat;
public :
   work_expr (unsigned u_in_cat, int i_in_value, const char * cp_in_func = NULL) : work_item ()
   {
      u_cat = u_in_cat;
      switch (u_cat)
      {
         case 0:
            i_value = i_in_value;   
            char ca_s [20];
            sprintf (ca_s, "%d", i_value);
            S_value = ca_s;
            break;
         case 1 :
            S_value = cp_in_func;
            i_value = 0;
            break;
      }
      u_class = WORK_EXPR;
   }
   work_expr (const work_expr & copy) : work_item ()
   {
      u_cat = copy . u_cat;
      i_value = copy . i_value;
      S_value = copy . S_value;
      u_class = WORK_EXPR;
   }
   virtual const char * cp_get_value ()
   {
      return S_value . c_str ();
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("expr (%d)\n", i_value);
   }
   virtual int i_get_expr_value () 
   {
      if (u_cat)
         assert (false);
      return i_value;
   }
   virtual void v_apply (TiXmlNode * XNp_target, const char * cp_name, long & l_marker)
   {
      switch (u_cat)
      {
         case 0 :
            v_mark_children_name_order (XNp_target, cp_name, i_get_expr_value (), 
               l_marker, l_marker + 1); 
            l_marker += 1;
            break;
         case 1 :
            if (S_value == "last")
            {
               v_mark_children_name_last (XNp_target, cp_name, l_marker, l_marker + 1); 
               l_marker += 1;
            }
            else
               assert (false);
            break;
         default :
            assert (false);
            break;
      }
   }
} ;

/// Specialized work_item for NameTest
class work_name_test : public work_item
{
   TIXML_STRING S_value, S_total;
   unsigned u_type;
public :
   work_name_test (unsigned u_in_type, const char * cp_in = "") : work_item ()
   {
      u_type = u_in_type;
      S_value = cp_in;
      u_class = WORK_NAME_TEST;
      if (u_type == 1)
      {
         S_total = cp_in;
         S_total += ":*";
      }
   }
   work_name_test (const work_name_test & )
   {
      assert (false);
   }
   virtual const char * cp_get_value ()
   {  
      switch (u_type)
      {
         case 0 : return "*";
         case 1 : return S_total . c_str ();
         case 2 : return S_value . c_str ();
      }
      return "????";
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("name_test \"%s\"\n", cp_get_value ());
   }
} ;

/// Specialized work_item for NodeTest
class work_node_test : public work_item
{
   TIXML_STRING S_value, S_total;
   unsigned u_type, u_lex, u_nb_predicate;
   work_item ** wipp_list;
public :
   work_node_test (unsigned u_in_type, unsigned u_in_lex, const char * cp_in = "") : work_item ()
   {
      u_type = u_in_type;
      u_lex = u_in_lex;
      S_value = cp_in;
      u_class = WORK_NODE_TEST;
      if (u_type == 1)
      {
         S_total = cp_in;
         S_total += ":*";
      }
      u_nb_predicate = 0;
      wipp_list = NULL;
   }
   work_node_test (const work_node_test & copy) : work_item ()
   {
      u_type = copy . u_type;
      u_lex = copy . u_lex;
      S_value = copy . S_value;
      u_class = WORK_NODE_TEST;
      v_set_predicate_list (copy . u_nb_predicate, copy . wipp_list);
      if (u_type == 1)
      {
         S_total = S_value;
         S_total += ":*";
      }
   }
   virtual ~ work_node_test ()
   {
      unsigned u_pre;

      if (wipp_list)
      {
         for (u_pre = 0; u_pre < u_nb_predicate; u_pre++)
            delete wipp_list [u_pre];
         delete [] wipp_list;
      }
   }
   void v_set_predicate_list (unsigned u_in_nb_predicate, work_item ** wipp_in_list)
   {
      unsigned u_predicate;

      u_nb_predicate = u_in_nb_predicate;
      if (u_nb_predicate)
      {
         wipp_list = new work_item * [u_nb_predicate];
         for (u_predicate = 0; u_predicate < u_nb_predicate; u_predicate++)
         {
            switch (wipp_in_list [u_predicate] -> u_get_class ())
            {
               case WORK_EXPR :
                  wipp_list [u_predicate] = new 
                        work_expr (* (work_expr *) wipp_in_list [u_predicate]);
                  break;
               default :
                  assert (false);
            }
         }
      }
      else
         wipp_list = NULL;
   }
   virtual const char * cp_get_value ()
   {  
      TIXML_STRING S_ret;
      switch (u_type)
      {
         case 0 : return cp_disp_class_lex ((lexico) u_lex);
         case 1 : return "processing-instruction()";
         case 2 : 
            S_ret = "processing-instruction(";
            S_ret += S_value;
            S_ret += ")";
            return S_ret . c_str ();
         case 3 :
            return S_value . c_str ();
      }
      return "????";
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("node_test \"%s\"\n", cp_get_value ());
   }

   // Look for all first-level items that have the S_value name, and mark them with
   // the attribute xpath-selected=i_id
   void v_find_node (TiXmlNode * XNp_target, long l_id)
   {
      if (S_value == "*")
         v_mark_first_level (XNp_target, l_id);      
      else
         v_mark_first_level_name (XNp_target, S_value . c_str (), l_id);      
   }

   // Find all subtree and mark all elements with xpath-selected = i_id
   void v_find_all (TiXmlDocument * XDp_target, long l_id)
   {
      v_mark_all_children (XDp_target, l_id);
   }

   // Mark all children of a selection with next level
   void v_find_child (TiXmlNode * XNp_target, long & l_id)
   {
      if (S_value == "*")
      {
         v_mark_children_inside (XNp_target, l_id, l_id + 1);
         l_id += 1;
      }
      else
      {
         switch (u_nb_predicate)
         {
            case 0 :
               v_mark_children_name (XNp_target, S_value . c_str (), l_id, l_id + 1); 
               l_id += 1;
               break;
            case 1 :
               wipp_list [0] -> v_apply (XNp_target, S_value . c_str (), l_id);
               break;
            default :
               assert (false);  // don't know how to process more than 1 predicate
               break;
         }
      }
   }

   // Mark all children of a selection with next level
   void v_find_child_attrib (TiXmlNode * XNp_target, long & l_id)
   {
		XNp_target -> Print (stdout, 0);
      v_mark_children_attrib (XNp_target, S_value . c_str (), l_id, l_id + 1); 
		XNp_target -> Print (stdout, 0);
      l_id += 1;
   }
} ;

/// Specialized work_item for QName
class work_qname : public work_item
{
   TIXML_STRING S_local, S_prefix, S_total;
public :
   work_qname (const char * cp_in_prefix, const char * cp_in_local) : work_item ()
   {
      if (cp_in_prefix)
      {
         S_prefix = cp_in_prefix;
         S_local = cp_in_local;
         S_total = S_prefix;
         S_total += '.';
         S_total += S_local;
      }
      else
      {
         S_prefix = "";
         S_local = cp_in_local;
         S_total = cp_in_local;
      }
      u_class = WORK_QNAME;
   }
   work_qname (const work_qname & )
   {
      assert (false);
   }
   virtual const char * cp_get_value ()
   {
      return S_total . c_str ();
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("qname \"%s\"\n", cp_get_value ());
   }
} ;

/// Specialized work_item for Step
class work_step : public work_item
{
   work_axis * wp_axis;
   work_node_test * wp_node_test;
   work_step * wp_next_step;
public :
   work_step (const work_axis * wp_in_axis, const work_node_test * wp_in_node_test) : work_item ()
   {
      u_class = WORK_STEP;
      wp_axis = new work_axis (* wp_in_axis);
      wp_node_test = new work_node_test (* wp_in_node_test);
      wp_next_step = NULL;
   }
   work_step (const work_step & copy) : work_item ()
   {
      u_class = WORK_STEP;
      wp_axis = new work_axis (* (copy . wp_axis));
      wp_node_test = new work_node_test (* (copy . wp_node_test));
      if (copy . wp_next_step)
         wp_next_step = new work_step (* (copy . wp_next_step));
      else
         wp_next_step = NULL;
   }

   ~ work_step ()
   {
      assert (wp_axis);
      delete wp_axis;
      assert (wp_node_test);
      delete wp_node_test;
      if (wp_next_step)
         delete wp_next_step;
   }
   virtual const char * cp_get_value ()
   {
      return " (pointers to axis and node_test)";
   }
   virtual void v_dump (int i_level)
   {
		v_levelize (i_level);
      printf ("step %s\n", cp_get_value ());
		v_levelize (i_level);
      printf (" |-- axis : "); 
      wp_axis -> v_dump (i_level + 1);
		v_levelize (i_level);
      printf (" |-- node_test : "); 
      wp_node_test -> v_dump (i_level + 1);
      if (wp_next_step)
      {
			v_levelize (i_level);
         printf (" |-- next step : ");
         wp_next_step -> v_dump (i_level + 1);
      }
   }
   void v_step_it (TiXmlNode * XNp_context, long & l_mark_level)
   {
      wp_node_test -> v_find_node (XNp_context, l_mark_level);
      if (wp_next_step)
         wp_next_step -> v_step_child (XNp_context, l_mark_level);
   }
   void v_step_all (TiXmlDocument * XDp_target, long & l_mark_level)
   {
      wp_node_test -> v_find_all (XDp_target, l_mark_level);
      v_step_child (XDp_target, l_mark_level);
   }
   void v_step_child (TiXmlNode * XNp_context, long & l_mark_level)
   {
		if (wp_axis -> o_is_at ())
		{
			wp_node_test -> v_find_child_attrib (XNp_context, l_mark_level);
			if (wp_next_step)
				wp_next_step -> v_step_child (XNp_context, l_mark_level);
		}
		else
		{
			wp_node_test -> v_find_child (XNp_context, l_mark_level);
			if (wp_next_step)
				wp_next_step -> v_step_child (XNp_context, l_mark_level);
		}
   }
   void v_set_next_step (const work_step * wp_in_next)
   {
      work_step * wp_next;
      wp_next = new work_step (* wp_in_next);
      wp_next_step = wp_next;
   }
} ;


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

/// Top-level object that applies an XPath expression to a source XML tree
class xpath_from_source
{
protected :
   xpath_stream * xsp_stream;
   TiXmlNode * XNp_source;
   TiXmlDocument * XDp_target;
public :
   xpath_from_source (TiXmlNode * XNp_source_tree, const char * cp_in_expr)
   {
      xsp_stream = new xpath_stream (cp_in_expr);
      XNp_source = XNp_source_tree;
      XDp_target = new TiXmlDocument;
   }
   ~ xpath_from_source ()
   {
      assert (XDp_target);
      delete XDp_target;
      assert (xsp_stream);
      delete xsp_stream;
   }
   void v_apply_rule (
		action_list * alp_in, 
		const char * cp_test_name, 
		FILE * Fp_html)
   {
      const action_item * aip_current;
      work_stack * wsp_stack;
      TIXML_STRING S_name, S_name_2;
      work_step * wp_step, * wp_step_2;
      work_node_test * wp_node_test;
      work_axis * wp_axis;
      long l_mark_level;
      TiXmlElement * XEp_root;
      unsigned u_nb_predicate, u_predicate;
      work_item ** wipp_list;

      wsp_stack = new work_stack;
      XDp_target -> Parse ("<?xml version=\"1.0\"?><xpath:root/>");

      XEp_root = XDp_target -> FirstChildElement ();
      assert (XEp_root);

      v_clone_children (XNp_source, XEp_root);
      v_mark_all_children (XDp_target, 1);

      XDp_target -> Print (stdout);

      l_mark_level = 2;
      while (alp_in -> o_is_valid ())
      {
         aip_current = alp_in -> aip_get_current ();
         alp_in -> v_skip ();
         switch (aip_current -> u_get_ref ())
         {
            case xpath_absolute_location_path :
               // [2]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                  case 1 :
                     wp_step = (work_step *) wsp_stack -> wip_top ();
                     wp_step -> v_step_it (XEp_root, l_mark_level);
                     wsp_stack -> v_pop ();
                     break;
                  case 2 :
                     printf ("[2]  absolute already processed\n");
                     break;
               }
               break;

            case xpath_relative_location_path :
               // [3]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     printf ("[3]   RelativeLocationPath / Step\n");
                     wp_step = (work_step *) wsp_stack -> wip_top ();
                     wp_step_2 = (work_step *) wsp_stack -> wip_top (1);
                     wp_step_2 -> v_set_next_step (wp_step);
                     wsp_stack -> v_pop ();
                     break;
                  case 1 :
                     printf ("[3]   RelativeLocationPath // Step\n");
                     break;
                  case 2 :
                     printf ("[3]   RelativeLocationPath is simple\n");
                     break;
               }
               break;
                              
            case xpath_step :
               // [4]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     printf ("[4]   Step is an abbreviated one (. or ..)\n");
                     break;
                  case 1 :
                     u_nb_predicate = aip_current -> u_get_var ();
                     printf ("[4]   Step is \"AxisSpecifier NodeTest Predicate x %d\"\n", 
                           u_nb_predicate);
                     wipp_list = NULL;
                     if (u_nb_predicate)
                     {
                        // predicate : don't know what to do with them yet
                        printf ("Predicates !\n");
                        wipp_list = new work_item * [u_nb_predicate];
                        for (u_predicate = 0; u_predicate < u_nb_predicate; u_predicate++)
                           wipp_list [u_predicate] = wsp_stack -> wip_top (u_predicate);
                     }
                     wp_node_test = (work_node_test *) wsp_stack -> wip_top (u_nb_predicate);
                     if (u_nb_predicate)
                     {
                        wp_node_test -> v_set_predicate_list (u_nb_predicate, wipp_list);
                        delete [] wipp_list;
                     }
                     wp_axis = (work_axis *) wsp_stack -> wip_top (u_nb_predicate + 1);
                     work_item * wip_new = new work_step (wp_axis, wp_node_test);
                     wsp_stack -> v_pop (u_nb_predicate + 2);
                     wsp_stack -> v_push (wip_new);
                     break;
               }
               break;
            case xpath_axis_specifier :
               // [5]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     printf ("[5]  Axis specifier with '@'\n");
                     wsp_stack -> v_dump ();
                     wsp_stack -> v_push (new work_axis (false, true, NULL));
                     break;
                  case 1 :
                     printf ("[5]  Axis specifier is AxisName ::\n");
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_axis (false, false, S_name . c_str ()));
                     break;
                  case 2 :
                     printf ("[5]  Axis specifier with no '@'\n");
                     break;
               }
               break;            
            case xpath_axis_name :
               // [6]
               printf ("[6] Axis name is %s\n", aip_current -> cp_get_label ());
               wsp_stack -> v_push (new work_string (aip_current -> cp_get_label ()));
               break;
            case xpath_node_test :
               // [7]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     printf ("[7] Node type is simple (%d)\n", aip_current -> u_get_var ());
                     wsp_stack -> v_push (new work_node_test (0, aip_current -> u_get_var ()));
                     break;
                  case 1 :
                     printf ("[7] Node type is processing-instruction ()");
                     wsp_stack -> v_push (new work_node_test (1, lex_processing_instruction));
                     break;
                  case 2 :
                     printf ("[7] Node type is processing-instruction ()");
                     wsp_stack -> v_push (new work_node_test (2, lex_processing_instruction, aip_current -> cp_get_label ()));
                     break;
                  case 3 :
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     printf ("[7] Node type is a name test (%s)\n", S_name . c_str ()); 
                     wsp_stack -> v_push (new work_node_test (3, 0, S_name . c_str ()));
                     break;
               }
               break;

            case xpath_abbreviated_absolute_location_path :
               // [10]
               wp_step = (work_step *) wsp_stack -> wip_top ();
					XDp_target -> Print (stdout, 0);
					wsp_stack -> v_dump ();
               wp_step -> v_step_all (XDp_target, l_mark_level);
               wsp_stack -> v_pop ();
               break;

            case xpath_abbreviated_axis_specifier :
               // [13]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     wsp_stack -> v_push (new work_axis (true, true));
                     break;
                  case 1 :
                     wsp_stack -> v_push (new work_axis (true, false));
                     break;
               }
               break;

            case xpath_primary_expr :
               switch  (aip_current -> u_get_sub ())
               {
                  case 0 :
                  case 1 :
                  case 2 :
                  case 3 :
                     // Houston, we have a number
                     wsp_stack -> v_push (new work_expr (0, atoi (aip_current -> cp_get_label ())));
                     break;
                  case 4 :
                     // Houston : it's getting worse : we have a function call
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_expr (1, 0, S_name . c_str ()));
                     break;
               }
               break;

            case xpath_name_test :
               // [37]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     // '*' 
                     wsp_stack -> v_push (new work_name_test (0));
                     break;
                  case 1 :
                     // NCName ':' '*' 
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_name_test (1, S_name . c_str ()));
                     break;
                  case 2 :
                     // QName 
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_name_test (2, S_name . c_str ()));
                     break;
               }
               break;

            case xpath_xml_q_name :
               // [206]
               switch (aip_current -> u_get_sub ())
               {
                  case 0 :
                     printf ("[206]   QName has a prefix\n");
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     S_name_2 = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_qname (S_name_2 . c_str (), S_name . c_str ()));
                     break;
                  case 1 :
                     printf ("[206]   QName has no prefix\n");
                     S_name = wsp_stack -> cp_get_top_value ();
                     wsp_stack -> v_pop ();
                     wsp_stack -> v_push (new work_qname (NULL, S_name . c_str ()));
                     break;
               }
               break;
            case xpath_xml_prefix :
               // [207]
               printf ("[207]   Prefix is %s\n", aip_current -> cp_get_label ());
               wsp_stack -> v_push (new work_string (aip_current -> cp_get_label ()));
               break;
            case xpath_xml_local_part :
               // [208]
               printf ("[207]   LocalPart is %s\n", aip_current -> cp_get_label ());
               wsp_stack -> v_push (new work_string (aip_current -> cp_get_label ()));
               break;
         }
      }

      XDp_target -> Print (stdout);

      v_retain_attrib_tree (XDp_target, l_mark_level);

      XDp_target -> Print (stdout);

	   if (Fp_html)
		{
			fprintf (Fp_html, "<h1>%s</h1>\n", cp_test_name);
			fprintf (Fp_html, "<table border=1><tr><th colspan=\"2\">XPath expression : %s</th></tr>\n", xsp_stream -> cp_get_expr ());
			fprintf (Fp_html, "<tr><th>Input</th><th>Output</th></tr>\n");
			fprintf (Fp_html, "<tr><td valign=\"top\">");
			fprintf (Fp_html, "<p>\n");
			v_out_html (Fp_html, XNp_source, 0);
			fprintf (Fp_html, "</p></td>\n");
			fprintf (Fp_html, "<td valign=\"top\"><p>\n");
			v_out_html (Fp_html, XDp_target -> FirstChildElement (), 0);
			fprintf (Fp_html, "</p></td></tr></table>\n");
		}
      delete wsp_stack;
   }
   void v_apply_xpath (const char * cp_test_name, FILE * Fp_html_out)
   {
      xsp_stream -> v_evaluate ();
      v_apply_rule (xsp_stream -> alp_get_action_list (), cp_test_name, Fp_html_out);
   }
} ;

static void v_apply_xml (TiXmlDocument * XDp_doc)
{
   TiXmlElement * XEp_source, * XEp_test;
   xpath_from_source * xfsp_engine;
   const char * cp_test_name, * cp_expr;
	FILE * Fp_html;

   try
   {
		Fp_html = fopen ("res.html", "wt");
	   if (Fp_html)
			fprintf (Fp_html, "<html><head><title>TinyXPath results</title>"  \
						"<style type=\"text/css\">"  \
						" P {font-family:Courier}"   \
						" B {color:red}"  \
						"</style>\n</head><body>\n");
      XEp_test = XDp_doc -> FirstChildElement ();
      if (! XEp_test)
         throw test_fail ();
      XEp_source = XEp_test -> FirstChildElement ("source");
      while (XEp_source)
      {
         cp_test_name = XEp_source -> Attribute ("name");
         if (! cp_test_name)
            throw test_fail ();
         cp_expr = XEp_source -> Attribute ("xpath_expr");
         if (! cp_expr)
            throw test_fail ();
         printf ("\nXPath expr --> %s <--\n\n", cp_expr);
         xfsp_engine = new xpath_from_source (XEp_source, cp_expr);
         xfsp_engine -> v_apply_xpath (cp_test_name, Fp_html);
         delete xfsp_engine;
         XEp_source = XEp_source -> NextSiblingElement ("source");
      }
	   if (Fp_html)
		{
			fprintf (Fp_html, "</body></html>\n");	
		   fclose (Fp_html);
		}
   }
   catch (test_fail)
   {
      printf ("test fail !\n");
   }
}

static void v_apply_1 (const char * cp_in_file_name)
{
   TiXmlDocument * XDp_doc;

   XDp_doc = new TiXmlDocument (cp_in_file_name);
   if (! XDp_doc -> LoadFile ())
      printf ("Can't load %s file !\n", cp_in_file_name);
   else
      v_apply_xml (XDp_doc);
   delete XDp_doc;
}

static void v_apply ()
{
   v_apply_1 ("basic_in.xml");
}

void main ()
{
   // v_generate_ascii_htm ();
   #ifdef TEST_SYNTAX
     v_test_syntax ();
   #endif
   v_apply ();
}