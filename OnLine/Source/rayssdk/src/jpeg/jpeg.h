/*
**	JPEG.H
**	Quake3 open source.
**
**	fox neatened, 2001.5.30.
**
*/
#ifndef _JPEG_H_INCLUDE_
#define	_JPEG_H_INCLUDE_


// image flags for LoadImageBuff().
#define	JPG_IMAGE	0
#define	TGA_IMAGE	1
#define	BMP_IMAGE	2


EXPORT	int		FNBACK	LoadJPG( const char *filename, unsigned char **pic, int *width, int *height );
EXPORT	int		FNBACK	LoadImage( const char *name, byte **pic, int *width, int *height );
EXPORT	int		FNBACK	LoadImageBuff( byte *buffer, byte **pic, int *width, int *height ,int flag);

#endif//_JPEG_H_INCLUDE_
