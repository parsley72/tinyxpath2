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

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		if (XEp_child -> GetUserValue () == l_marker)
			if (wp_item -> o_test_predicate (XEp_child, S_ret))
				XEp_child -> SetUserValue (l_marker + 1);
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
		   v_step_it (XNp_node -> FirstChildElement (), l_marker);
	}
	else
	{
		v_mark_children_name (XNp_node, cp_name, l_marker, l_marker + 1);
		l_marker++;
		if (wp_axis)
			wp_axis -> v_apply (XNp_node, wp_node_test -> cp_get_value (), l_marker);
	}
}

bool work_step::o_test_predicate (TiXmlElement * XEp_elem, TIXML_STRING & S_ret) 
{
	const char * cp_ret, * cp_lookup;
	char ca_ret [10];
	unsigned u_ret;

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
			S_ret = "";
			return (XEp_elem -> FirstAttribute () != NULL);
		}
		else
		{
			cp_ret = XEp_elem -> Attribute (cp_lookup);
			if (cp_ret)
			{
				S_ret = cp_ret;
				return true;
			}
			S_ret = "";
			return false;
		}
	}
	else
	{
		// not an attribute
		if (! strcmp (cp_lookup, "*"))
			// looking for any child, and counting
			u_ret = u_count_children (XEp_elem);
		else
			u_ret = u_count_children (XEp_elem, cp_lookup);
		sprintf (ca_ret, "%d", u_ret);
		S_ret = ca_ret;
		return (u_ret > 0);
	}
}
