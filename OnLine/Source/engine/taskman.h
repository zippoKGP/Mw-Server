/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : polling.h
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/1/10
******************************************************************************/ 

#ifndef _TASKMAN_H_
#define _TASKMAN_H_




#define MAX_SYSTEM_TASK				20

#define WINDOW_TYPE_NORMAL			0x00
#define WINDOW_TYPE_ON_TOP			0x01			// ÿһ�ζ�ִ��


/*******************************************************************************************************************}
{																													}
{		Struct define																								}
{																													}
********************************************************************************************************************/
// ----------------------- main task data ----------------------------------
typedef struct SYSTEM_TASK_TABLE
{
	SLONG task_id;							// ִ��ϵͳ ID
	UCHR task_name[80];						// ִ��ϵͳ���� ( Debug ��)
//	void (*task_action)();					// ִ��ϵͳ����
	SLONG (*task_action)();					// ִ��ϵͳ����
	SLONG ret_val;							// ִ��ϵͳ����ֵ
	SLONG task_ok_id;						// ִ��ϵͳ
	SLONG task_not_ok_id;					// ִ��ϵͳ����
	SLONG task_error_id;					// ִ��ϵͳ����
	
} SYSTEM_TASK_TABLE;


SLONG insert_task_list(void (*polling_action)(void),SLONG id);
SLONG delete_task_list(void (*polling_action)(void));
void execute_task_manger(void);
void display_task_man_info(void);

SLONG insert_window_base(RECT position ,void (*draw)(void),void (*exec)(void),SLONG window_type);
SLONG delete_window_base(SLONG handle);
void execute_window_base(void);
void set_window_base_active(SLONG handle);
void set_window_base_data(SLONG handle,RECT *pp,SLONG value);
SLONG read_window_base_data(SLONG handle,RECT *pp,SLONG *value);
SLONG check_have_touch_window(RECT tt);


SLONG execute_system_task(void);
SLONG change_task(SLONG change_id);
SLONG check_have_in_window(SLONG x,SLONG y);

bool is_window_on(void);

#endif  // POLLING.H