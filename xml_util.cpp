#include "xml_util.h"

/// Cardinality in the terms of XPath counts from 1 for the first element
int i_xml_cardinality (const TiXmlElement * XEp_elem)
{
	TiXmlNode * XNp_parent;
	TiXmlElement * XEp_child;
	int i_res;

	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
	i_res = 1;
	while (XEp_child)
	{
	   if (XEp_child == XEp_elem)
			return i_res;
		else
		{
         i_res++;
			XEp_child = XEp_child -> NextSiblingElement ();
		}
	}
	assert (false);
	return -1;
}

int i_xml_family_size (const TiXmlElement * XEp_elem)
{
	TiXmlElement * XEp_child;
	TiXmlNode * XNp_parent;
   int i_res;

	XNp_parent = XEp_elem -> Parent ();
	assert (XNp_parent);
	XEp_child = XNp_parent -> FirstChildElement ();
   i_res = 0;
	while (XEp_child)
	{
      i_res++;
		XEp_child = XEp_child -> NextSiblingElement ();
	}
	return i_res;
}
