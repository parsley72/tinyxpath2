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

#include "xpathappl.h"
#include "htmlutil.h"

xpath_from_source::xpath_from_source (TiXmlNode * XNp_source_tree, const char * cp_in_expr)
   : xpath_stream (cp_in_expr)
{
   XNp_source = XNp_source_tree;
   XDp_target = new TiXmlDocument;
}
xpath_from_source::~ xpath_from_source ()
{
   assert (XDp_target);
   delete XDp_target;
}

void xpath_from_source::v_action (
	unsigned u_rule, 
	unsigned u_sub, 
	unsigned u_variable, 
	const char * cp_explain)
{
   TIXML_STRING S_name, S_name_2;
   work_step * wp_step, * wp_step_2;
   work_node_test * wp_node_test;
   work_axis * wp_axis;
	work_func * wp_func;
   unsigned u_nb_predicate, u_predicate, u_arg;
   work_item ** wipp_list;

   switch (u_rule)
   {
      case xpath_absolute_location_path :
         // [2]
         switch (u_sub)
         {
            case 0 :
            case 1 :
               wp_step = (work_step *) wsp_stack -> wip_top ();
					wp_step -> v_set_absolute (true);
               // wsp_stack -> v_pop ();
               break;
            case 2 :
               printf ("[2]  absolute already processed\n");
               break;
         }
         break;

      case xpath_relative_location_path :
         // [3]
         switch (u_sub)
         {
            case 0 :
               printf ("[3]   RelativeLocationPath / Step\n");
               wp_step = (work_step *) wsp_stack -> wip_top ();
               wp_step_2 = (work_step *) wsp_stack -> wip_top (1);
               wp_step_2 -> v_set_next_step (wp_step);
               wsp_stack -> v_pop ();
               break;
            case 1 :
               printf ("[3]   RelativeLocationPath // Step\n");
               break;
            case 2 :
               printf ("[3]   RelativeLocationPath is simple\n");
               break;
         }
         break;
                        
      case xpath_step :
         // [4]
         switch (u_sub)
         {
            case 0 :
               printf ("[4]   Step is an abbreviated one (. or ..)\n");
               break;
            case 1 :
               u_nb_predicate = u_variable;
               printf ("[4]   Step is \"AxisSpecifier NodeTest Predicate x %d\"\n", 
                     u_nb_predicate);
               wipp_list = NULL;
               if (u_nb_predicate)
               {
                  wipp_list = new work_item * [u_nb_predicate];
                  for (u_predicate = 0; u_predicate < u_nb_predicate; u_predicate++)
                     wipp_list [u_predicate] = wip_copy (wsp_stack -> wip_top (u_predicate));
               }
					wsp_stack -> v_dump ();
               wp_node_test = (work_node_test *) wsp_stack -> wip_top (u_nb_predicate);
					assert (wp_node_test -> work_node_test::o_identity ());
               if (u_nb_predicate)
               {
                  wp_node_test -> v_set_predicate_list (u_nb_predicate, wipp_list);
						for (u_predicate = 0; u_predicate < u_nb_predicate; u_predicate++)
							delete wipp_list [u_predicate];
                  delete [] wipp_list;
               }
               wp_axis = (work_axis *) wsp_stack -> wip_top (u_nb_predicate + 1);
               work_item * wip_new = new work_step (wp_axis, wp_node_test);
               wsp_stack -> v_pop (u_nb_predicate + 2);
               wsp_stack -> v_push (wip_new);
               break;
         }
         break;
      case xpath_axis_specifier :
         // [5]
         switch (u_sub)
         {
            case 0 :
               printf ("[5]  Axis specifier with '@'\n");
               break;
            case 1 :
               printf ("[5]  Axis specifier is AxisName ::\n");
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_axis (false, false, S_name . c_str ()));
               break;
            case 2 :
               printf ("[5]  Axis specifier with no '@'\n");
					// do not push anything here, it's already been made in 
					// the case xpath_abbreviated_axis_specifier
               break;
         }
         break;            
      case xpath_axis_name :
         // [6]
         printf ("[6] Axis name is %s\n", cp_explain);
         wsp_stack -> v_push (new work_string (cp_explain));
         break;
      case xpath_node_test :
         // [7]
         switch (u_sub)
         {
            case 0 :
               printf ("[7] Node type is simple (%d)\n", u_variable);
               wsp_stack -> v_push (new work_node_test (0, u_variable));
               break;
            case 1 :
               printf ("[7] Node type is processing-instruction ()");
               wsp_stack -> v_push (new work_node_test (1, lex_processing_instruction));
               break;
            case 2 :
               printf ("[7] Node type is processing-instruction ()");
               wsp_stack -> v_push (new work_node_test (2, lex_processing_instruction, cp_explain));
               break;
            case 3 :
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               printf ("[7] Node type is a name test (%s)\n", S_name . c_str ()); 
               wsp_stack -> v_push (new work_node_test (3, 0, S_name . c_str ()));
               break;
         }
         break;

		case xpath_predicate :
			// [8] 
			printf ("Skipping predicate syntax rule\n");
			break;

      case xpath_abbreviated_absolute_location_path :
         // [10]
         wp_step = (work_step *) wsp_stack -> wip_top ();
			wp_step -> v_set_absolute (true);
			wp_step -> v_set_all (true);
         break;

      case xpath_abbreviated_axis_specifier :
         // [13]
         switch (u_sub)
         {
            case 0 :
               wsp_stack -> v_push (new work_axis (true, true));
               break;
            case 1 :
               wsp_stack -> v_push (new work_axis (true, false));
               break;
         }
         break;

      case xpath_primary_expr :
			// [15]
         switch  (u_sub)
         {
            case 0 :
            case 1 :
					assert (false);
					break;
            case 2 :
					wsp_stack -> v_push (new work_expr (e_work_expr_literal, 0, cp_explain));
					break;
            case 3 :
               // Houston, we have a number
               wsp_stack -> v_push (new work_expr (e_work_expr_value, atoi (cp_explain)));
               break;
            case 4 :
               // Houston : it's getting worse : we have a function call
					/*
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_expr (1, 0, S_name . c_str ()));
					*/
               break;
         }
         break;

		case xpath_function_call :
			// [16]
			switch (u_sub)
			{
				case 0 :
					// no arguments
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_expr (e_work_expr_func, 0, S_name . c_str ()));
					break;
				case 1 :
					// arguments
               wipp_list = new work_item * [u_variable];
               for (u_arg = 0; u_arg < u_variable; u_arg++)
                  wipp_list [u_arg] = wip_copy (wsp_stack -> wip_top (u_arg));
					wp_func = new work_func (u_variable, wipp_list);
					for (u_arg = 0; u_arg < u_variable; u_arg++)
						delete wipp_list [u_arg];
               delete [] wipp_list;
               wsp_stack -> v_pop (u_variable);
					wp_func -> v_set_func_name (wsp_stack -> cp_get_top_value ());
					wsp_stack -> v_pop ();
					wsp_stack -> v_push (wp_func);
					break;
			}
			printf ("Skipping function call !\n");
			break;

		case xpath_equality_expr :
			// [23]
			switch (u_sub)
			{
				case 0 : // equal
				case 1 : // not equal
					wsp_stack -> v_dump ();
					wipp_list = new work_item * [2];
					// we do not take copies, the work_func constructor will copy itself
					wipp_list [0] = wsp_stack -> wip_top (1);
					wipp_list [1] = wsp_stack -> wip_top (0);
					wp_func = new work_func (2, wipp_list);					
					delete [] wipp_list;
					if (u_sub == 1)
					   wp_func -> v_set_func_name ("__not_equal__");
					else
					   wp_func -> v_set_func_name ("__equal__");
					wsp_stack -> v_pop (2);
					wsp_stack -> v_push (wp_func);
					printf ("equality expr a = b\n");
					break;
				case 2 :
					// nothing. really
					break;
			}
			break;

      case xpath_name_test :
         // [37]
         switch (u_sub)
         {
            case 0 :
               // '*' 
               wsp_stack -> v_push (new work_name_test (0));
               break;
            case 1 :
               // NCName ':' '*' 
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_name_test (1, S_name . c_str ()));
               break;
            case 2 :
               // QName 
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_name_test (2, S_name . c_str ()));
               break;
         }
         break;

      case xpath_xml_q_name :
         // [206]
         switch (u_sub)
         {
            case 0 :
               printf ("[206]   QName has a prefix\n");
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               S_name_2 = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_qname (S_name_2 . c_str (), S_name . c_str ()));
               break;
            case 1 :
               printf ("[206]   QName has no prefix\n");
               S_name = wsp_stack -> cp_get_top_value ();
               wsp_stack -> v_pop ();
               wsp_stack -> v_push (new work_qname (NULL, S_name . c_str ()));
               break;
         }
         break;
      case xpath_xml_prefix :
         // [207]
         printf ("[207]   Prefix is %s\n", cp_explain);
         wsp_stack -> v_push (new work_string (cp_explain));
         break;
      case xpath_xml_local_part :
         // [208]
         printf ("[208]   LocalPart is %s\n", cp_explain);
         wsp_stack -> v_push (new work_string (cp_explain));
         break;
		default :
			printf ("[%d]   Skipping !\n", u_rule);
			break;
   }
}

void xpath_from_source::v_run (const char * cp_test_name, FILE * Fp_html)
{
   XDp_target -> Print (stdout);

   v_clone_children (XNp_source, XEp_root);

   v_mark_all_children (XDp_target, 1);
   l_mark_level = 2;

	wsp_stack -> v_dump ();

   wsp_stack -> wip_top () -> v_apply (XDp_target, "", l_mark_level);

   v_retain_attrib_tree (XDp_target, l_mark_level);

	if (Fp_html)
	{
		fprintf (Fp_html, "<h1>%s</h1>\n", cp_test_name);
		fprintf (Fp_html, "<table border=1><tr><th colspan=\"2\">XPath expression : %s</th></tr>\n", cp_get_expr ());
		fprintf (Fp_html, "<tr><th>Input</th><th>Output</th></tr>\n");
		fprintf (Fp_html, "<tr><td valign=\"top\">");
		fprintf (Fp_html, "<p>\n");
		v_out_html (Fp_html, XNp_source, 0);
		fprintf (Fp_html, "</p></td>\n");
		fprintf (Fp_html, "<td valign=\"top\"><p>\n");
		v_out_html (Fp_html, XDp_target -> FirstChildElement (), 0);
		fprintf (Fp_html, "</p></td></tr></table>\n");
	}
}

void xpath_from_source::v_apply_xpath (const char * cp_test_name, FILE * Fp_html_out)
{
	v_init ();
   v_evaluate ();
	v_run (cp_test_name, Fp_html_out);
	v_close ();
}
void xpath_from_source::v_init ()
{
   wsp_stack = new work_stack;
   XDp_target -> Parse ("<?xml version=\"1.0\"?><xpath:root/>");
   XEp_root = XDp_target -> FirstChildElement ();
   assert (XEp_root);
}
void xpath_from_source::v_close ()
{
   delete wsp_stack;
}
