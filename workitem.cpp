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
	expression_result er_res ;
	bool o_mark;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		if (l_get_user_value (XEp_child) == l_marker)
		{
			er_res = wp_item -> er_compute_predicate (XEp_child);
			o_mark = false;
			switch (er_res . e_type)
			{
				case e_bool :
					o_mark = er_res . o_get_bool ();
					break;
				case e_int :
					o_mark = i_xml_cardinality (XEp_child) == er_res . i_get_int ();
					break;
				case e_string :
					o_mark = true;
					// this is the case when we have an attribute as predicate,
					// because we return the attribute's value
					break;
			}
			if (o_mark)
				v_set_user_value (XEp_child, l_marker + 1);
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

expression_result work_step::er_compute_predicate (TiXmlElement * XEp_elem) 
{
	const char * cp_ret, * cp_lookup;
	expression_result er_res;

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
			er_res . v_set_bool (XEp_elem -> FirstAttribute () != NULL);
		}
		else
		{
			cp_ret = XEp_elem -> Attribute (cp_lookup);
			if (cp_ret)
				er_res . v_set_string (cp_ret);
			else
				er_res . v_set_bool (false);
		}
	}
	else
	{
		// not an attribute
		if (! strcmp (cp_lookup, "*"))
			// looking for any child, and counting
			er_res . v_set_int ((int) u_count_children (XEp_elem));
		else
			er_res . v_set_int ((int) u_count_children (XEp_elem, cp_lookup));
	}
	return er_res;
}

expression_result work_expr::er_compute_predicate (TiXmlElement * XEp_element) 
{
	expression_result er_res;
			
	switch (u_cat)
	{
		case e_work_expr_value :
			// here return (i_xml_cardinality (XEp_element) == i_get_expr_value ());
			er_res . v_set_int (i_get_expr_value ());
			break;

		case e_work_expr_func :
			if (S_value == "last")
				// er_res . v_set_bool (XEp_element -> NextSiblingElement () == NULL);
				er_res . v_set_int (i_xml_valid_sibling (XEp_element));
			else

			if (S_value == "name")
				er_res . v_set_string (XEp_element -> Value ());
			else

			if (S_value == "position")
				er_res . v_set_int (i_xml_cardinality (XEp_element));
			else

				assert (false);
			break;

		case e_work_expr_literal :
			er_res . v_set_string (S_value);
			break;

		case e_work_expr_double_value :
			er_res . v_set_double (d_value);
			break;

		default :
			assert (false);
	}
	return er_res;
}

expression_result work_func::er_compute_predicate (TiXmlElement * XEp_test) 
{
	TIXML_STRING S_first, S_inter, S_second;
	expression_result er_arg, er_arg_2, er_res;
	double d_val;

	if (S_name == "not")
	{
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_res . v_set_bool (! er_arg . o_get_bool ());
	}
	else

	if (S_name == "__equal__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		if (er_arg . e_type != er_arg_2 . e_type)
			er_res . v_set_bool (false);
		else
			switch (er_arg . e_type)
			{
				case e_bool :
					er_res . v_set_bool (er_arg . o_get_bool () == er_arg_2 . o_get_bool ());
					assert (false);   // the code is here above, but it doesn't make sense
											// to compare with an equal two booleans, now does it ?
					break;
				case e_int :
					er_res . v_set_bool (er_arg . i_get_int () == er_arg_2 . i_get_int ());
					break;
				case e_string :
					er_res . v_set_bool (! strcmp (er_arg . cp_get_string (), er_arg_2 . cp_get_string ()));
					break;
			}
	}
	else

	if (S_name == "normalize-space")
	{
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		if (er_arg . e_type == e_string)
		   er_res . v_set_string (S_remove_lead_trail (er_arg . cp_get_string ()));
		else
			er_res . v_set_bool (false);
	}
	else

	if (S_name == "count")
	{	
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		// ???
		er_res = er_arg;
	}
	else

	if (S_name == "starts-with")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_string);
		assert (er_arg_2 . e_type == e_string);
		er_res . v_set_bool (! strncmp (er_arg . cp_get_string (), er_arg_2 . cp_get_string (), 
					strlen (er_arg_2 . cp_get_string ())));
	}
	else

	if (S_name == "contains")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_string);
		assert (er_arg_2 . e_type == e_string);
		er_res . v_set_bool (strstr (er_arg . cp_get_string (), er_arg_2 . cp_get_string ()) 
					? true : false);
	}
	else

	if (S_name == "__less__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_int);
		assert (er_arg_2 . e_type == e_int);
		er_res . v_set_bool (er_arg . i_get_int () < er_arg_2 . i_get_int ());
	}
	else

	if (S_name == "__greater__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_int);
		assert (er_arg_2 . e_type == e_int);
		er_res . v_set_bool (er_arg . i_get_int () > er_arg_2 . i_get_int ());
	}
	else

	if (S_name == "__lesseq__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_int);
		assert (er_arg_2 . e_type == e_int);
		er_res . v_set_bool (er_arg . i_get_int () <= er_arg_2 . i_get_int ());
	}
	else

	if (S_name == "__greatereq__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_int);
		assert (er_arg_2 . e_type == e_int);
		er_res . v_set_bool (er_arg . i_get_int () >= er_arg_2 . i_get_int ());
	}
	else

	if (S_name == "string-length")
	{	
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_string);
		er_res . v_set_int (er_arg . S_get_string () . length ());
	}
	else

	if (S_name == "__mod__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_int);
		assert (er_arg_2 . e_type == e_int);
		er_res . v_set_int (er_arg . i_get_int () % er_arg_2 . i_get_int ());
	}
	else

	if (S_name == "floor")
	{
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		switch (er_arg . e_type)
		{
			case e_int :
				er_res . v_set_int (er_arg . i_get_int ());
				break;
			case e_double :
				d_val = floor (er_arg . d_get_double ());
				er_res . v_set_int ((int) d_val);
				break;
			default :
				assert (false);
		}
	}
	else

	if (S_name == "__plus__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		if (er_arg . e_type == e_int && er_arg_2 . e_type == e_int)
			er_res . v_set_int (er_arg . i_get_int () + er_arg_2 . i_get_int ());
		else
			er_res . v_set_double (er_arg . d_get_double () + er_arg_2 . d_get_double ());
	}
	else

	if (S_name == "__div__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		if (fabs (er_arg_2 . d_get_double ()) < 1.0e-6)
			assert (false);
   	er_res . v_set_int ((int) (er_arg . d_get_double () / er_arg_2 . d_get_double ()));
	}
	else


	if (S_name == "ceiling")
	{
		assert (u_nb_arg == 1);
		er_arg = wipp_list [0] -> er_compute_predicate (XEp_test);
		switch (er_arg  . e_type)
		{
			case e_int :
				er_res . v_set_int (er_arg . i_get_int ());
				break;
			case e_double :
				d_val = ceil (er_arg . d_get_double ());
				er_res . v_set_int ((int) d_val);
				break;
			default :	
				assert (false);
		}
	}
	else

	if (S_name == "__or_logical__")
	{
		assert (u_nb_arg == 2);
		er_arg_2 = wipp_list [0] -> er_compute_predicate (XEp_test);
		er_arg = wipp_list [1] -> er_compute_predicate (XEp_test);
		assert (er_arg . e_type == e_bool);
		assert (er_arg_2 . e_type == e_bool);
   	er_res . v_set_bool (er_arg . o_get_bool () || er_arg_2 . o_get_bool ());
	}
	else
	{
		printf ("Function %s not yet implemented\n", S_name . c_str ());
		assert (false);
	}
	return er_res;
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
	if (value == "descendant")
	{
		v_mark_descendant (XNp_node, cp_node, false, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "parent")
	{
		v_mark_parent (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "ancestor")
	{
		v_mark_ancestor (XNp_node, cp_node, false, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "following-sibling")
	{
		v_mark_following_sibling (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "preceding-sibling")
	{
		v_mark_preceding_sibling (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "following")
	{
		v_mark_following (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "preceding")
	{
		v_mark_preceding (XNp_node, cp_node, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "descendant-or-self")
	{
		v_mark_descendant (XNp_node, cp_node, true, l_marker, l_marker + 1);
		l_marker++;
	}
	else

	if (value == "ancestor-or-self")
	{
		v_mark_ancestor (XNp_node, cp_node, true, l_marker, l_marker + 1);
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
