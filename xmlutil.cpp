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
		XEp_child -> SetUserValue (l_value);
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
      XEp_child -> SetUserValue (l_value);
      XEp_child = XEp_child -> NextSiblingElement (cp_lookup_name);
   }
}

void v_add_attribute_marker (TiXmlElement * XEp_child, long l_id)
{
	TiXmlAttribute * XAp_attrib;
	TIXML_STRING S_value;

	XAp_attrib = XEp_child -> FirstAttribute ();
	while (XAp_attrib)
	{
		XAp_attrib -> SetUserValue (l_id);
		XAp_attrib = XAp_attrib -> Next ();
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
		v_add_attribute_marker (XEp_child, l_id);
      XEp_child -> SetUserValue (l_id);
      v_mark_all_children (XEp_child, l_id);
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
		l_test = XEp_child -> GetUserValue ();
      if (l_test == l_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         while (XEp_child_2)
         {
            XEp_child_2 -> SetUserValue (l_child_value);
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
        }
      }
      v_mark_children_name (XEp_child, cp_name, l_mother_value, l_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

bool o_is_modified_attrib (
	const TiXmlElement * XEp_ptr,
	const char * cp_name,
	TIXML_STRING & S_out,
	int & i_modified_value)
{
	TiXmlAttribute * XAp_att;
	TIXML_STRING S_res, S_test;
	unsigned u_loop;
	bool o_end;

	XAp_att = XEp_ptr -> FirstAttribute ();
	while (XAp_att)
	{
		S_res = XAp_att -> Name ();
		if (S_res . at (0) == '[')
		{
			o_end = false;
			for (u_loop = 1; ! o_end && u_loop < S_res . length (); u_loop++)
				if (S_res . at (u_loop) == ']')
				{
					S_test = S_res . c_str () + u_loop + 1;
					if (S_test == cp_name)
					{
						S_out = XAp_att -> Value ();
						i_modified_value = atoi (S_res . c_str () + 1);
						return true;
					}
					o_end = true;
				}
		}	
		XAp_att = XAp_att -> Next ();
	}
	return false;
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


void v_mark_children_name_order (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   int i_order,
   long l_mother_value, 
   long l_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2;
   int i_child;
	long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = XEp_child -> GetUserValue ();
      if (l_test == l_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         for (i_child = 0; i_child < i_order && XEp_child_2; i_child++)
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
         if (XEp_child_2)
            XEp_child_2 -> SetUserValue (l_child_value);
      }
      v_mark_children_name (XEp_child, cp_name, l_mother_value, l_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_children_name_last (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long l_mother_value, 
   long l_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2, * XEp_to_mark;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = XEp_child -> GetUserValue ();
      if (l_test == l_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         XEp_to_mark = NULL;
         while (XEp_child_2)
         {
            XEp_to_mark = XEp_child_2;
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
         }
         if (XEp_to_mark)
            XEp_to_mark -> SetUserValue (l_child_value);
      }
      v_mark_children_name (XEp_child, cp_name, l_mother_value, l_child_value);
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
		l_test = XEp_child -> GetUserValue ();
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
	TIXML_STRING S_out;
	long l_test;

	XAp_att = XEp_source -> FirstAttribute ();
	while (XAp_att)
	{
		l_test = XAp_att -> GetUserValue ();
		if (l_test == l_source)
		{
			S_out = XAp_att -> Value ();
			S_out += "|xpath-final";
			XAp_att -> SetValue (S_out . c_str ());
		}
		XAp_att = XAp_att -> Next ();
	}
}


void v_retain_attrib_tree (
   TiXmlNode * XNp_target,
   long l_source_value,
   const char * cp_new_attrib,
   const char * cp_new_val)
{
   TiXmlElement * XEp_child;
   long l_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		l_test = XEp_child -> GetUserValue ();
      if (l_test == l_source_value)
         XEp_child -> SetAttribute (cp_new_attrib, cp_new_val);
		v_keep_bracket_if (XEp_child, l_source_value);
      v_retain_attrib_tree (XEp_child, l_source_value, cp_new_attrib, cp_new_val);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_by_order (
   TiXmlNode * XNp_target,
   const char * cp_attrib,
   int i_order,
   int i_mother_value,
   int i_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2;
   int i_test;
   unsigned u_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_attrib, i_test);
      if (i_test == i_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement ();
         for (u_child = 0; (int) u_child < i_order && XEp_child_2; u_child++)
            XEp_child_2 = XEp_child_2 -> NextSiblingElement ();
         if (XEp_child_2)
            XEp_child_2 -> SetAttribute (cp_attrib, i_child_value);
      }
      v_mark_by_order (XEp_child, cp_attrib, i_order, i_mother_value, i_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}
