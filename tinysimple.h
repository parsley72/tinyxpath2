#ifndef __TINYSIMPLE_H
#define __TINYSIMPLE_H

#include "tinytokenlist.h"

class token_simplified_list : public token_list
{
public :
   token_simplified_list () : token_list ()
   {
   }
   void v_tokenize_expression ();
} ;     // class token_simplified_list



#endif
