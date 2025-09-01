/*
**    VTGA.H
**    TGA file functions header.
*/
#ifndef _VTGA_H_INCLUDE_
#define _VTGA_H_INCLUDE_      1

#pragma pack(push)
#pragma pack(1)
typedef struct tagTGA_HEAD
{
   UCHR  bIdSize;
   UCHR  bColorMapType;
   UCHR  bImageType;
   UHINT iColorMapStart;
   UHINT iColorMapLength;
   UCHR  bColorMapBits;

   UHINT ixStart;
   UHINT iyStart;
   UHINT iWidth;
   UHINT iHeight;

   UCHR  bBitsPerPixel;
   UCHR  bDescriptor;
} TGA_HEAD,*LPTGA_HEAD;;
#pragma pack(pop)


//----------------------------------------------------------------------------------------------------------------
//�\�@��G  Ū�JTGA���v����ƨ줺�s��
//�ѡ@�ơG  USTR    *filename;          //TGA���W
//          SLONG   *image_width;       //�s��TGA�v���e�ת��ƾڦa�}
//          SLONG   *image_height;      //�s��TGA�v�����ת��ƾڦa�}
//          USTR    **image_buffer;     //�s��TGA�v��RGB��ƪ����s�w�İ�
//          USTR    **alpha_buffer;     //�s��TGA�v��ALPHA��ƪ����s�w�İ�
//��^�ȡG  �p�GŪ�����\�A��^ TTN_OK�F
//          �_�h�A��^ TTN_ERROR�C
//���@���G  (1) �v��RGB��Ʀbimage_buffer�����s�����R,G,B�����ǡC
//          (2) �p�GTGA�����tALPHA��T�A��o��alpha_buffer���C�Ӽƾڳ��N��255�C
//          (3) ����ƱN�۰ʤ��t���s��image_buffer�Malpha_buffer�A�ҥH�A�ڭ̦b�ϥΧ�image_buffer�Malpha_buffer��A
//              ���������Ӥ��s�C
//----------------------------------------------------------------------------------------------------------------
SLONG   TGA_load_file_image(USTR *filename, SLONG *image_width, SLONG *image_height, 
                            USTR **image_buffer, USTR **alpha_buffer);



#endif//_VTGA_H_INCLUDE_

