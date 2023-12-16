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

#ifndef __XPATH_EXPR_H
#define __XPATH_EXPR_H

#include <stdexcept>

#include "node_set.h"
#include "tinyxml2.h"
#include "tinyxpath_conf.h"

namespace TinyXPath {

/// Expression types
typedef enum { e_bool, e_string, e_int, e_double, e_node_set, e_invalid } e_expression_type;

/// Class holding the result of an expression (e_expression_type)
class expression_result {
   protected:
    /// String content
    std::string _S_content;
#ifdef TINYXPATH_DEBUG
    /// Comment. This is for debugging only, for stack dump
    std::string _S_comment;
#endif
    /// bool content
    bool _o_content;
    /// integer content
    int _i_content;
    /// double content
    double _d_content;
    /// node set content
    node_set _ns_set;
    /// Ptr to the root node
    const tinyxml2::XMLNode* _XNp_root;

   public:
    /// expression type
    e_expression_type _e_type;
    /// Dummy constructor
    expression_result(const tinyxml2::XMLNode* XNp_in_root) : _XNp_root(XNp_in_root) {
        _e_type = e_invalid;
        _o_content = false;
        _i_content = 0;
        _d_content = 0.0;
    }

    expression_result() {
        _XNp_root = nullptr;
        _e_type = e_invalid;
        _o_content = false;
        _i_content = 0;
        _d_content = 0.0;
    }

    void v_set_root(const tinyxml2::XMLNode* XNp_in) {
        _XNp_root = XNp_in;
    }

    /// Copy constructor
    expression_result(const expression_result& er_2) {
        *this = er_2;
    }

    expression_result& operator=(const expression_result& er_2) {
        _XNp_root = er_2._XNp_root;
        _e_type = er_2._e_type;
        switch (_e_type) {
            case e_bool:
                _o_content = er_2._o_content;
                break;
            case e_int:
                _i_content = er_2._i_content;
                break;
            case e_string:
                _S_content = er_2._S_content;
                break;
            case e_double:
                _d_content = er_2._d_content;
                break;
            case e_node_set:
                _ns_set = er_2._ns_set;
                break;
            case e_invalid:
                throw std::runtime_error("Invalid XPath expression");
        }
#ifdef TINYXPATH_DEBUG
        _S_comment = er_2._S_comment;
#endif
        return *this;
    }
    /// Set expression_result to a bool
    void v_set_bool(bool o_in) {
        _e_type = e_bool;
        _o_content = o_in;
    }
    /// Set expression_result to an int
    void v_set_int(int i_in) {
        _e_type = e_int;
        _i_content = i_in;
    }
    /// Set expression_result to a string
    void v_set_string(const char* cp_in) {
        _e_type = e_string;
        _S_content = cp_in;
    }
    /// Set expression_result to a string
    void v_set_string(std::string S_in) {
        _e_type = e_string;
        _S_content = S_in;
    }
    /// Set expression_result to a double
    void v_set_double(double d_in) {
        _e_type = e_double;
        _d_content = d_in;
    }
    /// Set the comment associated with a stack element. This is for debugging
    void v_set_comment(const char* cp_in) {
#ifdef TINYXPATH_DEBUG
        _S_comment = cp_in;
#endif
    }
    int i_get_int();
    std::string S_get_string();
    /// Get the expression_result as a string
    const char* cp_get_string() {
        assert(_e_type == e_string);
        return _S_content.c_str();
    }
    bool o_get_bool();
    double d_get_double();
    /// Set the expression_result as a node set
    void v_set_node_set(node_set* nsp_source) {
        _e_type = e_node_set;
        _ns_set = *nsp_source;
    }
    /// Set the expression_result as a node set
    void v_set_node_set(tinyxml2::XMLNode* _XNp_root) {
        _e_type = e_node_set;
        _ns_set.v_copy_node_children(_XNp_root);
    }
    /// Set the expression_result as a node set
    void v_set_node_set(tinyxml2::XMLNode* _XNp_root, const char* cp_lookup) {
        _e_type = e_node_set;
        _ns_set.v_copy_node_children(_XNp_root, cp_lookup);
    }
    /// Set the expression_result as a node set
    void v_set_node_set_recursive(tinyxml2::XMLNode* _XNp_root) {
        _e_type = e_node_set;
        _ns_set.v_copy_selected_node_recursive(_XNp_root);
    }
    /// Set the expression_result as a node set
    void v_set_node_set_recursive(tinyxml2::XMLNode* _XNp_root, const char* cp_lookup) {
        _e_type = e_node_set;
        _ns_set.v_copy_selected_node_recursive(_XNp_root, cp_lookup);
    }
    /// Set the expression_result as an empty node set
    void v_set_node_set() {
        _e_type = e_node_set;
    }
    /// Get the expression_result as a node set
    node_set* nsp_get_node_set() {
        return &_ns_set;
    }
#ifdef TINYXPATH_DEBUG
    void v_dump();
#endif
};

}  // namespace TinyXPath

#endif  // __XPATH_EXPR_H
