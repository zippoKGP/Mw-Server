
#if !defined(AFX_MAP_H_INCLUDED_)
#define AFX_MAP_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//ͷ���
#define  CHECK_MAP_HEAD		"MAP_HEAD"
#define  CHECK_MAP_IMG		"MAP_IMG"
#define  CHECK_IMG_HEAD		"IMG_HEAD"
#define  CHECK_EVENT		"EVENT"
#define  CHECK_CONTROL		"CONTROL"

#define  FILE_HEAD_NULL		256

//��������
#define BITBAR                  0x8000
#define BITEVENT                0x4000
#define BITPATH                 0x2000
#define BITSHADE                0x1000
#define BITLINK                 0x0200
#define BITSOUND                0x0100

enum
{
	IMG_LAYER_TOP  = 0xc0,		//ͼƬ����
	IMG_LAYER_BOTTOM = 0x80,
	IMG_LAYER_MIDDLE = 0x40,
	IMG_LAYER_CUT = 0x20,

	CELL_WIDTH = 16,			//���ӿ��
	CELL_HEIGHT = 8,

	MAP_ANGEL = 45,				//��ͼ�Ƕ�

	CUT_MAX_WIDTH = 64,			//�и���

	MAP_VERSION = 10001,		//�ļ��汾
};

typedef struct MAP_HEAD_STRUCT
{
	UCHR copyright[19];             // 'MAP_HEAD' �n���f��, �z��n�������
	UCHR text_end_code;             // 0x1A ���֙n�Y���a
	UCHR map_info[20];				//��ͼ˵��
	SLONG cell_width;               // Cell size
	SLONG cell_height;              //
	SLONG map_width;                // �؈D�� Pixel
	SLONG map_height;               // �؈D�� Pixel
	SLONG map_angel;                // �؈D�Ƕ�

	SLONG offset_img;				//ͼƬƫ��
	SLONG offset_event;				//��ͼ����ƫ��
	SLONG offset_control;			//��Ч����ƫ��
	SLONG version_number; 			//�汾��		
		
} MAP_HEAD;

typedef struct MAP_IMG_HEAD_STRUCT
{
    UCHR copyright[19];          // 'MAP_IMG' �n���f��, �z��n�������
    UCHR text_end_code;          // 0x1A ���֙n�Y���a
	SLONG total_img;              	// ��������ͼƬ	
	SLONG img_date_offset;		//ͼƬ�б�ƫ��
	
}MAP_IMG_HEAD;

typedef struct IMG_HEAD_STRUCT
{
	UCHR copyright[19];          // 'IMG_HEAD' �n���f��, �z��n�������
	UCHR text_end_code;          // 0x1A ���֙n�Y���a
	SLONG offset_img;			 //ͼƬƫ��
	SLONG size;					 //ͼƬ�ߴ�
	SLONG xlen;					 //��
	SLONG ylen;					 //��
	SLONG orig_x;                // ԭʼ����λ��
	SLONG orig_y;
	SHINT center_x;              // �����c����
	SHINT center_y;
	UCHR type;					 //ͼƬ�����磺��ͼ���м�㣬�ײ㣬�ϲ㼰�ò�ڼ���ͼ
}IMG_HEAD;



typedef struct EVENT_HEAD_STRUCT
{
	UCHR  copyright[19];             // 'EVENT' ����˵��, ��鵵��������
	UCHR  text_end_code;             // 0x1A ���ֵ�������
	SLONG total_cell;                // �ܹ�����CELL
	SLONG event_offset;              // event ƫ����
} EVENT_HEAD;


typedef struct CONTROL_HEAD_STRUCT
{
	UCHR  copyright[19];             // 'CONTROL' ����˵��, ��鵵��������
	UCHR  text_end_code;             // 0x1A ���ֵ�������
	SLONG total_effect;              // �ܹ����ٸ���Ч
	SLONG total_link;                // �ܹ����ٸ���ͼ���ӵ�
	SLONG effect_offset;             // ��Ч ƫ����
	SLONG link_offset;               // ��ͼ����� ƫ����
} CONTROL_HEAD;

typedef struct MAP_EFFECT_STRUCT
{
	UCHR  filename[80];             // ��Ч����������(����·��)
	SLONG x;                        // ��Ч��������
	SLONG y;
	SLONG times;                    // ���Ŵ���  0 -> ��Ч, 1 -> MDA
	SLONG delay;                    // �м���  0 -> ����  1 -> HightLight
} MAP_EFFECT;


typedef struct MAP_LINK_STRUCT
{
	UCHR  map_filename[80];         // ���ӵ�ͼ����(����·��)
	SLONG event_no;                 // �����¼����
	SLONG start_x;                  // ��������������
	SLONG start_y;
	SLONG end_x;                        
	SLONG end_y;
	UCHR  intro[120];                // ��ͼ˵��
	SLONG src_x;                     // Show ��λ��
	SLONG src_y;                     // 
} MAP_LINK;


#endif