/*
   This is an example source code and a regression test program for the TinyXPath project
   We load a very small test.xml file and test the return value of the string variant of 
   TinyXPath against a known output.
   The LIBXML_CHECK define may be turned ON if we need to verify the output against libxml 
   (from the Gnome project).
*/
#include "xpath_processor.h"
#include "htmlutil.h"

static FILE * Fp_out_html;

// #define LIBXML_CHECK

#ifdef LIBXML_CHECK
   #include "libxml/tree.h"
   #include "libxml/xpath.h"

   /// Return the text result (if any) of the xpath expression
   TIXML_STRING S_xpath_expr (const xmlDoc * Dp_ptr, const char * cp_xpath_expr)
   {
      xmlXPathObjectPtr XPOp_ptr;
      xmlXPathContextPtr XPCp_ptr;
      const xmlChar * Cp_ptr;
      TIXML_STRING S_out;
   
      S_out = "";
      if (Dp_ptr)
      {
         XPCp_ptr = xmlXPathNewContext ((xmlDoc *) Dp_ptr);
         if (XPCp_ptr)
         {
            // Evaluate 
            XPOp_ptr = xmlXPathEval ((const xmlChar *) cp_xpath_expr, XPCp_ptr);
            if (XPOp_ptr)
            {
               Cp_ptr = xmlXPathCastToString (XPOp_ptr);
               if (Cp_ptr)
                  S_out = (const char *) Cp_ptr;
               xmlXPathFreeObject (XPOp_ptr);
            }
         }
         if (XPCp_ptr)
            xmlXPathFreeContext (XPCp_ptr);
      }

      return S_out;
   }

   static void v_test_one_string_libxml (const TiXmlNode * XNp_root, const char * cp_expr, const xmlDoc * Dp_ptr)
   {
      TIXML_STRING S_expected, S_res;
      bool o_ok;

      S_expected = S_xpath_expr (Dp_ptr, cp_expr);
      printf ("-- expr : [%s] --\n", cp_expr);
      fprintf (Fp_out_html, "<tr><td>%s</td>", cp_expr);
      xpath_processor xp_proc (XNp_root, cp_expr);
      S_res = xp_proc . S_compute_xpath ();
      o_ok = strcmp (S_res . c_str (), S_expected . c_str ()) == 0;
      if (o_ok)
         fprintf (Fp_out_html, "<td>%s</td><td>%s</td></tr>\n", S_res . c_str (), S_expected . c_str ());
      else
         fprintf (Fp_out_html, "<td><em>%s</em></td><td><em>%s</em></td></tr>\n", S_res . c_str (), S_expected . c_str ());
   }

#endif

static void v_test_one_string_tiny (const TiXmlNode * XNp_root, const char * cp_expr, const char * cp_expected)
{
   TIXML_STRING S_res;
   bool o_ok;

   printf ("-- expr : [%s] --\n", cp_expr);
   fprintf (Fp_out_html, "<tr><td>%s</td>", cp_expr);
   TinyXPath::xpath_processor xp_proc (XNp_root, cp_expr);
   S_res = xp_proc . S_compute_xpath ();
   o_ok = strcmp (S_res . c_str (), cp_expected) == 0;
   if (o_ok)
      fprintf (Fp_out_html, "<td>%s</td><td>%s</td></tr>\n", S_res . c_str (), cp_expected);
   else
      fprintf (Fp_out_html, "<td><em>%s</em></td><td><em>%s</em></td></tr>\n", S_res . c_str (), cp_expected);
}

#ifdef LIBXML_CHECK 
   #define v_test_one_string(x,y,z) v_test_one_string_libxml(x,y,Dp_doc)
#else
   #define v_test_one_string v_test_one_string_tiny
#endif


int main ()
{
   TiXmlDocument * XDp_doc;
   TiXmlElement * XEp_main;

   XDp_doc = new TiXmlDocument;
   if (! XDp_doc -> LoadFile ("test.xml"))
   {
      printf ("Can't find test.xml !\n");
      return 99;
   }

   #ifdef LIBXML_CHECK
      xmlDoc * Dp_doc;
      Dp_doc = xmlParseFile ("test.xml");
   #endif

   Fp_out_html = fopen ("out.htm", "wt");
   if (! Fp_out_html)
      return 1;
   fprintf (Fp_out_html, "<html><head><title>Result</title>\n<style>");
   fprintf (Fp_out_html, "em{color: red;}</style>\n");
   fprintf (Fp_out_html, "</head><body>\n");

   fprintf (Fp_out_html, "Input XML tree :<br /><br />\n");
   v_out_html (Fp_out_html, XDp_doc, 0);
   fprintf (Fp_out_html, "<br /><br />\n");
   fprintf (Fp_out_html, "<table border='1'><tr><th>Expression</th><th>Result</th><th>Expected (%s)</th></tr>\n",
      #ifdef LIBXML_CHECK
         "libXML"
      #else
         "compiled"
      #endif
         );

   XEp_main = XDp_doc -> RootElement ();

   v_test_one_string (XEp_main, "//*/comment()", " -122.0 ");
   v_test_one_string (XEp_main, "count(//*/comment())", "2");
   v_test_one_string (XEp_main, "sum(//@*)", "123");
   v_test_one_string (XEp_main, "sum(//*/comment())", "378");
   v_test_one_string (XEp_main, "true()", "true");
   v_test_one_string (XEp_main, "not(false())", "true");

   v_test_one_string (XEp_main, "count(//*[position()=2])", "2");
   v_test_one_string (XEp_main, "name(/*/*/*[position()=2])", "c");
   v_test_one_string (XEp_main, "name(/*/*/*[last()])", "d");

   v_test_one_string (XEp_main, "count(//c/following::*)", "2");
   v_test_one_string (XEp_main, "count(/a/b/b/following::*)", "3");
   v_test_one_string (XEp_main, "count(//d/preceding::*)", "2");
   v_test_one_string (XEp_main, "name(//attribute::*)", "val");
   v_test_one_string (XEp_main, "count(//b/child::*)", "3");
   v_test_one_string (XEp_main, "count(//x/ancestor-or-self::*)", "2");
   v_test_one_string (XEp_main, "count(//b/descendant-or-self::*)", "4");
   v_test_one_string (XEp_main, "count(//self::*)", "6");
   v_test_one_string (XEp_main, "count(/a/descendant::*)", "5");
   v_test_one_string (XEp_main, "count(/a/descendant::x)", "1");
   v_test_one_string (XEp_main, "count(/a/descendant::b)", "2");
   v_test_one_string (XEp_main, "count(/a/descendant::b[@val=123])", "1");
   v_test_one_string (XEp_main, "count(//c/ancestor::a)", "1");
   v_test_one_string (XEp_main, "name(//d/parent::*)", "b");
   v_test_one_string (XEp_main, "count(//c/ancestor::*)", "2");
   v_test_one_string (XEp_main, "name(/a/b/ancestor::*)", "a");
   v_test_one_string (XEp_main, "name(/a/b/c/following-sibling::*)", "d");
   v_test_one_string (XEp_main, "count(//b/following-sibling::*)", "3");
   v_test_one_string (XEp_main, "count(//b|//a)", "3");
   v_test_one_string (XEp_main, "count(//d/preceding-sibling::*)", "2");

   v_test_one_string (XEp_main, "-3 * 4", "-12");
   v_test_one_string (XEp_main, "-3.1 * 4", "-12.4");
   v_test_one_string (XEp_main, "12 div 5", "2.4");
   v_test_one_string (XEp_main, "3 * 7", "21");

   v_test_one_string (XEp_main, "-5.5 >= -5.5", "true");
   v_test_one_string (XEp_main, "-5.5 < 3", "true");
   v_test_one_string (XEp_main, "-6.0 < -7", "false");
   v_test_one_string (XEp_main, "12 < 14", "true");
   v_test_one_string (XEp_main, "12 > 14", "false");
   v_test_one_string (XEp_main, "14 <= 14", "true");

   v_test_one_string (XEp_main, "/a or /b", "true");
   v_test_one_string (XEp_main, "/c or /b", "false");

   v_test_one_string (XEp_main, "/a and /b", "false");
   v_test_one_string (XEp_main, "/a and /*/b", "true");

   v_test_one_string (XEp_main, "18-12", "6");
   v_test_one_string (XEp_main, "18+12", "30");

   v_test_one_string (XEp_main, "count(//a|//b)", "3");
   v_test_one_string (XEp_main, "count(//*[@val])", "1");
   v_test_one_string (XEp_main, "name(//*[@val=123])", "b");

   v_test_one_string (XEp_main, "3=4", "false");
   v_test_one_string (XEp_main, "3!=4", "true");
   v_test_one_string (XEp_main, "12=12", "true");
   v_test_one_string (XEp_main, "'here is a string'='here is a string'", "true");
   v_test_one_string (XEp_main, "'here is a string'!='here is a string'", "false");

   v_test_one_string (XEp_main, "/a/b/@val", "123");
   v_test_one_string (XEp_main, "count(//*/b)", "2");
   v_test_one_string (XEp_main, "name(/*/*/*[2])", "c");
   v_test_one_string (XEp_main, "name(/*)", "a");
   v_test_one_string (XEp_main, "name(/a)", "a");
   v_test_one_string (XEp_main, "name(/a/b)", "b");
   v_test_one_string (XEp_main, "name(/*/*)", "b");
   v_test_one_string (XEp_main, "name(/a/b/c)", "c");
   v_test_one_string (XEp_main, "count(/a/b/*)", "3");
   v_test_one_string (XEp_main, "ceiling(3.5)", "4");
   v_test_one_string (XEp_main, "concat('first ','second',' third','')", "first second third");
   v_test_one_string (XEp_main, "ceiling(5)", "5");
   v_test_one_string (XEp_main, "floor(3.5)", "3");
   v_test_one_string (XEp_main, "floor(5)", "5");
   v_test_one_string (XEp_main, "string-length('try')", "3");
   v_test_one_string (XEp_main, "concat(name(/a/b[1]/*[1]),' ',name(/a/b/*[2]))", "b c");
   v_test_one_string (XEp_main, "count(/a/b/*)", "3");
   v_test_one_string (XEp_main, "count(//*)", "6");
   v_test_one_string (XEp_main, "count(//b)", "2");
   v_test_one_string (XEp_main, "contains('base','as')", "true");
   v_test_one_string (XEp_main, "contains('base','x')", "false");
   v_test_one_string (XEp_main, "not(contains('base','as'))", "false");
   v_test_one_string (XEp_main, "starts-with('blabla','bla')", "true");
   v_test_one_string (XEp_main, "starts-with('blebla','bla')", "false");
   v_test_one_string (XEp_main, "substring('12345',2,3)", "234");
   v_test_one_string (XEp_main, "substring('12345',2)", "2345");
   v_test_one_string (XEp_main, "substring('12345',2,6)", "2345");
   v_test_one_string (XEp_main, "concat('[',normalize-space('  before and   after      '),']')", "[before and after]");


   const TiXmlAttribute * XAp_attrib;
   unsigned u_res;

   TinyXPath::xpath_processor xp_proc (XEp_main, "//*[@val]/@val");
   u_res = xp_proc . u_compute_xpath_node_set ();
   if (u_res)
   {
      XAp_attrib = xp_proc . XAp_get_xpath_attribute (0);
   }

   fprintf (Fp_out_html, "</table>\n");
   fprintf (Fp_out_html, "</body></html>\n");
   fclose (Fp_out_html);

   #ifdef LIBXML_CHECK
      xmlFreeDoc (Dp_doc);
   #endif
   delete XDp_doc;
   return 0;
}
