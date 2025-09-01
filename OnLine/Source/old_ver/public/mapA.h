
#if !defined(AFX_MAP_H_INCLUDED_)
#define AFX_MAP_H_INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


//头检测
#define  CHECK_MAP_HEAD		"MAP_HEAD"
#define  CHECK_MAP_IMG		"MAP_IMG"
#define  CHECK_IMG_HEAD		"IMG_HEAD"
#define  CHECK_EVENT		"EVENT"
#define  CHECK_CONTROL		"CONTROL"

#define  FILE_HEAD_NULL		256

//格子属性
#define BITBAR                  0x8000
#define BITEVENT                0x4000
#define BITPATH                 0x2000
#define BITSHADE                0x1000
#define BITLINK                 0x0200
#define BITSOUND                0x0100

enum
{
	IMG_LAYER_TOP  = 0xc0,		//图片类型
	IMG_LAYER_BOTTOM = 0x80,
	IMG_LAYER_MIDDLE = 0x40,
	IMG_LAYER_CUT = 0x20,

	CELL_WIDTH = 16,			//格子宽度
	CELL_HEIGHT = 8,

	MAP_ANGEL = 45,				//地图角度

	CUT_MAX_WIDTH = 64,			//切割宽度

	MAP_VERSION = 10001,		//文件版本
};

typedef struct MAP_HEAD_STRUCT
{
	UCHR copyright[19];             // 'MAP_HEAD' 檔案說明, 檢查檔案類型用
	UCHR text_end_code;             // 0x1A 文字檔結束碼
	UCHR map_info[20];				//地图说明
	SLONG cell_width;               // Cell size
	SLONG cell_height;              //
	SLONG map_width;                // 地圖寬 Pixel
	SLONG map_height;               // 地圖高 Pixel
	SLONG map_angel;                // 地圖角度

	SLONG offset_img;				//图片偏移
	SLONG offset_event;				//地图属性偏移
	SLONG offset_control;			//音效数据偏移
	SLONG version_number; 			//版本号		
		
} MAP_HEAD;

typedef struct MAP_IMG_HEAD_STRUCT
{
    UCHR copyright[19];          // 'MAP_IMG' 檔案說明, 檢查檔案類型用
    UCHR text_end_code;          // 0x1A 文字檔結束碼
	SLONG total_img;              	// 總共多少图片	
	SLONG img_date_offset;		//图片列表偏移
	
}MAP_IMG_HEAD;

typedef struct IMG_HEAD_STRUCT
{
	UCHR copyright[19];          // 'IMG_HEAD' 檔案說明, 檢查檔案類型用
	UCHR text_end_code;          // 0x1A 文字檔結束碼
	SLONG offset_img;			 //图片偏移
	SLONG size;					 //图片尺寸
	SLONG xlen;					 //长
	SLONG ylen;					 //宽
	SLONG orig_x;                // 原始座標位置
	SLONG orig_y;
	SHINT center_x;              // 重心點座標
	SHINT center_y;
	UCHR type;					 //图片类型如：底图，中间层，底层，上层及该层第几张图
}IMG_HEAD;



typedef struct EVENT_HEAD_STRUCT
{
	UCHR  copyright[19];             // 'EVENT' 档案说明, 检查档案类型用
	UCHR  text_end_code;             // 0x1A 文字档结束码
	SLONG total_cell;                // 总共多少CELL
	SLONG event_offset;              // event 偏移量
} EVENT_HEAD;


typedef struct CONTROL_HEAD_STRUCT
{
	UCHR  copyright[19];             // 'CONTROL' 档案说明, 检查档案类型用
	UCHR  text_end_code;             // 0x1A 文字档结束码
	SLONG total_effect;              // 总共多少个音效
	SLONG total_link;                // 总共多少个地图连接点
	SLONG effect_offset;             // 音效 偏移量
	SLONG link_offset;               // 地图连结点 偏移量
} CONTROL_HEAD;

typedef struct MAP_EFFECT_STRUCT
{
	UCHR  filename[80];             // 音效档档案名称(不含路径)
	SLONG x;                        // 音效发声座标
	SLONG y;
	SLONG times;                    // 拨放次数  0 -> 音效, 1 -> MDA
	SLONG delay;                    // 中间间隔  0 -> 正常  1 -> HightLight
} MAP_EFFECT;


typedef struct MAP_LINK_STRUCT
{
	UCHR  map_filename[80];         // 连接地图名称(不含路径)
	SLONG event_no;                 // 启动事件编号
	SLONG start_x;                  // 进入後座标区域
	SLONG start_y;
	SLONG end_x;                        
	SLONG end_y;
	UCHR  intro[120];                // 地图说明
	SLONG src_x;                     // Show 字位置
	SLONG src_y;                     // 
} MAP_LINK;


#endif