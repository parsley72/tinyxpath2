#ifndef __XMLUTIL_H
#define __XMLUTIL_H

#include "tinyxml.h"

extern void v_clone_children (
   TiXmlNode * XNp_source, 
   TiXmlNode * XNp_target);
extern void v_mark_first_level (
   TiXmlNode * XNp_target, 
   const char * cp_attrib_name,
   int i_value);
extern void v_mark_first_level_name (
   TiXmlNode * XNp_target, 
   const char * cp_lookup_name,
   const char * cp_attrib_name,
   int i_value);
extern void v_mark_all_children (
   TiXmlNode * XNp_source, 
   const char * cp_attrib_name,
   int i_id);
extern void v_mark_children_name (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_mother_value, 
   int i_child_value);
extern void v_mark_children_name_order (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_order,
   int i_mother_value, 
   int i_child_value);
extern void v_mark_children_name_last (
   TiXmlNode * XNp_target, 
   const char * cp_attrib, 
   const char * cp_name,
   int i_mother_value, 
   int i_child_value);
extern void v_mark_children_inside (
   TiXmlNode * XNp_target,
   const char * cp_attrib,
   int i_mother_value,
   int i_child_value);
extern void v_retain_attrib_tree (
   TiXmlNode * XNp_target,
   const char * cp_source_attrib,
   int i_source_value,
   const char * cp_new_attrib,
   int i_new_value);
extern void v_mark_by_order (
   TiXmlNode * XNp_target,
   const char * cp_source_attrib,
   int i_order,
   int i_mother_value,
   int i_child_value);

#endif

