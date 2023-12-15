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

#ifndef __NODE_SET_H
#define __NODE_SET_H

#include <cassert>
#include <string>

#include "tinyxml2.h"
#include "tinyxpath_conf.h"

namespace TinyXPath {

/// Node set class. A node set is an unordered collection of node
class node_set {
   public:
    /// constructor : creates an empty set
    node_set() {
        _u_nb_node = 0;
        _vpp_node_set = nullptr;
        _op_attrib = nullptr;
    }
    /// copy constructor
    node_set(const node_set& ns2);
    /// destructor
    ~node_set() {
        if (_u_nb_node && _vpp_node_set)
            delete[] _vpp_node_set;
        if (_u_nb_node && _op_attrib)
            delete[] _op_attrib;
        _u_nb_node = 0;
        _vpp_node_set = nullptr;
        _op_attrib = nullptr;
    }

    node_set& operator=(const node_set& ns2);
    void v_add_base_in_set(const void* XBp_member, bool o_attrib);

    /// Adds an attribute in the node set
    void v_add_attrib_in_set(const tinyxml2::XMLAttribute* XAp_attrib) {
        v_add_base_in_set(XAp_attrib, true);
    }

    /// Adds a node in the node set
    void v_add_node_in_set(const tinyxml2::XMLNode* XNp_node) {
        v_add_base_in_set(XNp_node, false);
    }

    bool o_exist_in_set(const void* XBp_member);
    void v_add_all_foll_node(const tinyxml2::XMLNode* XNp_node, const std::string& S_name);
    void v_add_all_prec_node(const tinyxml2::XMLNode* XNp_node, const std::string& S_name);

    /// Add a new node, if the name is "*" or if the name is the same as the node
    void v_add_node_in_set_if_name_or_star(const tinyxml2::XMLNode* XNp_node, const std::string& S_name) {
        bool o_keep;
        if (S_name == "*")
            o_keep = true;
        else
            o_keep = !strcmp(XNp_node->Value(), S_name.c_str());
        if (o_keep)
            v_add_node_in_set(XNp_node);
    }

    /// Add a new attrib, if the name is "*" or if the name is the same as the node
    void v_add_attrib_in_set_if_name_or_star(const tinyxml2::XMLAttribute* XAp_attrib, const std::string& S_name) {
        bool o_keep;
        if (S_name == "*")
            o_keep = true;
        else
            o_keep = !strcmp(XAp_attrib->Name(), S_name.c_str());
        if (o_keep)
            v_add_attrib_in_set(XAp_attrib);
    }

    /// Get nb of nodes in the node set
    unsigned u_get_nb_node_in_set() const {
        return _u_nb_node;
    }

    /// Get a node
    const tinyxml2::XMLNode* XNp_get_node_in_set(unsigned u_which) {
        assert(u_which < _u_nb_node);
        assert(!o_is_attrib(u_which));
        return (const tinyxml2::XMLNode*)_vpp_node_set[u_which];
    }

    /// Get an attribute
    const tinyxml2::XMLAttribute* XAp_get_attribute_in_set(unsigned u_which) {
        assert(u_which < _u_nb_node);
        assert(o_is_attrib(u_which));
        return (const tinyxml2::XMLAttribute*)_vpp_node_set[u_which];
    }

    /// Check if a node is an attribute or another node. This is needed because TinyXML has a weird exception for
    /// attributes not being children of XMLNode
    bool o_is_attrib(unsigned u_which) {
        assert(u_which < _u_nb_node);
        return _op_attrib[u_which];
    }

    /// Get a node value. The value is the name for an element, and the attribute value for an attribute
    std::string S_get_value(unsigned u_which) {
        std::string S_res;

        if (o_is_attrib(u_which))
            S_res = XAp_get_attribute_in_set(u_which)->Value();
        else
            S_res = XNp_get_node_in_set(u_which)->Value();
        return S_res;
    }

    /// Get the integer value of a node
    int i_get_value(unsigned u_which) {
        return atoi(S_get_value(u_which).c_str());
    }

    /// Get the real value of a node
    double d_get_value(unsigned u_which) {
        return atof(S_get_value(u_which).c_str());
    }

    void v_copy_node_children(const tinyxml2::XMLNode* XNp_root);
    void v_copy_node_children(const tinyxml2::XMLNode* XNp_root, const char* cp_lookup);
    void v_copy_selected_node_recursive(const tinyxml2::XMLNode* XNp_root);
    void v_copy_selected_node_recursive(const tinyxml2::XMLNode* XNp_root, const char* cp_lookup);
    void v_copy_selected_node_recursive_no_attrib(const tinyxml2::XMLNode* XNp_root, const char* cp_lookup);
    void v_copy_selected_node_recursive_root_only(const tinyxml2::XMLNode* XNp_root, const tinyxml2::XMLNode* XNp_base);
    std::string S_get_string_value() const;
    void v_dump();
    void v_document_sort(const tinyxml2::XMLNode* XNp_root);

   protected:
    /// Nb of nodes in the set
    unsigned _u_nb_node;
    /// List of node pointers to the
    const void** _vpp_node_set;
    /// Attributes flag list
    bool* _op_attrib;
};

}  // namespace TinyXPath

#endif  // __NODE_SET_H
