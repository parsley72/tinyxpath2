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

/**
   \file tinyutil.h
   \author Yves Berquin
   Miscellaneous utilities for the TinyXPath project
*/

#ifndef __TINYUTIL_H
#define __TINYUTIL_H

#include "tinyxml.h"

#ifdef _MSC_VER
   #pragma warning (disable : 4514)  
#endif

typedef unsigned char _byte_;

/// Lexical XPath elements
enum lexico {
            // single chars

            lex_null, lex_none, lex_space, lex_digit, lex_bchar, lex_extend, lex_slash, lex_at,
            lex_dot, lex_minus, lex_under, lex_colon, lex_scolon, lex_2_quote, lex_1_quote, 
            lex_dollar, lex_oparen, lex_cparen, lex_star, lex_plus, lex_comma, lex_lt,
            lex_gt, lex_equal, lex_obrack, lex_cbrack, lex_orchar, lex_exclam,
            
            // constructions
            
            lex_ncname, lex_number, lex_literal, lex_2_colon, lex_2_slash, lex_2_dot, lex_not_equal,
            lex_lt_equal, lex_gt_equal,

            // keywords

            lex_start_keyword,
            lex_or = lex_start_keyword, lex_and, lex_mod, lex_div,

            lex_start_axis_name,
            lex_ancestor = lex_start_axis_name, lex_ancestor_or_self, lex_attribute, lex_child, lex_descendant, 
            lex_descendant_or_self, lex_following, lex_following_sibling, lex_namespace, 
            lex_parent, lex_preceding, lex_preceding_sibling, lex_self, 
            lex_end_axis_name = lex_self,

            lex_processing_instruction,
            lex_comment, lex_node, lex_text,
            lex_end_keyword = lex_text

             };

/// XPath constructions. The ordinals are rules of XPath or XML definitions in w3c
enum xpath_construct {
   xpath_unknown = 0,
   xpath_location_path = 1,
   xpath_absolute_location_path = 2,
   xpath_relative_location_path = 3,
   xpath_step = 4,
   xpath_axis_specifier = 5,
   xpath_axis_name = 6,
   xpath_node_test = 7,
   xpath_predicate = 8,
   xpath_predicate_expr = 9,
   xpath_abbreviated_absolute_location_path = 10,
   xpath_abbrieviated_step = 12,
   xpath_abbreviated_axis_specifier = 13,
   xpath_expr = 14,
   xpath_primary_expr = 15,
   xpath_function_call = 16,
   xpath_argument = 17,
   xpath_union_expr = 18,
   xpath_path_expr = 19,
   xpath_filter_expr = 20,
   xpath_or_expr = 21,
   xpath_and_expr = 22,
   xpath_equality_expr = 23,
   xpath_relational_expr = 24,
   xpath_additive_expr = 25,
   xpath_multiplicative_expr = 26,
   xpath_unary_expr = 27,
   xpath_multiply_operator = 34,
   xpath_variable_reference = 36,
   xpath_name_test = 37,
   xpath_xml_q_name = 206,
   xpath_xml_prefix = 207,
   xpath_xml_local_part = 208};

extern const char * cp_disp_class_lex (lexico lex_in);
extern const char * cp_disp_class (_byte_ b_in);
extern bool o_is_axis_name (lexico lex_test);
extern lexico lex_get_class (_byte_ b_in);
extern void v_generate_ascii_htm ();
extern lexico lex_test_id (const _byte_ * bp_str, unsigned u_size);
extern TIXML_STRING S_remove_lead_trail (const char * cp_in);

#endif
