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

#ifndef __EXPR_H
#define __EXPR_H

#include "tinyxml.h"
#include "xmlutil.h"

/// Expression types
typedef enum {e_bool, e_string, e_int, e_double, e_node_set, e_invalid} e_expression_type;

/// Node set class. A node set is an unordered collection of node
class node_set
{
public :
   /// constructor : creates an empty set
   node_set () 
   {
      u_nb_node = 0; 
      XBpp_node_set = NULL;
      op_attrib = NULL;
   }
   /// copy constructor
   node_set (const node_set & ns2)
   {
      u_nb_node = ns2 . u_nb_node;
      XBpp_node_set = new const TiXmlBase * [u_nb_node];
      memcpy (XBpp_node_set, ns2 . XBpp_node_set, u_nb_node * sizeof (TiXmlBase *));
      op_attrib = new bool [u_nb_node];
      memcpy (op_attrib, ns2 . op_attrib, u_nb_node * sizeof (bool));
   }
   /// destructor
   ~ node_set ()
   {
      if (u_nb_node && XBpp_node_set) 
         delete [] XBpp_node_set;
      if (u_nb_node && op_attrib)
         delete [] op_attrib;
      u_nb_node = 0; 
      XBpp_node_set = NULL;
      op_attrib = NULL;
   }
   /// Adds a new node in the node set
   void v_add_node_in_set (const TiXmlBase * XBp_member, bool o_attrib)
   {
      const TiXmlBase ** XBpp_new_list;
      bool * op_new_list;

      XBpp_new_list = new const TiXmlBase * [u_nb_node + 1];
      op_new_list = new bool [u_nb_node + 1];
      if (u_nb_node)
      {
         memcpy (XBpp_new_list, XBpp_node_set, u_nb_node * sizeof (TiXmlBase *));
         delete [] XBpp_node_set;
         memcpy (op_new_list, op_attrib, u_nb_node * sizeof (bool));
         delete [] op_attrib;
      }
      XBpp_new_list [u_nb_node] = XBp_member;
      XBpp_node_set = XBpp_new_list;
      op_new_list [u_nb_node] = o_attrib;
      op_attrib = op_new_list;
      u_nb_node++;
   }
   /// Get nb of nodes in the node set
   unsigned u_get_nb_node_in_set () const 
   {
      return u_nb_node;
   }
   /// Get a node.
   const TiXmlBase * XBp_get_node_in_set (unsigned u_which) 
   {
      assert (u_which < u_nb_node);
      return XBpp_node_set [u_which];
   }
   /// Check if a node is an attribute or another node. This is needed because TinyXML has a weird exception for
   /// attributes not being children of TiXmlNode
   bool o_is_attrib (unsigned u_which)
   {
      assert (u_which < u_nb_node);
      return op_attrib [u_which];
   }
   void v_copy_node_children (const TiXmlNode * XNp_root);
   void v_copy_node_children (const TiXmlNode * XNp_root, const char * cp_lookup);
   void v_copy_selected_node_recursive (const TiXmlNode * XNp_root);
   void v_copy_selected_node_recursive (const TiXmlNode * XNp_root, const char * cp_lookup);
   TIXML_STRING S_get_string_value () const;
   int i_get_first_marked (const TiXmlNode * & XNp_node, const TiXmlAttribute * & XAp_out) const;
protected :
   /// Nb of nodes in the set
   unsigned u_nb_node;
   /// List of node pointers to the 
	const TiXmlBase ** XBpp_node_set;
   /// Attributes flag list
   bool * op_attrib;
} ;

/// Class holding the result of an expression (e_expression_type)
class expression_result
{
protected :	
   /// String content
	TIXML_STRING S_content;
   /// bool content
	bool o_content;
   /// integer content
	int i_content;
   /// double content
	double d_content;
   /// node set content
   node_set ns_set;

public :
   /// expression type
	e_expression_type e_type;
   /// Dummy constructor
	expression_result ()
	{
		e_type = e_invalid;
	}
   /// Copy constructor
	expression_result (const expression_result & er_2)
	{
		e_type = er_2 . e_type;
      switch (e_type)
      {
         case e_bool :
            o_content = er_2 . o_content;
            break;
         case e_int :
            i_content = er_2 . i_content;
            break;
         case e_string :
            S_content = er_2 . S_content;
            break;
         case e_double :
            d_content = er_2 . d_content;
            break;
         case e_node_set :
            ns_set = er_2 . ns_set;
            break;
      }
	}
   /// Set expression_result to a bool
	void v_set_bool (bool o_in)
	{
		e_type = e_bool;
		o_content = o_in;
	}
   /// Set expression_result to an int
	void v_set_int (int i_in)
	{
		e_type = e_int;
		i_content = i_in;
	}
   /// Set expression_result to a string
	void v_set_string (const char * cp_in)
	{
		e_type = e_string;
		S_content = cp_in;
	}
   /// Set expression_result to a string
	void v_set_string (TIXML_STRING S_in)
	{
		e_type = e_string;
		S_content = S_in;
	}
   /// Set expression_result to a double
	void v_set_double (double d_in)
	{
		e_type = e_double;
		d_content = d_in;
	}
   /// Get the expression_result as an int
	int i_get_int ()
	{
		assert (e_type == e_int);
		return i_content;
	}	
   /// Get the expression_result as a string
	TIXML_STRING & S_get_string ()
	{
		assert (e_type == e_string);
		return S_content;
	}	
   /// Get the expression_result as a string
	const char * cp_get_string ()
	{
		assert (e_type == e_string);
		return S_content . c_str ();
	}	
   /// Get the expression_result as a bool
	bool o_get_bool ()
	{
		assert (e_type == e_bool);
		return o_content;
	}	
   /// Get the expression_result as a double
	double d_get_double ()
	{
		switch (e_type)
		{
			case e_double :
				return d_content;
			case e_int :
				return i_content;
		}
		assert (false);
		return 0;
	}
   /// Set the expression_result as a node set
	void v_set_node_set (TiXmlNode * XNp_root)
	{
		e_type = e_node_set;
      ns_set . v_copy_node_children (XNp_root);
	}
   /// Set the expression_result as a node set
	void v_set_node_set (TiXmlNode * XNp_root, const char * cp_lookup)
	{
		e_type = e_node_set;
      ns_set . v_copy_node_children (XNp_root, cp_lookup);
	}
   /// Set the expression_result as a node set
	void v_set_node_set_recursive (TiXmlNode * XNp_root)
	{
		e_type = e_node_set;
      ns_set . v_copy_selected_node_recursive (XNp_root);
	}
   /// Set the expression_result as a node set
	void v_set_node_set_recursive (TiXmlNode * XNp_root, const char * cp_lookup)
	{
		e_type = e_node_set;
      ns_set . v_copy_selected_node_recursive (XNp_root, cp_lookup);
	}
   /// Set the expression_result as an empty node set
	void v_set_node_set ()
   {
      e_type = e_node_set;
   }
   /// Get the expression_result as a node set
   node_set * nsp_get_node_set ()
   {
      return & ns_set;
   }
} ;

#endif


