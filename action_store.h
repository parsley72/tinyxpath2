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
   \file action_store.h
   \author Yves Berquin
*/

#ifndef __ACTION_STORE_H
#define __ACTION_STORE_H

#include <string>

#include "tinyxml2.h"

namespace TinyXPath {

/// Action item for the XPath action placeholder
/// \n Able to store 1 quadruplet (3 integers and a string)
class action_item {
   public:
    /// constructor
    action_item(int i_in_1, int i_in_2, int i_in_3, const char* cp_string)
        : _i_1(i_in_1), _i_2(i_in_2), _i_3(i_in_3), _S_string(cp_string) {
    }

    /// Retrieve the set of values
    void v_get(int& i_out_1, int& i_out_2, int& i_out_3, std::string& S_out) {
        i_out_1 = _i_1;
        i_out_2 = _i_2;
        i_out_3 = _i_3;
        S_out = _S_string;
    }

   protected:
    /// Integer triplet values
    int _i_1, _i_2, _i_3;
    /// String value
    std::string _S_string;
};

/// The XPath action stack. Not a stack per se, only a placeholder
/// \n It's able to store quadruplets (3 integers and a string)
/// \n It allocates them by set of 100
class action_store {
   public:
    action_store();
    ~action_store();
    /// add an element on the placeholder, given its details
    void v_add(int _i_1, int _i_2, int _i_3, const char* cp_string);
    /// Get the current nb of stored elements
    int i_get_size() {
        return _i_size;
    }
    /// Get one element from the placeholder
    void v_get(int _i_position, int& _i_1, int& _i_2, int& _i_3, std::string& S_out);
    /// Get the current position. See _i_position.
    int i_get_position() {
        return _i_position;
    }
    /// Set the position to an arbitrary value. See _i_position.
    void v_set_position(int i_where) {
        _i_position = i_where;
    }
    /// Decrement the position. See _i_position.
    void v_dec_position() {
        _i_position--;
    }

   protected:
    /// Used number of elements
    int _i_size;
    /// This value is informative and is not related
    /// to the nb of elements in the placeholder
    int _i_position;
    /// Nb of allocated elements
    int _i_alloc;
    /// Pointers to the allocated elements
    action_item** _aipp_list;
};

}  // namespace TinyXPath

#endif  // __ACTION_STORE_H
