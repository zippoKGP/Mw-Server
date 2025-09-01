/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : utility.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#ifndef _UTILITY_H_
#define _UTILITY_H_

#include "idoorserver.h"
#include "tier0/dbg.h"
#include "ichat.h"
#include "player_info.h"
#include "jpg.h"
#include <time.h>
#include "findpath.h"


#define CURSO_MDA_FILE			"MDA\\point.mda"

// ---------- Define Ani command type
#define MAX_COMMAND_LENGTH			10240
#define MAX_MDA_STRING				20
#define ANI_MAIN_COMMAND			1
#define ANI_DATABASE				2
#define ANI_EFFECT1					3
#define ANI_EFFECT2					4

// ---------- Define Media type
#define FILE_TYPE_UNKNOW			0
#define FILE_TYPE_WAV				1
#define FILE_TYPE_IMG				2

// --------- Define MDA command
#define MDA_COMMAND_ZOOM			0x0001
#define MDA_COMMAND_NEXT_FRAME		0x0002
#define MDA_COMMAND_FIRST			0x0004
#define MDA_COMMAND_LAST			0x0008
#define MDA_COMMAND_LOOP			0x0010
#define MDA_COMMAND_ALPHA			0x0020
#define MDA_COMMAND_HIGHTLIGHT		0x0040
#define MDA_COMMAND_ASSIGN          0x0080
#define MDA_COMMAND_PALETTE			0x0100				// use system_palette
#define MDA_COMMAND_PALETTE_OR		0x0200				// use palette and system_palette OR
#define MDA_COMMAND_PALETTE_AND		0x0400				// use palette and system_palette AND
#define MDA_COMMAND_PALETTE_SUB		0x0800				// 降下亮度
#define MDA_COMMAND_PALETTE_ADD		0x1000				// 提高亮度
#define MDA_COMMAND_PALETTE_GRAY	0x2000				// 变成 gray


#define MDA_MASK_NEXT_FRAME         0xFFFD

// --------- Define MDA return code
#define MDA_EXEC_ERROR				0
#define MDA_EXEC_OK					1
#define MDA_EXEC_FIRST				2
#define MDA_EXEC_LAST				3



#define NPC_PATH_MOVE_SPEED				30
#define MAP_MOVE_SPEED					30


#define NPC_CELL_MOVE_SPEED				100


#define NPC_STAGE_NONE					0
#define NPC_STAGE_WALK					1
#define NPC_STAGE_IDLE					2

// ---------- Define MAP CONTROL Command
#define MAP_MOVE_UP						NPC_DIR_UP
#define MAP_MOVE_DOWN					NPC_DIR_DOWN
#define MAP_MOVE_LEFT					NPC_DIR_LEFT
#define MAP_MOVE_RIGHT					NPC_DIR_RIGHT

#define MAP_MAX_BOTTOM_BUFFER			9		// Bottom buffer 最大数量

#define MAP_MAX_LAYER_TOUCH				16		// 同一个NPC 最大接触最大数量

#define MAP_EFFECT_WAV					0
#define MAP_EFFECT_IMG					1

#define MAP_EFFECT_X_RANGE				1200	// 1440000 + 810000
#define MAP_EFFECT_Y_RANGE				900
#define MAP_EFFECT_OFFSET				1500 //  sqrt(1200*1200+900*900)

// ---------- Define NPC CONTROL Command
#define NPC_CHARACTER_TYPE				0
#define NPC_CPU_TYPE					1
#define NPC_BAOBAO_TYPE					2

#define NPC_MAX_DIR						8		// NPC 最大方向



// --
#define SORT_TYPE_NPC					0
#define SORT_TYPE_MDA					1
#define SORT_TYPE_EXTRA					2
#define SORT_TYPE_FRAME					3


#define EXTRA_BOTTOM_LAYER				0
#define EXTRA_MIDDLE_LAYER				1
#define EXTRA_TOP_LAYER					2

#define STAND_MOVE_STEP					3				// pixel

#define NPC_IDLE_BASE					15000

#define NPC_MASK_ALPHA					80


#define MAP_VOICE_VOLUME_MAX			0
#define MAP_VOICE_VOLUME_MIN			-3000


#define MAX_MESSAGE_STACK				100



#define CHAT_WINDOW_WIDTH				400





/***************************************************************************************}
{	Voice struct																		}
****************************************************************************************/
typedef struct VOICE_CONTROL_STRUCT
{
	SLONG busy[VOICE_CHANNEL_MAX];
	SLONG loop[VOICE_CHANNEL_MAX];
	ULONG play_time[VOICE_CHANNEL_MAX];
	SLONG voice_type[VOICE_CHANNEL_MAX];			// Voice file type 0 -> file, 1 -> buffer
} VOICE_CONTROL;


/***************************************************************************************}
{	IMG256 struct																		}
****************************************************************************************/
typedef struct IMG256_HEAD_STRUCT
{
	UCHR copyright[19];         // 'RAYS IMG256 FILE' 
	UCHR text_end_code;         // 0x1A
	UHINT xlen;					// img256 xlen 
	UHINT ylen;					// img256 ylen
	UHINT orig_x;               // screen orig position
	UHINT orig_y;
	SHINT center_x;             // 中心点坐标
	SHINT center_y;
	SLONG image_compress_size;	// Image compress size
	UCHR palette[768];			// 256 palette
	SLONG img_start_offset;		// Image start offset
} IMG256_HEAD;


/***************************************************************************************}
{	MDA struct																			}
****************************************************************************************/
typedef struct MDA_HEAD_STRUCT
{
	UCHR copyright[19];          // 'RAYS MEDIA FILE' 
	UCHR text_end_code;          // 0x1A
	SLONG version;               // version 100
	SLONG total_database_frame;  // total database
	SLONG total_ani_frame;       // total ani 
	SLONG data_start_offset;     // database start offset
	SLONG ani_start_offset;      // ani command start offset
} MDA_HEAD;


typedef struct OFFSET_SIZE_STRUCT
{
	SLONG offset;
	SLONG size;
} OFFSET_SIZE;


typedef struct INDEX_OFFSET_SIZE_STRUCT
{
	UCHR name[32];
	SLONG offset;
	SLONG size;
} INDEX_OFFSET_SIZE;


typedef struct MDA_STRING_STRUCT
{
	SLONG idx;
	UCHR text[MAX_MDA_STRING];		// Max
} MDA_STRING;


typedef struct MDA_GROUP_STRUCT
{
	ULONG handle;					// MDA_GROUP handle
	UCHR filename[256];				// mda filename 
	SLONG total_database;			// total database frame
	SLONG total_ani_command;		// total ani command 
	SLONG database_start_offset;	// database start offset
	SLONG command_start_offset;		// ani command start offset
//	UCHR main_command_text[MAX_COMMAND_LENGTH];
//	UCHR database_command_text[MAX_COMMAND_LENGTH];
//	UCHR effect_command_text[MAX_COMMAND_LENGTH];

	SLONG total_main_text;
	MDA_STRING *main_command_text;
	SLONG total_database_text;
	MDA_STRING *database_command_text;
	SLONG total_effect_text;
	MDA_STRING *effect_command_text;
	
	IMG256_HEAD *img256_head;
	// --- Point area
	SLONG *now_frame;				// Ani command now Frame 
	SLONG *total_frame;				// total Ani command Frame
	INDEX_OFFSET_SIZE *database_index;
	UCHR *body;						// MDA body
} MDA_GROUP;


typedef struct IMG256_GROUP_STRUCT
{
	UCHR *image;
	RECT rect;
} IMG256_GROUP;


#define MAX_EXTRA_MDA			50			// 最大数量 EXTRA MDA
#define MAX_EXTRA_FIGHT_MDA		20			// 最大数量 EXTRA_FIGHT_MDA

typedef struct EXTRA_MDA_DATA_STRUCT
{
	SLONG handle;				// MDA handle
	SLONG mda_index;			// mda index
	SLONG display_x;			// 实际坐标 
	SLONG display_y;
	SLONG xl;					// 大小
	SLONG yl;
	SLONG center_x;				// 中心点
	SLONG center_y;
	bool display;				// 是否 display
	bool loop;					// 是否 Loop 播放
	bool play_end;				// 是否已经到最后1祯
	bool auto_close;			// 是否到最后1祯自动关闭(loop 为 false) 或者更换地图关闭
	bool hight_light_flag;		// 是否需要 HightLight
	SLONG level;				// 等级 0 -> 底层  1-> 中间层  2-> 最上层
	UCHR command[256];			// 播放命令
	ULONG timer;				// 播放时间控制
} EXTRA_MDA_DATA;


typedef struct BUTTON_STATUS_STRUCT
{
	SLONG command_no;					// MDA command_no
	RECT rect;							// 按钮区域
	SLONG ret_code;						// 选中的代码
	UCHR inst[17];						// 按钮说明
	
} BUTTON_STATUS;



/***************************************************************************************}
{	MAP struct																			}
****************************************************************************************/
typedef struct MAP_LAYER_DATA_STRUCT
{
	SLONG size;						//图片尺寸
	SLONG xlen;						//长
	SLONG ylen;						//宽
	SLONG orig_x;					// 原始座標位置
	SLONG orig_y;
	SLONG center_x;					// 重心點座標
	SLONG center_y;
	UCHR type;						//图片类型如：底图，中间层，底层，上层及该层第几张图
	UCHR *image;
	
} MAP_LAYER_DATA;

typedef struct MAP_EVENT_DATA_STRUCT
{
	SLONG max_cell_x;
	SLONG max_cell_y;
	SLONG total_cell;
	UHINT *event_data;
} MAP_EVENT_DATA;

typedef struct MAP_LINK_DATA_STRUCT
{
	UCHR  map_filename[80];         // 连接地图名称(不含路径)
	SLONG event_no;                 // 启动事件编号
	SLONG start_x;                  // 进入後座标区域 (CELL 坐标)
	SLONG start_y;
	SLONG end_x;                        
	SLONG end_y;
	UCHR  intro[120];                // 地图说明
	SLONG src_x;                     // Show 字位置 (CELL 坐标)
	SLONG src_y;                     // 
	
} MAP_LINK_DATA;


typedef struct MAP_EFFECT_DATA_STRUCT
{
	UCHR  filename[80];             // 音效档档案名称(不含路径)
	SLONG x;                        // 音效发声座标 (Pixel 坐标)
	SLONG y;
	SLONG times;                    // 拨放次数 0 -> WAV  1 -> MDA
	SLONG delay;                    // 中间间隔 0 -> Normal  1 -> HightLight
// ----- MDA Control
	SLONG handle;					// IMG handle 
	SLONG img_xl;					// IMG Data 
	SLONG img_yl;
	ULONG idle_timer;
	SLONG now_frame;			
	SLONG total_frame;
// ---- WAV Control
	SLONG channel;					// 播放 wav 的通道
	SLONG wav_active;				// 目前是否已经播放
	ULONG wav_idle_timer;			// 检查 timer

} MAP_EFFECT_DATA;


typedef struct MAP_BOTTOM_IMAGE_STRUCT
{
	RECT image_rect;				// 原地图坐标
	BMP *image;						// Image data
} MAP_BOTTOM_IMAG;


typedef struct MAP_BASE_DATA_STRUCT
{
// ----- 地图档案资料 ----
	UCHR map_filename[256];			// 地图档案名称
	UCHR map_info[20];				// 地图档案说明
	SLONG cell_width;               // Cell size
	SLONG cell_height;              //
	SLONG map_width;                // 地圖寬 Pixel
	SLONG map_height;               // 地圖高 Pixel
	SLONG map_angel;                // 地圖角度
	SLONG total_map_layer;			// 总共多少 layer
	MAP_LAYER_DATA *map_layer_data; // Layer 资料	
	MAP_EVENT_DATA map_event_data;	// event data 
	SLONG total_map_link;			// 总共多少地图连接
	MAP_LINK_DATA *map_link_data;	// 地图连接资料
	SLONG total_map_effect;			// 总共多少地图音效
	MAP_EFFECT_DATA *map_effect_data; // 地图音效资料
	
// ----- Redraw Map 资料
	MAP_BOTTOM_IMAG map_bottom_image[MAP_MAX_BOTTOM_BUFFER];	// 地图 bottom 缓冲
	
} MAP_BASE_DATA;


typedef struct MAP_CONTROL_STRUCT
{
	SLONG start_x;					// 地图左上角坐标
	SLONG start_y;
	SLONG target_x;					// 地图移动目的地坐标
	SLONG target_y;					// 呼叫 Redraw map 自动计算
// -------------
	BMP *screen_mask;				// screen mask 用
	bool draw_mask;					// 上图时候是否需要 draw_mask
	UHINT mask_npc;					// 目前上 mask 的 NPC 编号 
	bool draw_point_color;			// 目前 NPC 使用 指定画法
// -------------
	UHINT character_title_color;	// 玩家称谓颜色
	UHINT character_name_color;		// 玩家姓名颜色
	UHINT npc_name_color;			// NPC 姓名颜色
	UHINT point_color;				// 指定的颜色
	UHINT fight_npc_color;			// 战斗NPC&char 颜色
// --------------
	SLONG point_idx;				// 点击在地面的 MDA 档案 handle

	
} MAP_CONTROL;


typedef struct POSITION_STRICT
{
	SLONG x;
	SLONG y;
} POSITION;


typedef struct MAP_LAYER_TOUCH_STRUCT
{
	BOOL flag;
	BOOL character;
	SLONG total;
	SLONG idx[MAP_MAX_LAYER_TOUCH];
} MAP_LAYER_TOUCH;


/***************************************************************************************}
{	NPC struct																			}
****************************************************************************************/
typedef struct NPC_MDA_CONTROL_STRUCT
{
	SLONG total_frame;					// 总共 frame
	SLONG now_frame;					// now control frame
} NPC_MDA_CONTROL;


typedef struct FIGHT_NPC_MDA_CONTROL_STRUCT
{
	SLONG total_frame;					// 总共 frame
	SLONG now_frame;					// now control frame
	SLONG motion;						// MainCommand no	
} FIGHT_NPC_MDA_CONTROL;

typedef struct PATH_DATA_STRUCT
{
	SHINT x;
	SHINT y;
	SHINT dir_x;
	SHINT dir_y;
} PATH_DATA;


typedef struct MAP_NPC_GROUP_STRUCT
{
	BASE_NPC_INFO npc_info;				// NPC 基本资料

	SLONG handle;						// MDA GROUP handle
	SLONG npc_type;						// NPC 种类 0 -> 主角, 1 -> NPC  2-> 宝宝
	ULONG npc_timer;					// 播放时间控制
	ULONG npc_move_timer;				// NPC 走路移动时间
	ULONG npc_idle_timer;				// NPC 闲置时间
	NPC_MDA_CONTROL npc_mda_control[NPC_MAX_DIR] ;	// frame data 
// ---------- Auto Control 
	SLONG npc_stage;					// 控制用的
	SLONG npc_status;					// 显示状态
	ULONG follow_id;					// 跟随 ID 
	SLONG follow_range;					// 跟随距离

	ULONG follow_timer;					// 自动跟随更新时间

} MAP_NPC_GROUP;



typedef struct NPC_LAYER_SORT_STRUCT
{
	SLONG type;							// 进来排序的 type
	SLONG idx;							// 目标 idx
	SLONG y;							// y 坐标 ( Pixel ) 
} NPC_LAYER_SORT;





/***************************************************************************************}
{	Tool struct																			}
****************************************************************************************/
typedef struct DEBUG_INFO_STRUCT
{
	bool map_debug;						// 地图
	bool map_stop;						// 阻挡
	bool map_event;						// 事件
	bool map_path;						// 路径
	bool map_shadow;					// 影子
	bool map_link;						// 连接点
	bool map_sound;						// 音效

} DEBUG_INFO;


typedef struct FILE_BUFFER_STRUCT
{
	ULONG file_size;
	UCHR *buffer;
} FILE_BUFFER;


typedef struct MESSAGE_STACK_STRUCT
{
	bool active;
	ULONG timer;
	SLONG x;
	SLONG y;
	SLONG xl;
	SLONG yl;
	UCHR message[1024];
	SLONG window_handle;
} MESSAGE_STACK;



typedef struct TEXT_OUT_DATA_STRUCT
{
	CChat* g_pChat;						// Body 
	POINT pt;							// mouse position 
	RECT chatRect;						// Text out window 
	CHATNET chatnet;					// 
} TEXT_OUT_DATA;



typedef struct FRIEND_DATA_RECORD_STRUCT
{
	ULONG send_id;
	ULONG receive_id;
	long time;
	UCHR nickname[MAX_NAME_LENGTH+1];
	UCHR text[MAX_CHAT_LENGTH];
	
} FRIEND_DATA_RECORD;




/*******************************************************************************************************************}
{	ANI file struct																									}
{																													}
{ (1)  Control_Ani_head																								}
{ (2)  character_view																								}
{ (3)  Layer_view																									}
{ (4)  Frame_data																									}
********************************************************************************************************************/
#define FRAME_COMMAND_NONE              0
#define FRAME_COMMAND_ATTACK            1
#define FRAME_COMMAND_HURT              2
#define FRAME_COMMAND_BACK              3
#define FRAME_COMMAND_EXEC              4
#define FRAME_COMMAND_DEFINE1           5
#define FRAME_COMMAND_DEFINE2           6
#define FRAME_COMMAND_DEFINE3           7
#define FRAME_COMMAND_DEFINE4           8

typedef struct CONTROL_ANI_HEAD_STRUCT
{
	UCHR copyright[19];				// 'RAYS CONTROL FILE' 檔案說明, 檢查檔案類型用
	UCHR text_end_code;				// 0x1A 文字檔結束碼
	SLONG control_type;				// 控制檔類型  0-> NPC 過場  1-> 戰鬥控制檔
	SLONG total_frame;				// 總共Frame 數
	SLONG play_speed;				// 速度
	SLONG total_character;			// 共有幾位主角
	SLONG total_layer;				// 共有幾曾
} CONTROL_ANI;

// --------- 主角資料 -------------
typedef struct CHAR_VIEW_DATA_STRUCT
{
	SLONG handle;					// 主角的 Handle
	SLONG mda_command;				// 主角基本mda
	SLONG zoom_value;				// 主角基本縮放比例
	SLONG alpha_value;				// 主角基本alpha 數值
	SLONG assign_value;				// 主角指定的 Frame no
	UCHR filename[256];				// 主角檔案名稱
	UCHR directory[256];			// 原始檔案儲存路徑
} CHAR_VIEW_DATA;

// ----------- Layer -----------
typedef struct LAYER_VIEW_DATA_STRUCT
{
	SLONG handle;					// 主角 Handle
	SLONG start;					// 開始 Frame  
	SLONG end;						// 結束 Frame
	SLONG mda_command;				// 主角 Mda 屬性
} LAYER_VIEW_DATA;

// ---------- Frame 资料 ---------------
typedef struct FRAME_DATA_STRUCT
{
	SLONG center_x;						// 中心點座標  X	
    SLONG center_y;						// 中心點座標  Y
    SLONG zoom;							// 鏡頭縮放比例
    SLONG effect;						// 特效或命令編號
    SLONG fade_alpha_value;				// 底層 Alpha 數值
    SLONG fade_alpha_color_r;			// 底層 Alpha 的顏色 R
    SLONG fade_alpha_color_g;			// 底層 Alpha 的顏色 G
    SLONG fade_alpha_color_b;			// 底層 Alpha 的顏色 B	
    SLONG key_frame;					// 是否為 Key Frame	
    SLONG total_handle;					// 總共幾個 Handle
    SLONG *handle;						// Handle 數組資料
    SLONG *character_frame_no;			// Frame 數組資料
    SLONG *track_no;					// Layer Track 數組資料
    SLONG *char_x;						// 中心點座標數組資料 X
    SLONG *char_y;						// 中心點座標數組資料 Y	
    SLONG *zoom_value;					// 放大縮小比例數組資料
    SLONG *alpha_value;					// Alpha 數組資料
} FRAME_DATA;


#define MAP_CONTROL_TYPE        0
#define FIGHT_CONTROL_TYPE      1
#define MAX_MDA_HANDLE          1024


typedef struct EXTRA_FRAME_DATA_STRUCT
{
	SLONG handle;				// Handle
	SLONG character_frame_no;	// character Frame no
	SLONG mda_command;			// MDA command
	SLONG char_x;				// position
	SLONG char_y;		
	SLONG zoom_value;			// zoom
	SLONG alpha_value;			// alpha 
} EXTRA_FRAME_DATA;


/***************************************************************************************}
{																						}
{	UTILITY.CPP  Protype Define															}
{																						}
****************************************************************************************/

void get_version_string(char *text,int version_no);
SLONG check_file_exist(UCHR *filename);
SLONG check_file_size(UCHR *filename);
SLONG load_jpg_file(UCHR *filename,BMP *buffer,SLONG x,SLONG y,SLONG xlen,SLONG ylen);
void bit24_to_BMP(UCHR *bit24_screen_buffer,BMP *bmp_buffer,SLONG xlen,SLONG ylen);
void bit24_to_partBMP(UCHR *bit24_screen_buffer,BMP *bmp_buffer,SLONG sx,SLONG sy,SLONG xlen,SLONG ylen,SLONG bit24_xlen,SLONG bit24_ylen);
void display_error_message(UCHR *message,bool wait_key_flag);
void display_message(UCHR *message,bool wait_key_flag);

void display_map_event(BMP *buffer);
void display_system_message(UCHR *message);
void set_text_out_window(RECT rect);



// --- ribble 
void init_ripple(void);
void free_ripple(void);
void RippleSpread(void);
void RippleRender(void);
void reset_ripple(void);

void push_ripple(SLONG x,SLONG y,SLONG value);
// --- Voice & music 
SLONG auto_play_voice(SLONG file_type,UCHR *filename,UCHR *buffer,SLONG buffer_size,SLONG loop_flag,SLONG volume,SLONG pan,SLONG *channel);
void auto_stop_voice(SLONG channel);
void auto_set_voice_control(SLONG channel,SLONG volume,SLONG pan);
SLONG play_voice_file(UCHR *filename,SLONG loop,SLONG volume,SLONG pan);
void stop_all_voice(void);
SLONG play_music_file(UCHR *filename,SLONG loop,SLONG volume);
void auto_set_music_control(SLONG volume);
void stop_music_file(void);
//--- MDA ---
SLONG mda_group_open(UCHR *filename,SLONG *handle);
void mda_group_close(SLONG *handle);
SLONG get_mda_index(ULONG mda_handle);
void free_mda_group(void);
SLONG check_is_mda_file(UCHR *filename);
SLONG add_mda_group(void);
SLONG get_total_ani_command(UCHR *body,SLONG mda_group_index);
void decode_ani_command(UCHR *body,SLONG mda_group_index);
ULONG find_next_mda_handle(void);
SLONG read_mda_frame(UCHR *mda_filename,SLONG frame_no,UCHR **buffer);
SLONG read_mda_database(UCHR *mda_filename,SLONG frame_no,UCHR **buffer,ULONG *size);

void get_img256_rect(UCHR *buffer,RECT *rect);

// --- MDA EXEC ---
SLONG mda_group_exec(SLONG handle,UCHR *command,SLONG x,SLONG y,BMP *bitmap,SLONG mda_command,SLONG value,SLONG alpha_value,SLONG frame_value);
SLONG check_media_type(UCHR *data_buffer);
SLONG check_is_wav_type(UCHR *data_buffer);
SLONG check_is_img256_type(UCHR *data_buffer);
SLONG find_database(SLONG mda_group_index,UCHR *database_text);
SLONG decode_database_str(SLONG command_no,SLONG now_frame,SLONG idx,UCHR *database_text);
SLONG decode_effect_str(SLONG command_no,SLONG now_frame,SLONG idx,UCHR *database_text);
SLONG decode_command_str(UCHR *command,SLONG idx);
void exec_user_command(UCHR *src_command,UCHR *data_buffer);
void take_space(UCHR *data);
void get_part_str(UCHR *in,UCHR *out,SLONG *idx,UCHR key,SLONG max);
void system_quit_process(void);
SLONG getServerInfoList(void);
SLONG get_mda_handle(UCHR *filename);
SLONG find_emery_message_idx(void);
SLONG is_emery_message_stack(void);
void get_mda_group_image(SLONG handle,SLONG index,UCHR **image);
void fight_charge_start(void);
void fight_charge_pause(void);
void fight_charge_continue(void);

// ----- ANI file
SLONG read_control_ani_file(UCHR *filename);
void free_control_ani_file(void);
void free_frame_data(void);



// ----- IE ----
BOOL openHttp(LPCTSTR url);

// ----- DEBUG MSG-----
SpewRetval_t MySpewFunc( SpewType_t type, char const *pMsg );
const char* format_time(long long_time);


// ----- Game ---
SLONG load_map_file(UCHR *filename,SLONG center_x,SLONG center_y);
SLONG load_map_background(SLONG start_x,SLONG start_y);
void exec_map_control(void);
void map_control_move(SLONG move_dir,SLONG move_step);
void move_map_background(SLONG move_dir,SLONG step);
void redraw_map(BMP *buffer);
SLONG check_box_intersect(SLONG dx,SLONG dy,SLONG dx1,SLONG dy1,SLONG sx,SLONG sy,SLONG sx1,SLONG sy1);
SLONG check_need_load_background_buffer(SLONG x,SLONG y);
SLONG get_destory_background_buffer(SLONG x,SLONG y);
void load_jpg_to_background_buffer(SLONG screen_x,SLONG screen_y,SLONG idx);
void process_map_task(void);
SLONG get_print_command_number(UCHR *text);


SLONG add_map_npc(BASE_NPC_INFO& add_npc);
SLONG add_map_npc_group(void);
void delete_map_npc_group(ULONG user_id);
void clear_all_map_npc(void);

SLONG get_map_npc_index(ULONG user_id);
void free_map_npc_group(void);
void check_need_del_mda_handle(SLONG map_npc_group_index);
void draw_single_npc(SLONG idx,BMP *buffer);
void draw_single_mda(SLONG idx,BMP *buffer);
void draw_single_extra_mda(SLONG idx,BMP *buffer);

void draw_npc_layer(BMP *buffer);
void clear_npc_layer_sort(void);
void sort_npc_layer(void);
void add_npc_layer_sort(SLONG type,SLONG idx,SLONG y);
SLONG npc_auto_path_move(SLONG map_npc_group_index);
SLONG npc_auto_cell_move(SLONG idx);
SLONG get_dir_data(SLONG x,SLONG y,SLONG x1,SLONG y1,SLONG move_step);
void adjust_npc_frame_data(SLONG map_npc_index);
void change_npc_motion(SLONG idx,SLONG motion);
void change_npc_next_frame(SLONG idx,SLONG now_dir);
void map_auto_move(void);
SLONG check_need_npc_mask(SLONG npc_x,SLONG npc_y,SLONG npc_width,SLONG npc_height,SLONG cc_y);
SLONG check_map_layer_touch(SLONG real_x,SLONG real_y);
SLONG check_cell_path(SLONG cell_x,SLONG cell_y);
SLONG check_cell_stop(SLONG cell_x,SLONG cell_y);
SLONG check_cell_shadow(SLONG cell_x,SLONG cell_y);
SLONG check_map_wave_active(SLONG x,SLONG y);
SLONG get_map_wave_pan(SLONG x,SLONG y);
SLONG get_map_wave_volume(SLONG x,SLONG y);
SLONG npc_move(SLONG npc_index,SLONG npc_dir);
SLONG check_same_npc_id(ULONG id);
void update_player_move(PLAYER_MOVE& pp);
void clear_screen_mask(void);
SLONG add_extra_mda(UCHR *filename,SLONG x,SLONG y,bool loop,bool auto_close,bool hight_light,SLONG level,UCHR *command);
void auto_close_extra_mda(void);
void reset_extra_mda_frame(SLONG idx);
void draw_button_extra_mda(BMP *buffer);
void draw_top_extra_mda(BMP *buffer);
void clear_all_screen_text(void);
void add_screen_stack(SLONG idx);
void delete_screen_stack(SLONG idx);
void push_chat_stack(UCHR *text);
void get_chat_stack(UCHR *text,UCHR key);
void reactive_map_effect(void);
void display_chat_inter_window(BMP *buffer);
void push_chat_data(SLONG channel,ULONG id,UCHR *nickname,UCHR *text);
void display_single_name(SLONG idx,BMP *buffer);
void scroll_chat_data(UCHR key);
void change_map(void);
void show_friend_list(SLONG x,SLONG y,SLONG function);
void clear_all_button_now_frame(void);
void scroll_friend_list(UCHR dir);
void add_friend_list(ULONG friend_id,SLONG type);
void update_friend_list(CHAT_FRIEND_LIST data);
void update_friend_list_online(ULONG friend_id,char online);
UCHR *get_nexus_string(SLONG type,SLONG num);
void change_friend_list_friend_type(ULONG id,SLONG type);
void update_friend_list_friend_type(ULONG id,SLONG type);
void delete_friend_list(ULONG friend_id);
void refresh_friend_list(CHAT_FRIEND_LIST data);
void display_hirstory_chat_inter_window(void);
void scroll_hirstory_chat_data(UCHR key);
void push_friend_data(ULONG send_id,ULONG receive_id,long time,UCHR *name,UCHR *text);
void reload_friend_data_record(ULONG id);
void scroll_friend_data_record(UCHR key);
void display_friend_data_record_window(void);
void request_team_list_data(void);
void player_leave_team(ULONG id);
void team_request_add_list(ULONG id);
void team_request_delete_list(ULONG id);
void create_small_map(void);
void touch_system_npc(SLONG player_id,SLONG npc_id);
SLONG jpg_map_read(BYTE* pJpegData, SLONG nLenJData, JPEGAREA& area,
				   BYTE** pBmpData, SLONG& nLenBData );
SLONG check_is_ani_file(UCHR *filename);


SLONG get_screen_text_idx(void);
void exec_screen_text(BMP *buffer);
void draw_message_font(SLONG x,SLONG y,SLONG xl,SLONG yl,BMP *buffer);
void draw_message_box(SLONG x,SLONG y,SLONG xl,SLONG yl,BMP *buffer);
SLONG read_mda_button(SLONG handle,SLONG command_no,BUTTON_STATUS *button,UCHR *inst,SLONG code);


SLONG read_file_to_buffer(UCHR *filename,FILE_BUFFER *file_buffer);
SLONG get_textbuffer_total_line(FILE_BUFFER *file_buffer);
void get_textbuffer_data(SLONG idx,FILE_BUFFER *file_buffer,UCHR *text);

void request_mission_data(void);
void request_base_character_data(void);
void request_login_data(void);
void request_base_baobao_data(void);
void check_baobao_stack(void);
void insert_baobao_data(SERVER_CHARACTER_BAOBAO *new_baobao);
void disable_baobao_action(void);
void enable_baobao_action(SLONG id);
void disable_baobao_show(ULONG id);
void enable_baobao_show(ULONG id);
void delete_baobao_data(ULONG baobao_id);
SLONG get_baobao_data_idx(ULONG id);
void send_character_point_adjust(void);
void send_baobao_point_adjust(ULONG baobao_id);
SLONG npc_auto_follow_move(SLONG idx);
void add_baobao_show(CLIENT_BAOBAO_SHOW baobao_data);
void request_character_fastness_data(void);
void request_baobao_fastness_data(void);
void release_baobao(void);
void change_map_npc_name(ULONG id,char *name);
void request_character_pocket_data(void);
void send_character_item_use(SLONG item_idx);
void send_character_item_drop(SLONG item_idx);
void delete_character_item_pocket(SLONG item_idx,SLONG number);
void send_character_item_change(SLONG src_idx,SLONG tar_idx);
void character_item_drop(void);
void send_item_give(void);
void send_my_deal_ready(void);
void send_my_deal_ready(void);
void send_item_deal(void);
void close_window_deal(void);
void request_deal(void);
SLONG open_window_deal(void);
void send_cancel_deal(void);
void refresh_all_map_npc(void);
void adjust_fight_npc_frame_data(SLONG index);
void send_character_equip_take_off(SLONG idx);
void set_buy_item_index(SLONG idx);
void set_sell_item_index(SLONG idx,SLONG number);
void send_store_money(SLONG money);
void send_withdraw_money(SLONG money);
void set_push_item_index(SLONG idx,SLONG number);
void request_character_popshop_data(void);
void set_pop_item_index(SLONG idx,SLONG number);
void request_detail_inst(void);
void send_fight_ready(void);
void request_character_skill_data(void);
void request_baobao_skill_data(void);
void send_baobao_item_use(SLONG baobao_id,SLONG item_idx);
void exec_character_auto_full_hp(void);
void exec_character_auto_full_mp(void);
void exec_baobao_auto_full_hp(ULONG baobao_id);
void exec_baobao_auto_full_mp(ULONG baobao_id);
void send_character_fight_command(void);
void send_baobao_fight_command(void);
void send_fight_action_finish(void);
void get_part_str1(UCHR *in,UCHR *out,SLONG *idx,UCHR key,SLONG max);



#endif
