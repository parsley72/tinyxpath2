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

#include "expr.h"

/// Copy 1 node to the node_set
void node_set::v_copy_node_children (const TiXmlNode * XNp_root)
{
   v_copy_node_children (XNp_root, NULL);
}

void node_set::v_copy_node_children (const TiXmlNode * XNp_root, const char * cp_lookup)
{
   const TiXmlNode * XNp_child;

   XNp_child = XNp_root -> FirstChild ();
   while (XNp_child)
   {
      if ((! cp_lookup) || ! strcmp (XNp_child -> Value (), cp_lookup))
         v_add_node_in_set (XNp_child, false);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (const TiXmlNode * XNp_root)
{
   v_copy_selected_node_recursive (XNp_root, NULL);
}

/// Copy all nodes in the tree to the node_set
void node_set::v_copy_selected_node_recursive (const TiXmlNode * XNp_root, const char * cp_lookup)
{
   TiXmlAttribute * XAp_attrib;
   const TiXmlNode * XNp_child;

   if ((! cp_lookup) || ! strcmp (XNp_root -> Value (), cp_lookup))
      v_add_node_in_set (XNp_root, false);
   if (XNp_root -> Type () == TiXmlNode::ELEMENT)
   {
      XAp_attrib = XNp_root -> ToElement () -> FirstAttribute ();      
      while (XAp_attrib)
      {  
         v_add_node_in_set (XAp_attrib, true);
         XAp_attrib = XAp_attrib -> Next ();
      }
   }
   XNp_child = XNp_root -> FirstChild ();
   while (XNp_child)
   {
      v_copy_selected_node_recursive (XNp_child, cp_lookup);
      XNp_child = XNp_child -> NextSiblingElement ();
   }
}

/// Return the string value aka concatenation of all text items
TIXML_STRING node_set::S_get_string_value () const
{
   TIXML_STRING S_res;
   const TiXmlNode * XNp_node;
   unsigned u_node;

   S_res = "";
   for (u_node = 0; u_node < u_nb_node; u_node++)
   {
      if (! op_attrib [u_node])
      {
         XNp_node = (const TiXmlNode *) XBpp_node_set [0];
         if (XNp_node -> Type () == TiXmlNode::TEXT)
            S_res += XNp_node -> Value ();
      }
   }
   return S_res;
}

/// Return 0 : none, 1 : node, 2 : attribute
int node_set::i_get_first_marked (const TiXmlNode * & XNp_node, const TiXmlAttribute * & XAp_out) const
{
   if (! u_nb_node)
      return 0;
   if (op_attrib [0])
   {
      XAp_out = (const TiXmlAttribute *) XBpp_node_set [0];
      return 2;
   }
   else
   {
      XNp_node = (const TiXmlNode *) XBpp_node_set [0];
      return 1;
   }
}
