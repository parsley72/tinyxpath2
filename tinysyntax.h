#ifndef __TINYSYNTAX_H
#define __TINYSYNTAX_H

#include <stdio.h>
#include "tinysimple.h"
#include "tinyaction.h"

class syntax_error
{
public :
   syntax_error (const char * cp_mess = NULL)
   {
      if (cp_mess)
         strcpy (ca_mess, cp_mess);
      else
         ca_mess [0] = 0;
   }
   char ca_mess [200];
} ;

class syntax_backtrack
{
public :
   syntax_backtrack (syntax_error s)
   {
      strcpy (ca_mess, s . ca_mess);
   }
   char ca_mess [200];
} ;

class syntax_overflow {} ;

class token_syntax_decoder : public token_simplified_list
{
protected :
   unsigned u_nb_recurs;
   action_list * alp_action_list;
public :
   token_syntax_decoder () : token_simplified_list ()
   {
      alp_action_list = new action_list;
   }
   ~ token_syntax_decoder ()
   {
      delete alp_action_list;
   }
   void v_syntax_decode ();
   void v_action (unsigned u_rule, unsigned u_sub, unsigned u_variable = 0, const char * cp_explain = "");
   void v_recognize (xpath_construct xc_current, bool o_final);
   action_list * alp_get_action_list ()
   {
      return alp_action_list;
   }
} ;

#endif
