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
#ifndef __XMLUTIL_H
#define __XMLUTIL_H

#include "tinyxml.h"

extern void v_clone_children (
   TiXmlNode * XNp_source, 
   TiXmlNode * XNp_target);
extern void v_mark_first_level (
   TiXmlNode * XNp_target, 
   long l_value);
extern void v_mark_first_level_name (
   TiXmlNode * XNp_target, 
   const char * cp_lookup_name,
   long l_value);
extern void v_mark_all_children (
   TiXmlNode * XNp_source, 
   long l_id);
extern void v_mark_children_attrib (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long l_mother_value, 
   long l_child_value);
extern void v_mark_children_any_attrib (
   TiXmlNode * XNp_target, 
   long l_mother_value, 
   long l_child_value);
extern void v_mark_children_name (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long l_mother_value, 
   long l_child_value);
extern void v_mark_children_name_order (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   int i_order,
   long l_mother_value, 
   long l_child_value);
extern void v_mark_children_name_last (
   TiXmlNode * XNp_target, 
   const char * cp_name,
   long _mother_value, 
   long l_child_value);
extern void v_mark_children_inside (
   TiXmlNode * XNp_target,
   long l_mother_value,
   long l_child_value);
extern void v_retain_attrib_tree (
   TiXmlNode * XNp_target,
   long l_source_value);
extern void v_mark_by_order (
   TiXmlNode * XNp_target,
   const char * cp_source_attrib,
   int i_order,
   int i_mother_value,
   int i_child_value);
extern void v_mark_not_attrib (
   TiXmlNode * XNp_target,
   int i_mother_value,
	int i_attrib_value,
   int i_child_value);

extern void v_out_html (
	FILE * Fp_out,
	TiXmlNode * XNp_source,
	unsigned u_level);
extern void v_levelize (int i_level, FILE * Fp_out = stdout, bool o_html = false);

#endif

