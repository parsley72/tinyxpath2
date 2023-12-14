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
#ifndef __TINYLEXTOKEN_H
#define __TINYLEXTOKEN_H

#include <stdio.h>
#include <string.h>

#include "lex_util.h"

namespace TinyXPath {

/**
   A lexical token stream, part of a single linked list
*/
class lex_token {
    /// Representation
    char* _cp_value;
    /// lexical value
    lexico _l_enum;
    /// pointer to next element
    lex_token *_ltp_next, *_ltp_prev;

   public:
    /// constructor
    lex_token(lexico l_in_enum, const _byte_* bp_in_value, unsigned u_in_size) {
        _l_enum = l_in_enum;
        _cp_value = new char[u_in_size + 1];
        if (u_in_size)
            memcpy(_cp_value, bp_in_value, u_in_size);
        _cp_value[u_in_size] = 0;
        _ltp_next = nullptr;
        _ltp_prev = nullptr;
    }
    /// set the next element in list
    void v_set_next(lex_token* ltp_in_next) {
        _ltp_next = ltp_in_next;
    }
    /// set the previous element in list
    void v_set_prev(lex_token* ltp_in_prev) {
        _ltp_prev = ltp_in_prev;
    }
    /// destructor. doesn't destroys next in list
    ~lex_token() {
        if (_cp_value)
            delete[] _cp_value;
    }

    /// get next in list
    lex_token* ltp_get_next() const {
        return _ltp_next;
    }

    /// get next in list
    lex_token* ltp_get_prev() const {
        return _ltp_prev;
    }

    /// Return the next i-th element in the list
    lex_token* ltp_get_next(int i_nb) {
        lex_token* ltp_ret;
        int i;

        ltp_ret = this;
        for (i = 0; i < i_nb; i++) {
            if (!ltp_ret)
                return nullptr;
            ltp_ret = ltp_ret->ltp_get_next();
            if (!ltp_ret || !ltp_ret->o_is_valid())
                return nullptr;
        }
        return ltp_ret;
    }

    /// return the value of a lexical element
    lexico lex_get_value() const {
        return _l_enum;
    }

    /// Set the string value of a lexical element
    void v_set(lexico lex_in, const char* cp_repre) {
        unsigned u_length;

        _l_enum = lex_in;
        delete[] _cp_value;
        u_length = strlen(cp_repre);
        _cp_value = new char[u_length + 1];
        strcpy(_cp_value, cp_repre);
    }

    /// return the string value of a lexical element
    const char* cp_get_literal() {
        return _cp_value;
    }

    /// check if the lexical element is valid
    bool o_is_valid() const {
        return (_l_enum != lex_null);
    }
};

}  // namespace TinyXPath

#endif  // __TINYLEXTOKEN_H
