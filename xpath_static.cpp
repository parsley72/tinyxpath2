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
   \file xpath_static.cpp
   \author Yves Berquin
*/

#include "xpath_static.h"

#include <string>

using namespace std;
using namespace tinyxml2;

namespace TinyXPath {
/// Static function to compute an integer XPath expression, without an error check
int i_xpath_int(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    return xp_proc.i_compute_xpath();
}

/// Static function to compute a double XPath expression, without an error check
double d_xpath_double(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    return xp_proc.d_compute_xpath();
}

/// Static function to compute a bool XPath expression, without an error check
bool o_xpath_bool(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    return xp_proc.o_compute_xpath();
}

/// Static function to compute a string XPath expression, without an error check
string S_xpath_string(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    return xp_proc.S_compute_xpath();
}

/// Static function to compute a node XPath expression, without an error check
const XMLNode* XNp_xpath_node(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    unsigned u_nb;

    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    u_nb = xp_proc.u_compute_xpath_node_set();
    if (!u_nb)
        return nullptr;
    return xp_proc.XNp_get_xpath_node(0);
}

/// Static function to compute an attribute XPath expression, without an error check
const XMLAttribute* XAp_xpath_attribute(const XMLNode* XNp_source_tree, const char* cp_xpath_expr) {
    unsigned u_nb;

    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    u_nb = xp_proc.u_compute_xpath_node_set();
    if (!u_nb)
        return nullptr;
    return xp_proc.XAp_get_xpath_attribute(0);
}

/// Static function to compute an integer XPath expression, with an error check
bool o_xpath_int(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, int& i_res) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    i_res = xp_proc.i_compute_xpath();
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

/// Static function to compute a double XPath expression, without an error check
bool o_xpath_double(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, double& d_res) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    d_res = xp_proc.d_compute_xpath();
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

/// Static function to compute a bool XPath expression, without an error check
bool o_xpath_bool(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, bool& o_res) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    o_res = xp_proc.o_compute_xpath();
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

/// Static function to compute a string XPath expression, without an error check
bool o_xpath_string(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, string& S_res) {
    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    S_res = xp_proc.S_compute_xpath();
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

/// Static function to compute a node XPath expression, without an error check
bool o_xpath_node(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, const XMLNode*& XNp_node) {
    unsigned u_nb;

    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    u_nb = xp_proc.u_compute_xpath_node_set();
    if (!u_nb)
        return false;
    XNp_node = xp_proc.XNp_get_xpath_node(0);
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

/// Static function to compute an attribute XPath expression, without an error check
bool o_xpath_attribute(const XMLNode* XNp_source_tree, const char* cp_xpath_expr, const XMLAttribute*& XAp_attrib) {
    unsigned u_nb;

    xpath_processor xp_proc(XNp_source_tree, cp_xpath_expr);
    u_nb = xp_proc.u_compute_xpath_node_set();
    if (!u_nb)
        return false;
    XAp_attrib = xp_proc.XAp_get_xpath_attribute(0);
    return xp_proc._e_error == TinyXPath::xpath_processor::e_no_error;
}

}  // namespace TinyXPath
