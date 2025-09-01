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
#define MAX_RAIN_POINT		4000					// ������ֵ
#define AUTO_CHANGE_WIND_DELAY		1000*60*5		// �Զ���������
#define MAX_CLOUD			10						// �����ֵ

#define WEATHER_REFRESH_TIME		20			

#define WIND_RIGHT			0						// ����
#define WIND_RIGHT_DOWN		1
#define WIND_DOWN			2
#define WIND_LEFT_DOWN		3
#define WIND_LEFT			4
#define WIND_LEFT_UP		5
#define WIND_UP				6
#define WIND_UP_RIGHT		7

#define WIND_NONE			0   // �޷�
#define WIND_SMALL			1	// С��
#define WIND_MIDDLE			2	// �з�
#define WIND_LARGE			3   // ���
#define WIND_DUMMY          4	// ��������

// ===== ������������
#define WEATHER_NORMAL		0x00000000          // ����
#define WEATHER_RAIN		0x00000001          // ����
#define WEATHER_SNOW		0x00000002			// ��ѩ
#define WEATHER_CLOUD		0x00000004  		// �����
#define WEATHER_THUNDER		0x00000008  		// ����
#define WEATHER_DUMMY       0x80000000          // ��������


#define WEATHER_RAIN_STOP		0xFFFE			// ֹͣ����
#define WEATHER_SNOW_STOP		0xFFFD			// ֹͣ��ѩ
#define WEATHER_CLOUD_STOP		0xFFFB			// ֹͣ����
#define WEATHER_THUNDER_STOP	0xFFF7			// ֹͣ����

// ===== ���ſ�������
#define WEATHER_CLEAR		0					// ���
#define WEATHER_STARTUP		1					// ��ʼ
#define WEATHER_STOP		2					// ������

// ===== ������������
#define WEATHER_SMALL		0					// С �b�Ƶ����   ����
#define WEATHER_MIDDLE		1					// ��			   ����
#define WEATHER_LARGE		2					// ��			   ����



// S T R U C T U R E S /////////////////////////////////////////////////////////////////////////////////////////////
typedef struct WEATHER_PARAMETER_STRUCT
{
	SLONG wind_dir;								// ����
	SLONG rain_wind_range;						// �����С(��)
	SLONG snow_wind_range;						// �����С(ѩ)
	SLONG thunder_wind_range;					// �����С(��)
	SLONG status;								// ������������

	SLONG rain_command;							// ���겥������
	SLONG rain_range;							// �����С

	SLONG snow_command;							// ��ѩ����
	SLONG snow_range;							// ��ѩ��С

	SLONG thunder_command;						// ��������
	SLONG thunder_range;						// ���״�С

	SLONG cloud_command;						// ��������
	SLONG cloud_range;							// �����С

	SLONG x_offset;								// ���ٵ� offset
	SLONG y_offset;
	SLONG rain_process_count;					// ���� ���� count
	SLONG snow_process_count;					// ��ѩ ���� count
	SLONG thunder_process_count;				// ���� ���� count
	SLONG cloud_process_count;					// ���� ���� count

} WEATHER_PARAMETER ;


typedef struct WEATHER_RAIN_DATA_STRUCT
{
	SLONG active;								// ��� flag  
												//			0 -> disable
												//			1 -> in sky 
												//			2 -> in ground
	SLONG x;									// �������
	SLONG y;
	SLONG height;								// �߶�

} WEATHER_RAIN_DATA ;


typedef struct WEATHER_SNOW_DATA_STRUCT
{
	SLONG active;								// active flag  
												//			0 -> disable
												//			1 -> in sky 
												//			2 -> in ground
	SLONG x;									// ��������
	SLONG y;
	SLONG height;								// �߶�
	SLONG size;									// ��С

} WEATHER_SNOW_DATA ;


typedef struct WEATHER_CLOUD_DATA_STRUCT
{
	SLONG active;								// active 
												//			0 -> disable
												//			1 -> startup 
												//			2 -> show
												//          3 -> close
	SLONG x;									// ����
	SLONG y;
	SLONG timer;
	SLONG type;									// ����

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
