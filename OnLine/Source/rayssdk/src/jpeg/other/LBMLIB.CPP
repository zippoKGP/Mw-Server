// lbmlib.c

//#include "stdafx.h"
#include "cmdlib.h"
#include "lbmlib.h"
#include "bmp.h"
#include "..\\pakstuff.h"
#include "..\\jpeglib.h"
//
//zjian,2001.5.30. added for our lib
#include "rays.h"
#include "..\jpeg.h"

/*
============================================================================

  LBM STUFF
  
	============================================================================
*/


typedef unsigned char	UBYTE;
//conflicts with windows typedef short			WORD;
typedef unsigned short	UWORD;
typedef long			LONG;

typedef enum
{
	ms_none,
		ms_mask,
		ms_transcolor,
		ms_lasso
} mask_t;

typedef enum
{
	cm_none,
		cm_rle1
} compress_t;

typedef struct
{
	UWORD		w,h;
	short		x,y;
	UBYTE		nPlanes;
	UBYTE		masking;
	UBYTE		compression;
	UBYTE		pad1;
	UWORD		transparentColor;
	UBYTE		xAspect,yAspect;
	short		pageWidth,pageHeight;
} bmhd_t;

extern	bmhd_t	bmhd;						// will be in native byte order



#define FORMID ('F'+('O'<<8)+((int)'R'<<16)+((int)'M'<<24))
#define ILBMID ('I'+('L'<<8)+((int)'B'<<16)+((int)'M'<<24))
#define PBMID  ('P'+('B'<<8)+((int)'M'<<16)+((int)' '<<24))
#define BMHDID ('B'+('M'<<8)+((int)'H'<<16)+((int)'D'<<24))
#define BODYID ('B'+('O'<<8)+((int)'D'<<16)+((int)'Y'<<24))
#define CMAPID ('C'+('M'<<8)+((int)'A'<<16)+((int)'P'<<24))


bmhd_t  bmhd;

int    Align (int l)
{
	if (l&1)
		return l+1;
	return l;
}



/*
================
LBMRLEdecompress

  Source must be evenly aligned!
  ================
*/
byte  *LBMRLEDecompress (byte *source,byte *unpacked, int bpwidth)
{
	int     count;
	byte    b,rept;
	
	count = 0;
	
	do
	{
		rept = *source++;
		
		if (rept > 0x80)
		{
			rept = (rept^0xff)+2;
			b = *source++;
			memset(unpacked,b,rept);
			unpacked += rept;
		}
		else if (rept < 0x80)
		{
			rept++;
			memcpy(unpacked,source,rept);
			unpacked += rept;
			source += rept;
		}
		else
			rept = 0;               // rept of 0x80 is NOP
		
		count += rept;
		
	} while (count<bpwidth);
	
	if (count>bpwidth)
		Error ("Decompression exceeded width!\n");
	
	
	return source;
}


/*
=================
LoadLBM
=================
*/
EXPORT	void	FNBACK	LoadLBM (char *filename, byte **picture, byte **palette)
{
	byte    *LBMbuffer, *picbuffer, *cmapbuffer;
	int             y;
	byte    *LBM_P, *LBMEND_P;
	byte    *pic_p;
	byte    *body_p;
	
	int    formtype,formlength;
	int    chunktype,chunklength;
	
	// qiet compiler warnings
	picbuffer = NULL;
	cmapbuffer = NULL;
	
	//
	// load the LBM
	//
	LoadFile (filename, (void **)&LBMbuffer);
	
	if (LBMbuffer == NULL)
	{
		return;
	}
	//
	// parse the LBM header
	//
	LBM_P = LBMbuffer;
	if ( *(int *)LBMbuffer != LittleLong(FORMID) )
		Error ("No FORM ID at start of file!\n");
	
	LBM_P += 4;
	formlength = BigLong( *(int *)LBM_P );
	LBM_P += 4;
	LBMEND_P = LBM_P + Align(formlength);
	
	formtype = LittleLong(*(int *)LBM_P);
	
	if (formtype != ILBMID && formtype != PBMID)
		Error ("Unrecognized form type: %c%c%c%c\n", formtype&0xff
		,(formtype>>8)&0xff,(formtype>>16)&0xff,(formtype>>24)&0xff);
	
	LBM_P += 4;
	
	//
	// parse chunks
	//
	
	while (LBM_P < LBMEND_P)
	{
		chunktype = LBM_P[0] + (LBM_P[1]<<8) + (LBM_P[2]<<16) + (LBM_P[3]<<24);
		LBM_P += 4;
		chunklength = LBM_P[3] + (LBM_P[2]<<8) + (LBM_P[1]<<16) + (LBM_P[0]<<24);
		LBM_P += 4;
		
		switch ( chunktype )
		{
		case BMHDID:
			memcpy (&bmhd,LBM_P,sizeof(bmhd));
			bmhd.w = BigShort(bmhd.w);
			bmhd.h = BigShort(bmhd.h);
			bmhd.x = BigShort(bmhd.x);
			bmhd.y = BigShort(bmhd.y);
			bmhd.pageWidth = BigShort(bmhd.pageWidth);
			bmhd.pageHeight = BigShort(bmhd.pageHeight);
			break;
			
		case CMAPID:
			cmapbuffer = (unsigned char*)malloc (768);
			memset (cmapbuffer, 0, 768);
			memcpy (cmapbuffer, LBM_P, chunklength);
			break;
			
		case BODYID:
			body_p = LBM_P;
			
			pic_p = picbuffer = (unsigned char*)malloc (bmhd.w*bmhd.h);
			if (formtype == PBMID)
			{
				//
				// unpack PBM
				//
				for (y=0 ; y<bmhd.h ; y++, pic_p += bmhd.w)
				{
					if (bmhd.compression == cm_rle1)
						body_p = LBMRLEDecompress ((byte *)body_p
						, pic_p , bmhd.w);
					else if (bmhd.compression == cm_none)
					{
						memcpy (pic_p,body_p,bmhd.w);
						body_p += Align(bmhd.w);
					}
				}
				
			}
			else
			{
				//
				// unpack ILBM
				//
				Error ("%s is an interlaced LBM, not packed", filename);
			}
			break;
		}
		
		LBM_P += Align(chunklength);
	}
	
	free (LBMbuffer);
	
	*picture = picbuffer;
	
	if (palette)
		*palette = cmapbuffer;
}


/*
============================================================================

  WRITE LBM
  
============================================================================
*/

/*
==============
WriteLBMfile
==============
*/
EXPORT	void	FNBACK	WriteLBMfile (char *filename, byte *data,	int width, int height, byte *palette)
{
	byte    *lbm, *lbmptr;
	int    *formlength, *bmhdlength, *cmaplength, *bodylength;
	int    length;
	bmhd_t  basebmhd;
	
	lbm = lbmptr = (unsigned char*)malloc (width*height+1000);
	
	//
	// start FORM
	//
	*lbmptr++ = 'F';
	*lbmptr++ = 'O';
	*lbmptr++ = 'R';
	*lbmptr++ = 'M';
	
	formlength = (int*)lbmptr;
	lbmptr+=4;                      // leave space for length
	
	*lbmptr++ = 'P';
	*lbmptr++ = 'B';
	*lbmptr++ = 'M';
	*lbmptr++ = ' ';
	
	//
	// write BMHD
	//
	*lbmptr++ = 'B';
	*lbmptr++ = 'M';
	*lbmptr++ = 'H';
	*lbmptr++ = 'D';
	
	bmhdlength = (int *)lbmptr;
	lbmptr+=4;                      // leave space for length
	
	memset (&basebmhd,0,sizeof(basebmhd));
	basebmhd.w = BigShort((short)width);
	basebmhd.h = BigShort((short)height);
	basebmhd.nPlanes = 8;
	basebmhd.xAspect = 5;
	basebmhd.yAspect = 6;
	basebmhd.pageWidth = BigShort((short)width);
	basebmhd.pageHeight = BigShort((short)height);
	
	memcpy (lbmptr,&basebmhd,sizeof(basebmhd));
	lbmptr += sizeof(basebmhd);
	
	length = lbmptr-(byte *)bmhdlength-4;
	*bmhdlength = BigLong(length);
	if (length&1)
		*lbmptr++ = 0;          // pad chunk to even offset
	
	//
	// write CMAP
	//
	*lbmptr++ = 'C';
	*lbmptr++ = 'M';
	*lbmptr++ = 'A';
	*lbmptr++ = 'P';
	
	cmaplength = (int *)lbmptr;
	lbmptr+=4;                      // leave space for length
	
	memcpy (lbmptr,palette,768);
	lbmptr += 768;
	
	length = lbmptr-(byte *)cmaplength-4;
	*cmaplength = BigLong(length);
	if (length&1)
		*lbmptr++ = 0;          // pad chunk to even offset
	
	//
	// write BODY
	//
	*lbmptr++ = 'B';
	*lbmptr++ = 'O';
	*lbmptr++ = 'D';
	*lbmptr++ = 'Y';
	
	bodylength = (int *)lbmptr;
	lbmptr+=4;                      // leave space for length
	
	memcpy (lbmptr,data,width*height);
	lbmptr += width*height;
	
	length = lbmptr-(byte *)bodylength-4;
	*bodylength = BigLong(length);
	if (length&1)
		*lbmptr++ = 0;          // pad chunk to even offset
	
	//
	// done
	//
	length = lbmptr-(byte *)formlength-4;
	*formlength = BigLong(length);
	if (length&1)
		*lbmptr++ = 0;          // pad chunk to even offset
	
	//
	// write output file
	//
	SaveFile (filename, lbm, lbmptr-lbm);
	free (lbm);
}

/*
============================================================================

 LOAD PCX
  
============================================================================
*/
typedef struct
{
	char	manufacturer;
	char	version;
	char	encoding;
	char	bits_per_pixel;
	unsigned short	xmin,ymin,xmax,ymax;
	unsigned short	hres,vres;
	unsigned char	palette[48];
	char	reserved;
	char	color_planes;
	unsigned short	bytes_per_line;
	unsigned short	palette_type;
	char	filler[58];
	unsigned char	data;			// unbounded
} pcx_t;
	
/*
==============
LoadPCX
==============
*/
EXPORT	int		FNBACK	LoadPCX (char *filename, byte **pic, byte **palette, int *width, int *height)
{
	byte	*raw = 0;
	pcx_t	*pcx;
	int		x, y;
	int		len;
	int		dataByte, runLength;
	byte	*out, *pix;
	
	if (pic)
		*pic = NULL;
	if (palette)
		*palette = NULL;
	if (width)
		*width = 0;
	if (height)
		*height = 0;
	
	//
	// load the file
	//
	len = LoadFile (filename, (void **)&raw);
	if (len == -1)
		return -1;
	
	//
	// parse the PCX file
	//
	pcx = (pcx_t *)raw;
	raw = &pcx->data;
	
	pcx->xmin = LittleShort(pcx->xmin);
	pcx->ymin = LittleShort(pcx->ymin);
	pcx->xmax = LittleShort(pcx->xmax);
	pcx->ymax = LittleShort(pcx->ymax);
	pcx->hres = LittleShort(pcx->hres);
	pcx->vres = LittleShort(pcx->vres);
	pcx->bytes_per_line = LittleShort(pcx->bytes_per_line);
	pcx->palette_type = LittleShort(pcx->palette_type);
	
	if (pcx->manufacturer != 0x0a
		|| pcx->version != 5
		|| pcx->encoding != 1
		|| pcx->bits_per_pixel != 8
		|| pcx->xmax >= 640
		|| pcx->ymax >= 480)
		Error ("Bad pcx file %s", filename);
	
	if (palette)
	{
		*palette = (unsigned char*)malloc(768);
		memcpy (*palette, (byte *)pcx + len - 768, 768);
	}
	
	if (width)
		*width = pcx->xmax+1;
	if (height)
		*height = pcx->ymax+1;
	
	if (!pic)
	{
		free(pcx);
		return -1;
	}
	
	out = (unsigned char*)malloc ( (pcx->ymax+1) * (pcx->xmax+1) );
	if (!out)
		Error ("Skin_Cache: couldn't allocate");
	
	*pic = out;
	
	pix = out;
	
	for (y=0 ; y<=pcx->ymax ; y++, pix += pcx->xmax+1)
	{
		for (x=0 ; x<=pcx->xmax ; )
		{
			dataByte = *raw++;
			
			if((dataByte & 0xC0) == 0xC0)
			{
				runLength = dataByte & 0x3F;
				dataByte = *raw++;
			}
			else
				runLength = 1;
			
			while(runLength-- > 0)
				pix[x++] = dataByte;
		}
		
	}
	
	if ( raw - (byte *)pcx > len)
		Error ("PCX file %s was malformed", filename);
	
	free (pcx);
	return 0;
}
/* 
============== 
WritePCXfile 
============== 
*/ 
EXPORT	void	FNBACK	WritePCXfile (char *filename, byte *data, int width, int height, byte *palette) 
{
	int		i, j, length;
	pcx_t	*pcx;
	byte		*pack;
	
	pcx = (pcx_t*)malloc (width*height*2+1000);
	memset (pcx, 0, sizeof(*pcx));
	
	pcx->manufacturer = 0x0a;	// PCX id
	pcx->version = 5;			// 256 color
	pcx->encoding = 1;		// uncompressed
	pcx->bits_per_pixel = 8;		// 256 color
	pcx->xmin = 0;
	pcx->ymin = 0;
	pcx->xmax = LittleShort((short)(width-1));
	pcx->ymax = LittleShort((short)(height-1));
	pcx->hres = LittleShort((short)width);
	pcx->vres = LittleShort((short)height);
	pcx->color_planes = 1;		// chunky image
	pcx->bytes_per_line = LittleShort((short)width);
	pcx->palette_type = LittleShort(2);		// not a grey scale
	
	// pack the image
	pack = &pcx->data;
	
	for (i=0 ; i<height ; i++)
	{
		for (j=0 ; j<width ; j++)
		{
			if ( (*data & 0xc0) != 0xc0)
				*pack++ = *data++;
			else
			{
				*pack++ = 0xc1;
				*pack++ = *data++;
			}
		}
	}
	
	// write the palette
	*pack++ = 0x0c;	// palette ID byte
	for (i=0 ; i<768 ; i++)
		*pack++ = *palette++;
	
	// write output file 
	length = pack - (byte *)pcx;
	SaveFile (filename, pcx, length);
	
	free (pcx);
} 
	
/*
============================================================================

  LOAD IMAGE
  
============================================================================
*/
	
/*
==============
Load256Image

  Will load either an lbm or pcx, depending on extension.
  Any of the return pointers can be NULL if you don't want them.
  ==============
*/
void Load256Image (char *name, byte **pixels, byte **palette,
				   int *width, int *height)
{
	char	ext[128];
	bitmap_t	bmp;
	
	ExtractFileExtension (name, ext);
	if (stricmp(ext, "lbm"))
	{
		LoadLBM (name, pixels, palette);
		if (width)
			*width = bmhd.w;
		if (height)
			*height = bmhd.h;
	}
	else if (stricmp (ext, "pcx"))
	{
		LoadPCX (name, pixels, palette, width, height);
	}
	else if (stricmp (ext, "bmp"))
	{
		LoadBMP (name, &bmp);
		if (bmp.palette)
		{
			*palette = (unsigned char*)malloc (768);
			memcpy (*palette, bmp.palette, 768);
		}
		FreeBMP (&bmp);		
	}
	else
		Error ("%s doesn't have a known image extension", name);
}


/*
==============
Save256Image

  Will save either an lbm or pcx, depending on extension.
  ==============
*/
void Save256Image (char *name, byte *pixels, byte *palette,
				   int width, int height)
{
	char	ext[128];
	
	ExtractFileExtension (name, ext);
	if (!strcmp (ext, "lbm"))
	{
		WriteLBMfile (name, pixels, width, height, palette);
	}
	else if (!stricmp (ext, "pcx"))
	{
		WritePCXfile (name, pixels, width, height, palette);
	}
	else
		Error ("%s doesn't have a known image extension", name);
}


	
/*
============================================================================

  TARGA IMAGE
  
============================================================================
*/
typedef struct _TargaHeader {
	unsigned char 	id_length, colormap_type, image_type;
	unsigned short	colormap_index, colormap_length;
	unsigned char	colormap_size;
	unsigned short	x_origin, y_origin, width, height;
	unsigned char	pixel_size, attributes;
} TargaHeader;

int fgetLittleShort (FILE *f)
{
	byte	b1, b2;
	
	b1 = fgetc(f);
	b2 = fgetc(f);
	
	return (short)(b1 + b2*256);
}

int getLittleShort (byte*& p)
{
	byte	b1, b2;
	b1 = *p++;
	b2 = *p++;
	return (short)(b1 + b2*256);
}

int getLittleLong (byte*& p)
{
	byte	b1, b2, b3, b4;
	
	b1 = *p++;
	b2 = *p++;
	b3 = *p++;
	b4 = *p++;
	return b1 + (b2<<8) + (b3<<16) + (b4<<24);
}
/*
char getc(byte*& p)
{
return *p++;
}
*/
/*
=========================================================

  BMP LOADING
  
	=========================================================
*/
typedef struct
{
	char id[2];
	unsigned long fileSize;
	unsigned long reserved0;
	unsigned long bitmapDataOffset;
	unsigned long bitmapHeaderSize;
	unsigned long width;
	unsigned long height;
	unsigned short planes;
	unsigned short bitsPerPixel;
	unsigned long compression;
	unsigned long bitmapDataSize;
	unsigned long hRes;
	unsigned long vRes;
	unsigned long colors;
	unsigned long importantColors;
	unsigned char palette[256][4];
} BMPHeader_t;
	
	
static int LoadBMPBuff( byte *buffer, byte **pic, int *width, int *height )
{
	int		columns, rows, numPixels;
	byte	*pixbuf;
	int		row, column;
	byte	*buf_p;
	byte	*t_buf_p;
	
	BMPHeader_t bmpHeader;
	byte		*bmpRGBA;
	
	buf_p = buffer;
	
	bmpHeader.id[0] = *buf_p++;
	bmpHeader.id[1] = *buf_p++;
	bmpHeader.fileSize = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.reserved0 = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.bitmapDataOffset = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.bitmapHeaderSize = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.width = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.height = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.planes = LittleShort( * ( short * ) buf_p );
	buf_p += 2;
	bmpHeader.bitsPerPixel = LittleShort( * ( short * ) buf_p );
	buf_p += 2;
	bmpHeader.compression = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.bitmapDataSize = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.hRes = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.vRes = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.colors = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	bmpHeader.importantColors = LittleLong( * ( long * ) buf_p );
	buf_p += 4;
	
	memcpy( bmpHeader.palette, buf_p, sizeof( bmpHeader.palette ) );
	
	if ( bmpHeader.bitsPerPixel == 8 )
		buf_p += 1024;
	
	if ( bmpHeader.id[0] != 'B' && bmpHeader.id[1] != 'M' ) 
	{
		return -1;
	}
	if ( bmpHeader.compression != 0 )
	{
		return -1;
	}
	if ( bmpHeader.bitsPerPixel < 8 )
	{
		return -1;
	}
	
	columns = bmpHeader.width;
	rows = bmpHeader.height;
	if ( rows < 0 )
		rows = -rows;
	
	if ( width ) 
		*width = columns;
	if ( height )
		*height = rows;
	
	//for debug
	int	temp_data;
	
	temp_data = ((columns*bmpHeader.bitsPerPixel/8)+3)/4*4;
	numPixels = columns * rows;
	bmpRGBA = reinterpret_cast<unsigned char*>(malloc( numPixels * 4 ));
	*pic = bmpRGBA;
	
	t_buf_p = buf_p;
	//	t_buf_p -=4;
	for ( row = rows-1; row >= 0; row-- )
	{
		pixbuf = bmpRGBA + row*columns*4;
		buf_p  = t_buf_p+(rows-row-1)*temp_data;
		
		for ( column = 0; column < columns; column++ )
		{
			unsigned char red, green, blue, alpha;
			int palIndex;
			unsigned short shortPixel;
			
			switch ( bmpHeader.bitsPerPixel )
			{
			case 8:
				palIndex = *buf_p++;
				*pixbuf++ = bmpHeader.palette[palIndex][2];
				*pixbuf++ = bmpHeader.palette[palIndex][1];
				*pixbuf++ = bmpHeader.palette[palIndex][0];
				*pixbuf++ = 0xff;
				break;
			case 16:
				shortPixel = * ( unsigned short * ) pixbuf;
				pixbuf += 2;
				*pixbuf++ = ( shortPixel & ( 31 << 10 ) ) >> 7;
				*pixbuf++ = ( shortPixel & ( 31 << 5 ) ) >> 2;
				*pixbuf++ = ( shortPixel & ( 31 ) ) << 3;
				*pixbuf++ = 0xff;
				break;
				
			case 24:
				blue = *buf_p++;
				green = *buf_p++;
				red = *buf_p++;
				*pixbuf++ = red;
				*pixbuf++ = green;
				*pixbuf++ = blue;
				*pixbuf++ = 255;
				break;
			case 32:
				blue = *buf_p++;
				green = *buf_p++;
				red = *buf_p++;
				alpha = *buf_p++;
				*pixbuf++ = red;
				*pixbuf++ = green;
				*pixbuf++ = blue;
				*pixbuf++ = alpha;
				break;
			default:
				return -1;
				break;
			}
		}
	}
	
	return 0;
}

static int LoadBMP( const char *name, byte **pic, int *width, int *height )
{
	byte	*buffer;
	unsigned int		length;
	int		result;
	
	*pic = NULL;
	
	//
	// load the file
	//
	length = LoadFile( ( char * ) name, (void **)&buffer);
	if (length == -1)
	{
		length = PakLoadAnyFile( (char*)name, (void**)&buffer);
		if (length == -1)
		{
			return -1;
		}
	}
	
	result = LoadBMPBuff(buffer,pic,width,height);
	free( buffer );
	return result;
}
/*
=================================================================

  PCX LOADING
  
	=================================================================
*/


/*
==============
LoadPCX
==============
*/
static int LoadPCX ( const char *filename, byte **pic, byte **palette, int *width, int *height)
{
	byte	*raw;
	pcx_t	*pcx;
	int		x, y;
	int		len;
	int		dataByte, runLength;
	byte	*out, *pix;
	int		xmax, ymax;
	
	*pic = NULL;
	*palette = NULL;
	
	//
	// load the file
	//
	len = LoadFile( ( char * ) filename, (void **)&raw);
	if (len == -1) 
	{
		len = PakLoadAnyFile( (char*)filename, (void**)&raw);
		if (len == -1)
		{
			return -1;
		}
	}
	
	//
	// parse the PCX file
	//
	pcx = (pcx_t *)raw;
	raw = &pcx->data;
	
	xmax = LittleShort(pcx->xmax);
    ymax = LittleShort(pcx->ymax);
	
	if (pcx->manufacturer != 0x0a
		|| pcx->version != 5
		|| pcx->encoding != 1
		|| pcx->bits_per_pixel != 8
		|| xmax >= 1024
		|| ymax >= 1024)
	{
		Sys_Printf ("Bad pcx file %s (%i x %i) (%i x %i)\n", filename, xmax+1, ymax+1, pcx->xmax, pcx->ymax);
		return -1;
	}
	
	out = reinterpret_cast<unsigned char*>(malloc ( (ymax+1) * (xmax+1) ));
	
	*pic = out;
	
	pix = out;
	
	if (palette)
	{
		*palette = reinterpret_cast<unsigned char*>(malloc(768));
		memcpy (*palette, (byte *)pcx + len - 768, 768);
	}
	
	if (width)
		*width = xmax+1;
	if (height)
		*height = ymax+1;
	// FIXME: use bytes_per_line here?
	
	for (y=0 ; y<=ymax ; y++, pix += xmax+1)
	{
		for (x=0 ; x<=xmax ; )
		{
			dataByte = *raw++;
			
			if((dataByte & 0xC0) == 0xC0)
			{
				runLength = dataByte & 0x3F;
				dataByte = *raw++;
			}
			else
				runLength = 1;
			
			while(runLength-- > 0)
				pix[x++] = dataByte;
		}
		
	}
	
	if ( raw - (byte *)pcx > len)
	{
		Sys_Printf ("PCX file %s was malformed", filename);
		free (*pic);
		*pic = NULL;
	}
	
	free(pcx);
	return 0;
}


/*
==============
LoadPCX32
==============
*/
static int LoadPCX32 ( const char *filename, byte **pic, int *width, int *height) 
{
	byte	*palette;
	byte	*pic8;
	int		i, c, p;
	byte	*pic32;
	
	LoadPCX (filename, &pic8, &palette, width, height);
	if (!pic8) {
		*pic = NULL;
		return -1;
	}
	
	c = (*width) * (*height);
	pic32 = *pic = reinterpret_cast<unsigned char*>(malloc(4 * c ));
	for (i = 0 ; i < c ; i++) {
		p = pic8[i];
		pic32[0] = palette[p*3];
		pic32[1] = palette[p*3 + 1];
		pic32[2] = palette[p*3 + 2];
		pic32[3] = 255;
		pic32 += 4;
	}
	
	free (pic8);
	free (palette);
	return 0;
}

/*
=========================================================

  TARGA LOADING
  
	=========================================================
*/

/*
=============
LoadTGA
=============
*/
int LoadTGABuff ( byte *buffer, byte **pic, int *width, int *height)
{
	int		columns, rows, numPixels;
	byte	*pixbuf;
	int		row, column;
	byte	*buf_p;
	TargaHeader	targa_header;
	byte		*targa_rgba;
	
	buf_p = buffer;
	
	targa_header.id_length = *buf_p++;
	targa_header.colormap_type = *buf_p++;
	targa_header.image_type = *buf_p++;
	
	targa_header.colormap_index = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.colormap_length = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.colormap_size = *buf_p++;
	targa_header.x_origin = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.y_origin = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.width = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.height = LittleShort ( *(short *)buf_p );
	buf_p += 2;
	targa_header.pixel_size = *buf_p++;
	targa_header.attributes = *buf_p++;
	
	//++timo debug
	bool bAlphaOK = false;
	
	if (targa_header.image_type!=2 
		&& targa_header.image_type!=10
		&& targa_header.image_type != 3 ) 
	{
		Sys_Printf("LoadTGA: Only type 2 (RGB), 3 (gray), and 10 (RGB) TGA images supported\n");
	}
	
	if ( targa_header.colormap_type != 0 )
	{
		Sys_Printf("LoadTGA: colormaps not supported\n" );
	}
	
	if ( ( targa_header.pixel_size != 32 && targa_header.pixel_size != 24 ) && targa_header.image_type != 3 )
	{
		Sys_Printf("LoadTGA: Only 32 or 24 bit images supported (no colormaps)\n");
	}
	
	columns = targa_header.width;
	rows = targa_header.height;
	numPixels = columns * rows;
	
	if (width)
		*width = columns;
	if (height)
		*height = rows;
	
	targa_rgba = reinterpret_cast<unsigned char*>(malloc (numPixels*4));
	*pic = targa_rgba;
	
	if (targa_header.id_length != 0)
		buf_p += targa_header.id_length;  // skip TARGA image comment
	
	if ( targa_header.image_type==2 || targa_header.image_type == 3 )
	{ 
		// Uncompressed RGB or gray scale image
		for(row=rows-1; row>=0; row--) 
		{
			pixbuf = targa_rgba + row*columns*4;
			for(column=0; column<columns; column++) 
			{
				unsigned char red,green,blue,alphabyte;
				switch (targa_header.pixel_size) 
				{
					
				case 8:
					blue = *buf_p++;
					green = blue;
					red = blue;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;
					
				case 24:
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = 255;
					break;
				case 32:
					blue = *buf_p++;
					green = *buf_p++;
					red = *buf_p++;
					alphabyte = *buf_p++;
					//++timo debug: detect if the whole alpha channel is 0
					if (alphabyte != 0)
						bAlphaOK = true;
					*pixbuf++ = red;
					*pixbuf++ = green;
					*pixbuf++ = blue;
					*pixbuf++ = alphabyte;
					break;
				default:
					return -1;
					break;
				}
			}
		}
		
		//++timo debug
		if (bAlphaOK)
			Sys_Printf("alpha channel OK");
		else
			Sys_Printf("empty alpha channel!");
	}
	else if (targa_header.image_type==10) {   // Runlength encoded RGB images
		
		//++timo debug
		Sys_Printf("runlength encode RGB image");
		
		unsigned char red,green,blue,alphabyte,packetHeader,packetSize,j;
		
		red = 0;
		green = 0;
		blue = 0;
		alphabyte = 0xff;
		
		for(row=rows-1; row>=0; row--) {
			pixbuf = targa_rgba + row*columns*4;
			for(column=0; column<columns; ) {
				packetHeader= *buf_p++;
				packetSize = 1 + (packetHeader & 0x7f);
				if (packetHeader & 0x80) {        // run-length packet
					switch (targa_header.pixel_size) {
					case 24:
						blue = *buf_p++;
						green = *buf_p++;
						red = *buf_p++;
						alphabyte = 255;
						break;
					case 32:
						blue = *buf_p++;
						green = *buf_p++;
						red = *buf_p++;
						alphabyte = *buf_p++;
						break;
					default:
						return -1;
						break;
					}
					
					for(j=0;j<packetSize;j++) {
						*pixbuf++=red;
						*pixbuf++=green;
						*pixbuf++=blue;
						*pixbuf++=alphabyte;
						column++;
						if (column==columns) { // run spans across rows
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}
					}
				}
				else {                            // non run-length packet
					for(j=0;j<packetSize;j++) {
						switch (targa_header.pixel_size) {
						case 24:
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = 255;
							break;
						case 32:
							blue = *buf_p++;
							green = *buf_p++;
							red = *buf_p++;
							alphabyte = *buf_p++;
							*pixbuf++ = red;
							*pixbuf++ = green;
							*pixbuf++ = blue;
							*pixbuf++ = alphabyte;
							break;
						default:
							return -1;
							break;
						}
						column++;
						if (column==columns) { // pixel packet run spans across rows
							column=0;
							if (row>0)
								row--;
							else
								goto breakOut;
							pixbuf = targa_rgba + row*columns*4;
						}						
					}
				}
			}
breakOut:;
		}
	}
	return 0;
}


EXPORT	int		FNBACK	LoadTGA ( const char *name, byte **pic, int *width, int *height)
{
	byte	*buffer;
	int		result;
	
	*pic = NULL;
	
	//
	// load the file
	//
	int nLen = LoadFile ( ( char * ) name, (void **)&buffer);
	if (nLen == -1) 
	{
		nLen = PakLoadAnyFile((char*)name, (void**)&buffer);
		if (nLen == -1)
		{
			return -1;
		}
	}
	
	result = LoadTGABuff(buffer,pic,width,height);
	
	free(buffer);
	return result;
}




EXPORT	int		FNBACK	LoadJPG( const char *filename, unsigned char **pic, int *width, int *height ) 
{
	byte	*fbuffer = NULL;
	int	result;
	int nLen = LoadFile( ( char * ) filename, (void **)&fbuffer);
	if (nLen == -1) 
	{
		nLen = PakLoadAnyFile((char*)filename, (void**)&fbuffer);
		if (nLen == -1)
		{
			return -1;
		}
	}
	result = LoadJPGBuff(fbuffer, pic, width, height);
	free(fbuffer);
	return result;
}

//===================================================================

/*
=================
LoadImage

  Loads any of the supported image types into a cannonical
  32 bit format.
  =================
*/
EXPORT	int		FNBACK	LoadImage( const char *name, byte **pic, int *width, int *height )
{
	int		len;
	int		result;
	*pic = NULL;
	*width = 0;
	*height = 0;
	
	len = strlen(name);
	if (len<5) 
	{
		return -1;
	}
	
	if ( !stricmp( name+len-4, ".tga" ) ) 
	{
		result = LoadTGA( name, pic, width, height );
		return result;
	}
	else if ( !stricmp(name+len-4, ".pcx") ) 
	{
		result = LoadPCX32( name, pic, width, height );
		return result;
	} 
	else if ( !stricmp( name+len-4, ".bmp" ) ) 
	{
		result = LoadBMP( name, pic, width, height );
		return result;
	} 
	else if ( !stricmp( name+len-4, ".jpg" ) ) 
	{
		result = LoadJPG( name, pic, width, height ); 
		return result;
	}
	return -1;
}


EXPORT	int		FNBACK	LoadImageBuff( byte *buffer, byte **pic, int *width, int *height ,int flag)
{
	int		result;
	*pic = NULL;
	*width = 0;
	*height = 0;
	
	if (flag == TGA_IMAGE) 
	{
		result = LoadTGABuff( buffer, pic, width, height );
		return result;
	}
	else if (flag = BMP_IMAGE) 
	{
		result = LoadBMPBuff( buffer, pic, width, height );
		return result;
	} 
	else if (flag == JPG_IMAGE) 
	{
		result = LoadJPGBuff( buffer, pic, width, height ); 
		return result;
	}
	
	return -1;
}

//
#define SCROLLBACK_MAX_LINES	600
#define SCROLLBACK_DEL_CHARS	500

void Sys_Printf (char *text, ...)
{
/*	va_list argptr;
char	buf[32768];
char	*out;
LRESULT	result;				// PGM
DWORD	oldPosS, oldPosE;	// PGM

  va_start (argptr,text);
  vsprintf (buf, text,argptr);
  va_end (argptr);
  
	out = TranslateString (buf);
	
	  #ifdef LATER
	  Sys_Status(out);
	  #else
	  //PGM
	  result = SendMessage (g_qeglobals.d_hwndEdit, EM_GETLINECOUNT, 0, 0);
	  if(result > SCROLLBACK_MAX_LINES)
	  {
	  char	replaceText[5];
	  
		replaceText[0] = '\0';
		
		  SendMessage (g_qeglobals.d_hwndEdit, WM_SETREDRAW, (WPARAM)0, (LPARAM)0);
		  SendMessage (g_qeglobals.d_hwndEdit, EM_GETSEL, (WPARAM)&oldPosS, (LPARAM)&oldPosE);
		  SendMessage (g_qeglobals.d_hwndEdit, EM_SETSEL, 0, SCROLLBACK_DEL_CHARS);
		  SendMessage (g_qeglobals.d_hwndEdit, EM_REPLACESEL, (WPARAM)0, (LPARAM)replaceText);
		  SendMessage (g_qeglobals.d_hwndEdit, EM_SETSEL, oldPosS, oldPosE);
		  SendMessage (g_qeglobals.d_hwndEdit, WM_SETREDRAW, (WPARAM)1, (LPARAM)0);
		  }
		  //PGM
		  
			SendMessage (g_qeglobals.d_hwndEdit, EM_REPLACESEL, 0, (LPARAM)out);
			#endif
	*/
}
