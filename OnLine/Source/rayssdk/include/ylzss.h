/*
**      YLZSS.H
**      YLZSS compress functions.
**      ZJian,2000/09/06.
*/
#ifndef _YLZSS_H_INCLUDE_
#define _YLZSS_H_INCLUDE_      1

EXPORT  SLONG   FNBACK  LZSS_compress_file(USTR *input_filename,USTR *output_filename);
EXPORT  SLONG   FNBACK  LZSS_expand_file(USTR *input_filename,USTR *output_filename);
EXPORT  SLONG   FNBACK  LZSS_compress_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size);
EXPORT  SLONG   FNBACK  LZSS_expand_data(USTR *input,USTR *output,ULONG *input_size,ULONG *output_size);

#endif//_YLZSS_H_INCLUDE_
