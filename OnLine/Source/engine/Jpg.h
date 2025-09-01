#ifndef _JPG_H_INCLUDE_
#define _JPG_H_INCLUDE_       1

#include "ijl.h"
//////////////////////////////////////////////////////////////////////////

enum { JPG_OK                   = 0,     //�����ɹ�
	   JPG_DECODE_ERROR         = 1,     //�������
	   JPG_RECT_ERROR           = 2,     //�����Ҫ���������̫��
	   JPG_MEMORYPOINTER_ERROR  = 3,     //������ڴ�ָ�����(pJpegData ��Ϊ�գ�pBmpData Ϊ��)
	   JPG_NOMONEY_ERROR        = 4,     //�����pBmpData�ڴ����
};

typedef struct  tagJPEG_INFO{
	SLONG  nWidth;        //ͼƬ���
	SLONG  nHeight;       //ͼƬ�߶�
	SLONG  nJPGChannels;  //ÿ������BYTE��  3
}JPEGINFO;

typedef struct tagJPEG_AREA{
	SLONG  xPos;        //����Xλ��
	SLONG  yPos;        //����Yλ��
	SLONG  nWidth;      //���
	SLONG  nHeight;     //�߶�
}JPEGAREA;

int jpeg_read(BYTE* pJpegData,SLONG nLenJData,JPEGAREA& area,BYTE** pBmpData,SLONG& nLenBData);  //����JPEG
 /*  ������
  *	in	pJpegData        Ҫ�����JPEG��������  
  *	in  nLenJData        pJpegData�ĳ���        
  *	in  rect             Ҫ���������        left,top Ϊ����  right,buttonΪ����
  * out *pBmpData        ������λͼ����       ������NULLֵ
  *	out	nLenBData        ������λͼ���ݳ���   
  *
  */

int jpeg_free(BYTE* pBmpData);   //�ͷ��ڴ�


int jpeg_get(BYTE* pJpegData,SLONG nLenJData,JPEGINFO &jpeginfo);    //�õ�JPEG������
 
#endif