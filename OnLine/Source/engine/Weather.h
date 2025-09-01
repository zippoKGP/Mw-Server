/*
**	Weather.h
**  weather system.
**  
**  stephen, 2004.12.14.
*/
#ifndef _WEATHER_H_
#define _WEATHER_H_


// D E F I N E S ///////////////////////////////////////////////////////////////////////////////////////////////////
// --------- Weather System Define area
#define MAX_RAIN_POINT		4000					// 雨点最大值
#define AUTO_CHANGE_WIND_DELAY		1000*60*5		// 自动更换风向
#define MAX_CLOUD			10						// 云最大值

#define WEATHER_REFRESH_TIME		20			

#define WIND_RIGHT			0						// 风向
#define WIND_RIGHT_DOWN		1
#define WIND_DOWN			2
#define WIND_LEFT_DOWN		3
#define WIND_LEFT			4
#define WIND_LEFT_UP		5
#define WIND_UP				6
#define WIND_UP_RIGHT		7

#define WIND_NONE			0   // 无风
#define WIND_SMALL			1	// 小风
#define WIND_MIDDLE			2	// 中风
#define WIND_LARGE			3   // 大风
#define WIND_DUMMY          4	// 不做控制

// ===== 天气控制命令
#define WEATHER_NORMAL		0x00000000          // 晴天
#define WEATHER_RAIN		0x00000001          // 下雨
#define WEATHER_SNOW		0x00000002			// 下雪
#define WEATHER_CLOUD		0x00000004  		// 雾或云
#define WEATHER_THUNDER		0x00000008  		// 打雷
#define WEATHER_DUMMY       0x80000000          // 不做控制


#define WEATHER_RAIN_STOP		0xFFFE			// 停止下雨
#define WEATHER_SNOW_STOP		0xFFFD			// 停止下雪
#define WEATHER_CLOUD_STOP		0xFFFB			// 停止云雾
#define WEATHER_THUNDER_STOP	0xFFF7			// 停止打雷

// ===== 播放控制命令
#define WEATHER_CLEAR		0					// 清除
#define WEATHER_STARTUP		1					// 开始
#define WEATHER_STOP		2					// 结束

// ===== 数量控制命令
#define WEATHER_SMALL		0					// 小 云的情况   灰云
#define WEATHER_MIDDLE		1					// 中			   黑云
#define WEATHER_LARGE		2					// 大			   白云



// S T R U C T U R E S /////////////////////////////////////////////////////////////////////////////////////////////
typedef struct WEATHER_PARAMETER_STRUCT
{
	SLONG wind_dir;								// 风向
	SLONG rain_wind_range;						// 风向大小(雨)
	SLONG snow_wind_range;						// 风向大小(雪)
	SLONG thunder_wind_range;					// 风向大小(雷)
	SLONG status;								// 天气控制命令

	SLONG rain_command;							// 下雨播放命令
	SLONG rain_range;							// 下雨大小

	SLONG snow_command;							// 下雪命令
	SLONG snow_range;							// 下雪大小

	SLONG thunder_command;						// 打雷命令
	SLONG thunder_range;						// 打雷大小

	SLONG cloud_command;						// 云雾命令
	SLONG cloud_range;							// 云雾大小

	SLONG x_offset;								// 风速的 offset
	SLONG y_offset;
	SLONG rain_process_count;					// 下雨 处理 count
	SLONG snow_process_count;					// 下雪 处理 count
	SLONG thunder_process_count;				// 打雷 处理 count
	SLONG cloud_process_count;					// 云雾 处理 count

} WEATHER_PARAMETER ;


typedef struct WEATHER_RAIN_DATA_STRUCT
{
	SLONG active;								// 雨滴 flag  
												//			0 -> disable
												//			1 -> in sky 
												//			2 -> in ground
	SLONG x;									// 落地坐标
	SLONG y;
	SLONG height;								// 高度

} WEATHER_RAIN_DATA ;


typedef struct WEATHER_SNOW_DATA_STRUCT
{
	SLONG active;								// active flag  
												//			0 -> disable
												//			1 -> in sky 
												//			2 -> in ground
	SLONG x;									// 地面坐标
	SLONG y;
	SLONG height;								// 高度
	SLONG size;									// 大小

} WEATHER_SNOW_DATA ;


typedef struct WEATHER_CLOUD_DATA_STRUCT
{
	SLONG active;								// active 
												//			0 -> disable
												//			1 -> startup 
												//			2 -> show
												//          3 -> close
	SLONG x;									// 坐标
	SLONG y;
	SLONG timer;
	SLONG type;									// 种类

} WEATHER_CLOUD_DATA;


// F U N C T I O N S ///////////////////////////////////////////////////////////////////////////////////////////////
// ----------- subtinue define area ------------------------------------------------
int init_weather(void);
void free_weather(void);
void active_weather(int flag);
void refresh_weather(void);
void control_wind(SLONG wind_dir,SLONG wind_range);
void control_weather(SLONG weather_type,SLONG weather_command,SLONG weather_range);
void exec_weather(BMP *bitmap);
void set_weather_type(SLONG type);
void get_weather_report(UCHR *report_text);
void get_weather_sample_report(UCHR *report_text);
SLONG get_weather_icon_no(void);


#endif//_WEATHER_H_
