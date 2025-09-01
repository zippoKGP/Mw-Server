#ifndef _JPG_H_INCLUDE_
#define _JPG_H_INCLUDE_       1

#include "ijl.h"
//////////////////////////////////////////////////////////////////////////

enum { JPG_OK                   = 0,     //操作成功
	   JPG_DECODE_ERROR         = 1,     //解码错误
	   JPG_RECT_ERROR           = 2,     //传入的要解码的区域太大
	   JPG_MEMORYPOINTER_ERROR  = 3,     //传入的内存指针错误(pJpegData 不为空，pBmpData 为空)
	   JPG_NOMONEY_ERROR        = 4,     //分配给pBmpData内存出错
};

typedef struct  tagJPEG_INFO{
	SLONG  nWidth;        //图片宽度
	SLONG  nHeight;       //图片高度
	SLONG  nJPGChannels;  //每像数的BYTE数  3
}JPEGINFO;

typedef struct tagJPEG_AREA{
	SLONG  xPos;        //顶点X位置
	SLONG  yPos;        //顶点Y位置
	SLONG  nWidth;      //宽度
	SLONG  nHeight;     //高度
}JPEGAREA;

int jpeg_read(BYTE* pJpegData,SLONG nLenJData,JPEGAREA& area,BYTE** pBmpData,SLONG& nLenBData);  //解码JPEG
 /*  参数：
  *	in	pJpegData        要解码的JPEG完整数据  
  *	in  nLenJData        pJpegData的长度        
  *	in  rect             要解码的区域        left,top 为顶点  right,button为宽、高
  * out *pBmpData        解码后的位图数据       必须是NULL值
  *	out	nLenBData        解码后的位图数据长度   
  *
  */

int jpeg_free(BYTE* pBmpData);   //释放内存


int jpeg_get(BYTE* pJpegData,SLONG nLenJData,JPEGINFO &jpeginfo);    //得到JPEG的数据
 
#endif