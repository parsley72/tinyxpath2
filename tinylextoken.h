#ifndef __TINYLEXTOKEN_H
#define __TINYLEXTOKEN_H

#include <stdio.h>
#include <string.h>

#include "tinyutil.h"

/**
   A lexical token stream, part of a single linked list 
*/
class lex_token 
{
   /// Representation
   char * cp_value;
   /// lexical value
   lexico l_enum;
   /// pointer to next element
   lex_token * ltp_next, * ltp_prev;
public :
   /// constructor
   lex_token (lexico l_in_enum, const _byte_ * bp_in_value, unsigned u_in_size)
   {
      l_enum = l_in_enum;
      cp_value = new char [u_in_size + 1];
      if (u_in_size)
         memcpy (cp_value, bp_in_value, u_in_size);
      cp_value [u_in_size] = 0;
      ltp_next = NULL;
      ltp_prev = NULL;
   }
   /// set the next element in list
   void v_set_next (lex_token * ltp_in_next)
   {
      ltp_next = ltp_in_next;
   }
   /// set the previous element in list
   void v_set_prev (lex_token * ltp_in_prev)
   {
      ltp_prev = ltp_in_prev;
   }
   /// destructor. doesn't destroys next in list
   ~ lex_token ()
   {
      if (cp_value)
         delete [] cp_value;
   }

   #ifdef TINYXPATH_DEBUG
      /// (debug) : dumps one item to stdout
      void v_dump () const
      {
         if (o_is_valid ())
         {
            printf ("[%d][%s][%s]\n", l_enum, cp_disp_class_lex (l_enum), cp_value);
            ltp_next -> v_dump ();
         }
      }
   #endif

   /// get next in list
   lex_token * ltp_get_next () const
   {
      return ltp_next;
   }

   /// get next in list
   lex_token * ltp_get_prev () const
   {
      return ltp_prev;
   }

   lex_token * ltp_get_next (int i_nb)
   {
      lex_token * ltp_ret;
      int i;

      ltp_ret = this;
      for (i = 0; i < i_nb; i++)
      {
         if (! ltp_ret)
            return NULL;
         ltp_ret = ltp_ret -> ltp_get_next ();
         if (! ltp_ret || ! ltp_ret -> o_is_valid ())
            return NULL;
      }
      return ltp_ret;
   }

   lexico lex_get_value () const
   {
      return l_enum;
   }

   void v_set (lexico lex_in, const char * cp_repre)
   {
      unsigned u_length;

      l_enum = lex_in;
      delete [] cp_value;
      u_length = strlen (cp_repre);
      cp_value = new char [u_length + 1];
      strcpy (cp_value, cp_repre);
   }
   const char * cp_get_literal ()
   {
      return cp_value;
   }
   bool o_is_valid () const {return (l_enum != lex_null);}
} ;

#endif
