#ifndef __TINYBYTESTREAM_H
#define __TINYBYTESTREAM_H

#include <string.h>
#include "tinyutil.h"

/**
   A Byte stream class 
*/
class byte_stream
{
   /// Length of the total string
   unsigned u_length;
   /// Total string
   _byte_ * bp_in;
   /// Current read position
   _byte_ * bp_current;
   /// First invalid position
   _byte_ * bp_end;
   /// true when there are still some byte to read
   bool o_valid;
public :
   /// constructor
   byte_stream (const char * cp_in)
   {
      u_length = strlen (cp_in) + 1;
      bp_in = new _byte_ [u_length] ;
      memcpy (bp_in, cp_in, u_length);
      bp_current = bp_in;
      bp_end = bp_in + u_length - 1;
      o_valid = (bp_current != bp_end);
   }
   /// destructor
   ~ byte_stream ()
   {
      if (bp_in)
         delete [] bp_in;
   }
   /// Returns the byte on top 
   _byte_ b_top ()
   {
      return * bp_current;
   }
   /// Consumes one byte
   _byte_ b_pop ()
   {
      if (! o_is_valid ())
         return 0;
      bp_current++;
      o_valid = (bp_current != bp_end);
      return * (bp_current - 1);
   }
   /// true if there are still some byte to consume
   bool o_is_valid ()
   {
      return o_valid;
   }
   /// number of bytes still to consume
   unsigned u_remain ()
   {
      return (unsigned) (bp_end - bp_current);
   }
   /// peek a byte a little further down the stream
   _byte_ b_forward (unsigned u_nb_char)
   {
      if (u_remain () > u_nb_char)
         return bp_current [u_nb_char];
      return 0;
   }
   /// get a byte backward pointer to the stream 
   const _byte_ * bp_get_backward (unsigned u_amount)
   {
      return bp_current - u_amount + 1;
   }
} ;

#endif
