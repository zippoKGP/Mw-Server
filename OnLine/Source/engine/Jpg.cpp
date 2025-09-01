#include "rays.h"
#include "Jpg.h"

int jpeg_read(BYTE* pJpegData,SLONG nLenJData,JPEGAREA& area,BYTE** pBmpData,SLONG& nLenBData)
{
	JPEG_CORE_PROPERTIES  jcpJpeg;
	DWORD                 jerr;

	if(!pJpegData)
	{
		return JPG_MEMORYPOINTER_ERROR;
	}

	jerr = ijlInit( &jcpJpeg );
	
	if( jerr != IJL_OK )
	{
		return JPG_DECODE_ERROR;
	}
	
	jcpJpeg.JPGFile			= NULL;
	jcpJpeg.JPGBytes		= pJpegData;
	jcpJpeg.JPGSizeBytes	= nLenJData;

	jerr = ijlRead( &jcpJpeg, IJL_JBUFF_READPARAMS );
	
    if( jerr != IJL_OK )
    {
		return JPG_DECODE_ERROR;
    }
	
    switch(jcpJpeg.JPGChannels)
    {
    case 1:
		jcpJpeg.JPGColor    = IJL_G;
		jcpJpeg.DIBChannels = 3;
		jcpJpeg.DIBColor    = IJL_BGR;
		break;
		
    case 3:
		jcpJpeg.JPGColor    = IJL_YCBCR;
		jcpJpeg.DIBChannels = 3;
		jcpJpeg.DIBColor    = IJL_BGR;
		break;
		
    case 4:
		jcpJpeg.JPGColor    = IJL_YCBCRA_FPX;
		jcpJpeg.DIBChannels = 4;
		jcpJpeg.DIBColor    = IJL_RGBA_FPX;
		break;
		
    default:
		// This catches everything else, but no
		// color twist will be performed by the IJL.
		jcpJpeg.DIBColor = (IJL_COLOR)IJL_OTHER;
		jcpJpeg.JPGColor = (IJL_COLOR)IJL_OTHER;
		jcpJpeg.DIBChannels = jcpJpeg.JPGChannels;
		break;
    }

	
	if(area.xPos < 0 || (area.xPos+area.nWidth) > jcpJpeg.JPGWidth ||
		area.yPos  < 0 || (area.yPos+area.nHeight) > jcpJpeg.JPGHeight)
	{
		return JPG_RECT_ERROR;
	}


	jcpJpeg.DIBHeight		= area.nHeight;
	jcpJpeg.DIBWidth		= area.nWidth;
	jcpJpeg.DIBPadBytes		= IJL_DIB_PAD_BYTES(jcpJpeg.DIBWidth,
		jcpJpeg.DIBChannels);
	
	jcpJpeg.jprops.roi.left		= area.xPos;
	jcpJpeg.jprops.roi.right	= area.xPos+area.nWidth;
	jcpJpeg.jprops.roi.top		= area.yPos;
	jcpJpeg.jprops.roi.bottom	= area.yPos+area.nHeight;

	nLenBData = (jcpJpeg.DIBWidth*jcpJpeg.DIBChannels + jcpJpeg.DIBPadBytes)*
		jcpJpeg.DIBHeight;
	
	
	if(!*pBmpData)
	{
		*pBmpData=(BYTE*)malloc(nLenBData);

		if(!*pBmpData)
		{
			return JPG_NOMONEY_ERROR;
		}

		memset((void*)(*pBmpData),0,nLenBData);
	}
	
    jcpJpeg.DIBBytes = *pBmpData;
	
	jerr = ijlRead( &jcpJpeg, IJL_JBUFF_READENTROPY );
	
	if (jerr != IJL_OK && jerr != IJL_ROI_OK)
	{
		return JPG_DECODE_ERROR;
	}
	
	jerr = ijlFree( &jcpJpeg );
	
    if( jerr != IJL_OK )
    {
		return JPG_DECODE_ERROR;
    }

	return JPG_OK;
}


int jpeg_free(BYTE* pBmpData)
{
	if (pBmpData)
	{
		free(pBmpData);
		pBmpData=NULL;
	}
	
	return JPG_OK;
}

int jpeg_get(BYTE* pJpegData,SLONG nLenJData,JPEGINFO &jpeginfo)
{
	JPEG_CORE_PROPERTIES  jcpJpeg;
	DWORD                 jerr;
	
	if(!pJpegData)
	{
		return JPG_MEMORYPOINTER_ERROR;
	}
	
	jerr = ijlInit( &jcpJpeg );
	
	if( jerr != IJL_OK )
	{
		return JPG_DECODE_ERROR;
	}
	
	jcpJpeg.JPGFile			= NULL;
	jcpJpeg.JPGBytes		= pJpegData;
	jcpJpeg.JPGSizeBytes	= nLenJData;
	
	jerr = ijlRead( &jcpJpeg, IJL_JBUFF_READPARAMS );
	
    if( jerr != IJL_OK )
    {
		return JPG_DECODE_ERROR;
    }
	
    jpeginfo.nHeight=jcpJpeg.JPGHeight;
	jpeginfo.nWidth=jcpJpeg.JPGWidth;
	jpeginfo.nJPGChannels=jcpJpeg.JPGChannels;

	jerr = ijlFree( &jcpJpeg );
	
    if( jerr != IJL_OK )
    {
		return JPG_DECODE_ERROR;
    }
	
	return JPG_OK;
}