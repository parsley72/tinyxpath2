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
   const char * cp_attrib_name,
   int i_value)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> SetAttribute (cp_attrib_name, i_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

// Look for all first-level items that have a name, and mark them with
// the attribute 
void v_mark_first_level_name (
   TiXmlNode * XNp_target, 
   const char * cp_lookup_name,
   const char * cp_attrib_name,
   int i_value)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement (cp_lookup_name);
   while (XEp_child)
   {
      XEp_child -> SetAttribute (cp_attrib_name, i_value);
      XEp_child = XEp_child -> NextSiblingElement (cp_lookup_name);
   }
}

void v_mark_all_children (
   TiXmlNode * XNp_target, 
   const char * cp_attrib_name,
   int i_id)
{
   TiXmlElement * XEp_child;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> SetAttribute (cp_attrib_name, i_id);
      v_mark_all_children (XEp_child, cp_attrib_name, i_id);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_children_name (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_mother_value, 
   int i_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2;
   int i_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_attrib, i_test);
      if (i_test == i_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         while (XEp_child_2)
         {
            XEp_child_2 -> SetAttribute (cp_attrib, i_child_value);
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
        }
      }
      v_mark_children_name (XEp_child, cp_attrib, cp_name, i_mother_value, i_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_children_name_order (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_order,
   int i_mother_value, 
   int i_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2;
   int i_child, i_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_attrib, i_test);
      if (i_test == i_mother_value)
      {
         // gotcha
         XEp_child_2 = XEp_child -> FirstChildElement (cp_name);
         for (i_child = 0; i_child < i_order && XEp_child_2; i_child++)
            XEp_child_2 = XEp_child_2 -> NextSiblingElement (cp_name);
         if (XEp_child_2)
            XEp_child_2 -> SetAttribute (cp_attrib, i_child_value);
      }
      v_mark_children_name (XEp_child, cp_attrib, cp_name, i_mother_value, i_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_mark_children_name_last (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_mother_value, 
   int i_child_value)
{
   TiXmlElement * XEp_child, * XEp_child_2, * XEp_to_mark;
   int i_child, i_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_attrib, i_test);
      if (i_test == i_mother_value)
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
            XEp_to_mark -> SetAttribute (cp_attrib, i_child_value);
      }
      v_mark_children_name (XEp_child, cp_attrib, cp_name, i_mother_value, i_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}


void v_mark_children_inside (
   TiXmlNode * XNp_target,
   const char * cp_attrib,
   int i_mother_value,
   int i_child_value)
{
   TiXmlElement * XEp_child;
   int i_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_attrib, i_test);
      if (i_test == i_mother_value)
         v_mark_all_children (XEp_child, cp_attrib, i_child_value);
      v_mark_children_inside (XEp_child, cp_attrib, i_mother_value, i_child_value);
      XEp_child = XEp_child -> NextSiblingElement ();
   }
}

void v_retain_attrib_tree (
   TiXmlNode * XNp_target,
   const char * cp_source_attrib,
   int i_source_value,
   const char * cp_new_attrib,
   int i_new_value)
{
   TiXmlElement * XEp_child;
   int i_test;

   XEp_child = XNp_target -> FirstChildElement ();
   while (XEp_child)
   {
      XEp_child -> Attribute (cp_source_attrib, i_test);
      XEp_child -> RemoveAttribute (cp_source_attrib);
      if (i_test == i_source_value)
         XEp_child -> SetAttribute (cp_new_attrib, i_new_value);
      v_retain_attrib_tree (XEp_child, cp_source_attrib, i_source_value, cp_new_attrib, i_new_value);
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
