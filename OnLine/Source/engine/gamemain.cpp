/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 2003 ALL RIGHTS RESERVED.

  *file name    : gamemain.cpp
  *owner  		: Stephen
  *description  : 
  *modified     : 2004/12/6
******************************************************************************/ 

#include "engine_global.h"
#include "variable.h"
#include "system.h"
#include "jpg.h"
#include "utility.h"
#include "graph.h"
#include "weather.h"
#include "system.h"
#include "taskman.h"
#include "player_info.h"
#include "net.h"
#include "ichat.h"
#include "taskman.h"
#include "data.h"


// ------------- Real Routinue 
int enter_game_main(void)
{
SLONG main_pass;

// ------- Set Debug Info ----------
	debug_info.map_debug=false;
	debug_info.map_stop=true;
	debug_info.map_event=true;						// 事件
	debug_info.map_path=true;						// 路径
	debug_info.map_shadow=true;						// 影子
	debug_info.map_link=true;						// 连接点
	debug_info.map_sound=true;						// 音效

	
	show_mouse(SHOW_IMAGE_CURSOR);
	set_mouse_cursor(MOUSE_IMG_STAND);
	set_mouse_position(400,300);


// ------ Enter Main Loop ----------
	now_execute_task_id=TASK_SYSTEM_INITIALIZE;

	main_pass=0;
	while(main_pass==0)
	{
		if(execute_system_task()==TASK_SYSTEM_QUIT)
			main_pass=1;
	}

	show_mouse(SHOW_WINDOW_CURSOR);
// -------------- Stop device ----------
	update_system_config();				// 储存系统设置
	stop_all_voice();
	stop_music_file();
	system_quit_process();
	return 1;
}




