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

#ifndef __XPATHAPPL_H
#define __XPATHAPPL_H

#include "xmlutil.h"
#include "tinyxpstream.h"
#include "workitem.h" 
#include "workstack.h"

/// Top-level object that applies an XPath expression to a source XML tree
class xpath_from_source : public xpath_stream
{
protected :
   TiXmlNode * XNp_source;
   TiXmlDocument * XDp_target;

   work_stack * wsp_stack;
   long l_mark_level;
   TiXmlElement * XEp_root;
	TIXML_STRING S_run (const char * cp_test_name, FILE * Fp_html);
   TIXML_STRING S_evaluate_xpath_string (long l_mark_level);
	void v_init ();
	void v_close ();

public :
   xpath_from_source (TiXmlNode * XNp_source_tree, const char * cp_in_expr);
   virtual ~ xpath_from_source ();
   virtual void v_action (unsigned u_rule, unsigned u_sub, unsigned u_variable, const char * cp_explain);
   TIXML_STRING S_apply_xpath (const char * cp_test_name, FILE * Fp_html_out);
} ;

#endif
