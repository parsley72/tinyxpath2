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

void v_mark_node_and_attribute (
	TiXmlNode * XNp_node, 
	long l_marker)
{
	TiXmlElement * XEp_elem;
	TiXmlAttribute * XAp_att;

	XNp_node -> SetUserValue (l_marker);
	XEp_elem = XNp_node -> ToElement ();
	if (XEp_elem)
	{
		XAp_att = XEp_elem -> FirstAttribute ();
		while (XAp_att)
		{
			XAp_att -> SetUserValue (l_marker);
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
      v_mark_node_and_attribute (XEp_child, l_id);
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
            v_mark_node_and_attribute (XEp_child_2, l_child_value);
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

void v_mark_children_any_attrib (
   TiXmlNode * XNp_target, 
   long l_mother_value, 
   long l_child_value)
{
   TiXmlElement * XEp_child;
	TiXmlAttribute * XAp_attrib;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
		XAp_attrib = XEp_child -> FirstAttribute ();
		while (XAp_attrib)
		{
			v_set_value_of_attrib_if_mother (XEp_child, XAp_attrib -> Name (), l_mother_value, l_child_value);
			XAp_attrib = XAp_attrib -> Next ();
		}
      v_mark_children_any_attrib (XEp_child, l_mother_value, l_child_value);
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
            v_mark_node_and_attribute (XEp_child_2, l_child_value);
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
            v_mark_node_and_attribute (XEp_to_mark, l_child_value);
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
			XAp_att -> SetUserValue (1);
		else
			XAp_att -> SetUserValue (0);
		XAp_att = XAp_att -> Next ();
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
		l_test = XEp_child -> GetUserValue ();
      if (l_test == l_source_value)
			XEp_child -> SetUserValue (1);
		else
			XEp_child -> SetUserValue (0);
		v_keep_bracket_if (XEp_child, l_source_value);
      v_retain_attrib_tree (XEp_child, l_source_value);
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

void v_levelize (int i_level, FILE * Fp_out, bool o_html)
{
	int i_loop;
	for (i_loop = 0; i_loop < i_level; i_loop++)
		if (o_html)
   		fprintf (Fp_out, "&nbsp;&nbsp;&nbsp;");
		else
	   	fprintf (Fp_out, "   ");
}

void v_out_html (
	FILE * Fp_out,
	TiXmlNode * XNp_source,
	unsigned u_level)
{
	TiXmlNode * XNp_child;
	TiXmlAttribute * XAp_att;

	XNp_child = XNp_source -> FirstChild ();
	while (XNp_child)
	{
		if (XNp_child -> ToDocument ())
		{
			fprintf (Fp_out, "\nStart document\n");
		}
		else
		if (XNp_child -> ToElement ())
		{
			v_levelize ((int) u_level, Fp_out, true);
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "<b>");
			fprintf (Fp_out, "&lt;%s", XNp_child -> ToElement () -> Value ());
			XAp_att = XNp_child -> ToElement () -> FirstAttribute ();
			while (XAp_att)
			{
				if (XAp_att -> GetUserValue ())
				   fprintf (Fp_out, "<b>");
				fprintf (Fp_out, " %s='%s'", XAp_att -> Name (), XAp_att -> Value ());
				if (XAp_att -> GetUserValue ())
				   fprintf (Fp_out, "</b>");
				XAp_att = XAp_att -> Next ();
			}
			fprintf (Fp_out, "&gt;");
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "</b>");
			fprintf (Fp_out, "<br>\n");
		}
		else
		if (XNp_child -> ToComment ())
		{
			fprintf (Fp_out, "&lt;!-- %s --&gt;<br>\n", XNp_child -> ToComment () -> Value ());
		}
		else
		if (XNp_child -> ToText ())
		{
			fprintf (Fp_out, "%s\n", XNp_child -> ToText () -> Value ());
		}
		else
		if (XNp_child -> ToDeclaration ())
		{
		}
		else
		if (XNp_child -> ToUnknown ())
		{
		}
		else
			assert (false);
		v_out_html (Fp_out, XNp_child, u_level + 1);
		if (XNp_child -> ToDocument ())
		{
			fprintf (Fp_out, "\nEnd document\n");
		}
		else
		if (XNp_child -> ToElement ())
		{
			v_levelize ((int) u_level, Fp_out, true);
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "<b>");
			fprintf (Fp_out, "&lt;/%s&gt;<br>\n", XNp_child -> ToElement () -> Value ());
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "</b>");
		}
		else
		if (XNp_child -> ToComment ())
		{
		}
		else
		if (XNp_child -> ToText ())
		{
		}
		else
		if (XNp_child -> ToDeclaration ())
		{
		}
		else
		if (XNp_child -> ToUnknown ())
		{
		}
		XNp_child = XNp_child -> NextSibling ();
	}
}