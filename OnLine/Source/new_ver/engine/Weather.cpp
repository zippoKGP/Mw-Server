/*
**	Weather.cpp
**
**	Stephen, 2002/10/08.
*/
#include "engine_global.h"
#include "map_info.h"
#include "weather.h"
#include "utility.h"
#include "data.h"
#include "vari-ext.h"



SLONG cloud_handle=-1;

// ----------- parameter define area

WEATHER_PARAMETER weather_parameter;
WEATHER_RAIN_DATA weather_rain_data[MAX_RAIN_POINT];
WEATHER_SNOW_DATA weather_snow_data[MAX_RAIN_POINT];
WEATHER_CLOUD_DATA weather_cloud_data[MAX_CLOUD];


/**************************************************************************************************}
{	启动天气系统																				   }
{	Input  : None																				   }
{	Output : None																				   }
{**************************************************************************************************/
int  init_weather(void)
{
    control_wind(WIND_DOWN,WIND_SMALL);
    // -------- Clear all weather type
    control_weather( WEATHER_RAIN | WEATHER_SNOW | WEATHER_CLOUD | WEATHER_THUNDER,
        WEATHER_CLEAR,
        0);
    control_weather( WEATHER_NORMAL,0,0);

	mda_group_open((USTR*)"system\\cloud.ani",&cloud_handle);
	if(cloud_handle<0)
		return(1);
    
    return(0);
}


void control_wind(SLONG dir,SLONG range)
{
    SLONG x_off,y_off;
    SLONG rr;
    
    weather_parameter.x_offset=0;
    weather_parameter.y_offset=0;
    switch(dir)
    {
    case WIND_RIGHT:
        x_off=1;
        y_off=0;
        break;
    case WIND_RIGHT_DOWN:
        x_off=1;
        y_off=1;
        break;
    case WIND_DOWN:
        x_off=0;
        y_off=1;
        break;
    case WIND_LEFT_DOWN:
        x_off=-1;
        y_off=1;
        break;
    case WIND_LEFT:
        x_off=-1;
        y_off=0;
        break;
    case WIND_LEFT_UP:
        x_off=-1;
        y_off=-1;
        break;
    case WIND_UP:
        x_off=0;
        y_off=-1;
        break;
    case WIND_UP_RIGHT:
        x_off=1;
        y_off=-1;
        break;
    }
    
    rr=0;
    switch(range)
    {
    case WIND_NONE:
        break;
    case WIND_SMALL:
        rr=1;
        break;
    case WIND_MIDDLE:
        rr=2;
        break;
    case WIND_LARGE:
        rr=4;
        break;
    }
        
    weather_parameter.wind_dir = dir;
	weather_parameter.rain_wind_range = range;
	weather_parameter.snow_wind_range = range;
	weather_parameter.thunder_wind_range = range;
    
    weather_parameter.x_offset=x_off*rr;
    weather_parameter.y_offset=y_off*rr;
}


// --------- for dummy runtinue
void free_weather(void)
{
	mda_group_close(&cloud_handle);
//    destroy_cake_frame_ani(&cloud_cfa);
}

void active_weather(int flag)
{
}

/***************************************************************************************************}
{  设定天气系统																						}
{	Input  : weather_type																			}
{	         weather_command			 															}
{			 weather_range 																	}
{	Output : None																					}
{***************************************************************************************************/
void control_weather(SLONG weather_type,SLONG weather_command,SLONG weather_range)
{
    SLONG i;
    
    // --- Step 1 Process rain 
    if(weather_type==WEATHER_NORMAL)
    {
        weather_parameter.status=WEATHER_NORMAL;
        return;
    }
    
    
    if(weather_type&WEATHER_RAIN)
    {
        switch(weather_command)
        {
        case WEATHER_CLEAR:
            // ----- clear rain status 
            for(i=0;i<MAX_RAIN_POINT;i++)
                weather_rain_data[i].active=0;
            weather_parameter.rain_process_count=0;
            break;
        case WEATHER_STARTUP:
            break;
        case WEATHER_STOP:
            break;
        }
        weather_parameter.rain_command=weather_command;
		weather_parameter.rain_range=weather_range;
    }
    
    // --- step 2 process snow
    if(weather_type&WEATHER_SNOW)
    {
        switch(weather_command)
        {
        case WEATHER_CLEAR:
            // ----- clear rain status 
            for(i=0;i<MAX_RAIN_POINT;i++)
                weather_snow_data[i].active=0;
            weather_parameter.snow_process_count=0;
            break;
        case WEATHER_STARTUP:
            break;
        case WEATHER_STOP:
            break;
        }
        weather_parameter.snow_command=weather_command;
		weather_parameter.snow_range=weather_range;
    }
    
    
    // --- step 3 process thunder
    if(weather_type&WEATHER_THUNDER)
    {
        switch(weather_command)
        {
        case WEATHER_CLEAR:
            weather_parameter.thunder_process_count=0;
            break;
        case WEATHER_STARTUP:
            weather_parameter.thunder_process_count=20*30+rand()%50;
            break;
        case WEATHER_STOP:
            weather_parameter.thunder_process_count=0;
            break;
        }
        weather_parameter.thunder_command=weather_command;
        weather_parameter.thunder_range=weather_range;
    }
    
    // --- step 3 process cloud
    if(weather_type&WEATHER_CLOUD)
    {
        switch(weather_command)
        {
        case WEATHER_CLEAR:
            for(i=0;i<MAX_CLOUD;i++)
                weather_cloud_data[i].active=0;
            weather_parameter.cloud_process_count=0;
            break;
        case WEATHER_STARTUP:
            break;
        case WEATHER_STOP:
            break;
        }
        weather_parameter.cloud_command=weather_command;
        weather_parameter.cloud_range=weather_range;
    }
    
    
    
    // ------- Update to struct 
    weather_parameter.status=weather_parameter.status|weather_type;
}


void get_weather_report(UCHR *report_text)
{
	UCHR weather_text[80];
	UCHR dir_text[80];

	switch(weather_parameter.wind_dir)
	{
	case WIND_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_DOWN);
		break;
	case WIND_LEFT_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_EN);
		break;
	case WIND_RIGHT_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_WN);
		break;
	default:
		strcpy((char *)dir_text,WEATHER_TEXT_DOWN);
		break;
	}

	
	strcpy((char *)weather_text,"");
	if(weather_parameter.status&WEATHER_RAIN)
	{
		if(weather_parameter.rain_command!=WEATHER_STOP)
		{
			switch(weather_parameter.rain_range)
			{
			case WEATHER_SMALL:
				strcat((char *)weather_text,WEATHER_TEXT_SMALL);
				break;
			case WEATHER_MIDDLE:
				strcat((char *)weather_text,WEATHER_TEXT_MIDDLE);
				break;
			case WEATHER_LARGE:
				strcat((char *)weather_text,WEATHER_TEXT_LARGE);
				break;				
			}
			strcat((char *)weather_text,WEATHER_TEXT_RANG);
		}
		
	}
	if(weather_parameter.status&WEATHER_SNOW)
	{
		if(weather_parameter.snow_command!=WEATHER_STOP)
		{
			switch(weather_parameter.snow_range)
			{
			case WEATHER_SMALL:
				strcat((char *)weather_text,WEATHER_TEXT_SMALL);
				break;
			case WEATHER_MIDDLE:
				strcat((char *)weather_text,WEATHER_TEXT_MIDDLE);
				break;
			case WEATHER_LARGE:
				strcat((char *)weather_text,WEATHER_TEXT_LARGE);
				break;				
			}
			strcat((char *)weather_text,WEATHER_TEXT_SNOW);
		}
	}
	
	if(weather_parameter.status&WEATHER_CLOUD)
	{
		if(weather_parameter.cloud_command!=WEATHER_STOP)
			strcat((char *)weather_text,WEATHER_TEXT_CLOUD);
	}
	if(weather_parameter.status&WEATHER_THUNDER)
	{
		if(weather_parameter.thunder_command!=WEATHER_STOP)
			strcat((char *)weather_text,WEATHER_TEXT_THUNDER);
	}
	
	if(strlen((char *)weather_text)<=1)
		strcpy((char *)weather_text,WEATHER_TEXT_NONE);
	
	sprintf((char *)report_text,WEATHER_TEXT_REPORT,change_map_info.map_name,
		weather_text,dir_text);
	
	
}


void get_weather_sample_report(UCHR *report_text)
{
	UCHR weather_text[80];
	UCHR dir_text[80];
	
	switch(weather_parameter.wind_dir)
	{
	case WIND_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_DOWN);
		break;
	case WIND_LEFT_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_EN);
		break;
	case WIND_RIGHT_DOWN:
		strcpy((char *)dir_text,WEATHER_TEXT_WN);
		break;
	default:
		strcpy((char *)dir_text,WEATHER_TEXT_DOWN);
		break;
	}
	
	
	strcpy((char *)weather_text,"");
	if(weather_parameter.status&WEATHER_RAIN)
	{
		switch(weather_parameter.rain_range)
		{
		case WEATHER_SMALL:
			strcat((char *)weather_text,WEATHER_TEXT_SMALL);
			break;
		case WEATHER_MIDDLE:
			strcat((char *)weather_text,WEATHER_TEXT_MIDDLE);
			break;
		case WEATHER_LARGE:
			strcat((char *)weather_text,WEATHER_TEXT_LARGE);
			break;				
		}
		strcat((char *)weather_text,WEATHER_TEXT_RANG);
		
	}
	if(weather_parameter.status&WEATHER_SNOW)
	{
		switch(weather_parameter.snow_range)
		{
		case WEATHER_SMALL:
			strcat((char *)weather_text,WEATHER_TEXT_SMALL);
			break;
		case WEATHER_MIDDLE:
			strcat((char *)weather_text,WEATHER_TEXT_MIDDLE);
			break;
		case WEATHER_LARGE:
			strcat((char *)weather_text,WEATHER_TEXT_LARGE);
			break;				
		}
		strcat((char *)weather_text,WEATHER_TEXT_SNOW);
	}
	
	if(weather_parameter.status&WEATHER_CLOUD)
	{
		strcat((char *)weather_text,WEATHER_TEXT_CLOUD);
	}
	if(weather_parameter.status&WEATHER_THUNDER)
	{
		strcat((char *)weather_text,WEATHER_TEXT_THUNDER);
	}
	
	if(strlen((char *)weather_text)<=1)
		strcpy((char *)weather_text,WEATHER_TEXT_NONE);
	
	sprintf((char *)report_text,WEATHER_TEXT_SAMPLE_REPORT,weather_text,dir_text);
	
	
}



void refresh_weather(void)
{
    SLONG rain_count;
    SLONG snow_count;
    SLONG i;
    SLONG finish;
	UCHR weather_text[1024];
	SLONG wind_dir,wind_range;
    static ULONG loop_count=0;
    

	if(weather_parameter.status==WEATHER_NORMAL)
		return;
	if(system_control.system_timer<game_control.weather_timer)
		return;
	game_control.weather_timer=system_control.system_timer+WEATHER_REFRESH_TIME*4;
	
	if(system_control.system_timer>game_control.weather_change_timer)
	{
		game_control.weather_change_timer=system_control.system_timer+AUTO_CHANGE_WIND_DELAY;
		
		switch(rand()%3)
		{
		case 0:
			wind_dir=WIND_DOWN;
			break;
		case 1:
			wind_dir=WIND_LEFT_DOWN;
			break;
		case 2:
			wind_dir=WIND_RIGHT_DOWN;
			break;
		default:
			wind_dir=WIND_DOWN;
			break;
		}
		
		switch(rand()%3)
		{
		case 0:
			wind_range=WIND_SMALL;
			break;
		case 1:
			wind_range=WIND_MIDDLE;
			break;
		case 2:
			wind_range=WIND_LARGE;
			break;
		default:
			wind_range=WIND_SMALL;
			break;
		}
		
		control_wind(wind_dir,wind_range);
		get_weather_report(weather_text);
		push_chat_data(CHAT_CHANNEL_MESSGAE,0,(UCHR *)"",weather_text);
	}
		
// --- 自动控制风向与大小

    // ----------------- Process rain ---------------------
    if(weather_parameter.status&WEATHER_RAIN)
    {
        switch(weather_parameter.rain_command)
        {
        case WEATHER_CLEAR:
            break;
        case WEATHER_STARTUP:
            {
                rain_count=5;
                switch(weather_parameter.rain_range)
                {
                case WEATHER_SMALL:
                    rain_count=10;
                    weather_parameter.rain_wind_range=5;		
                    break;
                case WEATHER_MIDDLE:
                    rain_count=20;
                    weather_parameter.rain_wind_range=10;
                    break;
                case WEATHER_LARGE:
                    rain_count=100;
                    weather_parameter.rain_wind_range=30;
                    break;
                }
                for(i=0;i<rain_count;i++)
                {

                    weather_rain_data[weather_parameter.rain_process_count].active=1;
                    weather_rain_data[weather_parameter.rain_process_count].x=(map_control.start_x-400)+rand()%1600;
                    weather_rain_data[weather_parameter.rain_process_count].y=(map_control.start_y-300)+rand()%1200;
                    weather_rain_data[weather_parameter.rain_process_count].height=200+rand()%50;
                    
                    weather_parameter.rain_process_count++;
                    if(weather_parameter.rain_process_count>=MAX_RAIN_POINT)
                        weather_parameter.rain_process_count=0;
                }
            }
            break;
        case WEATHER_STOP:
            finish=1;
            for(i=0;i<MAX_RAIN_POINT;i++)
            {
                if(weather_rain_data[i].active>0)
                {
                    finish=0;
                    break;
                }
            }
            if(finish==1)
            {
                weather_parameter.rain_command=WEATHER_CLEAR;
                weather_parameter.status=weather_parameter.status&WEATHER_RAIN_STOP;
            }
            break;
        }
    }
    
    
    // --------------- Process snow ---------------------
    if(weather_parameter.status&WEATHER_SNOW)
    {
        switch(weather_parameter.snow_command)
        {
        case WEATHER_CLEAR:
            break;
        case WEATHER_STARTUP:
			snow_count=5;
			switch(weather_parameter.snow_range)
			{
			case WEATHER_SMALL:
				snow_count=2;
				weather_parameter.snow_wind_range=1;
				break;
			case WEATHER_MIDDLE:
				snow_count=15;
				weather_parameter.snow_wind_range=3;
				break;
			case WEATHER_LARGE:
				snow_count=50;
				weather_parameter.snow_wind_range=10;
				break;
			}
			for(i=0;i<snow_count;i++)
			{
				weather_snow_data[weather_parameter.snow_process_count].active=1;
				weather_snow_data[weather_parameter.snow_process_count].x=(map_control.start_x-400)+rand()%1600;
				weather_snow_data[weather_parameter.snow_process_count].y=(map_control.start_y-300)+rand()%1200;
				weather_snow_data[weather_parameter.snow_process_count].height=200+rand()%50;
				weather_snow_data[weather_parameter.snow_process_count].size=rand()%3;
				
				weather_parameter.snow_process_count++;
				if(weather_parameter.snow_process_count>=MAX_RAIN_POINT)
					weather_parameter.snow_process_count=0;
			}
            break;
        case WEATHER_STOP:
            finish=1;
            for(i=0;i<MAX_RAIN_POINT;i++)
            {
                if(weather_snow_data[i].active>0)
                {
                    finish=0;
                    break;
                }
            }
            if(finish==1)
            {
                weather_parameter.snow_command=WEATHER_CLEAR;
                weather_parameter.status=weather_parameter.status&WEATHER_SNOW_STOP;
            }
            break;
        }
    }
    
    // --------------- Process thunder ---------------------
    if(weather_parameter.status&WEATHER_THUNDER)
    {
        switch(weather_parameter.thunder_command)
        {
        case WEATHER_CLEAR:
            break;
        case WEATHER_STARTUP:
            weather_parameter.thunder_process_count--;
            if(weather_parameter.thunder_process_count<0)
                weather_parameter.thunder_process_count=20*30+rand()%50;
            break;
        case WEATHER_STOP:
            weather_parameter.thunder_command=WEATHER_CLEAR;
            weather_parameter.status=weather_parameter.status&WEATHER_THUNDER_STOP;
            break;
        }
    }
    
    
    
    // --------------- Process cloud ---------------------
    if(weather_parameter.status&WEATHER_CLOUD)
    {
        switch(weather_parameter.cloud_command)
        {
        case WEATHER_CLEAR:
            break;
        case WEATHER_STARTUP:
            loop_count++;
            if(loop_count%20!=0)break;
            if(weather_cloud_data[weather_parameter.cloud_process_count].active==0)
            {
                weather_cloud_data[weather_parameter.cloud_process_count].active=1;
                weather_cloud_data[weather_parameter.cloud_process_count].x=(map_control.start_x-400)+rand()%1600;
                weather_cloud_data[weather_parameter.cloud_process_count].y=(map_control.start_y-300)+rand()%1200;
                weather_cloud_data[weather_parameter.cloud_process_count].timer=0;
                weather_cloud_data[weather_parameter.cloud_process_count].type=0;

				weather_cloud_data[weather_parameter.cloud_process_count].type=rand()%4;
				
/*
                switch(weather_parameter.cloud_range)
                {
                case WEATHER_LARGE:
                    weather_cloud_data[weather_parameter.cloud_process_count].type=0+(rand()%2*3);
                    break;
                case WEATHER_MIDDLE:
                    weather_cloud_data[weather_parameter.cloud_process_count].type=1+(rand()%2*3);
                    break;
                case WEATHER_SMALL:
                    weather_cloud_data[weather_parameter.cloud_process_count].type=2+(rand()%2*3);
                    break;
                }
*/
                
                weather_parameter.cloud_process_count++;
                if(weather_parameter.cloud_process_count>=MAX_CLOUD)
                    weather_parameter.cloud_process_count=0;
            }
            
            break;
        case WEATHER_STOP:
            finish=1;
            for(i=0;i<MAX_CLOUD;i++)
            {
                if(weather_cloud_data[i].active>0)
                {
                    finish=0;
                    break;
                }
            }
            if(finish==1)
            {
                weather_parameter.cloud_command=WEATHER_CLEAR;
                weather_parameter.status=weather_parameter.status&WEATHER_CLOUD_STOP;
            }
            break;
        }
    }


	if((weather_parameter.rain_command==WEATHER_CLEAR)&&
		(weather_parameter.snow_command==WEATHER_CLEAR)&&
		(weather_parameter.thunder_command==WEATHER_CLEAR)&&
		(weather_parameter.cloud_command=WEATHER_CLEAR))
	{
		weather_parameter.status=WEATHER_NORMAL;
	}
	
    
}


/***************************************************************************************************}
{  执行天气系统																						}
{	Input  : bitmap 																				}
{	Output : 																						}
{***************************************************************************************************/
void exec_weather(BMP *bitmap)
{
    SLONG i;
    SLONG real_x,real_y;
    SLONG target_x,target_y;
    SLONG ff;
//    CAKE_FRAME_ANI  *cloud_frame;
	SLONG channel;
	bool add_offset;
	UCHR command_text[80];


	if(!game_control.weather_enable)return;
	
	add_offset=false;
	if(system_control.system_timer<game_control.weather_add_timer)
	{
		add_offset=false;
	}
	else
	{
		add_offset=true;
		game_control.weather_add_timer=system_control.system_timer+WEATHER_REFRESH_TIME;
	}


    // --------------- process rain
    if(weather_parameter.status&WEATHER_RAIN)
    {
        for(i=0;i<MAX_RAIN_POINT;i++)
        {
            switch(weather_rain_data[i].active)
            {
            case 0:
                continue;
                break;
            case 1:
				if(add_offset)
				{
					weather_rain_data[i].x+=weather_parameter.x_offset;
					weather_rain_data[i].y+=weather_parameter.y_offset;
				}
                real_x=weather_rain_data[i].x-map_control.start_x;
                real_y=weather_rain_data[i].y-map_control.start_y-weather_rain_data[i].height;
                target_x=real_x+weather_parameter.x_offset;				// wind_dir
                target_y=real_y+weather_parameter.rain_wind_range+weather_parameter.y_offset;			// speed

                
                if((real_x>=1)&&(real_y>=1)&&(real_x<799)&&(real_y<599)&&
                    (target_x>=1)&&(target_y>=1)&&(target_x<799)&&(target_y<599))
                {
                    put_line(real_x,real_y,target_x,target_y,SYSTEM_WHITE,bitmap);
                }

				if(add_offset)
	                weather_rain_data[i].height-=weather_parameter.rain_wind_range*2;
                if(weather_rain_data[i].height<=0)
                {
                    weather_rain_data[i].active=2;
                }
                break;
            case 2:
                real_x=weather_rain_data[i].x-map_control.start_x;
                real_y=weather_rain_data[i].y-map_control.start_y;
                if((real_x>=1)&&(real_y>=1)&&(real_x<799)&&(real_y<599))
                    put_ellipse(real_x,real_y,2,1,SYSTEM_WHITE,bitmap);
                weather_rain_data[i].active=3;
                break;
            case 3:
                real_x=weather_rain_data[i].x-map_control.start_x;
                real_y=weather_rain_data[i].y-map_control.start_y;
                if((real_x>=1)&&(real_y>=1)&&(real_x<799)&&(real_y<599))
                    put_ellipse(real_x,real_y,4,2,SYSTEM_WHITE,bitmap);
                weather_rain_data[i].active=4;
                break;
            case 4:
                real_x=weather_rain_data[i].x-map_control.start_x;
                real_y=weather_rain_data[i].y-map_control.start_y;
                if((real_x>=1)&&(real_y>=1)&&(real_x<799)&&(real_y<599))
                    put_ellipse(real_x,real_y,6,3,SYSTEM_WHITE,bitmap);
                weather_rain_data[i].active=0;
                break;
            }
        }
    }
    
    
    // --------------- process snow
    if(weather_parameter.status&WEATHER_SNOW)
    {
        for(i=0;i<MAX_RAIN_POINT;i++)
        {
            switch(weather_snow_data[i].active)
            {
            case 0:
                continue;
                break;
            case 1:
				if(add_offset)
				{
					weather_snow_data[i].x+=weather_parameter.x_offset;
					weather_snow_data[i].y+=weather_parameter.y_offset;
				}
				
				real_x=weather_snow_data[i].x-map_control.start_x;
				real_y=weather_snow_data[i].y-map_control.start_y-weather_snow_data[i].height;
				ff=real_y%5;
				real_x=real_x+(ff-3);
				
				switch(weather_snow_data[i].size)
				{
				case 0:
					if((real_x>=5)&&(real_y>=5)&&(real_x<799-5)&&(real_y<599-5))
						put_pixel(real_x,real_y,SYSTEM_WHITE,bitmap);
					break;
				case 1:
					if((real_x>=5)&&(real_y>=5)&&(real_x<799-5)&&(real_y<599-5))
						put_bar(real_x,real_y,2,2,SYSTEM_WHITE,bitmap);
					break;
				case 2:
					if((real_x>=5)&&(real_y>=5)&&(real_x<799-5)&&(real_y<599-5))
					{
						put_pixel(real_x+1,real_y-1,SYSTEM_WHITE,bitmap);
						put_pixel(real_x-1,real_y+1,SYSTEM_WHITE,bitmap);
						put_pixel(real_x+3,real_y+1,SYSTEM_WHITE,bitmap);
						put_pixel(real_x+1,real_y+3,SYSTEM_WHITE,bitmap);
						put_bar(real_x,real_y,3,3,SYSTEM_WHITE,bitmap);
					}
					break;
				default:
					weather_snow_data[i].height=0;
					break;
				}

				if(add_offset)
					weather_snow_data[i].height-=weather_parameter.snow_wind_range;
				if(weather_snow_data[i].height<=0)
				{
					if(weather_snow_data[i].size==0)
						weather_snow_data[i].active=0;
					else
						weather_snow_data[i].active=2;
				}
                break;
            case 2:
            case 3:
            case 4:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
                real_x=weather_snow_data[i].x-map_control.start_x;
                real_y=weather_snow_data[i].y-map_control.start_y;
                if((real_x>=5)&&(real_y>=5)&&(real_x<799-5)&&(real_y<599-5))
                    put_bar(real_x,real_y,2,2,SYSTEM_WHITE,bitmap);
                weather_snow_data[i].active++;
                break;
            case 10:
            case 11:
            case 12:
            case 13:
                real_x=weather_snow_data[i].x-map_control.start_x;
                real_y=weather_snow_data[i].y-map_control.start_y;
                if((real_x>=5)&&(real_y>=5)&&(real_x<799-5)&&(real_y<599-5))
                    put_pixel(real_x,real_y,SYSTEM_WHITE,bitmap);
                weather_snow_data[i].active++;
                break;
            case 14:
                weather_snow_data[i].active=0;
                break;
            default:
                weather_snow_data[i].active=0;
                break;
            }
        }
    }

    // --------------- process thunder
    if(weather_parameter.status&WEATHER_THUNDER)
    {
        switch(weather_parameter.thunder_process_count)
        {
        case 20:
        case 18:
            put_bar(0,0,800,600,color_control.white,bitmap);
            break;
        case 12:
        case 10:
        case 8:
            put_bar(0,0,800,600,color_control.white,bitmap);
            break;
        case 4:
        case 3:
        case 2:
            put_bar(0,0,800,600,color_control.white,bitmap);
            break;
            // 挤㏑
        case 1:
			auto_play_voice(0,(UCHR *)"EFC\\thunder.wav",NULL,0,0,0,0,&channel);
            break;
        }
    }
    
    
    // --------------- process cloud
    if(weather_parameter.status&WEATHER_CLOUD)
    {
        for(i=0;i<MAX_CLOUD;i++)
        {
            if(weather_cloud_data[i].active==0)continue;

			if(add_offset)
			{
				weather_cloud_data[i].x=weather_cloud_data[i].x+weather_parameter.x_offset;
				weather_cloud_data[i].y=weather_cloud_data[i].y+weather_parameter.y_offset;
			}
            
            real_x=weather_cloud_data[i].x-map_control.start_x;
            real_y=weather_cloud_data[i].y-map_control.start_y;
            
            
            

			sprintf((char *)command_text,"MainCommand%d",weather_cloud_data[i].type);

            // ---- add 熬簿秖
            if(weather_cloud_data[i].timer>20*5)
            {
                if(weather_cloud_data[i].active>=0)
                {
                    weather_cloud_data[i].active-=5;
				
					mda_group_exec(cloud_handle,command_text,real_x,real_y,screen_buffer,MDA_COMMAND_FIRST|MDA_COMMAND_ALPHA,0,255-weather_cloud_data[i].active,0);
                }
                else
                {
                    weather_cloud_data[i].active=0;
                }
            }
            else
            {
                if(weather_cloud_data[i].active<=255)
                {
                    weather_cloud_data[i].active+=5;

					mda_group_exec(cloud_handle,command_text,real_x,real_y,screen_buffer,MDA_COMMAND_FIRST|MDA_COMMAND_ALPHA,0,255-weather_cloud_data[i].active,0);
				
                }
                else
                {
					mda_group_exec(cloud_handle,command_text,real_x,real_y,screen_buffer,MDA_COMMAND_FIRST|MDA_COMMAND_ALPHA,0,0,0);

                    weather_cloud_data[i].active=255;
                    weather_cloud_data[i].timer++;
                }
            }
        }
    }


}


void set_weather_type(SLONG type)
{
	SLONG weather_type,weather_command,weather_range;

	if(type==WEATHER_TYPE_NONE)						// STOP 
	{
		weather_type=WEATHER_RAIN|WEATHER_SNOW|WEATHER_CLOUD|WEATHER_THUNDER;
		weather_command=WEATHER_STOP;
		weather_range=WEATHER_SMALL;
		control_weather(weather_type,weather_command,weather_range);
		return;
	}


	weather_type=0x00;
	if(type&WEATHER_TYPE_RAIN)
		weather_type=weather_type|WEATHER_RAIN;

	if(type&WEATHER_TYPE_SNOW)
		weather_type=weather_type|WEATHER_SNOW;
	
	if(type&WEATHER_TYPE_CLOUD)
		weather_type=weather_type|WEATHER_CLOUD;
	
	if(type&WEATHER_TYPE_THUNDER)
		weather_type=weather_type|WEATHER_THUNDER;

	weather_range=WEATHER_SMALL;

	if(type&WEATHER_RANGE_MIDDLE)
		weather_range=WEATHER_MIDDLE;
	
	if(type&WEATHER_RANGE_LARGE)
		weather_range=WEATHER_LARGE;
	
	weather_command=WEATHER_STARTUP;
	control_weather(weather_type,weather_command,weather_range);

		
}



SLONG get_weather_icon_no(void)
{

	if(weather_parameter.status==WEATHER_NORMAL)		// 晴天
		return(0);

	if(weather_parameter.status==WEATHER_THUNDER)
		return(1);

	if(weather_parameter.status==WEATHER_CLOUD)
		return(2);

	if(weather_parameter.status==WEATHER_RAIN)
		return(3);

	if(weather_parameter.status==WEATHER_SNOW)
		return(4);

    if(weather_parameter.status&WEATHER_SNOW)
		return(5);

    if(weather_parameter.status&WEATHER_RAIN)
		return(6);
	
	return(7);
		

}





