/*
**      BITIO.CPP
**      Bit input/output functions.
**
**      ZJian, 1999.05.15.
**          Created.
**      ZJian, 2001.1.9.
**          Modified for make library.
**
*/
#include "rays.h"
//#include <windows.h>
//#include <stdio.h>
//#include <stdlib.h>
#include "ybitio.h"


EXPORT  BITFILE *   FNBACK  open_input_bitfile( char *filename )
{
    BITFILE *bitfile = NULL;
    
    if(NULL == ( bitfile = (BITFILE *)GlobalAlloc(GPTR, sizeof(BITFILE)) ) )
        return NULL;
    if(NULL == ( bitfile->file = fopen((const char *)filename,"rb") ) )
    {
        GlobalFree(bitfile);
        return NULL;
    }
    bitfile->mask=0x80;
    bitfile->rack=0;
    bitfile->counter=0;
    return(bitfile);
}


EXPORT  void    FNBACK  close_input_bitfile( BITFILE *bitfile )
{
    if(bitfile->file) fclose(bitfile->file);
    GlobalFree(bitfile);
}


EXPORT  BITFILE *   FNBACK  open_output_bitfile( char *filename )
{
    BITFILE *bitfile=NULL;
    
    if(NULL == ( bitfile = (BITFILE *)GlobalAlloc(GPTR, sizeof(BITFILE)) ) )
        return NULL;
    if(NULL == ( bitfile->file = fopen((const char *)filename,"wb") ) )
    {
        GlobalFree(bitfile);
        return NULL;
    }
    bitfile->mask=0x80;
    bitfile->rack=0;
    bitfile->counter=0;
    return(bitfile);
}


EXPORT  void    FNBACK  close_output_bitfile( BITFILE *bitfile )
{
    if( 0x80 != bitfile->mask )
    {
        if(putc(bitfile->rack,bitfile->file) != bitfile->rack)
        {
            //sprintf((char *)print_rec,"%s","BITIO_close_output_bitfile error");
            //log_error(1,print_rec);
        }
    }
    if(bitfile->file) fclose(bitfile->file);
    GlobalFree(bitfile);
}


EXPORT  void    FNBACK  output_bit( BITFILE *bitfile, int bit)
{
    if(bit)
        bitfile->rack |= bitfile->mask;
    bitfile->mask >>= 1;
    if(0 == bitfile->mask)
    {
        if( putc(bitfile->rack,bitfile->file) != bitfile->rack )
        {
            //sprintf((char *)print_rec,"%s","BITIO_output_bit error");
            //log_error(1,print_rec);
        }
        else
        {
            bitfile->counter ++;
            bitfile->rack = 0;
            bitfile->mask = 0x80;
        }
    }
}


EXPORT  void    FNBACK  output_bits(BITFILE *bitfile, unsigned long code, int count)
{
    unsigned long mask;
    
    mask = 1L << (count-1);
    while( mask != 0)
    {
        if(mask & code)
            bitfile->rack |= bitfile->mask;
        bitfile->mask >>= 1;
        if(0 == bitfile->mask)
        {
            if(putc(bitfile->rack,bitfile->file) != bitfile->rack)
            {
                //sprintf((char *)print_rec,"%s","BITIO_output_bit error");
                //log_error(1,print_rec);
            }
            else
            {
                bitfile->counter++;
                bitfile->rack=0;
                bitfile->mask=0x80;
            }
        }
        mask >>= 1;
    }
}


EXPORT  int     FNBACK  input_bit(BITFILE *bitfile)
{
    int value;
    
    if(bitfile->mask == 0x80)
    {
        bitfile->rack = (short int)fgetc(bitfile->file);
        if(bitfile->rack == EOF)
        {
            //sprintf((char *)print_rec,"%s","BITIO_input_bit error");
            //log_error(1,print_rec);
        }
        bitfile->counter ++;
    }
    value = bitfile->rack & bitfile->mask;
    bitfile->mask >>= 1;
    if(bitfile->mask == 0)
        bitfile->mask = 0x80;
    return(value ? 1 : 0);
}


EXPORT  unsigned long   FNBACK  input_bits(BITFILE *bitfile, int bit_count)
{
    unsigned long mask;
    unsigned long ret_val;
    
    mask=1L<<(bit_count-1);
    ret_val=0;
    while(mask!=0)
    {
        if(bitfile->mask==0x80)
        {
            bitfile->rack=(short int)fgetc(bitfile->file);
            if(bitfile->rack == EOF)
            {
                //sprintf((char *)print_rec,"%s","BITIO_input_bits error");
                //log_error(1,print_rec);
            }
            bitfile->counter++;
        }
        if(bitfile->rack & bitfile->mask)
            ret_val|=mask;
        bitfile->mask>>=1;
        if(bitfile->mask==0)
            bitfile->mask=0x80;
        mask=mask>>1;
    }
    return(ret_val);
}


EXPORT  void    FNBACK  binprintf_bits(FILE *file, unsigned long code, int bits)
{
    unsigned long mask;
    
    mask = 1L << (bits-1);
    while(mask != 0)
    {
        if(code & mask)
            fputc('1',file);
        else
            fputc('0',file);
        mask >>= 1;
    }
}

