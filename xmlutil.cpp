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
#include "tinyxml.h"
#include "xmlutil.h"

/// Clone all children of source to children of target
void v_clone_children (
   TiXmlNode * XNp_source, 
   TiXmlNode * XNp_target)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_source -> FirstChildElement ();
   while (XEp_child)
   {
      XNp_target -> InsertEndChild (* XEp_child);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_set_user_value (TiXmlNode * XNp_node, long l_value)
{
	XNp_node -> SetUserValue ((XNp_node -> GetUserValue () & l_select_mask) | l_value);
}

long l_get_user_value (TiXmlNode * XNp_node)
{
	return XNp_node -> GetUserValue () & l_value_mask;
}

long l_get_user_value (TiXmlAttribute * XAp_att)
{
	return XAp_att -> GetUserValue () & l_value_mask;
}

void v_set_user_value (TiXmlAttribute * XAp_att, long l_value)
{
	XAp_att -> SetUserValue ((XAp_att -> GetUserValue () & l_select_mask) | l_value);
}

static void v_select_user_value (TiXmlNode * XNp_node)
{
	XNp_node -> SetUserValue (XNp_node -> GetUserValue () | l_select_value);
}

static void v_select_user_value (TiXmlAttribute * XAp_att)
{
	XAp_att -> SetUserValue (XAp_att -> GetUserValue () | l_select_value);
}

void v_mark_node_and_attribute (
	TiXmlNode * XNp_node, 
	long l_marker)
{
	TiXmlElement * XEp_elem;
	TiXmlAttribute * XAp_att;

	v_set_user_value (XNp_node, l_marker);
	XEp_elem = XNp_node -> ToElement ();
	if (XEp_elem)
	{
		XAp_att = XEp_elem -> FirstAttribute ();
		while (XAp_att)
		{
			v_set_user_value (XAp_att, l_marker);
			XAp_att = XAp_att ->Next ();
		}
	}
}

// Look for all first-level items that have a name, and mark them with
// the attribute 
void v_mark_first_level (
   TiXmlNode * XNp_target, 
   long l_value)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		v_mark_node_and_attribute (XEp_child, l_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

// Look for all first-level items that have a name, and mark them with
// the attribute 
void v_mark_first_level_name (
   TiXmlNode * XNp_target, 
   const char * cp_lookup_name,
   long l_value)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement (cp_lookup_name);
   while (XEp_child)
   {
      v_mark_node_and_attribute (XEp_child, l_value);
      XEp_child = XEp_child -> NextSiblingElement (cp_lookup_name);
   }
}

void v_mark_all_children (
   TiXmlNode * XNp_target, 
   long l_id)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      v_mark_node_and_attribute (XEp_child, l_id);
      v_mark_all_children (XEp_child, l_id);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

static void v_init_node_and_attribute (
	TiXmlNode * XNp_node)
{
	TiXmlElement * XEp_elem;
	TiXmlAttribute * XAp_att;

	XNp_node -> SetUserValue (1);
	XEp_elem = XNp_node -> ToElement ();
	if (XEp_elem)
	{
		XAp_att = XEp_elem -> FirstAttribute ();
		while (XAp_att)
		{
			XAp_att -> SetUserValue (1);
			XAp_att = XAp_att ->Next ();
		}
	}
}

void v_initialize_all_children (	
	TiXmlNode * XNp_target)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      v_init_node_and_attribute (XEp_child);
      v_initialize_all_children (XEp_child);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_children_name (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long l_mother_value, 
   long l_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         while (XEp_child_2)
         {
            v_mark_node_and_attribute (XEp_child_2, l_child_value);
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
        }
      }
      v_mark_children_name (XEp_child, cp_name, l_mother_value, l_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_set_value_of_attrib_if_mother (
	TiXmlElement * XEp_child,
	const char * cp_name,
	long l_mother_value, 
	long l_child_value)
{
	TIXML_STRING S_out, S_value;
	long l_test;
	
	l_test = XEp_child -> GetAttributeUserValue (cp_name);
	if (l_test != l_mother_value)
		return;
	XEp_child -> SetAttributeUserValue (cp_name, l_child_value);
}

void v_mark_children_attrib (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long l_mother_value, 
   long l_child_value)
{
   TiXmlElement * XEp_child;
	const char * cp_at;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		cp_at = XEp_child -> Attribute (cp_name);
		if (cp_at)
			v_set_value_of_attrib_if_mother (XEp_child, cp_name, l_mother_value, l_child_value);
      v_mark_children_attrib (XEp_child, cp_name, l_mother_value, l_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}


void v_mark_children_inside (
   TiXmlNode * XNp_target,
   long l_mother_value,
   long l_child_value)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother_value)
         v_mark_all_children (XEp_child, l_child_value);
      v_mark_children_inside (XEp_child, l_mother_value, l_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

static void v_keep_bracket_if (
	TiXmlElement * XEp_source,
	long l_source)
{
	TiXmlAttribute * XAp_att;
	long l_test;

	XAp_att = XEp_source -> FirstAttribute ();
	while (XAp_att)
	{
		l_test = l_get_user_value (XAp_att);
		if (l_test == l_source)
			v_select_user_value (XAp_att );
		XAp_att = XAp_att -> Next ();
	}
}

static void v_upgrade_marker_attrib (
	TiXmlElement * XEp_source,
	long l_source, long l_new)
{
	TiXmlAttribute * XAp_att;
	long l_test;

	XAp_att = XEp_source -> FirstAttribute ();
	while (XAp_att)
	{
		l_test = l_get_user_value (XAp_att);
		if (l_test == l_source)
			v_set_user_value (XAp_att, l_new);
		XAp_att = XAp_att -> Next ();
	}
}

void v_upgrade_marker (
	TiXmlNode * XNp_target, 
	long l_source, 
	long l_new)
{
	TiXmlElement * XEp_child;
	long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		v_upgrade_marker_attrib (XEp_child, l_source, l_new);
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_source)
         v_set_user_value (XEp_child, l_new);
      v_upgrade_marker (XEp_child, l_source, l_new);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_retain_attrib_tree (
   TiXmlNode * XNp_target,
   long l_source_value)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_source_value)
			v_select_user_value (XEp_child);
		v_keep_bracket_if (XEp_child, l_source_value);
      v_retain_attrib_tree (XEp_child, l_source_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

/*
/// Cardinality in the terms of XPath counts from 1 for the first element
int i_xml_cardinality (TiXmlElement * XEp_elem)
{
	TiXmlNode * XNp_parent;
	TiXmlElement * XEp_child;
	int i_look;

	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
	for (i_look = 0; XEp_child; i_look++)
	   if (XEp_child == XEp_elem)
			return 1 + i_look;
		else
			XEp_child = XEp_child -> NextSiblingElement ();
	assert (false);
	return -1;
}
*/

/// Cardinality in the terms of XPath counts from 1 for the first element
int i_xml_cardinality (TiXmlElement * XEp_elem)
{
	TiXmlNode * XNp_parent;
	TiXmlElement * XEp_child;
	int i_res;
	long l_ref_val;

	l_ref_val = l_get_user_value (XEp_elem);
	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
	i_res = 1;
	while (XEp_child)
	{
	   if (XEp_child == XEp_elem)
			return i_res;
		else
		{
			if (l_get_user_value (XEp_child) >= l_ref_val)
				i_res++;
			XEp_child = XEp_child -> NextSiblingElement ();
		}
	}
	assert (false);
	return -1;
}

int i_xml_valid_sibling (TiXmlElement * XEp_elem)
{
	TiXmlNode * XNp_parent;
	TiXmlElement * XEp_child;
	int i_res;
	long l_ref_val;

	l_ref_val = l_get_user_value (XEp_elem);
	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
	i_res = 0;
	while (XEp_child)
	{
		if (l_get_user_value (XEp_child) >= l_ref_val)
			i_res++;
		XEp_child = XEp_child -> NextSiblingElement ();
	}
	return i_res;
}

unsigned u_count_children (TiXmlElement * XEp_elem, const char * cp_elem)
{
	TiXmlElement * XEp_child;
	int i_look;

	if (! XEp_elem)
		return 0;
	XEp_child = XEp_elem -> FirstChildElement (cp_elem);
	for (i_look = 0; XEp_child; i_look++)
      XEp_child = XEp_child -> NextSiblingElement (cp_elem);
	return i_look;
}

unsigned u_count_children (TiXmlElement * XEp_elem)
{
	TiXmlElement * XEp_child;
	int i_look;

	if (! XEp_elem)
		return 0;
	XEp_child = XEp_elem -> FirstChildElement ();
	for (i_look = 0; XEp_child; i_look++)
      XEp_child = XEp_child -> NextSiblingElement ();
	return i_look;
}

static void v_mark_all_children_with_name (
	TiXmlElement * XEp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlElement * XEp_child;

	XEp_child = XEp_target -> FirstChildElement ();
	while (XEp_child)
	{	
		if (! strcmp (cp_label, "*") || ! strcmp (XEp_child -> Value (), cp_label))
			v_set_user_value (XEp_child, l_target);
		v_mark_all_children_with_name (XEp_child, cp_label, l_target);
		XEp_child = XEp_child -> NextSiblingElement ();
	}
}

void v_mark_descendant (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	bool o_self,
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			if (o_self && (! strcmp (cp_label, "*") || ! strcmp (XEp_child -> Value (), cp_label)))
				v_set_user_value (XEp_child, l_target);
			v_mark_all_children_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_descendant (XEp_child, cp_label, o_self, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_parent (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			if (! strcmp (cp_label, "*") || ! strcmp (XNp_target -> Value (), cp_label))
				v_set_user_value (XNp_target, l_target);
		}
		v_mark_parent (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

static void v_mark_all_ancestor_with_name (
	TiXmlElement * XEp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlElement * XEp_parent;

	XEp_parent = XEp_target -> Parent () -> ToElement ();
	if (XEp_parent)
	{	
		if (! strcmp (cp_label, "*") || ! strcmp (XEp_parent -> Value (), cp_label))
			v_set_user_value (XEp_parent, l_target);
		v_mark_all_ancestor_with_name (XEp_parent, cp_label, l_target);
	}
}


void v_mark_ancestor (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	bool o_self,
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			if (o_self && (! strcmp (cp_label, "*") || ! strcmp (XEp_child -> Value (), cp_label)))
				v_set_user_value (XEp_child, l_target);
			v_mark_all_ancestor_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_ancestor (XEp_child, cp_label, o_self, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_self (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			if (! strcmp (cp_label, "*") || ! strcmp (XEp_child -> Value (), cp_label))
				v_set_user_value (XEp_child, l_target);
		}
		v_mark_self (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

static void v_mark_all_following_with_name (
	TiXmlElement * XEp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlElement * XEp_sibling;

	if (! strcmp (cp_label, "*"))
	   XEp_sibling = XEp_target -> NextSiblingElement ();
	else
	   XEp_sibling = XEp_target -> NextSiblingElement (cp_label);
	while (XEp_sibling)
	{
		v_set_user_value (XEp_sibling, l_target);
		if (! strcmp (cp_label, "*"))
			XEp_sibling = XEp_sibling -> NextSiblingElement ();
		else
			XEp_sibling = XEp_sibling -> NextSiblingElement (cp_label);
	}
}

static void v_mark_all_preceding_with_name (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlNode * XNp_sibling;

	// There is no PreviousSiblingElement ... we have to mimic it ourselves
	if (! strcmp (cp_label, "*"))
	   XNp_sibling = XNp_target -> PreviousSibling ();
	else
	   XNp_sibling = XNp_target -> PreviousSibling (cp_label);
	while (XNp_sibling)
	{
		if (XNp_sibling -> ToElement ())
		   v_set_user_value (XNp_sibling, l_target);
		if (! strcmp (cp_label, "*"))
			XNp_sibling = XNp_sibling -> PreviousSibling ();
		else
			XNp_sibling = XNp_sibling -> PreviousSibling (cp_label);
	}
}

void v_mark_following_sibling (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			v_mark_all_following_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_following_sibling (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_preceding_sibling (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			v_mark_all_preceding_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_preceding_sibling (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

/// Warning : following has the mean of next in document order ...\n
/// This includes uncles and the likes
static void v_mark_following_with_name (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlElement * XEp_follow;

	// lookup for infants
	if (! strcmp (cp_label, "*"))
	   XEp_follow = XNp_target -> NextSiblingElement ();
	else
	   XEp_follow = XNp_target -> NextSiblingElement (cp_label);
	while (XEp_follow)
	{
		v_set_user_value (XEp_follow, l_target);
		v_mark_all_children (XEp_follow, l_target);
		if (! strcmp (cp_label, "*"))
			XEp_follow = XEp_follow -> NextSiblingElement ();
		else
			XEp_follow = XEp_follow -> NextSiblingElement (cp_label);
	}
	// lookup for parents
	if (XNp_target -> Parent () -> ToElement ())
		 v_mark_following_with_name (XNp_target -> Parent (), cp_label, l_target);
}

void v_mark_following (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			v_mark_following_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_following (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

/// Warning : following has the mean of next in document order ...\n
/// This includes uncles and the likes
static void v_mark_preceding_with_name (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_target)
{
	TiXmlNode * XNp_follow;

	// lookup for infants
	if (! strcmp (cp_label, "*"))
	   XNp_follow = XNp_target -> PreviousSibling ();
	else
	   XNp_follow = XNp_target -> PreviousSibling (cp_label);
	while (XNp_follow)
	{
		if (XNp_follow -> ToElement ())
		{
			v_set_user_value (XNp_follow -> ToElement (), l_target);
			v_mark_all_children (XNp_follow -> ToElement (), l_target);
		}
		if (! strcmp (cp_label, "*"))
			XNp_follow = XNp_follow -> PreviousSibling ();
		else
			XNp_follow = XNp_follow -> PreviousSibling (cp_label);
	}
	// lookup for parents
	if (XNp_target -> Parent () -> ToElement ())
		 v_mark_preceding_with_name (XNp_target -> Parent (), cp_label, l_target);
}

void v_mark_preceding (
	TiXmlNode * XNp_target, 
	const char * cp_label, 
	long l_mother, long l_target)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = l_get_user_value (XEp_child);
      if (l_test == l_mother)
		{
			v_mark_preceding_with_name (XEp_child, cp_label, l_target);
		}
		v_mark_preceding (XEp_child, cp_label, l_mother, l_target);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

