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

/**
   \file workitem.cpp
   \author Yves Berquin
   Action items for the XPath expressions
*/

#include <math.h>
#include "workitem.h"

work_item * wip_copy (const work_item * wip_in)
{
	work_item * wip_ret;

	wip_ret = NULL;
   switch (wip_in -> u_get_class ())
   {
      case WORK_EXPR :
         wip_ret = new work_expr (* (work_expr *) wip_in);
         break;
		case WORK_STEP :
			// we can have a predicate which is a step //BBB[@name]
         wip_ret = new work_step (* (work_step *) wip_in);
			break;
		case WORK_FUNC :
			wip_ret = new work_func (* (work_func *) wip_in);
			break;
      default :
         assert (false);
   }
	return wip_ret;
}

void work_func::v_set_arg_list (unsigned u_in_nb_arg, work_item ** wipp_arg)
{
	unsigned u_arg;
	u_nb_arg = u_in_nb_arg;
	if (u_nb_arg)
	{
		wipp_list = new work_item * [u_nb_arg];
		for (u_arg = 0; u_arg < u_nb_arg; u_arg++)
			wipp_list [u_arg] = wip_copy (wipp_arg [u_arg]);
	}
	else
		wipp_list = NULL;
}
void work_node_test::v_set_predicate_list (unsigned u_in_nb_predicate, work_item ** wipp_in_list)
{
   unsigned u_predicate;

   u_nb_predicate = u_in_nb_predicate;
   if (u_nb_predicate)
   {
      wipp_list = new work_item * [u_nb_predicate];
      for (u_predicate = 0; u_predicate < u_nb_predicate; u_predicate++)
         wipp_list [u_predicate] = wip_copy (wipp_in_list [u_predicate]);
   }
   else
      wipp_list = NULL;
}

void work_node_test::v_apply_predicate (
	TiXmlNode * XNp_target, 
	work_item * wp_item, 
	const char * cp_label, 
	long & l_marker)
{
	TiXmlElement * XEp_child;
	TIXML_STRING S_ret;
	expression_result * erp_res;
	bool o_mark;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		if (l_get_user_value (XEp_child) == l_marker)
		{
			erp_res = wp_item -> erp_compute_predicate (XEp_child, XNp_target);
			o_mark = false;
			switch (erp_res -> e_type)
			{
				case e_bool :
					o_mark = erp_res -> o_get_bool ();
					break;
				case e_int :
					o_mark = i_xml_cardinality (XEp_child) == erp_res -> i_get_int ();
					break;
				case e_string :
					o_mark = true;
					// this is the case when we have an attribute as predicate,
					// because we return the attribute's value
					break;
			}
			if (o_mark)
				// v_set_user_value (XEp_child, l_marker + 1);
            v_mark_node_and_attribute (XEp_child, l_marker + 1);
         delete erp_res;
		}
      v_apply_predicate (XEp_child, wp_item, cp_label, l_marker);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void work_step::v_apply (TiXmlNode * XNp_node, const char * cp_name, long & l_marker)
{
	printf ("work_step::v_apply (%s, %ld)\n", cp_name, l_marker);
	if (o_absolute)
	{	
		if (o_all)
			v_step_all (XNp_node, l_marker);
		else
			if (wp_axis && ! wp_axis -> o_is_at () && ! wp_axis -> o_is_abbreviated ())
				wp_axis -> v_apply (XNp_node, "", l_marker);
			else
		      v_step_it (XNp_node -> FirstChildElement (), l_marker);
	}
	else
		// we never arrive here
		assert (false);
	/*
	else
	{
		v_mark_children_name (XNp_node, cp_name, l_marker, l_marker + 1);
		l_marker++;
		if (wp_axis)
			wp_axis -> v_apply (XNp_node, wp_node_test -> cp_get_value (), l_marker);
	}
	*/
}

expression_result * work_step::erp_compute_predicate (TiXmlElement * XEp_elem, TiXmlNode * XNp_root) 
{
	const char * cp_ret, * cp_lookup;
	expression_result * erp_res;
   TiXmlElement * XEp_root;

   assert (XNp_root);   
   v_dump (0);
   erp_res = new expression_result;
	// predicates for step ...
	printf ("Predicate for step\n");
	assert (wp_axis);
   cp_lookup = wp_node_test -> cp_get_value ();
	if (wp_axis -> o_is_at ())
	{
		// it's an attribute lookup
		printf ("Predicate is the attribute %s\n", cp_lookup);
		if (! strcmp (cp_lookup, "*"))
		{
			// looking for any attribute
			erp_res -> v_set_bool (XEp_elem -> FirstAttribute () != NULL);
		}
		else
		{
			cp_ret = XEp_elem -> Attribute (cp_lookup);
			if (cp_ret)
				erp_res -> v_set_string (cp_ret);
			else
				erp_res -> v_set_bool (false);
		}
	}
	else
	   if (o_absolute)
		{
         XEp_root = XNp_root -> FirstChildElement ();
         assert (XEp_root);
			if (! strcmp (cp_lookup, "*"))
  				erp_res -> v_set_node_set_recursive (XEp_root -> FirstChildElement ());
			else
  				erp_res -> v_set_node_set_recursive (XEp_root -> FirstChildElement (), cp_lookup);
		}
		else
		{
			if (! strcmp (cp_lookup, "*"))
  				erp_res -> v_set_node_set (XEp_elem);
			else
  				erp_res -> v_set_node_set (XEp_elem, cp_lookup);
			/* !!!
			// not an attribute
			if (! strcmp (cp_lookup, "*"))
				// looking for any child, and counting
				erp_res -> v_set_int ((int) u_count_children (XEp_elem));
			else
				erp_res -> v_set_int ((int) u_count_children (XEp_elem, cp_lookup));
			!!! */
		}
   if (wp_next_step)
   {
      expression_result * erp_temp;

      erp_temp = erp_res;
      erp_res = wp_next_step -> erp_compute_predicate (erp_temp);
      delete erp_temp;
   }
	return erp_res;
}

expression_result * work_step::erp_compute_predicate (expression_result * erp_pre)
{
   expression_result * erp_res;
	const char * cp_lookup;
   TiXmlElement * XEp_elem;
   unsigned u_node;
   node_set * nsp_in_set;
   const TiXmlBase * XBp_current;
   bool o_current_attrib;
   TiXmlNode * XNp_child, * XNp_current;

   erp_res = new expression_result;
	assert (wp_axis);
   cp_lookup = wp_node_test -> cp_get_value ();
   assert (erp_pre -> e_type == e_node_set);
   nsp_in_set = erp_pre -> nsp_get_node_set ();
   for (u_node = 0; u_node < nsp_in_set -> u_get_nb_node_in_set (); u_node++)
   {
      XBp_current = nsp_in_set -> XBp_get_node_in_set (u_node);
      o_current_attrib = nsp_in_set -> o_is_attrib (u_node);
	   if (wp_axis -> o_is_at ())
	   {
         // skip this input node if it is not an attribute
         if (o_current_attrib)
         {

         /* todo ...

		      // it's an attribute lookup
		      printf ("Predicate is the attribute %s\n", cp_lookup);
		      if (! strcmp (cp_lookup, "*"))
		      {
			      // looking for any attribute
			      erp_res -> v_set_bool (XEp_elem -> FirstAttribute () != NULL);
		      }
		      else
		      {
			      cp_ret = XEp_elem -> Attribute (cp_lookup);
			      if (cp_ret)
				      erp_res -> v_set_string (cp_ret);
			      else
				      erp_res -> v_set_bool (false);
		      }

            ... */

         }
	   }
	   else
      {
         // skip this input node if it is an attribute
         if (! o_current_attrib)
         {
	         if (o_absolute)
		      {
               // we shouldn't have an absolute predicate in a 2nd rule
               assert (false);
               /*
			      if (! strcmp (cp_lookup, "*"))
  				      erp_res -> v_set_node_set (XEp_root -> FirstChildElement ());
			      else
  				      erp_res -> v_set_node_set (XEp_root -> FirstChildElement (), cp_lookup);
               */
		      }
		      else
		      {
               erp_res -> v_set_node_set ();
               XNp_current = (TiXmlNode *) XBp_current;
               XNp_child = XNp_current -> FirstChildElement ();
               while (XNp_child)
               {
                  XEp_elem = XNp_child -> ToElement ();
                  if (XEp_elem)
                  {
			            if (! strcmp (cp_lookup, "*") || ! strcmp (XEp_elem -> Value (), cp_lookup))
                        erp_res -> nsp_get_node_set () -> v_add_node_in_set (XEp_elem, false);
                  }
                  XNp_child = XNp_child -> NextSibling ();
               } 
		      }
         }
      }
   }
   if (wp_next_step)
   {
      expression_result * erp_temp;

      erp_temp = erp_res;
      erp_res = wp_next_step -> erp_compute_predicate (erp_temp);
      delete erp_temp;
   }
   return erp_res;
}

expression_result * work_expr::erp_compute_predicate (TiXmlElement * XEp_element, TiXmlNode *) 
{
	expression_result * erp_res;
			
   erp_res = new expression_result;
   switch (u_cat)
	{
		case e_work_expr_value :
			erp_res -> v_set_int (i_get_expr_value ());
         // erp_res -> v_set_bool (i_xml_cardinality (XEp_element) == i_get_expr_value ());
			break;

		case e_work_expr_func :
			if (S_value == "last")
				// erp_res -> v_set_bool (XEp_element -> NextSiblingElement () == NULL);
				erp_res -> v_set_int (i_xml_valid_sibling (XEp_element));
			else

			if (S_value == "name")
				erp_res -> v_set_string (XEp_element -> Value ());
			else

			if (S_value == "position")
				erp_res -> v_set_int (i_xml_cardinality (XEp_element));
			else

				assert (false);
			break;

		case e_work_expr_literal :
			erp_res -> v_set_string (S_value);
			break;

		case e_work_expr_double_value :
			erp_res -> v_set_double (d_value);
			break;

		default :
			assert (false);
	}
	return erp_res;
}

expression_result * work_func::erp_func_not (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_res -> v_set_bool (! erp_arg -> o_get_bool ());
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_internal_equal (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	if (erp_arg -> e_type != erp_arg_2 -> e_type)
		erp_res -> v_set_bool (false);
	else
		switch (erp_arg -> e_type)
		{
			case e_bool :
				erp_res -> v_set_bool (erp_arg -> o_get_bool () == erp_arg_2 -> o_get_bool ());
				break;
			case e_int :
				erp_res -> v_set_bool (erp_arg -> i_get_int () == erp_arg_2 -> i_get_int ());
				break;
			case e_string :
				erp_res -> v_set_bool (! strcmp (erp_arg -> cp_get_string (), erp_arg_2 -> cp_get_string ()));
				break;
			default :
				erp_res -> v_set_bool (false);
				break;
		}
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_normalize_space (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	if (erp_arg -> e_type == e_string)
		erp_res -> v_set_string (S_remove_lead_trail (erp_arg -> cp_get_string ()));
	else
		erp_res -> v_set_bool (false);
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_count (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;
	unsigned u_count, u_nb, u_node;
   const TiXmlNode * XNp_node;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_node_set);
   u_nb = erp_arg -> nsp_get_node_set () -> u_get_nb_node_in_set ();
   u_count = 0;
   printf (" start set\n");
   for (u_node = 0; u_node < u_nb; u_node++)
      if (! erp_arg -> nsp_get_node_set () -> o_is_attrib (u_node))
      {
         XNp_node = (const TiXmlNode *) erp_arg -> nsp_get_node_set () -> XBp_get_node_in_set (u_node);
         if (XNp_node -> Type () == TiXmlNode::ELEMENT)
         {
            printf (" (set) : %s\n", XNp_node -> Value ());
            u_count++;
         }
      }
   printf (" end set : %d\n", u_count);
	erp_res -> v_set_int ((int) u_count);
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_starts_with (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_string);
	assert (erp_arg_2 -> e_type == e_string);
	erp_res -> v_set_bool (! strncmp (erp_arg -> cp_get_string (), erp_arg_2 -> cp_get_string (), 
				strlen (erp_arg_2 -> cp_get_string ())));
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_contains (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_string);
	assert (erp_arg_2 -> e_type == e_string);
	erp_res -> v_set_bool (strstr (erp_arg -> cp_get_string (), erp_arg_2 -> cp_get_string ()) ? true : false);
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_internal_less (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_int);
	assert (erp_arg_2 -> e_type == e_int);
	erp_res -> v_set_bool (erp_arg -> i_get_int () < erp_arg_2 -> i_get_int ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_internal_greater (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_int);
	assert (erp_arg_2 -> e_type == e_int);
	erp_res -> v_set_bool (erp_arg -> i_get_int () > erp_arg_2 -> i_get_int ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_internal_less_or_equal (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_int);
	assert (erp_arg_2 -> e_type == e_int);
	erp_res -> v_set_bool (erp_arg -> i_get_int () <= erp_arg_2 -> i_get_int ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_internal_greater_or_equal (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_int);
	assert (erp_arg_2 -> e_type == e_int);
	erp_res -> v_set_bool (erp_arg -> i_get_int () >= erp_arg_2 -> i_get_int ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_string_length (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_string);
	erp_res -> v_set_int (erp_arg -> S_get_string () . length ());
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_modulo (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_int);
	assert (erp_arg_2 -> e_type == e_int);
	erp_res -> v_set_int (erp_arg -> i_get_int () % erp_arg_2 -> i_get_int ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_floor (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;
	double d_val;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	switch (erp_arg -> e_type)
	{
		case e_int :
			erp_res -> v_set_int (erp_arg -> i_get_int ());
			break;
		case e_double :
			d_val = floor (erp_arg -> d_get_double ());
			erp_res -> v_set_int ((int) d_val);
			break;
		default :
			assert (false);
	}
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_name (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;
   int i_res;
   TiXmlNode * XNp_first;
   TiXmlAttribute * XAp_first;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
   switch (erp_arg -> e_type)
   {
      case e_node_set :
         i_res = erp_arg -> nsp_get_node_set () -> i_get_first_marked (XNp_first, XAp_first);
         switch (i_res)
         {
            case 0 :
               erp_res -> v_set_string ("");
               break;
            case 1 :
               erp_res -> v_set_string (XNp_first -> Value ());
               break;
            case 2 :
               erp_res -> v_set_string (XAp_first -> Value ());
               break;
            default :   
               assert (false);
         }
         break;
      default :
         erp_res -> v_set_string ("");
         break;
   }
   delete erp_arg;
   return erp_res;
}

expression_result * work_func::erp_func_internal_plus (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	if (erp_arg -> e_type == e_int && erp_arg_2 -> e_type == e_int)
		erp_res -> v_set_int (erp_arg -> i_get_int () + erp_arg_2 -> i_get_int ());
	else
		erp_res -> v_set_double (erp_arg -> d_get_double () + erp_arg_2 -> d_get_double ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

/// Evaluates the result of the div operation, which is a real operation, not an int like in Pascal
expression_result * work_func::erp_func_internal_div (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	if (fabs (erp_arg_2 -> d_get_double ()) < 1.0e-6)
		assert (false);
   erp_res -> v_set_double (erp_arg -> d_get_double () / erp_arg_2 -> d_get_double ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_ceiling (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;
	double d_val;

	assert (u_nb_arg == 1);
   erp_res = new expression_result;
	erp_arg = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	switch (erp_arg -> e_type)
	{
		case e_int :
			erp_res -> v_set_int (erp_arg -> i_get_int ());
			break;
		case e_double :
			d_val = ceil (erp_arg -> d_get_double ());
			erp_res -> v_set_int ((int) d_val);
			break;
		default :	
			assert (false);
	}
   delete erp_arg;
	return erp_res;
}

expression_result * work_func::erp_func_internal_or (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_arg_2, * erp_res;

	assert (u_nb_arg == 2);
   erp_res = new expression_result;
	erp_arg_2 = wipp_list [0] -> erp_compute_predicate (XEp_test, XNp_root);
	erp_arg = wipp_list [1] -> erp_compute_predicate (XEp_test, XNp_root);
	assert (erp_arg -> e_type == e_bool);
	assert (erp_arg_2 -> e_type == e_bool);
   erp_res -> v_set_bool (erp_arg -> o_get_bool () || erp_arg_2 -> o_get_bool ());
   delete erp_arg;
   delete erp_arg_2;
	return erp_res;
}

expression_result * work_func::erp_func_concat (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_arg, * erp_res;
	TIXML_STRING S_res, S_to_add;
	unsigned u_arg;

	assert (u_nb_arg >= 2);
   erp_res = new expression_result;
	S_res = "";
	for (u_arg = 0; u_arg < u_nb_arg; u_arg++)
	{
		erp_arg = wipp_list [u_nb_arg - u_arg - 1] -> erp_compute_predicate (XEp_test, XNp_root);
		switch (erp_arg -> e_type)
		{
			case e_string :
				S_to_add = erp_arg -> S_get_string ();
				break;
			case e_node_set :	
            S_to_add = erp_arg -> nsp_get_node_set () -> S_get_string_value ();
				break;
			default :
				assert (false);
				break;
		}
      delete erp_arg;
		S_res += S_to_add;
	}
   erp_res -> v_set_string (S_res . c_str ());
	return erp_res;
}

expression_result * work_func::erp_compute_predicate (TiXmlElement * XEp_test, TiXmlNode * XNp_root) 
{
	expression_result * erp_res;

	if (S_name == "ceiling")
		return erp_func_ceiling (XEp_test, XNp_root);
	else

	if (S_name == "contains")
		return erp_func_contains (XEp_test, XNp_root);
	else

	if (S_name == "concat")
		return erp_func_concat (XEp_test, XNp_root);
	else

	if (S_name == "count")
		return erp_func_count (XEp_test, XNp_root);
	else

	if (S_name == "floor")
		return erp_func_floor (XEp_test, XNp_root);
	else

	if (S_name == "normalize-space")
		return erp_func_normalize_space (XEp_test, XNp_root);
	else

	if (S_name == "not")
		return erp_func_not (XEp_test, XNp_root);
	else

	if (S_name == "starts-with")
		return erp_func_starts_with (XEp_test, XNp_root);
	else

	if (S_name == "string-length")
		return erp_func_string_length (XEp_test, XNp_root);
	else

	if (S_name == "__div__")
		return erp_func_internal_div (XEp_test, XNp_root);
	else

	if (S_name == "__equal__")
		return erp_func_internal_equal (XEp_test, XNp_root);
	else

	if (S_name == "__greater__")
		return erp_func_internal_greater (XEp_test, XNp_root);
	else

	if (S_name == "__greatereq__")
		return erp_func_internal_greater_or_equal (XEp_test, XNp_root);
	else

	if (S_name == "__less__")
		return erp_func_internal_less (XEp_test, XNp_root);
	else

	if (S_name == "__lesseq__")
		return erp_func_internal_less_or_equal (XEp_test, XNp_root);
	else
	if (S_name == "__mod__")
		return erp_func_modulo (XEp_test, XNp_root);
	else

	if (S_name == "__or_logical__")
		return erp_func_internal_or (XEp_test, XNp_root);
	else

	if (S_name == "__plus__")
		return erp_func_internal_plus (XEp_test, XNp_root);
	else

	if (S_name == "name")
		return erp_func_name (XEp_test, XNp_root);
	else


	{
      erp_res = new expression_result;
		printf ("Function %s not yet implemented\n", S_name . c_str ());
		assert (false);
		erp_res -> v_set_bool (false);
		return erp_res;
	}
}

void work_axis::v_apply (TiXmlNode * XNp_node, const char * , long & l_marker)
{
	if (value == "descendant")
	{
		v_mark_children_inside (XNp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else
	{
		printf ("Axis %s not yet implemented\n", value . c_str ());
		assert (false);
	}
}

void work_axis::v_mark_axis (TiXmlNode * XNp_node, const char * cp_node, long & l_marker)
{
	if (value == "ancestor")
	{
		v_mark_ancestor (XNp_node, cp_node, false, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "ancestor-or-self")
	{
		v_mark_ancestor (XNp_node, cp_node, true, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "descendant")
	{
		v_mark_descendant (XNp_node, cp_node, false, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "descendant-or-self")
	{
		v_mark_descendant (XNp_node, cp_node, true, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "following")
	{
		v_mark_following (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "following-sibling")
	{
		v_mark_following_sibling (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "parent")
	{
		v_mark_parent (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "preceding")
	{
		v_mark_preceding (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "preceding-sibling")
	{
		v_mark_preceding_sibling (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "self")
	{
		v_mark_self (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else
	{
		printf ("Axis %s not yet implemented\n", value . c_str ());
		assert (false);
	}
}

void work_func::v_apply (TiXmlNode * XNp_node, const char * cp_name, long & l_marker)
{
	if (S_name == "__or__")
	{
	   assert (u_nb_arg == 2);
		wipp_list [0] -> v_apply (XNp_node, cp_name, l_marker);
		v_retain_attrib_tree (XNp_node, l_marker);
		wipp_list [1] -> v_apply (XNp_node, cp_name, l_marker);
		v_retain_attrib_tree (XNp_node, l_marker);
	}
	else
   {
      expression_result * erp_res;
      erp_res = erp_compute_predicate (XNp_node -> FirstChildElement (), XNp_node);
      v_set_result (XNp_node, erp_res);
      l_marker++;
   }
/*
   if (S_name == "name")
   {
      TIXML_STRING S_res;
      TiXmlNode * XNp_first;
      TiXmlComment * XCp_comment;
      TiXmlAttribute * XAp_first;
      int i_res;

	   assert (u_nb_arg == 1);
		wipp_list [0] -> v_apply (XNp_node, cp_name, l_marker);
      i_res = i_get_first_marked (XNp_node -> FirstChildElement (), l_marker, XNp_first, XAp_first);
      switch (i_res)
      {
         case 0 :
            S_res = "";
            break;
         case 1 :
            S_res = XNp_first -> Value ();
            break;
         case 2 :
            S_res = XAp_first -> Value ();
            break;
         default :   
            assert (false);
      }
      XCp_comment = new TiXmlComment ();
      XCp_comment -> SetValue (S_res . c_str ());
      XNp_first = XNp_node -> FirstChild ();
      XNp_node -> InsertAfterChild (XNp_first, * XCp_comment);
      l_marker++;
   }
   else

   if (S_name == "count")
   {
      int i_res;

	   assert (u_nb_arg == 1);
		wipp_list [0] -> v_apply (XNp_node, cp_name, l_marker);
      i_res = i_count_marked_element (XNp_node -> FirstChildElement (), l_marker);
      v_set_int_result (XNp_node, i_res);
      l_marker++;
   }
   else

   if (S_name == "__plus__")
   {
      expression_result erp_res;
      erp_res = er_func_internal_plus (XNp_node -> FirstChildElement(), XNp_node);
      v_set_result (XNp_node, erp_res);
      l_marker++;
   }
   else
		assert (false);
*/
}

void work_func::v_set_result (TiXmlNode * XNp_node, expression_result * erp_res)
{
   switch (erp_res -> e_type)
   {
      case e_bool :  
         v_set_string_result (XNp_node, erp_res -> o_get_bool () ? "true" : "false");
         break;
      case e_string :
         v_set_string_result (XNp_node, erp_res -> cp_get_string ());
         break;
      case e_int :
         v_set_int_result (XNp_node, erp_res -> i_get_int ());
         break;
      case e_double :
         v_set_double_result (XNp_node, erp_res -> d_get_double ());
         break;
      case e_node_set :
      case e_invalid :
         assert (false);
   }
}

void work_func::v_set_int_result (TiXmlNode * XNp_node, int i_res)
{
   char ca_res [20];
   sprintf (ca_res, "%d", i_res);
   v_set_string_result (XNp_node, ca_res);
}

void work_func::v_set_double_result (TiXmlNode * XNp_node, double d_res)
{
   char ca_res [200];
   sprintf (ca_res, "%.3f", d_res);
   v_set_string_result (XNp_node, ca_res);
}

void work_func::v_set_string_result (TiXmlNode * XNp_node, const char * cp_res)
{
   TiXmlNode * XNp_first;
   TiXmlComment * XCp_comment;

   XCp_comment = new TiXmlComment ();
   XCp_comment -> SetValue (cp_res);
   XNp_first = XNp_node -> FirstChild ();
   XNp_node -> InsertAfterChild (XNp_first, * XCp_comment);
}
