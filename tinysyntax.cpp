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
#include <stdio.h>
#include "tinysyntax.h"

void token_syntax_decoder::v_syntax_decode ()
{
   v_tokenize_expression ();
   v_set_current (0);
   u_nb_recurs = 0;
   try
   {
      v_recognize (xpath_expr, true);
      if (ltp_get (0))
         printf ("!!! Still to decode : %s !!!\n", cp_disp_class_lex (ltp_get (0) -> lex_get_value ()));
      else
         printf ("Completely parsed\n");
      printf ("%d recursions\n", u_nb_recurs);
   }
   catch (syntax_backtrack s)
   {
      printf ("Weird : backtrack at the upper level [%s]\n", s . ca_mess);
   }
   catch (syntax_error s)
   {
      printf ("Syntax error : %s\n", s . ca_mess);
   }
   catch (syntax_overflow)
   {
      printf ("Syntax overflow !\n");
   }
}

void token_syntax_decoder::v_recognize (xpath_construct xc_current, bool o_final)
{
   lex_token * ltp_freeze;
   bool o_qname;
   bool o_found;
   bool o_empty;
   unsigned u_nb_argument, u_nb_predicate;
   static char ca_detail [200];

   u_nb_recurs++;
   if (u_nb_recurs > 10000)
      throw syntax_overflow ();
   ltp_freeze = NULL;
   try
   {
      switch (xc_current)
      {
         case xpath_location_path :
            //
            // [1]   LocationPath			::=   RelativeLocationPath 
			   // 					         | AbsoluteLocationPath 
            //
            if (! ltp_get (0))
               throw syntax_error ("[1]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_slash :
               case lex_2_slash :
                  v_recognize (xpath_absolute_location_path, o_final);
                  if (o_final)
                     v_action (xpath_location_path, 0);
                  break;
               default :
                  v_recognize (xpath_relative_location_path, o_final);
                  if (o_final)
                     v_action (xpath_location_path, 1);
                  break;
            }
            break;         

         case xpath_absolute_location_path :
            // [2]   AbsoluteLocationPath	::=   '/' RelativeLocationPath? 
			   // 					         | AbbreviatedAbsoluteLocationPath 
            if (! ltp_get (0))
               throw syntax_error ("[2/1]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_slash :
                  v_inc_current (1);
                  try
                  {
                     ltp_freeze = ltp_get (0);
                     v_recognize (xpath_relative_location_path, false);
                     v_set_current (ltp_freeze);
                     v_recognize (xpath_relative_location_path, o_final);
                     if (o_final)
                        v_action (xpath_absolute_location_path, 0);
                  }
                  catch (syntax_backtrack)
                  {
                     v_set_current (ltp_freeze);
                     if (o_final)
                        v_action (xpath_absolute_location_path, 1);
                  }
                  break;
               case lex_2_slash :
                  v_recognize (xpath_abbreviated_absolute_location_path, o_final);
                  if (o_final)
                     v_action (xpath_absolute_location_path, 2);
                  break;
               default :
                  throw syntax_error ("[2/2]");
            }
            break;         

         case xpath_relative_location_path :
            //
            // [3]   RelativeLocationPath	::=   Step 
			   // 					         | RelativeLocationPath '/' Step 
			   // 					         | AbbreviatedRelativeLocationPath
            // [11]   AbbreviatedRelativeLocationPath	::=   RelativeLocationPath '//' Step 
            //
            v_recognize (xpath_step, o_final);
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_slash)
            {
               v_inc_current (1);
               v_recognize (xpath_relative_location_path, o_final);
               if (o_final)
                  v_action (xpath_relative_location_path, 0);
            }
            else
               if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_2_slash)
               {
                  v_inc_current (1);
                  v_recognize (xpath_relative_location_path, o_final);
                  if (o_final)
                     v_action (xpath_relative_location_path, 1);
               }
               else
               {
                  if (o_final)
                     v_action (xpath_relative_location_path, 2);
               }
            break;         

         case xpath_step :
            // [4]   Step					::=   AxisSpecifier NodeTest Predicate* 
			   // 					         | AbbreviatedStep 
            if (! ltp_get (0))
               throw syntax_error ("[4]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_dot :
               case lex_2_dot :
                  v_recognize (xpath_abbrieviated_step, o_final);
                  if (o_final)
                     v_action (xpath_step, 0);
                  break;
               default :
                  v_recognize (xpath_axis_specifier, o_final);
                  v_recognize (xpath_node_test, o_final);
                  o_found = true;
                  u_nb_predicate = 0;
                  while (o_found && ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_obrack)
                  {
                     try
                     {
                        ltp_freeze = ltp_get (0);
                        v_recognize (xpath_predicate, false);
                        v_set_current (ltp_freeze);
                        v_recognize (xpath_predicate, o_final);
                        u_nb_predicate++;
                     }
                     catch (syntax_backtrack)
                     {
                        o_found = false;
                     }
                  }
                  if (o_final)
                  {
                     sprintf (ca_detail, "AxisSpecifier NodeTest Predicate (x%d)", u_nb_predicate);
                     v_action (xpath_step, 1, u_nb_predicate);
                  }
                  break;               
            }
            break;

         case xpath_axis_specifier :
            // 
            // [5]   AxisSpecifier			::=   AxisName '::' 
			   // 					         | AbbreviatedAxisSpecifier 
            //
            // [13]   AbbreviatedAxisSpecifier			::=   '@'?
            o_empty = false;
            if (ltp_get (0))
            {
               switch (ltp_get (0) -> lex_get_value ())
               {
                  case lex_at :
                     v_inc_current (1);
                     if (o_final)
                     {
                        v_action (xpath_abbreviated_axis_specifier, 0);
                        v_action (xpath_axis_specifier, 0);
                     }
                     break;
                  default :
                     if (o_is_axis_name (ltp_get (0) -> lex_get_value ()))
                     {
                        v_recognize (xpath_axis_name, o_final);
                        if (! ltp_get (0))
                           throw syntax_error ("[5/2]");
                        if (ltp_get (0) -> lex_get_value () != lex_2_colon)
                           throw syntax_error ("[5/3]");
                        v_inc_current (1);
                        if (o_final)
                           v_action (xpath_axis_specifier, 1);
                     }            
                     else
                        o_empty = true;
                     break;
               }
            }
            else
               o_empty = true;
            if (o_empty)
            {
               if (o_final)
               {
                  v_action (xpath_abbreviated_axis_specifier, 1);
                  v_action (xpath_axis_specifier, 2);
               }
            }            
            break;
         case xpath_axis_name :
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
      
            if (! ltp_get (0))
               throw syntax_error ("[6/1]");
            if (! o_is_axis_name (ltp_get (0) -> lex_get_value ()))
               throw syntax_error ("[6/2]");
            v_inc_current (1);
            if (o_final)
               v_action (xpath_axis_name, 0, ltp_get (0) -> lex_get_value ());
            break;
         case xpath_node_test :
            // [7]   NodeTest				::=   NameTest 
            // 								| NodeType '(' ')' 
            // 								| 'processing-instruction' '(' Literal ')'
            // [38]   NodeType				::=   'comment' 
				// 				| 'text' 
				// 				| 'processing-instruction' 
				// 				| 'node' 
            if (! ltp_get (0))
               throw syntax_error ("[7/1]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_comment :
               case lex_text :
               case lex_node :
                  v_inc_current (3);
                  if (o_final)
                     v_action (xpath_node_test, 0, ltp_get (0) -> lex_get_value ());
                  break;
               case lex_processing_instruction :
                  if (ltp_get (2) && ltp_get (2) -> lex_get_value () == lex_cparen)
                  {
                     // single
                     v_inc_current (3);
                     if (o_final)
                        v_action (xpath_node_test, 1, lex_processing_instruction);
                  }
                  else
                  {
                     // with literal
                     v_inc_current (3);
                     if (o_final)
                        v_action (xpath_node_test, 2, lex_processing_instruction, ltp_get (0) -> cp_get_literal ());
                     v_inc_current (1);
                  }
                  break;
               default :
                  v_recognize (xpath_name_test, o_final);
                  if (o_final)
                     v_action (xpath_node_test, 3);
            }
            break;
         case xpath_predicate :
            // [8]   Predicate				::=   '[' PredicateExpr ']' 
            if (! ltp_get (1))
               throw syntax_error ("[8/1]");
            if (ltp_get (0) -> lex_get_value () != lex_obrack)
               throw syntax_error ("[8/2]");
            v_inc_current (1);
            v_recognize (xpath_predicate_expr, o_final);
            if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_cbrack)
               throw syntax_error ("[8/3]");
            v_inc_current (1);
            if (o_final)
               v_action (xpath_predicate, 0);
            break;
         case xpath_predicate_expr :
            // [9]   PredicateExpr			::=   Expr
            v_recognize (xpath_expr, o_final);
            if (o_final)
               v_action (xpath_predicate_expr, 0);
            break;

         case xpath_abbreviated_absolute_location_path :
            // [10]   AbbreviatedAbsoluteLocationPath	::=   '//' RelativeLocationPath 
      
            if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_2_slash)
               throw syntax_error ("[10/1]");
            v_inc_current (1);
            v_recognize (xpath_relative_location_path, o_final);
            if (o_final)
               v_action (xpath_abbreviated_absolute_location_path, 0);
            break;

         // Note : [11] is processed by [3]

         case xpath_abbrieviated_step :
            // [12]   AbbreviatedStep					::=   '.' | '..' 
            if (! ltp_get (0))
               throw syntax_error ("[12/1]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_dot :
                  v_inc_current (1);
                  if (o_final)
                     v_action (xpath_abbrieviated_step, 0);
                  break;
               case lex_2_dot :
                  v_inc_current (1);
                  if (o_final)
                     v_action (xpath_abbrieviated_step, 1);
                  break;
               default :
                  throw syntax_error ("[12/2]");
            }
            break;

         // Note : [13] processed by [5]

         case xpath_expr :
            //
            // [14]   Expr					::=   OrExpr 
            //
            if (! ltp_get (0))
               throw syntax_error ("[14]");
            v_recognize (xpath_or_expr, o_final);
            if (o_final)
               v_action (xpath_expr, 0);
            break;

         case xpath_primary_expr :
            // [15]   PrimaryExpr			::=   VariableReference
				// 				            | '(' Expr ')' 
				// 				            | Literal 
				// 				            | Number 
				// 				            | FunctionCall
            if (! ltp_get (0))
               throw syntax_error ("[15]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_dollar :
                  v_recognize (xpath_variable_reference, o_final);
                  if (o_final)
                     v_action (xpath_primary_expr, 0);
                  break;
               case lex_oparen :
                  v_inc_current (1);
                  v_recognize (xpath_expr, o_final);
                  if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_cparen)
                     throw syntax_error ("[15/2]");
                  v_inc_current (1);
                  if (o_final)
                     v_action (xpath_primary_expr, 1);
                  break;
               case lex_literal :
                  if (o_final)
                     v_action (xpath_primary_expr, 2, 0, ltp_get (0) -> cp_get_literal ());
                  v_inc_current (1);
                  break;
               case lex_number :
                  if (o_final)
                     v_action (xpath_primary_expr, 3, 0, ltp_get (0) -> cp_get_literal ());
                  v_inc_current (1);
                  break;
               default :
                  v_recognize (xpath_function_call, o_final);
                  if (o_final)
                     v_action (xpath_primary_expr, 4);
                  break;
            }
            break;

         case xpath_function_call :
            // [16]   FunctionCall			::=   FunctionName '(' ( Argument ( ',' Argument )* )? ')' 
            if (! ltp_get (0))
               throw syntax_error ("[16/1]");
            v_recognize (xpath_xml_q_name, o_final);
            if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_oparen)
               throw syntax_error ("[16/2]");
            v_inc_current (1);
            if (! ltp_get (0))
               throw syntax_error ("[16/3]");
            if (ltp_get (0) -> lex_get_value () == lex_cparen)
            {
               v_inc_current (1);
               if (o_final)
                  v_action (xpath_function_call, 0);
            }
            else
            {
               ltp_freeze = ltp_get (0);
               o_found = true;
               u_nb_argument = 0;
               while (o_found)
               {
                  try
                  {
                     if (u_nb_argument)
                     {
                        if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_comma)
                           throw syntax_backtrack ("");
                        v_inc_current (1);
                     }
                     v_recognize (xpath_argument, o_final);               
                     u_nb_argument++;
                     ltp_freeze = ltp_get (0);
                  }
                  catch (syntax_backtrack)
                  {
                     o_found = false;
                  }
               }
               v_set_current (ltp_freeze);
               if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_cparen)
                  throw syntax_error ("[16/3]");
               v_inc_current (1);
               if (o_final)
                  v_action (xpath_function_call, 1, u_nb_argument);
            }
            break;

         case xpath_argument :
            // [17]   Argument				::=   Expr
            v_recognize (xpath_expr, o_final);
            if (o_final)
               v_action (xpath_argument, 0);
            break;

         case xpath_union_expr :
            //
            // [18]   UnionExpr			::=   PathExpr 
			   // 					         | UnionExpr '|' PathExpr 
            //
            v_recognize (xpath_path_expr, o_final);
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_orchar)
            {
               v_inc_current (1);
               v_recognize (xpath_path_expr, o_final);
               if (o_final)
                  v_action (xpath_union_expr, 0);
            }
            else
               if (o_final)
                  v_action (xpath_union_expr, 1);
            break;
         case xpath_path_expr :
            //
            // [19]   PathExpr				::=   LocationPath 
            // 	        							| FilterExpr 
            // 			      					| FilterExpr '/' RelativeLocationPath 
            // 					      			| FilterExpr '//' RelativeLocationPath 
            //
            ltp_freeze = ltp_get (0);
            try
            {
               v_recognize (xpath_filter_expr, false);
               v_set_current (ltp_freeze);
               v_recognize (xpath_filter_expr, o_final);
               if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_slash)
               {
                  v_inc_current (1);
                  v_recognize (xpath_relative_location_path, o_final);
                  if (o_final)
                     v_action (xpath_path_expr, 0);
               }
               else
                  if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_2_slash)
                  {
                     v_inc_current (1);
                     v_recognize (xpath_relative_location_path, o_final);
                     if (o_final)
                        v_action (xpath_path_expr, 1);
                  }
                  else
                  {
                     if (o_final)
                        v_action (xpath_path_expr, 2);
                  }
            }
            catch (syntax_backtrack)
            {
               v_set_current (ltp_freeze);
               v_recognize (xpath_location_path, o_final);
               if (o_final)
                  v_action (xpath_path_expr, 3);
            }
            break;

         case xpath_filter_expr :
            //
            // [20]   FilterExpr			::=   PrimaryExpr 
				// 				            | FilterExpr Predicate
            //
            v_recognize (xpath_primary_expr, o_final);
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_obrack)
            {
               v_recognize (xpath_predicate, o_final);
               if (o_final)
                  v_action (xpath_filter_expr, 0);
            }
            else
               if (o_final)
                  v_action (xpath_filter_expr, 1);
            break;

         case xpath_or_expr :
            //
            // [21]   OrExpr				::=   AndExpr 
			   // 					         | OrExpr 'or' AndExpr 
            //
            v_recognize (xpath_and_expr, o_final);
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_or)
            {
               v_inc_current (1);
               v_recognize (xpath_and_expr, o_final);
               if (o_final)
                  v_action (xpath_or_expr, 0);
            }
            else
               if (o_final)
                  v_action (xpath_or_expr, 1);
            break;
         case xpath_and_expr :
            //
            // [22]   AndExpr				::=   EqualityExpr 
            // 								| AndExpr 'and' EqualityExpr 
            //
            v_recognize (xpath_equality_expr, o_final);
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_and)
            {
               v_inc_current (1);
               v_recognize (xpath_equality_expr, o_final);
               if (o_final)
                  v_action (xpath_and_expr, 0);
            }
            else
               if (o_final)
                  v_action (xpath_and_expr, 1);
            break;
         case xpath_equality_expr :
            //
            // [23]   EqualityExpr			::=   RelationalExpr 
			   // 					         | EqualityExpr '=' RelationalExpr 
			   // 					         | EqualityExpr '!=' RelationalExpr 
            //
            v_recognize (xpath_relational_expr, o_final);
            if (ltp_get (0))
            {
               switch (ltp_get (0) -> lex_get_value ())
               {
                  case lex_equal :
                     v_inc_current (1);
                     v_recognize (xpath_relational_expr, o_final);
                     if (o_final)
                        v_action (xpath_equality_expr, 0);
                     break;
                  case lex_not_equal :
                     v_inc_current (1);
                     v_recognize (xpath_relational_expr, o_final);
                     if (o_final)
                        v_action (xpath_equality_expr, 1);
                     break;
                  default :
                     if (o_final)
                        v_action (xpath_equality_expr, 2);
                     break;
               }
            }
            else
            {
               if (o_final)
                  v_action (xpath_equality_expr, 2);
            }
            break;
         case xpath_relational_expr :
            //
            // [24]   RelationalExpr		::=   AdditiveExpr 
			   // 					         | RelationalExpr '<' AdditiveExpr 
			   // 					         | RelationalExpr '>' AdditiveExpr 
			   // 					         | RelationalExpr '<=' AdditiveExpr 
			   // 					         | RelationalExpr '>=' AdditiveExpr
            //
            v_recognize (xpath_additive_expr, o_final);
            if (ltp_get (0))
            {
               switch (ltp_get (0) -> lex_get_value ())
               {
                  case lex_lt :
                     v_inc_current (1);
                     v_recognize (xpath_additive_expr, o_final);
                     if (o_final)
                        v_action (xpath_relational_expr, 0);
                     break;
                  case lex_gt :
                     v_inc_current (1);
                     v_recognize (xpath_additive_expr, o_final);
                     if (o_final)
                        v_action (xpath_relational_expr, 1);
                     break;
                  case lex_lt_equal :
                     v_inc_current (1);
                     v_recognize (xpath_additive_expr, o_final);
                     if (o_final)
                        v_action (xpath_relational_expr, 2);
                     break;
                  case lex_gt_equal :
                     v_inc_current (1);
                     v_recognize (xpath_additive_expr, o_final);
                     if (o_final)
                        v_action (xpath_relational_expr, 3);
                     break;
                  default :
                     if (o_final)
                        v_action (xpath_relational_expr, 4);
                     break;
               }     // switch
            }
            else
               if (o_final)
                  v_action (xpath_relational_expr, 4);
            break;

         case xpath_additive_expr :
            // 
            // [25]   AdditiveExpr			::=   MultiplicativeExpr 
			   // 					         | AdditiveExpr '+' MultiplicativeExpr 
			   // 					         | AdditiveExpr '-' MultiplicativeExpr 
            // 
            v_recognize (xpath_multiplicative_expr, o_final);
            if (ltp_get (0))
            {
               switch (ltp_get (0) -> lex_get_value ())
               {
                  case lex_plus :
                     v_inc_current (1);
                     v_recognize (xpath_multiplicative_expr, o_final);
                     if (o_final)
                        v_action (xpath_additive_expr, 0);
                     break;
                  case lex_minus :
                     v_inc_current (1);
                     v_recognize (xpath_multiplicative_expr, o_final);
                     if (o_final)
                        v_action (xpath_additive_expr, 1);
                     break;
                  default :
                     if (o_final)
                        v_action (xpath_additive_expr, 2);
                     break;
               }     // switch
            }
            else
               if (o_final)
                  v_action (xpath_additive_expr, 2);
            break;
         case xpath_multiplicative_expr :
            //
            // [26]   MultiplicativeExpr   ::=   UnaryExpr 
			   // 					         | MultiplicativeExpr MultiplyOperator UnaryExpr 
			   // 					         | MultiplicativeExpr 'div' UnaryExpr 
			   // 					         | MultiplicativeExpr 'mod' UnaryExpr 
            //
            v_recognize (xpath_unary_expr, o_final);
            if (ltp_get (0))
            {
               switch (ltp_get (0) -> lex_get_value ())
               {
                  case lex_star :
                     //
                     // [34]   MultiplyOperator		::=   '*' 
                     //
                     v_inc_current (1);
                     v_recognize (xpath_unary_expr, o_final);
                     if (o_final)
                     {
                        v_action (xpath_multiply_operator, 0);
                        v_action (xpath_multiplicative_expr, 0);
                     }
                     break;
                  case lex_div :
                     v_inc_current (1);
                     v_recognize (xpath_unary_expr, o_final);
                     if (o_final)
                        v_action (xpath_multiplicative_expr, 1);
                     break;
                  case lex_mod :
                     v_inc_current (1);
                     v_recognize (xpath_unary_expr, o_final);
                     if (o_final)
                        v_action (xpath_multiplicative_expr, 2);
                     break;
                  default :
                     if (o_final)
                        v_action (xpath_multiplicative_expr, 3);
                     break;
               }
            }
            else
               if (o_final)
                  v_action (xpath_multiplicative_expr, 3);
            break;
         case xpath_unary_expr :
            //
            // [27]   UnaryExpr			::=   UnionExpr 
			   // 					         | '-' UnaryExpr
            //
            if (ltp_get (0) && ltp_get (0) -> lex_get_value () == lex_minus)
            {
               v_inc_current (1);
               v_recognize (xpath_unary_expr, o_final);
               if (o_final)
                  v_action (xpath_unary_expr, 0);
            }
            else
            {
               v_recognize (xpath_union_expr, o_final);
               if (o_final)
                  v_action (xpath_unary_expr, 1);
            }
            break;

         case xpath_variable_reference :
            // [36]   VariableReference	::=   '$' QName 
            if (! ltp_get (0) || ! ltp_get (1))
               throw syntax_error ("[36/1]");
            if (ltp_get (0) -> lex_get_value () != lex_dollar)
               throw syntax_error ("[36/2]");
            v_inc_current (1);
            v_recognize (xpath_xml_q_name, o_final);
            if (o_final)
               v_action (xpath_variable_reference, 0);
            break;

         case xpath_name_test :
            // [37]   NameTest				::=   '*' 
				// 				            | NCName ':' '*' 
				// 				            | QName 
            if (! ltp_get (0))
               throw syntax_error ("[37/1]");
            switch (ltp_get (0) -> lex_get_value ())
            {
               case lex_star :
                  v_inc_current (1);
                  if (o_final)
                     v_action (xpath_name_test, 0);
                  break;
               case lex_ncname :
                  o_qname = false;
                  if (ltp_get (1) && ltp_get (2) && 
                      ltp_get (1) -> lex_get_value () == lex_colon)
                  {
                     if (ltp_get (2) -> lex_get_value () == lex_star)
                     {
                        v_inc_current (3);
                        if (o_final)
                           v_action (xpath_name_test, 1);
                     }
                     else
                        o_qname = true;
                  }
                  else
                     o_qname = true;
                  if (o_qname)
                  {
                     v_recognize (xpath_xml_q_name, o_final);
                     if (o_final)
                        v_action (xpath_name_test, 2);
                  }
                  break;
            }
            break;
         case xpath_xml_q_name :
            // [Namespace XML : 6] QName					::= (Prefix ':')? LocalPart 
            // [Namespace XML : 7] Prefix					::= NCName 
            // [Namespace XML : 8] LocalPart				::= NCName
            if (! ltp_get (0) || ltp_get (0) -> lex_get_value () != lex_ncname)
               throw syntax_error ("[XML 6/1]");
            if (ltp_get (1) && ltp_get (1) -> lex_get_value () == lex_colon)
            {
               if (! ltp_get (2) || ltp_get (2) -> lex_get_value () != lex_ncname)
                  throw syntax_error ("[XML 6/2]");
               if (o_final)
               {
                  v_action (xpath_xml_prefix, 0, 0, ltp_get (0) -> cp_get_literal ());
                  v_action (xpath_xml_local_part, 0, 0, ltp_get (2) -> cp_get_literal ());
                  v_action (xpath_xml_q_name, 0);
               }
               v_inc_current (3);
            }
            else
            {
               if (o_final)
               {
                  v_action (xpath_xml_local_part, 0, 0, ltp_get (0) -> cp_get_literal ());
                  v_action (xpath_xml_q_name, 1);
               }
               v_inc_current (1);
            }
            break;
         default :
            if (o_final)
               v_action (xpath_unknown, 0);
            throw syntax_error ("[def]");
            break;
      }
   }
   catch (syntax_error s2)
   {
      if (! o_final)
         throw syntax_backtrack (s2);
      throw;
   }
}     // v_recognize ()
