/*
**      YBITIO.H
**      bit file I/O header.
**
**      ZJian,2000.9.6.
**          Created for compress functions.
**      ZJian,2001.1.9.
**          Change function prototypes for make static library.
**
*/
#ifndef _YBITIO_H_INCLUDE_
#define _YBITIO_H_INCLUDE_      1
#include <stdio.h>

typedef struct  tagBITFILE
{
   FILE         *file;
   unsigned char mask;
   short int    rack;
   int          counter;
} BITFILE,*LPBITFILE;


// input bitfile for read bit(s) from
EXPORT  BITFILE *       FNBACK  open_input_bitfile( char *filename );
EXPORT  int             FNBACK  input_bit( BITFILE *bf);
EXPORT  unsigned long   FNBACK  input_bits( BITFILE *bf, int bit_count);
EXPORT  void            FNBACK  close_input_bitfile( BITFILE *bf );


// output bitfile for write bit(s) to
EXPORT  BITFILE *       FNBACK  open_output_bitfile( char *filename );
EXPORT  void            FNBACK  output_bit( BITFILE *bf, int bit );
EXPORT  void            FNBACK  output_bits( BITFILE *bf, unsigned long code, int count );
EXPORT  void            FNBACK  close_output_bitfile( BITFILE *bf );


#endif//_YBITIO_H_INCLUDE_

