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

#ifndef __WORKITEM_H
#define __WORKITEM_H

#include "tinyxml.h"
#include "xmlutil.h"
#include "tinyutil.h"

enum WORK_ITEM_ENUM {WORK_NONE, WORK_STRING, WORK_QNAME, WORK_AXIS, WORK_NAME_TEST, WORK_NODE_TEST, WORK_STEP,
      WORK_EXPR, WORK_FUNC};

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
   work_item (WORK_ITEM_ENUM W_in) {u_class = W_in; wip_next = NULL;}
   /// virtual destructor
   virtual ~ work_item () {}
   /// Get the class of this work_item
   unsigned u_get_class () const {return u_class;}
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
	virtual bool o_identity () {return false;}
} ;

/// Specialized work_item for strings
class work_string : public work_item
{
   TIXML_STRING value;
public :
   work_string (const char * cp_in) : work_item (WORK_STRING)
   {
      value = cp_in;
   }
   work_string (const work_string & ) : work_item (WORK_STRING)
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
	virtual bool o_identity () {return u_class == WORK_STRING;}
} ;

/// Specialized work_item for axis
class work_axis : public work_item
{
   TIXML_STRING value;
   bool o_at;
   bool o_abbrev;
public :
   work_axis (bool o_abbreviated, bool o_in_at, const char * cp_in = "") 
			: work_item (WORK_AXIS)
   {
      o_at = o_in_at;
      o_abbrev = o_abbreviated;
      value = cp_in;
   }
   work_axis (const work_axis & copy) : work_item (WORK_AXIS)
   {
      o_at = copy . o_at;
      o_abbrev = copy . o_abbrev;
      value = copy . value;
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
	virtual bool o_identity () {return u_class == WORK_AXIS;}
   virtual void v_apply (TiXmlNode * XNp_target, const char * cp_name, long & l_marker);
} ;

enum {e_work_expr_value, e_work_expr_func};

/// Specialized work_item for expressions
class work_expr : public work_item
{
   int i_value;
   TIXML_STRING S_value;
   unsigned u_cat;
public :
   work_expr (unsigned u_in_cat, int i_in_value, const char * cp_in_func = NULL) 
				: work_item (WORK_EXPR)
   {
      u_cat = u_in_cat;
      switch (u_cat)
      {
         case e_work_expr_value :
            i_value = i_in_value;   
            char ca_s [20];
            sprintf (ca_s, "%d", i_value);
            S_value = ca_s;
            break;
         case e_work_expr_func :
            S_value = cp_in_func;
            i_value = 0;
            break;
      }
   }
   work_expr (const work_expr & copy) : work_item (WORK_EXPR)
   {
      u_cat = copy . u_cat;
      i_value = copy . i_value;
      S_value = copy . S_value;
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
      if (u_cat != e_work_expr_value)
         assert (false);
      return i_value;
   }
   virtual void v_apply (TiXmlNode * XNp_target, const char * cp_name, long & l_marker)
   {
      switch (u_cat)
      {
         case e_work_expr_value :
            v_mark_children_name_order (XNp_target, cp_name, i_get_expr_value (), 
               l_marker, l_marker + 1); 
            l_marker += 1;
            break;
         case e_work_expr_func :
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
	virtual bool o_identity () {return u_class == WORK_EXPR;}
} ;

class work_func : public work_item
{
	TIXML_STRING S_name;
   work_item ** wipp_list;
	unsigned u_nb_arg;
public :
	work_func (unsigned u_in_nb_arg, work_item ** wipp_arg) : work_item (WORK_FUNC)
	{
		v_set_arg_list (u_in_nb_arg, wipp_arg);
	}
	void v_set_arg_list (unsigned u_in_nb_arg, work_item ** wip_arg);
	work_func (work_func & copy) : work_item (WORK_FUNC)
	{
		v_set_arg_list (copy . u_nb_arg, copy . wipp_list);
		S_name = copy . S_name;
	}
	void v_set_func_name (const char * cp_func_name)
	{
		S_name = cp_func_name;
	}
	virtual bool o_identity () {return u_class == WORK_FUNC;}
   virtual void v_dump (int i_level) 
	{
		v_levelize (i_level);
		printf ("work_func (%s)(%d arguments)\n", S_name . c_str (), u_nb_arg);
	}
   virtual void v_apply (TiXmlNode * XNp_target, const char * cp_name, long & l_marker)
	{
		if (S_name == "not")
		{
			assert (u_nb_arg == 1);
			wipp_list [0] -> v_apply (XNp_target, cp_name, l_marker);
			v_mark_not_attrib (XNp_target, l_marker - 1, l_marker, l_marker + 1);
			l_marker++;
		}
		else
		{
			assert (false);
		}
	}
} ;     // work_func

/// Specialized work_item for NameTest
class work_name_test : public work_item
{
   TIXML_STRING S_value, S_total;
   unsigned u_type;
public :
   work_name_test (unsigned u_in_type, const char * cp_in = "") : work_item (WORK_NAME_TEST)
   {
      u_type = u_in_type;
      S_value = cp_in;
      if (u_type == 1)
      {
         S_total = cp_in;
         S_total += ":*";
      }
   }
   work_name_test (const work_name_test & ) : work_item (WORK_NAME_TEST)
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
	virtual bool o_identity () {return u_class == WORK_NAME_TEST;}
} ;

/// Specialized work_item for NodeTest
class work_node_test : public work_item
{
   TIXML_STRING S_value, S_total;
   unsigned u_type, u_lex, u_nb_predicate;
   work_item ** wipp_list;
public :
   work_node_test (unsigned u_in_type, unsigned u_in_lex, const char * cp_in = "") 
			: work_item (WORK_NODE_TEST)
   {
      u_type = u_in_type;
      u_lex = u_in_lex;
      S_value = cp_in;
      if (u_type == 1)
      {
         S_total = cp_in;
         S_total += ":*";
      }
      u_nb_predicate = 0;
      wipp_list = NULL;
   }
   work_node_test (const work_node_test & copy) : work_item (WORK_NODE_TEST)
   {
      u_type = copy . u_type;
      u_lex = copy . u_lex;
      S_value = copy . S_value;
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
   void v_set_predicate_list (unsigned u_in_nb_predicate, work_item ** wipp_in_list);
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
   void v_find_all (TiXmlNode * XNp_target, long l_id)
   {
      v_mark_all_children (XNp_target, l_id);
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
	virtual bool o_identity () {return u_class == WORK_NODE_TEST;}
} ;

/// Specialized work_item for QName
class work_qname : public work_item
{
   TIXML_STRING S_local, S_prefix, S_total;
public :
   work_qname (const char * cp_in_prefix, const char * cp_in_local) : work_item (WORK_QNAME)
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
   }
   work_qname (const work_qname & ) : work_item (WORK_QNAME)
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
	virtual bool o_identity () {return u_class == WORK_QNAME;}
} ;

/// Specialized work_item for Step
class work_step : public work_item
{
   work_axis * wp_axis;
   work_node_test * wp_node_test;
   work_step * wp_next_step;
	bool o_absolute, o_all;
public :
   work_step (const work_axis * wp_in_axis, const work_node_test * wp_in_node_test) 
		: work_item (WORK_STEP)
   {
      wp_axis = new work_axis (* wp_in_axis);
      wp_node_test = new work_node_test (* wp_in_node_test);
      wp_next_step = NULL;
		o_absolute = false;
		o_all = false;
   }
   work_step (const work_step & copy) : work_item (WORK_STEP)
   {
      wp_axis = new work_axis (* (copy . wp_axis));
      wp_node_test = new work_node_test (* (copy . wp_node_test));
      if (copy . wp_next_step)
         wp_next_step = new work_step (* (copy . wp_next_step));
      else
         wp_next_step = NULL;
		o_absolute = copy . o_absolute;
		o_all = copy . o_all;
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
   void v_step_all (TiXmlNode * XNp_target, long & l_mark_level)
   {
      wp_node_test -> v_find_all (XNp_target, l_mark_level);
      v_step_child (XNp_target, l_mark_level);
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
	virtual bool o_identity () {return u_class == WORK_STEP;}
   virtual void v_apply (TiXmlNode * , const char * , long & );
	void v_set_absolute (bool o_in) {o_absolute = o_in;}
	void v_set_all (bool o_in) {o_all = o_in;}
} ;

extern work_item * wip_copy (const work_item * wip_in);

#endif
