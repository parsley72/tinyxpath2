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

#include "tinyxml.h"
#include "htmlutil.h"

void v_levelize (int i_level, FILE * Fp_out, bool o_html)
{
	int i_loop;
	for (i_loop = 0; i_loop < i_level; i_loop++)
		if (o_html)
   		fprintf (Fp_out, "&nbsp;&nbsp;&nbsp;");
		else
	   	fprintf (Fp_out, "   ");
}

void v_out_html (
	FILE * Fp_out,
	TiXmlNode * XNp_source,
	unsigned u_level)
{
	TiXmlNode * XNp_child;
	TiXmlAttribute * XAp_att;

	XNp_child = XNp_source -> FirstChild ();
	while (XNp_child)
	{
		if (XNp_child -> ToDocument ())
		{
			fprintf (Fp_out, "\nStart document\n");
		}
		else
		if (XNp_child -> ToElement ())
		{
			v_levelize ((int) u_level, Fp_out, true);
			fprintf (Fp_out, "&lt;");   // '<'
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "<b>");
			fprintf (Fp_out, "%s", XNp_child -> ToElement () -> Value ());
			if (XNp_child -> GetUserValue ())
				fprintf (Fp_out, "</b>");
			XAp_att = XNp_child -> ToElement () -> FirstAttribute ();
			while (XAp_att)
			{
				if (XAp_att -> GetUserValue ())
				   fprintf (Fp_out, "<b>");
				fprintf (Fp_out, " %s='%s'", XAp_att -> Name (), XAp_att -> Value ());
				if (XAp_att -> GetUserValue ())
				   fprintf (Fp_out, "</b>");
				XAp_att = XAp_att -> Next ();
			}
			if (XNp_child -> FirstChild ())
			   fprintf (Fp_out, "&gt;<br>\n");   // '>\n'
			else
				fprintf (Fp_out, " /&gt;<br>\n");
		}
		else
		if (XNp_child -> ToComment ())
		{
			fprintf (Fp_out, "&lt;!-- %s --&gt;<br>\n", XNp_child -> ToComment () -> Value ());
		}
		else
		if (XNp_child -> ToText ())
		{
			fprintf (Fp_out, "%s\n", XNp_child -> ToText () -> Value ());
		}
		else
		if (XNp_child -> ToDeclaration ())
		{
		}
		else
		if (XNp_child -> ToUnknown ())
		{
		}
		else
			assert (false);

		v_out_html (Fp_out, XNp_child, u_level + 1);

		if (XNp_child -> ToDocument ())
		{
			fprintf (Fp_out, "\nEnd document\n");
		}
		else
		if (XNp_child -> ToElement ())
		{
			if (XNp_child -> FirstChild ())
			{
				v_levelize ((int) u_level, Fp_out, true);
				fprintf (Fp_out, "&lt;");
				if (XNp_child -> GetUserValue ())
					fprintf (Fp_out, "<b>");
				fprintf (Fp_out, "/%s", XNp_child -> ToElement () -> Value ());
				if (XNp_child -> GetUserValue ())
					fprintf (Fp_out, "</b>");
				fprintf (Fp_out, "&gt;<br>\n");   // '>\n'
			}
		}
		else
		if (XNp_child -> ToComment ())
		{
		}
		else
		if (XNp_child -> ToText ())
		{
		}
		else
		if (XNp_child -> ToDeclaration ())
		{
		}
		else
		if (XNp_child -> ToUnknown ())
		{
		}
		XNp_child = XNp_child -> NextSibling ();
	}
}