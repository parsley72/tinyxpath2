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
#ifndef __TINYSYNTAX_H
#define __TINYSYNTAX_H

#include <assert.h>
#include <stdio.h>
#include "tokenlist.h"

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

class token_syntax_decoder : public token_list
{
protected :
   unsigned u_nb_recurs;
public :
   token_syntax_decoder () : token_list ()
   {
   }
   ~ token_syntax_decoder ()
   {
   }
   void v_syntax_decode ();
   virtual void v_action (unsigned /* u_rule */, unsigned /* u_sub */, 
		unsigned /* u_variable */ = 0, const char * /* cp_explain */ = "")
	{
		// must be redefined 
		assert (false);
	}
   void v_recognize (xpath_construct xc_current, bool o_final);
} ;

#endif
