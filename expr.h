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

typedef enum {e_bool, e_string, e_int, e_double, e_node_set, e_invalid} e_expression_type;

class expression_result
{
protected :	
	TIXML_STRING S_content;
	bool o_content;
	int i_content;
	double d_content;
	TiXmlNode * XNp_node_set;

public :
	e_expression_type e_type;
	expression_result ()
	{
		e_type = e_invalid;
	}
	void v_set_bool (bool o_in)
	{
		e_type = e_bool;
		o_content = o_in;
	}
	void v_set_int (int i_in)
	{
		e_type = e_int;
		i_content = i_in;
	}
	void v_set_string (const char * cp_in)
	{
		e_type = e_string;
		S_content = cp_in;
	}
	void v_set_string (TIXML_STRING S_in)
	{
		e_type = e_string;
		S_content = S_in;
	}
	void v_set_double (double d_in)
	{
		e_type = e_double;
		d_content = d_in;
	}
	int i_get_int ()
	{
		assert (e_type == e_int);
		return i_content;
	}	
	TIXML_STRING & S_get_string ()
	{
		assert (e_type == e_string);
		return S_content;
	}	
	const char * cp_get_string ()
	{
		assert (e_type == e_string);
		return S_content . c_str ();
	}	
	bool o_get_bool ()
	{
		assert (e_type == e_bool);
		return o_content;
	}	
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
	void v_set_node_set (TiXmlNode * XNp_root)
	{
		e_type = e_node_set;
		XNp_node_set = XNp_copy_selected_node (XNp_root);
	}
	void v_set_node_set (TiXmlNode * XNp_root, const char * cp_lookup)
	{
		e_type = e_node_set;
		XNp_node_set = XNp_copy_selected_node (XNp_root, cp_lookup);
	}
	const TiXmlNode * XNp_get_node_set ()
	{
		return XNp_node_set;
	}
} ;

#endif


