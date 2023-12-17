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
   \file xpath_syntax.h
   \author Yves Berquin
   XPath Syntax analyzer for TinyXPath project
*/

#ifndef __XPATH_SYNTAX_H
#define __XPATH_SYNTAX_H

#include <stdio.h>

#include <cassert>

#include "tokenlist.h"

namespace TinyXPath {

/// Exception class for a syntax error
class syntax_error {
   public:
    syntax_error(const char* cp_mess = nullptr) {
        if (cp_mess && strlen(cp_mess) < sizeof(_ca_mess) - 1) {
            const size_t length = strlen(cp_mess);
            strncpy(_ca_mess, cp_mess, length);
            _ca_mess[length] = 0;
        } else {
            _ca_mess[0] = 0;
        }
    }
    char _ca_mess[200];
};

/// Exception class for an overflow in syntax decoding
class syntax_overflow {};

/// XPath syntax decoder class. Pure virtual :
/// one need to redefine v_action and i_get_action_counter
class token_syntax_decoder : public token_list {
   protected:
    /// Nb of recursions
    unsigned _u_nb_recurs;
    bool o_recognize(xpath_construct xc_current, bool o_final);

   public:
    token_syntax_decoder() : token_list(), _u_nb_recurs(0) {
    }
    virtual ~token_syntax_decoder() = default;
    void v_syntax_decode();
    /// Pure virtual : action taken when processing the rule
    virtual void v_action(
        xpath_construct xc_rule, xpath_sub xp_sub, lex u_variable = lex::null, const char* cp_literal = "") = 0;
    virtual int i_get_action_counter() = 0;
};

}  // namespace TinyXPath

#endif  // __XPATH_SYNTAX_H
