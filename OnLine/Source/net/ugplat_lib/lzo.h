/*
**  LZO.h
**  real-time data compression.
**  http://wildsau.idv.uni-linz.ac.at/mfx/lzo.html
**    
**  Hawke, 2005.1.4.
*/
#ifndef _LZO_H_
#define _LZO_H_

/*
**  We want to compress the data block at `in' with length `IN_LEN' to
**  the block at `out'. Because the input block may be incompressible,
**  we must provide a little more output space in case that compression
**  is not possible.
*/
//#define IN_LEN		(0x10000L)
//#define OUT_LEN		(IN_LEN + IN_LEN / 64 + 16 + 3)


/*
**   function : lzo compress data.
**  arguments : void *in        -- data buffer for input original data
**              unsigned in_len -- data length of input original data (bytes)
**              void *out       -- data buffer for output compressed data
**     return : int             -- data length of output compressed data (bytes)
**
**  PS: Because the input block may be incompressible, we must provide a little more
**      output space in case that compression is not possible.
**
**      For example, if the input block size is IN_LEN, the output block size is OUT_LEN,
**      the following is sugguested :
**            OUT_LEN >= (IN_LEN + IN_LEN / 64 + 16 + 3)
**
*/
extern  int lzo_compress (  void *in , unsigned  in_len, void *out);


/*
**   function : lzo decompress data.
**  arguments : void *in        -- data buffer for input compressed data
**              unsigned in_len -- data length of input compressed data (bytes)
**              void *out       -- data buffer for output original data
**     return : int             -- data length of output original data (bytes)
**
*/
extern  int lzo_decompress (  void *in , unsigned  in_len, void *out);



#endif//_LZO_H_

