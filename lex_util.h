/*
www.sourceforge.net/projects/tinyxpath
Copyright (c) 2002-2004 Yves Berquin (yvesb@users.sourceforge.net)

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

#include <string>

#include "tinyxml2.h"
#include "tinyxpath_conf.h"

#ifdef _MSC_VER
#pragma warning(disable : 4514)
#endif

namespace TinyXPath {

typedef unsigned char _byte_;

/// Lexical XPath elements
enum class lex {
    // single chars

    null,
    none,
    space,
    digit,
    bchar,
    extend,
    slash,
    at,
    dot,
    minus,
    under,
    colon,
    scolon,
    two_quote,
    one_quote,
    dollar,
    oparen,
    cparen,
    star,
    plus,
    comma,
    lt,
    gt,
    equal,
    obrack,
    cbrack,
    orchar,
    exclam,

    // constructions

    ncname,
    number,
    literal,
    two_colon,
    two_slash,
    two_dot,
    not_equal,
    lt_equal,
    gt_equal,

    // keywords

    start_keyword,
    _or = start_keyword,
    _and,
    mod,
    div,

    start_axis_name,
    ancestor = start_axis_name,
    ancestor_or_self,
    attribute,
    child,
    descendant,
    descendant_or_self,
    following,
    following_sibling,
    _namespace,
    parent,
    preceding,
    preceding_sibling,
    self,
    end_axis_name = self,

    processing_instruction,
    comment,
    node,
    text,
    end_keyword = text

};

/// XPath constructions. The ordinals are rules of XPath or XML definitions in w3c
enum class xpath_construct {
    unknown = 0,
    location_path = 1,
    absolute_location_path = 2,
    relative_location_path = 3,
    step = 4,
    axis_specifier = 5,
    axis_name = 6,
    node_test = 7,
    predicate = 8,
    predicate_expr = 9,
    abbreviated_absolute_location_path = 10,
    abbrieviated_step = 12,
    abbreviated_axis_specifier = 13,
    expr = 14,
    primary_expr = 15,
    function_call = 16,
    argument = 17,
    union_expr = 18,
    path_expr = 19,
    filter_expr = 20,
    or_expr = 21,
    and_expr = 22,
    equality_expr = 23,
    relational_expr = 24,
    additive_expr = 25,
    multiplicative_expr = 26,
    unary_expr = 27,
    multiply_operator = 34,
    variable_reference = 36,
    name_test = 37,
    xml_q_name = 206,
    xml_prefix = 207,
    xml_local_part = 208
};

enum xpath_sub {
    xpath_absolute_location_path_slash_rel,
    xpath_absolute_location_path_slash,
    xpath_absolute_location_path_abbrev,
    xpath_relative_location_path_rel_step,
    xpath_relative_location_path_rel_double_slash_step,
    xpath_relative_location_path_step,
    xpath_step_abbrev,
    xpath_step_full,
    xpath_primary_expr_variable,
    xpath_primary_expr_paren_expr,
    xpath_primary_expr_literal,
    xpath_primary_expr_number,
    xpath_primary_expr_function_call,
    xpath_name_test_star,
    xpath_name_test_ncname,
    xpath_name_test_qname,
    xpath_xml_q_name_colon,
    xpath_xml_q_name_simple,
    xpath_axis_specifier_at,
    xpath_axis_specifier_axis_name,
    xpath_axis_specifier_empty,
    xpath_path_expr_location_path,
    xpath_path_expr_filter,
    xpath_path_expr_slash,
    xpath_path_expr_2_slash,
    xpath_filter_expr_primary,
    xpath_filter_expr_predicate,
    xpath_location_path_rel,
    xpath_location_path_abs,
    xpath_node_test_reserved_keyword,
    xpath_node_test_pi,
    xpath_node_test_pi_lit,
    xpath_node_test_name_test,
    xpath_equality_expr_equal,
    xpath_equality_expr_not_equal,
    xpath_equality_expr_simple,
    xpath_union_expr_simple,
    xpath_union_expr_union,
    xpath_additive_expr_simple,
    xpath_additive_expr_plus,
    xpath_additive_expr_minus,
    xpath_additive_expr_more_plus,
    xpath_additive_expr_more_minus,
    xpath_or_expr_simple,
    xpath_or_expr_or,
    xpath_or_expr_more,
    xpath_and_expr_simple,
    xpath_and_expr_and,
    xpath_relational_expr_simple,
    xpath_relational_expr_lt,
    xpath_relational_expr_gt,
    xpath_relational_expr_lte,
    xpath_relational_expr_gte,
    xpath_unary_expr_simple,
    xpath_unary_expr_minus,
    xpath_multiplicative_expr_simple,
    xpath_multiplicative_expr_star,
    xpath_multiplicative_expr_div,
    xpath_multiplicative_expr_mod,
    dummy
};

extern const char* cp_disp_class_lex(lex lex_in);
extern bool o_is_axis_name(lex lex_test);
extern lex lex_get_class(_byte_ b_in);
extern lex lex_test_id(const _byte_* bp_str, unsigned u_size, lex);
extern std::string S_remove_lead_trail(const char* cp_in);
extern void v_assign_int_to_string(std::string& S_string, int i_val);
extern void v_assign_double_to_string(std::string& S_string, double d_val);
#ifdef TINYXPATH_DEBUG
extern const char* cp_disp_construct(xpath_construct xc);
extern void v_generate_ascii_htm();
extern const char* cp_disp_class(_byte_ b_in);
#endif
}  // namespace TinyXPath

#endif  // __TINYUTIL_H
