#ifndef __TINYTOKENLIST_H
#define __TINYTOKENLIST_H

#include "tinyutil.h"
#include "tinylextoken.h"

/**
   A token list class. This is the output of the lexical analysis  \n
   Note that the empty list consist of a single null element in order to speed
   up later insertions
*/
class token_list 
{
   /// Pointer to first and last elements
   lex_token * ltp_first, * ltp_last, * ltp_current;
public :
   /// constructor
   token_list () 
   {
      ltp_first = new lex_token (lex_null, NULL, 0);
      ltp_last = ltp_first;
      ltp_first -> v_set_prev (ltp_first);
      ltp_first -> v_set_next (ltp_first);
      ltp_current = NULL;
   }
   /// destructor
   ~ token_list ()
   {
      ltp_current = ltp_first -> ltp_get_next ();
      while (ltp_current -> o_is_valid ())
         v_delete_current ();
      delete ltp_first;
   }
   /// Adds a lexical token
   void v_add_token (lexico l_in_enum, const _byte_ * bp_in_value, unsigned u_in_size)
   {
      lex_token * ltp_new;

      ltp_new = new lex_token (l_in_enum, bp_in_value, u_in_size);
      ltp_last    -> v_set_next (ltp_new);
      ltp_new     -> v_set_next (ltp_first);
      ltp_first   -> v_set_prev (ltp_new);
      ltp_new     -> v_set_prev (ltp_last);
      ltp_last = ltp_new;
   }

   #ifdef TINYXPATH_DEBUG
      /// (debug) dump the list to stdout
      void v_dump (const char * cp_title)
      {
         printf (" (%s) \n", cp_title);
         ltp_first -> ltp_get_next () -> v_dump ();
      }
   #endif

   /// Set current linear index
   void v_set_current (int i_absolute)
   {
      ltp_current = ltp_first -> ltp_get_next (i_absolute + 1);
   }
   void v_set_current (lex_token * ltp_cur)
   {
      ltp_current = ltp_cur;
   }
   /// Get next X linear token
   lex_token * ltp_get (int i_offset) 
   {
      if (! ltp_current)
         return NULL;
      return ltp_current -> ltp_get_next (i_offset);
   }
   /// Increments the linear counter
   void v_inc_current (int i_rel)
   {
      if (! ltp_current)
         return;
      ltp_current = ltp_current -> ltp_get_next (i_rel);
   }

   void v_replace_current (lexico lex_in, const char * cp_rep)
   {
      if (! ltp_current)
         return;
      ltp_current -> v_set (lex_in, cp_rep);
   }
   void v_delete_current ()
   {
      lex_token * ltp_temp;

      ltp_temp = ltp_current;
      ltp_temp -> ltp_get_prev () -> v_set_next (ltp_temp -> ltp_get_next ());
      ltp_temp -> ltp_get_next () -> v_set_prev (ltp_temp -> ltp_get_prev ());
      ltp_current = ltp_temp -> ltp_get_next ();
      delete ltp_temp;
   }
} ;


#endif
