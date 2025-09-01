/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : polling.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2005/1/10
******************************************************************************/ 

#include "engine_global.h"
#include "polling.h"
#include "vari-ext.h"
#include "data.h"
#include "game.h"




// ----------------------- Polling program data --------------------
typedef struct TASK_POLLINGTABLE            // polling executing list
{
	struct TASK_POLLINGTABLE *next,*prev;
	void (*polling_action)();

// --------- TASK Info  ---------
	SLONG task_id;							// task ID
	ULONG task_exec_times;					// task exec 次数

} TASK_POLLINGTABLE;



typedef struct WINDOW_BASE
{
	struct WINDOW_BASE *next,*prev;
	// ---- paramater 
	SLONG handle;					
	SLONG type;							// window type
	RECT position;						// window 坐标大小
	// ---- routinue
	void (*draw_process)();				// 画底图 函数
	void (*exec_process)();				// 鼠标点击或键盘时的处理函数

	// ---- routinue paramater			// exec process 的 paramater 
	SLONG value;
	
} WINDOW_BASE;



/*******************************************************************************************************************}
{																													}
{		Globe define																								}
{																													}
********************************************************************************************************************/


static TASK_POLLINGTABLE polling_base={&polling_base,&polling_base,0x00 ,0x00 ,0x00};

static WINDOW_BASE window_base={
	&window_base,&window_base,
// ---- 
	-1,									// handle
	WINDOW_TYPE_NORMAL,					// window type
	{ 0, 0, 0, 0 },						// position
// -----
	NULL,NULL,							// draw,exec
	0									// 0
};

static SLONG window_base_handle=0;


/*******************************************************************************************************************}
{																													}
{		Routinue Start																								}
{																													}
********************************************************************************************************************/
//
// Insert a routine into polling list
//
SLONG insert_task_list(void (*polling_action)(void),SLONG task_id)
{
	TASK_POLLINGTABLE *pr;
	TASK_POLLINGTABLE *task_new;
	pr=polling_base.next;
	while(pr != &polling_base) {
		if(pr->polling_action == polling_action )   // Already in List Node found
			return(TTN_NOT_OK);
		pr=pr->next;
    }
	task_new=(TASK_POLLINGTABLE *)(calloc(1,sizeof(TASK_POLLINGTABLE)));
	task_new->next=&polling_base;
	task_new->prev=polling_base.prev;
	task_new->polling_action=polling_action;

	task_new->task_id=task_id;
	task_new->task_exec_times=0;

	polling_base.prev->next=task_new;
	polling_base.prev=task_new;
	return(TTN_OK);
}
//
// Delete a routine from polling list
//
SLONG delete_task_list(void (*polling_action)(void))
{
	TASK_POLLINGTABLE *pr;
	pr=polling_base.next;
	while(pr != &polling_base) {
		if(pr->polling_action == polling_action ) {  /* Node found */
			(pr->prev)->next=pr->next;
			(pr->next)->prev=pr->prev;
			free(pr);
			return(TTN_OK);
		}
		pr=pr->next;
    }
	return(TTN_NOT_OK);
}
//
// Execute Routine in Polling List
//
void execute_task_manger(void)
{
	static TASK_POLLINGTABLE *pr;

	pr=polling_base.next;
	while(pr != &polling_base) {
		(*pr->polling_action)();
		pr->task_exec_times++;				// exec times ++
		
		pr=pr->next;
	}
	
}



void display_task_man_info(void)
{
	static TASK_POLLINGTABLE *pr;
	UCHR text[1024];

	pr=polling_base.next;
	strcpy((char *)print_rec,"");
	while(pr != &polling_base) {
		sprintf((char *)text,"ID:%d(%d) ",pr->task_id,pr->task_exec_times);
		strcat((char *)print_rec,(char *)text);
		pr=pr->next;

	}
	
	print16(0,SCREEN_HEIGHT-20,print_rec,COPY_PUT,screen_buffer);

}




/*******************************************************************************************************************}
{																													}
{   Insert a window into window base																				}
{	return window handle																							}
{																													}
********************************************************************************************************************/
SLONG insert_window_base(RECT position ,void (*draw)(void),void (*exec)(void),SLONG window_type)
{
	WINDOW_BASE *pr;
	WINDOW_BASE *task_new;

	pr=window_base.next;

	while(pr != &window_base) {
		pr=pr->next;				// find last 
	}

	task_new=(WINDOW_BASE *)(calloc(1,sizeof(WINDOW_BASE)));
	task_new->next=&window_base;
	task_new->prev=window_base.prev;
	task_new->draw_process=draw;
	task_new->exec_process=exec;
	task_new->type=window_type;

	task_new->position.left=position.left;
	task_new->position.top=position.top;
	task_new->position.right=position.right;
	task_new->position.bottom=position.bottom;

	task_new->value=0;
	task_new->handle=window_base_handle++;
	if(window_base_handle<0)
		window_base_handle=0;
	
	window_base.prev->next=task_new;
	window_base.prev=task_new;

	return(task_new->handle);

}


/*******************************************************************************************************************}
{																													}
{   Delette a window from window base																				}
{	return OK or NOT_OK  																							}
{																													}
********************************************************************************************************************/
SLONG delete_window_base(SLONG handle)
{
	WINDOW_BASE *pr;
	pr=window_base.next;
	while(pr != &window_base) {
		if(pr->handle == handle ) {  /* Node found */
			(pr->prev)->next=pr->next;
			(pr->next)->prev=pr->prev;
			free(pr);
			return(TTN_OK);
		}
		pr=pr->next;
    }
	return(TTN_NOT_OK);
}



void set_window_base_active(SLONG handle)
{
	WINDOW_BASE *pr;
	pr=window_base.next;
	while(pr != &window_base) {
		if(pr->handle == handle ) {  /* Node found */
			(pr->prev)->next=pr->next;
			(pr->next)->prev=pr->prev;

			pr->next=&window_base;
			pr->prev=window_base.prev;
			(window_base.prev)->next=pr;
			window_base.prev=pr;
		}
		pr=pr->next;
    }
}



SLONG read_window_base_data(SLONG handle,RECT *pp,SLONG *value)
{
	WINDOW_BASE *pr;

	pr=window_base.next;
	while(pr != &window_base) {
		if(pr->handle == handle ) {  /* Node found */
			{
				pp->left=pr->position.left;
				pp->top=pr->position.top;
				pp->right=pr->position.right;
				pp->bottom=pr->position.bottom;
				*value=pr->value;
				return(TTN_OK);
			}
		}
		pr=pr->next;
    }

	return(TTN_NOT_OK);
}


void set_window_base_data(SLONG handle,RECT *pp,SLONG value)
{
	WINDOW_BASE *pr;
	pr=window_base.next;
	while(pr != &window_base) {
		if(pr->handle == handle ) {  /* Node found */
			{
				if(pp!=NULL)			// Set position
				{
					pr->position.left=pp->left;
					pr->position.top=pp->top;
					pr->position.right=pp->right;
					pr->position.bottom=pp->bottom;
				}
				pr->value=value;
				return;
			}
		}
		pr=pr->next;
    }
	
}


/*******************************************************************************************************************}
{																													}
{   Execute a window from window base																				}
{																													}
********************************************************************************************************************/
void execute_window_base(void)
{
	static WINDOW_BASE *pr;
	bool change_active;
	SLONG last_handle;
	SLONG type;
	SLONG mouse_x,mouse_y;

	pr=window_base.next;
	if(pr==&window_base)return;		// 空的


	mouse_x=system_control.mouse_x;
	mouse_y=system_control.mouse_y;

// ---- ( 0 - 0 ) check is change Active window 
	now_active_window_handle=window_base.prev->handle;
	change_active=false;

	last_handle=-1;
	switch(system_control.mouse_key)
	{
	case MS_LUp:					// 左键
	case MS_RUp:					// 右键
		pr=window_base.prev;
		while(pr != &window_base) {
			if((mouse_x>=pr->position.left)&&(mouse_x<pr->position.right)&&
				(mouse_y>=pr->position.top)&&(mouse_y<pr->position.bottom))
			{
				last_handle=pr->handle;
				if(last_handle==now_active_window_handle)	// in active handle
				{
					break;
				}
				else
				{
					change_active=true;
					break;
				}
			}
			pr=pr->prev;						// prec window 
		}
		break;
	}

	
	
// --------- process window type
	pr=window_base.next;
	while(pr != &window_base) {
			type=pr->type;
			if(pr->type&WINDOW_TYPE_ON_TOP)
			{
				change_active=true;
				last_handle=pr->handle;
			}
		pr=pr->next;						// next window 
	}
		

	if(change_active)
	{
		set_window_base_active(last_handle);
		now_active_window_handle=last_handle;
		reset_mouse_key();
		system_control.mouse_key=MS_Dummy;
	}
	else
	{
		pr=window_base.prev;
		now_active_window_handle=pr->handle;
	}


// ---- ( 0 - 0 ) 绘图部分
	now_exec_window_handle=-1;
	now_delete_window_handle=-1;
	pr=window_base.next;
	while(pr != &window_base) {
		now_exec_window_handle=pr->handle;
		(*pr->draw_process)();				// 呼叫绘图 
		pr=pr->next;						// next window 
	}


	if(main_loop_active_task)return;		// 目前有 main_loop_active_task,所以不执行 window
	
// ---- ( 0 - 1 ) 找出 Active Window 
		UpdateEditKey();
	

	now_exec_window_handle=-1;
	pr=window_base.prev;
	if(((mouse_x>=pr->position.left)&&(mouse_x<pr->position.right)&&
		(mouse_y>=pr->position.top)&&(mouse_y<pr->position.bottom))||
		(system_control.key!=NULL)||
		(system_control.data_key!=NULL))
	{
		now_exec_window_handle=pr->handle;
		(*pr->exec_process)();

	}

	
// --- ( 1 - 0 ) 看看有没有自动结束的 handle
	if(	now_delete_window_handle>=0)	
	{
		delete_window_base(now_delete_window_handle);
	}
	


}


SLONG check_have_touch_window(RECT tt)
{
	WINDOW_BASE *pr;
	
	pr=window_base.next;
	while(pr != &window_base) {
		if(check_box_intersect(tt.left,tt.top,tt.right,tt.bottom,
			pr->position.left,pr->position.top,pr->position.right,pr->position.bottom)==TTN_OK)
		{
			return(pr->handle);
		}
		pr=pr->next;
    }
	
	return(TTN_NOT_OK);
	
}


SLONG check_have_in_window(SLONG x,SLONG y)
{
	WINDOW_BASE *pr;
	
	
	if(now_active_window_handle==now_exec_window_handle)
		return(TTN_NOT_OK);

	pr=window_base.next;
	while(pr != &window_base) {

		if(pr->handle!=now_exec_window_handle)
		{
			if((x>pr->position.left)&&(x<=pr->position.right)&&
				(y>pr->position.top)&&(y<=pr->position.bottom))
			{
				return(TTN_OK);				
			}
		}

		pr=pr->next;
    }
	
	return(TTN_NOT_OK);
	
}


/*******************************************************************************************************************}
{																													}
{			执行主回路控制																							}
{																													}
********************************************************************************************************************/
/*
typedef struct SYSTEM_TASK_TABLE
{
	SLONG task_id;							// 执行系统 ID
	UCHR task_name[80];						// 执行系统名称 ( Debug 用)
	SLONG (*task_action)();					// 执行系统主体
	SLONG ret_val;							// 执行系统返回值
	SLONG task_ok_id;						// 执行系统
	SLONG task_not_ok_id;					// 执行系统主体
	SLONG task_error_id;					// 执行系统主体
	
} SYSTEM_TASK_TABLE;
*/

SLONG execute_system_task(void)
{
	SYSTEM_TASK_TABLE *pr;
	SLONG ret_val;

	pr=&system_task_table[now_execute_task_id];
// --- ( 0 - 0 ) Record Execute data 

	

// --- ( 1 - 0 ) Exec body
	if(pr->task_action!=NULL)
		ret_val=(*pr->task_action)();
	else
		ret_val=TASK_SYSTEM_QUIT;

	pr->ret_val=ret_val;


	if(ret_val>=RETURN_CHANGE_TASK)			// change task
	{
		now_execute_task_id=change_task(ret_val-RETURN_CHANGE_TASK);
	}
	else
	{
		switch(ret_val)
		{
		case RETURN_CANCEL:		// case TTN_NOT_OK:
			now_execute_task_id=change_task(pr->task_not_ok_id);
			break;
		case RETURN_ERROR:		// case TTN_ERROR:  
			now_execute_task_id=change_task(pr->task_error_id);
			break;
		case RETURN_OK:			// case TTN_OK:	 
			now_execute_task_id=change_task(pr->task_ok_id);
			break;
		default:
			display_error_message((UCHR *)"System internal error !!!! ( Return Code error )",1);
			break;
		}
	}



// ------ Clear 公用 数值
	system_control.mouse_key=MS_Dummy;
	system_control.key=NULL;
	system_control.data_key=NULL;
	reset_key();
	reset_mouse_key();

	return(now_execute_task_id);
}



SLONG change_task(SLONG change_id)
{
	SLONG i;

	if(change_id==TASK_SYSTEM_QUIT)
		return(TASK_SYSTEM_QUIT);
	
	for(i=0;i<MAX_SYSTEM_TASK;i++)
	{
		if(system_task_table[i].task_id==change_id)
			return(i);
	}

	sprintf((char *)print_rec,"System internal error !!!!(TASK Not define %d)",change_id);
	display_error_message(print_rec,1);
	return(TASK_SYSTEM_QUIT);
}




/*******************************************************************************************************************}
{																													}
{			Dummy																									}
{																													}
********************************************************************************************************************/


bool is_window_on(void)
{
	return (window_base.next != &window_base);
}