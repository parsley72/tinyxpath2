#include "tinysimple.h"

void token_simplified_list::v_tokenize_expression ()
{
   // replace double lex_colon to lex_2_colon
   v_set_current (0);
   while (ltp_get (1))
   {
      switch (ltp_get (0) -> lex_get_value ())
      {
         case lex_colon :
            if (ltp_get (1) -> lex_get_value () == lex_colon)
            {
               v_replace_current (lex_2_colon, "::");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_slash :
            if (ltp_get (1) -> lex_get_value () == lex_slash)
            {
               v_replace_current (lex_2_slash, "//");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_exclam :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_not_equal, "!=");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_lt :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_lt_equal, "<=");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_gt :
            if (ltp_get (1) -> lex_get_value () == lex_equal)
            {
               v_replace_current (lex_gt_equal, ">=");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_dot :
            if (ltp_get (1) -> lex_get_value () == lex_dot)
            {
               v_replace_current (lex_2_dot, "..");
               v_inc_current (1);
               v_delete_current ();
               v_inc_current (-1);
            }
            else
               v_inc_current (1);
            break;
         case lex_space :
            v_delete_current ();
            break;
         default :
            v_inc_current (1);
            break;
      }     // switch
   }     // while
}
