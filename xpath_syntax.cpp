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
   \file xpath_syntax.cpp
   \author Yves Berquin
   XPath Syntax analyzer for TinyXPath project
*/

/*
@history
 Modified on  16 December 2006 by  Aman Aggarwal
 Added support for Expressions like ( Expr or Expr or Expr)

*/

#include "xpath_syntax.h"

#include <stdio.h>

namespace TinyXPath {

/// Decodes the syntax of an XPath expression. On entry, the lexical analysis has already
/// produced a list of basic tokens.
/// \n Throws : syntax_error, syntax_overflow
void token_syntax_decoder::v_syntax_decode() {
    bool o_res;

    // group double tokens ('!=', '::', ...)
    v_tokenize_expression();

    // reset list start
    v_set_current_top();

    _u_nb_recurs = 0;

    // the XPath expression, well, ..., must be an xpath_expr
    o_res = o_recognize(xpath_construct::expr, true);
    if (!o_res)
        throw syntax_error("main level");
#ifdef DUMP_SYNTAX
    if (ltp_get(0))
        printf("!!! Still to decode : %s !!!\n", cp_disp_class_lex(ltp_get(0)->lex_get_value()));
    else
        printf("Completely parsed\n");
    printf("%d recursions\n", _u_nb_recurs);
#endif
}

/// Recognize one XPath construction
/// \n This function throws exceptions every time there's a failure in a backtracking attempt.
/// This should only happen when o_final is false, otherwise we have a syntax error
bool token_syntax_decoder::o_recognize(xpath_construct xc_current,  ///< XPath construction to recognize
    bool o_final)  ///< true if we need to go on, false if it's just a trial
                   /// in the backtracking
{
    lex_token* ltp_freeze;
    bool o_empty, o_found, o_location_path, o_qname, o_temp;
    bool o_test_more;

    _u_nb_recurs++;
    if (_u_nb_recurs > 10000)
        throw syntax_overflow();
    ltp_freeze = nullptr;

    switch (xc_current) {
        case xpath_construct::location_path: {
            //
            // [1]   LocationPath			::=   RelativeLocationPath
            // 					         | AbsoluteLocationPath
            //
            if (!ltp_get(0))
                return false;
            switch (ltp_get(0)->lex_get_value()) {
                case lex::slash:
                case lex::two_slash:
                    if (!o_recognize(xpath_construct::absolute_location_path, o_final))
                        return false;
                    if (o_final)
                        v_action(xpath_construct::location_path, xpath_location_path_abs);
                    break;
                default:
                    if (!o_recognize(xpath_construct::relative_location_path, o_final))
                        return false;
                    if (o_final)
                        v_action(xpath_construct::location_path, xpath_location_path_rel);
                    break;
            }
        } break;

        case xpath_construct::absolute_location_path: {
            // [2]   AbsoluteLocationPath	::=   '/' RelativeLocationPath?
            // 					         | AbbreviatedAbsoluteLocationPath
            if (!ltp_get(0))
                return false;
            const lex action_counter = static_cast<lex>(i_get_action_counter());
            lex_token* token = ltp_get(0);
            if (token) {
                switch (token->lex_get_value()) {
                    case lex::slash:
                        v_inc_current(1);
                        ltp_freeze = ltp_get(0);
                        o_temp = o_recognize(xpath_construct::relative_location_path, false);
                        if (o_temp) {
                            v_set_current(ltp_freeze);
                            o_recognize(xpath_construct::relative_location_path, o_final);
                            if (o_final) {
                                v_action(xpath_construct::absolute_location_path,
                                    xpath_absolute_location_path_slash_rel, action_counter);
                            }
                        } else {
                            v_set_current(ltp_freeze);
                            if (o_final) {
                                v_action(xpath_construct::absolute_location_path, xpath_absolute_location_path_slash,
                                    action_counter);
                            }
                        }
                        break;
                    case lex::two_slash:
                        if (!o_recognize(xpath_construct::abbreviated_absolute_location_path, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::absolute_location_path, xpath_absolute_location_path_abbrev,
                                action_counter);
                        }
                        break;
                    default:
                        return false;
                }
            } else {
                return false;
            }
        } break;

        case xpath_construct::relative_location_path: {
            //
            // [3]   RelativeLocationPath	::=   Step
            // 					         | RelativeLocationPath '/' Step
            // 					         | AbbreviatedRelativeLocationPath
            // [11]   AbbreviatedRelativeLocationPath	::=   RelativeLocationPath '//' Step
            //
            const lex action_counter = static_cast<lex>(i_get_action_counter());
            if (!o_recognize(xpath_construct::step, o_final)) {
                return false;
            }
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::slash) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::relative_location_path, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(
                        xpath_construct::relative_location_path, xpath_relative_location_path_rel_step, action_counter);
                }
            } else if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::two_slash) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::relative_location_path, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::relative_location_path,
                        xpath_relative_location_path_rel_double_slash_step, action_counter);
                }
            } else {
                if (o_final) {
                    v_action(
                        xpath_construct::relative_location_path, xpath_relative_location_path_step, action_counter);
                }
            }
        } break;

        case xpath_construct::step: {
            // [4]   Step					::=   AxisSpecifier NodeTest Predicate*
            // 					         | AbbreviatedStep
            if (!ltp_get(0)) {
                return false;
            }
            switch (ltp_get(0)->lex_get_value()) {
                case lex::dot:
                case lex::two_dot: {
                    if (!o_recognize(xpath_construct::abbrieviated_step, o_final)) {
                        return false;
                    }
                    if (o_final) {
                        v_action(xpath_construct::step, xpath_step_abbrev);
                    }
                } break;
                default: {
                    if (!o_recognize(xpath_construct::axis_specifier, o_final)) {
                        return false;
                    }
                    if (!o_recognize(xpath_construct::node_test, o_final)) {
                        return false;
                    }
                    o_found = true;
                    unsigned u_nb_predicate = 0;
                    while (o_found && ltp_get(0) && ltp_get(0)->lex_get_value() == lex::obrack) {
                        ltp_freeze = ltp_get(0);
                        if (!o_recognize(xpath_construct::predicate, false)) {
                            o_found = false;
                        } else {
                            v_set_current(ltp_freeze);
                            o_recognize(xpath_construct::predicate, o_final);
                            u_nb_predicate++;
                        }
                    }
                    if (o_final) {
                        v_action(xpath_construct::step, xpath_step_full, static_cast<lex>(u_nb_predicate));
                        break;
                    }
                }
            }
        } break;

        case xpath_construct::axis_specifier: {
            //
            // [5]   AxisSpecifier			::=   AxisName '::'
            // 					         | AbbreviatedAxisSpecifier
            //
            // [13]   AbbreviatedAxisSpecifier			::=   '@'?
            o_empty = false;
            if (ltp_get(0)) {
                switch (ltp_get(0)->lex_get_value()) {
                    case lex::at:
                        v_inc_current(1);
                        if (o_final) {
                            v_action(xpath_construct::axis_specifier, xpath_axis_specifier_at);
                        }
                        break;
                    default:
                        if (o_is_axis_name(ltp_get(0)->lex_get_value())) {
                            if (!o_recognize(xpath_construct::axis_name, o_final)) {
                                return false;
                            }
                            if (!ltp_get(0)) {
                                return false;
                            }
                            if (ltp_get(0)->lex_get_value() != lex::two_colon) {
                                return false;
                            }
                            v_inc_current(1);
                            if (o_final) {
                                v_action(xpath_construct::axis_specifier, xpath_axis_specifier_axis_name);
                            }
                        } else {
                            o_empty = true;
                        }
                        break;
                }
            } else {
                o_empty = true;
            }
            if (o_empty) {
                if (o_final) {
                    v_action(xpath_construct::abbreviated_axis_specifier, 1);
                    v_action(xpath_construct::axis_specifier, xpath_axis_specifier_empty);
                }
            }
        } break;

        case xpath_construct::axis_name: {
            // [6]   AxisName				::=   'ancestor'
            // 								| 'ancestor-or-self'
            // 								| 'attribute'
            // 								| 'child'
            // 								| 'descendant'
            // 								| 'descendant-or-self'
            // 								| 'following'
            // 								| 'following-sibling'
            // 								| 'namespace'
            // 								| 'parent'
            // 								| 'preceding'
            // 								| 'preceding-sibling'
            // 								| 'self'

            if (!ltp_get(0)) {
                return false;
            }
            if (!o_is_axis_name(ltp_get(0)->lex_get_value())) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::axis_name, 0, ltp_get(0)->lex_get_value());
            }
            v_inc_current(1);
        } break;

        case xpath_construct::node_test: {
            // [7]   NodeTest				::=   NameTest
            // 								| NodeType '(' ')'
            // 								| 'processing-instruction' '(' Literal
            // ')' [38]   NodeType				::=   'comment' 				| 'text'
            // | 'processing-instruction' | 'node'
            if (!ltp_get(0)) {
                return false;
            }
            switch (ltp_get(0)->lex_get_value()) {
                case lex::comment:
                case lex::text:
                case lex::node:
                    if (o_final)
                        v_action(
                            xpath_construct::node_test, xpath_node_test_reserved_keyword, ltp_get(0)->lex_get_value());
                    v_inc_current(3);
                    break;
                case lex::processing_instruction: {
                    lex_token* token = ltp_get(2);
                    if (token && token->lex_get_value() == lex::cparen) {
                        // single
                        v_inc_current(3);
                        if (o_final) {
                            v_action(xpath_construct::node_test, xpath_node_test_pi, lex::processing_instruction);
                        }
                    } else {
                        // with literal
                        v_inc_current(3);
                        if (o_final) {
                            v_action(xpath_construct::node_test, xpath_node_test_pi_lit, lex::processing_instruction,
                                ltp_get(0)->cp_get_literal());
                        }
                        v_inc_current(1);
                    }
                } break;
                default:
                    if (!o_recognize(xpath_construct::name_test, o_final))
                        return false;
                    if (o_final)
                        v_action(xpath_construct::node_test, xpath_node_test_name_test);
            }
        } break;

        case xpath_construct::predicate: {
            // [8]   Predicate				::=   '[' PredicateExpr ']'
            if (!ltp_get(1)) {
                return false;
            }
            if (ltp_get(0)->lex_get_value() != lex::obrack) {
                return false;
            }
            v_inc_current(1);
            if (!o_recognize(xpath_construct::predicate_expr, o_final)) {
                return false;
            }
            if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::cbrack) {
                return false;
            }
            v_inc_current(1);
            if (o_final) {
                v_action(xpath_construct::predicate, 0);
            }
        } break;

        case xpath_construct::predicate_expr: {
            // [9]   PredicateExpr			::=   Expr
            if (!o_recognize(xpath_construct::expr, o_final)) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::predicate_expr, 0);
            }
        } break;

        case xpath_construct::abbreviated_absolute_location_path: {
            // [10]   AbbreviatedAbsoluteLocationPath	::=   '//' RelativeLocationPath
            if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::two_slash) {
                return false;
            }
            v_inc_current(1);
            if (!o_recognize(xpath_construct::relative_location_path, o_final)) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::abbreviated_absolute_location_path, 0);
            }
        } break;

            // Note : [11] is processed by [3]

        case xpath_construct::abbrieviated_step: {
            // [12]   AbbreviatedStep					::=   '.' | '..'
            if (!ltp_get(0)) {
                return false;
            }
            switch (ltp_get(0)->lex_get_value()) {
                case lex::dot: {
                    v_inc_current(1);
                    if (o_final) {
                        v_action(xpath_construct::abbrieviated_step, 0);
                    }
                } break;
                case lex::two_dot: {
                    v_inc_current(1);
                    if (o_final) {
                        v_action(xpath_construct::abbrieviated_step, 1);
                    }
                } break;
                default:
                    return false;
            }
        } break;

            // Note : [13] processed by [5]

        case xpath_construct::expr: {
            //
            // [14]   Expr					::=   OrExpr
            //
            if (!ltp_get(0)) {
                return false;
            }
            if (!o_recognize(xpath_construct::or_expr, o_final)) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::expr, 0);
            }
        } break;

        case xpath_construct::primary_expr: {
            // [15]   PrimaryExpr			::=   VariableReference
            // 				            | '(' Expr ')'
            // 				            | Literal
            // 				            | Number
            // 				            | FunctionCall
            if (!ltp_get(0)) {
                return false;
            }
            switch (ltp_get(0)->lex_get_value()) {
                case lex::dollar:
                    if (!o_recognize(xpath_construct::variable_reference, o_final)) {
                        return false;
                    }
                    if (o_final) {
                        v_action(xpath_construct::primary_expr, xpath_primary_expr_variable);
                    }
                    break;
                case lex::oparen:
                    v_inc_current(1);
                    if (!o_recognize(xpath_construct::expr, o_final)) {
                        return false;
                    }
                    if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::cparen) {
                        return false;
                    }
                    v_inc_current(1);
                    if (o_final) {
                        v_action(xpath_construct::primary_expr, xpath_primary_expr_paren_expr);
                    }
                    break;
                case lex::literal:
                    if (o_final) {
                        v_action(xpath_construct::primary_expr, xpath_primary_expr_literal, lex::null,
                            ltp_get(0)->cp_get_literal());
                    }
                    v_inc_current(1);
                    break;
                case lex::number:
                    if (o_final) {
                        v_action(xpath_construct::primary_expr, xpath_primary_expr_number, lex::null,
                            ltp_get(0)->cp_get_literal());
                    }
                    v_inc_current(1);
                    break;
                default:
                    if (!o_recognize(xpath_construct::function_call, o_final)) {
                        return false;
                    }
                    if (o_final) {
                        v_action(xpath_construct::primary_expr, xpath_primary_expr_function_call);
                    }
                    break;
            }
        } break;

        case xpath_construct::function_call: {
            // [16]   FunctionCall			::=   FunctionName '(' ( Argument ( ',' Argument )* )? ')'
            // [35]   FunctionName	      ::=  	QName - NodeType
            if (!ltp_get(0)) {
                return false;
            }
            switch (ltp_get(0)->lex_get_value()) {
                case lex::comment:
                case lex::text:
                case lex::node:
                case lex::processing_instruction: {
                    if (o_final) {
                        v_action(xpath_construct::xml_local_part, 0, lex::null, ltp_get(0)->cp_get_literal());
                        v_action(xpath_construct::xml_q_name, xpath_xml_q_name_simple);
                    }
                    v_inc_current(1);
                } break;
                default: {
                    if (!o_recognize(xpath_construct::xml_q_name, o_final)) {
                        return false;
                    }
                } break;
            }
            if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::oparen) {
                return false;
            }
            v_inc_current(1);
            if (!ltp_get(0)) {
                return false;
            }
            if (ltp_get(0)->lex_get_value() == lex::cparen) {
                v_inc_current(1);
                if (o_final) {
                    v_action(xpath_construct::function_call, 0);
                }
            } else {
                ltp_freeze = ltp_get(0);
                o_found = true;
                unsigned u_nb_argument = 0;
                while (o_found) {
                    if (u_nb_argument) {
                        if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::comma) {
                            o_found = false;
                        } else {
                            v_inc_current(1);
                        }
                    }
                    if (o_found) {
                        if (!o_recognize(xpath_construct::argument, o_final)) {
                            o_found = false;
                        } else {
                            u_nb_argument++;
                            ltp_freeze = ltp_get(0);
                        }
                    }
                }
                v_set_current(ltp_freeze);
                if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::cparen) {
                    return false;
                }
                v_inc_current(1);
                if (o_final) {
                    v_action(xpath_construct::function_call, 1, static_cast<lex>(u_nb_argument));
                }
            }
        } break;

        case xpath_construct::argument: {
            // [17]   Argument				::=   Expr
            if (!o_recognize(xpath_construct::expr, o_final)) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::argument, 0);
            }
        } break;

        case xpath_construct::union_expr: {
            //
            // [18]   UnionExpr			::=   PathExpr
            // 					         | UnionExpr '|' PathExpr
            //
            if (!o_recognize(xpath_construct::path_expr, o_final)) {
                return false;
            }
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::orchar) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::union_expr, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::union_expr, xpath_union_expr_union);
                }
            } else if (o_final) {
                v_action(xpath_construct::union_expr, xpath_union_expr_simple);
            }
        } break;

        case xpath_construct::path_expr: {
            //
            // [19]   PathExpr				::=   LocationPath
            // 	        							| FilterExpr
            // 			      					| FilterExpr '/' RelativeLocationPath
            // 					      			| FilterExpr '//' RelativeLocationPath
            //
            ltp_freeze = ltp_get(0);
            o_location_path = false;

            if (!o_recognize(xpath_construct::filter_expr, false)) {
                o_location_path = true;
            } else {
                v_set_current(ltp_freeze);
                o_recognize(xpath_construct::filter_expr, o_final);
                if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::slash) {
                    v_inc_current(1);
                    if (!o_recognize(xpath_construct::relative_location_path, o_final)) {
                        o_location_path = true;
                    } else if (o_final) {
                        v_action(xpath_construct::path_expr, xpath_path_expr_slash);
                    }
                } else if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::two_slash) {
                    v_inc_current(1);
                    if (!o_recognize(xpath_construct::relative_location_path, o_final)) {
                        o_location_path = true;
                    } else if (o_final) {
                        v_action(xpath_construct::path_expr, xpath_path_expr_2_slash);
                    }
                } else {
                    if (o_final) {
                        v_action(xpath_construct::path_expr, xpath_path_expr_filter);
                    }
                }
            }

            if (o_location_path) {
                v_set_current(ltp_freeze);
                if (!o_recognize(xpath_construct::location_path, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::path_expr, xpath_path_expr_location_path);
                }
            }
        } break;

        case xpath_construct::filter_expr: {
            //
            // [20]   FilterExpr			::=   PrimaryExpr
            // 				            | FilterExpr Predicate
            //
            if (!o_recognize(xpath_construct::primary_expr, o_final)) {
                return false;
            }
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::obrack) {
                if (!o_recognize(xpath_construct::predicate, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::filter_expr, xpath_filter_expr_predicate);
                }
            } else if (o_final) {
                v_action(xpath_construct::filter_expr, xpath_filter_expr_primary);
            }
        } break;

        case xpath_construct::or_expr: {
            //
            // [21]   OrExpr				::=   AndExpr
            // 					         | OrExpr 'or' AndExpr
            //
            o_test_more = false;
            if (!o_recognize(xpath_construct::and_expr, o_final)) {
                return false;
            }
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::_or) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::and_expr, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::or_expr, xpath_or_expr_or);
                }
                o_test_more = true;
            } else if (o_final) {
                v_action(xpath_construct::or_expr, xpath_or_expr_simple);
            }
            if (o_test_more) {
                while (ltp_get(0) && (ltp_get(0)->lex_get_value() == lex::_or)) {
                    v_inc_current(1);
                    if (!o_recognize(xpath_construct::and_expr, o_final)) {
                        return false;
                    }
                    if (o_final) {
                        v_action(xpath_construct::or_expr, xpath_or_expr_more);
                    }
                }
            }
        } break;

        case xpath_construct::and_expr: {
            //
            // [22]   AndExpr				::=   EqualityExpr
            // 								| AndExpr 'and' EqualityExpr
            //
            if (!o_recognize(xpath_construct::equality_expr, o_final)) {
                return false;
            }
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::_and) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::equality_expr, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::and_expr, xpath_and_expr_and);
                }
            } else if (o_final) {
                v_action(xpath_construct::and_expr, xpath_and_expr_simple);
            }
        } break;

        case xpath_construct::equality_expr: {
            //
            // [23]   EqualityExpr			::=   RelationalExpr
            // 					         | EqualityExpr '=' RelationalExpr
            // 					         | EqualityExpr '!=' RelationalExpr
            //
            if (!o_recognize(xpath_construct::relational_expr, o_final)) {
                return false;
            }
            if (ltp_get(0)) {
                switch (ltp_get(0)->lex_get_value()) {
                    case lex::equal:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::relational_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::equality_expr, xpath_equality_expr_equal);
                        }
                        break;

                    case lex::not_equal:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::relational_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::equality_expr, xpath_equality_expr_not_equal);
                        }
                        break;

                    default:
                        if (o_final) {
                            v_action(xpath_construct::equality_expr, xpath_equality_expr_simple);
                        }
                        break;
                }
            } else {
                if (o_final) {
                    v_action(xpath_construct::equality_expr, xpath_equality_expr_simple);
                }
            }
        } break;

        case xpath_construct::relational_expr: {
            //
            // [24]   RelationalExpr		::=   AdditiveExpr
            // 					         | RelationalExpr '<' AdditiveExpr
            // 					         | RelationalExpr '>' AdditiveExpr
            // 					         | RelationalExpr '<=' AdditiveExpr
            // 					         | RelationalExpr '>=' AdditiveExpr
            //
            if (!o_recognize(xpath_construct::additive_expr, o_final)) {
                return false;
            }
            if (ltp_get(0)) {
                switch (ltp_get(0)->lex_get_value()) {
                    case lex::lt:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::additive_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::relational_expr, xpath_relational_expr_lt);
                        }
                        break;
                    case lex::gt:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::additive_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::relational_expr, xpath_relational_expr_gt);
                        }
                        break;
                    case lex::lt_equal:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::additive_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::relational_expr, xpath_relational_expr_lte);
                        }
                        break;
                    case lex::gt_equal:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::additive_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::relational_expr, xpath_relational_expr_gte);
                        }
                        break;
                    default:
                        if (o_final) {
                            v_action(xpath_construct::relational_expr, xpath_relational_expr_simple);
                        }
                        break;
                }  // switch
            } else if (o_final) {
                v_action(xpath_construct::relational_expr, xpath_relational_expr_simple);
            }
        } break;

        case xpath_construct::additive_expr: {
            //
            // [25]   AdditiveExpr			::=   MultiplicativeExpr
            // 					         | AdditiveExpr '+' MultiplicativeExpr
            // 					         | AdditiveExpr '-' MultiplicativeExpr
            //
            if (!o_recognize(xpath_construct::multiplicative_expr, o_final))
                return false;
            o_test_more = false;
            if (ltp_get(0)) {
                switch (ltp_get(0)->lex_get_value()) {
                    case lex::plus:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::multiplicative_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::additive_expr, xpath_additive_expr_plus);
                        }
                        o_test_more = true;
                        break;
                    case lex::minus:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::multiplicative_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::additive_expr, xpath_additive_expr_minus);
                        }
                        o_test_more = true;
                        break;
                    default:
                        if (o_final) {
                            v_action(xpath_construct::additive_expr, xpath_additive_expr_simple);
                        }
                        break;
                }  // switch
            } else if (o_final)
                v_action(xpath_construct::additive_expr, xpath_additive_expr_simple);
            if (o_test_more) {
                while (ltp_get(0) &&
                       (ltp_get(0)->lex_get_value() == lex::plus || ltp_get(0)->lex_get_value() == lex::minus)) {
                    if (ltp_get(0)->lex_get_value() == lex::plus) {
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::multiplicative_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::additive_expr, xpath_additive_expr_more_plus);
                        }
                    } else {
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::multiplicative_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::additive_expr, xpath_additive_expr_more_minus);
                        }
                    }
                }
            }
        } break;

        case xpath_construct::multiplicative_expr: {
            //
            // [26]   MultiplicativeExpr   ::=   UnaryExpr
            // 					         | MultiplicativeExpr MultiplyOperator UnaryExpr
            // 					         | MultiplicativeExpr 'div' UnaryExpr
            // 					         | MultiplicativeExpr 'mod' UnaryExpr
            //
            if (!o_recognize(xpath_construct::unary_expr, o_final))
                return false;
            if (ltp_get(0)) {
                switch (ltp_get(0)->lex_get_value()) {
                    case lex::star:
                        //
                        // [34]   MultiplyOperator		::=   '*'
                        //
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::unary_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::multiplicative_expr, xpath_multiplicative_expr_star);
                        }
                        break;
                    case lex::div:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::unary_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::multiplicative_expr, xpath_multiplicative_expr_div);
                        }
                        break;
                    case lex::mod:
                        v_inc_current(1);
                        if (!o_recognize(xpath_construct::unary_expr, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::multiplicative_expr, xpath_multiplicative_expr_mod);
                        }
                        break;
                    default:
                        if (o_final) {
                            v_action(xpath_construct::multiplicative_expr, xpath_multiplicative_expr_simple);
                        }
                        break;
                }
            } else if (o_final) {
                v_action(xpath_construct::multiplicative_expr, xpath_multiplicative_expr_simple);
            }
        } break;

        case xpath_construct::unary_expr: {
            //
            // [27]   UnaryExpr			::=   UnionExpr
            // 					         | '-' UnaryExpr
            //
            if (ltp_get(0) && ltp_get(0)->lex_get_value() == lex::minus) {
                v_inc_current(1);
                if (!o_recognize(xpath_construct::unary_expr, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::unary_expr, xpath_unary_expr_minus);
                }
            } else {
                if (!o_recognize(xpath_construct::union_expr, o_final)) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::unary_expr, xpath_unary_expr_simple);
                }
            }
        } break;

        case xpath_construct::variable_reference: {
            // [36]   VariableReference	::=   '$' QName
            if (!ltp_get(0) || !ltp_get(1)) {
                return false;
            }
            if (ltp_get(0)->lex_get_value() != lex::dollar) {
                return false;
            }
            v_inc_current(1);
            if (!o_recognize(xpath_construct::xml_q_name, o_final)) {
                return false;
            }
            if (o_final) {
                v_action(xpath_construct::variable_reference, 0);
            }
        } break;

        case xpath_construct::name_test: {
            // [37]   NameTest				::=   '*'
            // 				            | NCName ':' '*'
            // 				            | QName
            if (!ltp_get(0))
                return false;

            {
                lex lexVal = ltp_get(0)->lex_get_value();
                if (lexVal == lex::star) {
                    v_inc_current(1);
                    if (o_final) {
                        v_action(xpath_construct::name_test, xpath_name_test_star);
                    }
                } else if (lexVal == lex::ncname) {
                    o_qname = false;
                    lex_token* token1 = ltp_get(1);
                    lex_token* token2 = ltp_get(2);
                    if (token1 && token2 && token1->lex_get_value() == lex::colon) {
                        if (token2->lex_get_value() == lex::star) {
                            v_inc_current(3);
                            if (o_final) {
                                v_action(xpath_construct::name_test, xpath_name_test_ncname);
                            }
                        } else {
                            o_qname = true;
                        }
                    } else {
                        o_qname = true;
                    }
                    if (o_qname) {
                        if (!o_recognize(xpath_construct::xml_q_name, o_final)) {
                            return false;
                        }
                        if (o_final) {
                            v_action(xpath_construct::name_test, xpath_name_test_qname);
                        }
                    }
                }
            }
        } break;

        case xpath_construct::xml_q_name: {
            // [Namespace XML : 6] QName					::= (Prefix ':')? LocalPart
            // [Namespace XML : 7] Prefix					::= NCName
            // [Namespace XML : 8] LocalPart				::= NCName
            if (!ltp_get(0) || ltp_get(0)->lex_get_value() != lex::ncname) {
                return false;
            }
            if (ltp_get(1) && ltp_get(1)->lex_get_value() == lex::colon) {
                if (!ltp_get(2) || ltp_get(2)->lex_get_value() != lex::ncname) {
                    return false;
                }
                if (o_final) {
                    v_action(xpath_construct::xml_prefix, 0, lex::null, ltp_get(0)->cp_get_literal());
                    v_action(xpath_construct::xml_local_part, 0, lex::null, ltp_get(2)->cp_get_literal());
                    v_action(xpath_construct::xml_q_name, xpath_xml_q_name_colon);
                }
                v_inc_current(3);
            } else {
                if (o_final) {
                    v_action(xpath_construct::xml_local_part, 0, lex::null, ltp_get(0)->cp_get_literal());
                    v_action(xpath_construct::xml_q_name, xpath_xml_q_name_simple);
                }
                v_inc_current(1);
            }
        } break;

        default: {
            if (o_final) {
                v_action(xpath_construct::unknown, 0);
            }
            return false;
        } break;
    }
    return true;
}  // v_recognize ()

}  // namespace TinyXPath
