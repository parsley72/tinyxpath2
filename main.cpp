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
#include <string.h>
#include <assert.h>

#include "tinyxml.h"
#include "tinystr.h"
#include "tinyxpstream.h"

#include "xmlutil.h"
#include "workitem.h"
#include "workstack.h"
#include "xpathappl.h"

#ifdef TEST_SYNTAX
   static void v_decode (const char * cp_in)
   {
      xpath_stream xs (cp_in);

      printf ("Decoding --> %s <--\n", cp_in);
      xs . v_evaluate ();
      printf ("(end)\n");
   }

   static void v_test_syntax ()
   {
      FILE * Fp_in;
      char ca_s [202];

      Fp_in = fopen ("xpath_in.txt", "rt");
      if (! Fp_in)
         return;
      fgets (ca_s, 200, Fp_in);
      while (! feof (Fp_in))
      {
         ca_s [strlen (ca_s) - 1] = 0;
         v_decode (ca_s);
         fgets (ca_s, 200, Fp_in);
      }
      fclose (Fp_in);
   }
#endif

class test_fail {};

static void v_apply_xml (TiXmlDocument * XDp_doc)
{
   TiXmlElement * XEp_source, * XEp_test;
   xpath_from_source * xfsp_engine;
   const char * cp_test_name, * cp_expr;
	FILE * Fp_html;

   try
   {
		Fp_html = fopen ("res.html", "wt");
	   if (Fp_html)
		{
			fprintf (Fp_html, "<html><head><title>TinyXPath results</title>"  \
						"<style type=\"text/css\">"  \
						" P {font-family:Courier}"   \
						" B {color:red}"  \
						"</style>\n</head><body>\n");
			fprintf (Fp_html, "<a href=\"http://sourceforge.net/projects/tinyxpath\"> "\
							"<img src=\"http://sourceforge.net/sflogo.php?group_id=53396&type=5\" width='210' height='62' border='0' alt='SourceForge Logo'></a> ");
		}
      XEp_test = XDp_doc -> FirstChildElement ();
      if (! XEp_test)
         throw test_fail ();
      XEp_source = XEp_test -> FirstChildElement ("source");
      while (XEp_source)
      {
         cp_test_name = XEp_source -> Attribute ("name");
         if (! cp_test_name)
            throw test_fail ();
         cp_expr = XEp_source -> Attribute ("xpath_expr");
         if (! cp_expr)
            throw test_fail ();
         printf ("\nXPath expr --> %s <--\n\n", cp_expr);
         xfsp_engine = new xpath_from_source (XEp_source, cp_expr);
         xfsp_engine -> v_apply_xpath (cp_test_name, Fp_html);
         delete xfsp_engine;
         XEp_source = XEp_source -> NextSiblingElement ("source");
      }
	   if (Fp_html)
		{
			fprintf (Fp_html, "</body></html>\n");	
		   fclose (Fp_html);
		}
   }
   catch (test_fail)
   {
      printf ("test fail !\n");
   }
}

static void v_apply_1 (const char * cp_in_file_name)
{
   TiXmlDocument * XDp_doc;

   XDp_doc = new TiXmlDocument (cp_in_file_name);
   if (! XDp_doc -> LoadFile ())
      printf ("Can't load %s file !\n", cp_in_file_name);
   else
      v_apply_xml (XDp_doc);
   delete XDp_doc;
}

static void v_apply ()
{
   v_apply_1 ("basic_in.xml");
}

void main ()
{
   // v_generate_ascii_htm ();
   #ifdef TEST_SYNTAX
     v_test_syntax ();
   #endif
   v_apply ();
}