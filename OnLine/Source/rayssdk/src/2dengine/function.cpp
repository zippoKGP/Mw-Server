/*
**      FUNCTION.CPP
**      useful functions.
**      ZJian,2000.8.5.
*/
#include "rays.h"
#include "xgrafx.h"
#include "xinput.h"
#include "xtimer.h"
#include "xfont.h"
#include "xsystem.h"
#include "xkiss.h"
#include "function.h"
#include "menuwin.h"
#include "packfile.h"
#include "str2num.h"


EXPORT  void    FNBACK  show_help_info_old(char **help_string,BMP *bit_map)
{
    SLONG i, main_pass, sx, sy, xl, yl;
    UCHR ch;
    struct HELP_INFO_STRUCT
    {
        SLONG total;
        char *message[40];
    } help_info;

    main_pass = 0;
    sx = bit_map->w / 8;
    sy = bit_map->h / 8;
    xl = bit_map->w * 6 / 8;
    yl = bit_map->h * 6 / 8;

    for(i=0;(i<40) && help_string[i];i++)
        help_info.message[i]=help_string[i];
    help_info.total=i;
    
    put_menu_hibar(sx,sy,xl,yl,bit_map);
    for(i=0;i<help_info.total;i++)
        print16(sx+8,sy+8+i*18,(USTR *)help_info.message[i],PEST_PUT,bit_map);
    update_screen(bit_map);
    reset_key();
    while(main_pass==0)
    {
        idle_loop();
        ch = (UCHR) toupper( read_data_key() );
        if(ch)
            reset_key();
        else
            ch=get_mouse_key();
        reset_mouse();
        switch(ch)
        {
        case S_KB_F1:case S_Esc:case S_Enter:case ' ':
        case MS_LDn:case MS_RDn:
            main_pass=1;
            break;
        }
    }
    reset_key();
    reset_mouse();
}


EXPORT  void    FNBACK  show_help_info(char **help_string,BMP *bit_map)
{
    SLONG i, main_pass=0, sx, sy, xl, yl;
    UCHR ch;
    SLONG total,start_index,lines;
    SLONG mx,my;

    main_pass = 0;
    sx = bit_map->w / 8;
    sy = bit_map->h / 8;
    xl = bit_map->w * 6 / 8;
    yl = bit_map->h * 6 / 8;

    i=0;
    total=0;
    for(i=0;help_string[i];i++)
        total++;
    start_index=0;
    lines = (bit_map->h - bit_map->h/8 * 2 - 16)/18;
    reset_key();
    while(main_pass==0)
    {
        idle_loop();
        put_menu_hibar(sx,sy,xl,yl,bit_map);

        for(i=0;i<lines;i++)
        {
            if(start_index+i < total)
                print16(sx+8,sy+8+i*18,(USTR *)help_string[start_index+i],PEST_PUT,bit_map);
        }
        // show now line information ...
        if(total>lines)
        {
            if(start_index>0)
                put_up_scroll_button(sx+xl-20,sy+4,1,bit_map);
            if(start_index<total-lines)
                put_down_scroll_button(sx+xl-20,sy+yl-20,1,bit_map);
        }

        update_screen(bit_map);

        ch = (UCHR) toupper( read_data_key() );
        if(ch) reset_key();
        else ch=get_mouse_key();
        get_mouse_position(&mx,&my);
        switch(ch)
        {
        case S_PgUp:
            if(total>lines)
            {
                start_index -= lines;
                if(start_index<0)
                    start_index=0;
            }
            break;
        case S_PgDn:
            if(total>lines)
            {
                start_index+=lines;
                if(start_index>total-lines)
                    start_index=total-lines;
            }
            break;
        case S_Up: 
            if(total>lines)
            {
                if(start_index>0)
                    start_index--;
            }
            break;
        case S_Dn:
            if(total>lines)
            {
                if(start_index<total-lines)
                    start_index++;
            }
            break;
        case S_Esc:case 'Q':case 'q':case S_KB_F1:
        case ' ':case S_Enter:case MS_RDn:
            main_pass=1;
            break;
        case MS_Forward:
            {
                if(total>lines)
                {
                    if(start_index>0)
                        start_index--;
                }
                reset_mouse();
                break;
            }
        case MS_Backward:
            {
                if(total>lines)
                {
                    if(start_index<total-lines)
                        start_index++;
                }
                reset_mouse();
            }
            break;
        case MS_LDn:
            {
                if(total>lines)
                {
                    if(start_index>0)
                    {
                        if(mx>=sx+xl-20 && mx<sx+xl-20+16 && my>=sy+4 && my<sy+4+16)
                        {
                            if(start_index>0)
                                start_index--;
                        }
                    }
                    if(start_index<total-lines)
                    {
                        if(mx>=sx+xl-20 && mx<sx+xl-20+16 && my>=sy+yl-20 && my<sy+yl-20+16)
                        {
                            if(start_index<total-lines)
                                start_index++;
                        }
                    }
                }
                reset_mouse();
                break;
            }
        }
    }
    reset_key();
    reset_mouse();
}


EXPORT  SLONG   FNBACK  ask_yes_no_extend(USTR *title,USTR *yes,USTR *no,BMP *bitmap)
{

    enum 
    { MAX_LN = 12, 
    MAX_LNLEN = 128,  
    AYN_EDGE_SIZE = 4,
    AYN_TITLE_DY = 20,
    AYN_TITLE_BUTTON_DY = 12,
    AYN_BUTTON_DX = 12,
    AYN_RY = 2
    };
    USTR line_str[MAX_LN][MAX_LNLEN];
    USTR  disp_rec[256];
    SLONG lines,index,max_line_len;
    POINT tp[MAX_LN];
    SLONG i,len;
    WINDOW tw;
    WINDOW yw;
    WINDOW nw;
    SLONG main_pass,select;
    SLONG mouse_x,mouse_y;
    UCHR ch;
    BMP* backup_bitmap = NULL;

    // parse the title to lines strings -------------------------------------------------------------
    lines=0;
    index=0;
    max_line_len=0;
    memset(line_str,0,MAX_LN*MAX_LNLEN);
    len=(SLONG)strlen((const char *)title);
    for(i=0;i<len+1;i++)    // len+1 for include the tail '\0'
    {
        line_str[lines][index] = title[i];
        if(max_line_len < index)
            max_line_len = index;
        if( title[i] == '\0')
            break;
        else if( title[i] == '\n' || title[i] == '\r' )
        {
            line_str[lines][index] = '\0';
            index=0;
            lines++;
            if(lines > MAX_LN-1)
                break;
        }
        else
        {
            index++;
            if(index>MAX_LNLEN-1)
            {
                index = MAX_LNLEN-1;
                line_str[lines][index] = '\0';
            }
        }
    }
    lines += 1;

    // prepare the positions ----------------------------------------------------------------------
    for(i=0;i<lines;i++)
    {
        tp[i].x = (bitmap->w - (max_line_len * 8))/2;
        tp[i].y = bitmap->h/2 - AYN_EDGE_SIZE - (lines - i) * AYN_TITLE_DY;
    }
    tw.x1 = tp[0].x - AYN_EDGE_SIZE;
    tw.y1 = tp[0].y - AYN_EDGE_SIZE - AYN_EDGE_SIZE;
    tw.x2 = tp[0].x + max_line_len * 8 + AYN_EDGE_SIZE;
    tw.y2 = bitmap->h/2 ;

    len=((SLONG)strlen((const char *)yes)+1)/2*16;
    yw.x2 = bitmap->w/2 - AYN_BUTTON_DX/2;
    yw.x1 = yw.x2 - len - 2 * AYN_EDGE_SIZE;
    yw.y1 = tw.y2 + AYN_TITLE_BUTTON_DY;
    yw.y2 = yw.y1 + 20 + 2 * AYN_EDGE_SIZE;
    
    len=((SLONG)strlen((const char *)no)+1)/2*16;
    nw.x1 = bitmap->w/2 + AYN_BUTTON_DX/2;
    nw.y1 = tw.y2 + AYN_TITLE_BUTTON_DY;
    nw.x2 = nw.x1 + len + 2 * AYN_EDGE_SIZE;
    nw.y2 = nw.y1 + 20 + 2 * AYN_EDGE_SIZE;

    // ok,start select ------------------------------------------------------------------------------
    backup_bitmap = copy_bitmap(0,0,bitmap->w, bitmap->h, bitmap);
    
    main_pass=0;
    select=1;
    reset_key();
    reset_mouse();
    while(main_pass==0)
    {
        idle_loop();
        if(backup_bitmap)
        {
            put_bitmap(0,0,backup_bitmap,bitmap);
        }
        put_menu_box(tw.x1,tw.y1,tw.x2-tw.x1,tw.y2-tw.y1,bitmap);
        put_menu_box(yw.x1,yw.y1,yw.x2-yw.x1,yw.y2-yw.y1,bitmap);
        put_menu_box(nw.x1,nw.y1,nw.x2-nw.x1,nw.y2-nw.y1,bitmap);
        for(i=0;i<lines;i++)
        {
            sprintf((char *)print_rec,"~C8~O0%s~C0~O0",line_str[i]);
            print16(tp[i].x,tp[i].y,(USTR *)print_rec,PEST_PUT,bitmap);
        }

        switch(select)
        {
        case 0:
            sprintf((char *)disp_rec,"~C0%s~C0",yes);
            print16(yw.x1+AYN_EDGE_SIZE, yw.y1+AYN_EDGE_SIZE+AYN_RY, (USTR *)disp_rec,COPY_PUT_COLOR(SYSTEM_BLUE),bitmap);
            sprintf((char *)disp_rec,"~C8%s~C0",no);
            print16(nw.x1+AYN_EDGE_SIZE, nw.y1+AYN_EDGE_SIZE+AYN_RY, (USTR *)disp_rec,PEST_PUT,bitmap);
            break;
        case 1:
            sprintf((char *)disp_rec,"~C8%s~C0",yes);
            print16(yw.x1+AYN_EDGE_SIZE, yw.y1+AYN_EDGE_SIZE+AYN_RY, (USTR *)disp_rec,PEST_PUT,bitmap);
            sprintf((char *)disp_rec,"~C0%s~C0",no);
            print16(nw.x1+AYN_EDGE_SIZE, nw.y1+AYN_EDGE_SIZE+AYN_RY, (USTR *)disp_rec,COPY_PUT_COLOR(SYSTEM_BLUE),bitmap);
            break;
        }
        update_screen(bitmap);
        ch=read_system_key();
        if(ch) reset_key();
        else ch=get_mouse_key();
        get_mouse_position(&mouse_x,&mouse_y);
        switch(ch)
        {
        case Left:
            select=0;
            break;
        case Right:
            select=1;
            break;
        case Enter:
            main_pass=1;
            break;
        case Esc:
            select=1;
            main_pass=1;
            break;
        case MS_Move:
            if(mouse_x>=yw.x1 && mouse_x<yw.x2 && mouse_y>=yw.y1 && mouse_y<yw.y2)
                select=0;
            else if(mouse_x>=nw.x1 && mouse_x<nw.x2 && mouse_y>=nw.y1 && mouse_y<nw.y2)
                select=1;
            else
                ;
            break;
        case MS_LDn:
            if(mouse_x>=yw.x1 && mouse_x<yw.x2 && mouse_y>=yw.y1 && mouse_y<yw.y2)
            {
                select=0;
                main_pass=1;
            }
            else if(mouse_x>=nw.x1 && mouse_x<nw.x2 && mouse_y>=nw.y1 && mouse_y<nw.y2)
            {
                select=1;
                main_pass=1;
            }
            reset_mouse();
            break;
        }
    }
    if(backup_bitmap)
    {
        put_bitmap(0,0,backup_bitmap,screen_buffer);
        destroy_bitmap(&backup_bitmap);
    }
    update_screen(screen_buffer);
    return (1==select)?TTN_NOT_OK:TTN_OK;
}


EXPORT  SLONG   FNBACK  ask_yes_no(USTR *title,USTR *yes,USTR *no,BMP *bitmap)
{
    SLONG   main_pass;
    UCHR    ch;
    SLONG   x[3][2],y[3][2],i;
    SLONG   len,adjust,select;
    SLONG   mouse_x,mouse_y;
    USTR    disp_rec[1024];
    BMP*    backup_bitmap;


    backup_bitmap = copy_bitmap(0,0,bitmap->w,bitmap->h,bitmap);
    for(i=0;i<3;i++)
    {
        x[i][0]=0;
        y[i][0]=bitmap->h/2-8;
        x[i][1]=0;
        y[i][1]=16;
    }
    len=((SLONG)strlen((const char *)title)+1)/2*16;
    x[0][1]=x[0][0]+len;
    
    x[1][0]=x[0][1]+18;
    len=((SLONG)strlen((const char *)yes)+1)/2*16;
    x[1][1]=x[1][0]+len;
    
    x[2][0]=x[1][1]+18;
    len=((SLONG)strlen((const char *)no)+1)/2*16;
    x[2][1]=x[2][0]+len;
    
    adjust=(bitmap->w-(x[2][1]-x[0][0]))/2;
    for(i=0;i<3;i++)
    {
        x[i][1]-=x[i][0];
        x[i][0]+=adjust;
    }
    main_pass=0;
    select=1;
    reset_key();
    reset_mouse();
    while(main_pass==0)
    {
        idle_loop();
        if(backup_bitmap)
        {
            put_bitmap(0,0,backup_bitmap,bitmap);
        }
        for(i=0;i<3;i++)
            put_menu_box(x[i][0]-4,y[i][0]-4,x[i][1]+8,y[i][1]+8,bitmap);
        sprintf((char *)disp_rec,"~C8%s~C0",title);
        print16(x[0][0],y[0][0],(USTR *)disp_rec,PEST_PUT,bitmap);
        switch(select)
        {
        case 0:
            sprintf((char *)disp_rec,"~C0%s~C0",yes);
            print16(x[1][0],y[1][0],(USTR *)disp_rec,COPY_PUT_COLOR(SYSTEM_BLUE),bitmap);
            sprintf((char *)disp_rec,"~C8%s~C0",no);
            print16(x[2][0],y[2][0],(USTR *)disp_rec,PEST_PUT,bitmap);
            break;
        case 1:
            sprintf((char *)disp_rec,"~C8%s~C0",yes);
            print16(x[1][0],y[1][0],(USTR *)disp_rec,PEST_PUT,bitmap);
            sprintf((char *)disp_rec,"~C0%s~C0",no);
            print16(x[2][0],y[2][0],(USTR *)no,COPY_PUT_COLOR(SYSTEM_BLUE),bitmap);
            break;
        }
        update_screen(bitmap);
        ch=read_system_key();
        if(ch) reset_key();
        else ch=get_mouse_key();
        get_mouse_position(&mouse_x,&mouse_y);
        switch(ch)
        {
        case Left:
            select=0;
            break;
        case Right:
            select=1;
            break;
        case Enter:
            main_pass=1;
            break;
        case Esc:
            select=1;
            main_pass=1;
            break;
        case MS_Move:
            if(mouse_x>=x[1][0]-4 && mouse_x<x[1][0]+x[1][1]+4 && mouse_y>=y[1][0]-4 && mouse_y<y[1][0]+y[1][1]+4)
                select=0;
            else if(mouse_x>=x[2][0]-4 && mouse_x<x[2][0]+x[2][1]+4 && mouse_y>=y[2][0]-4 && mouse_y<y[2][0]+y[2][1]+4)
                select=1;
            else
                ;
            break;
        case MS_LDn:
            if(mouse_x>=x[1][0]-4 && mouse_x<x[1][0]+x[1][1]+4 && mouse_y>=y[1][0]-4 && mouse_y<y[1][0]+y[1][1]+4)
            {
                select=0;
                main_pass=1;
            }
            else if(mouse_x>=x[2][0]-4 && mouse_x<x[2][0]+x[2][1]+4 && mouse_y>=y[2][0]-4 && mouse_y<y[2][0]+y[2][1]+4)
            {
                select=1;
                main_pass=1;
            }
            reset_mouse();
            break;
        }
    }
    if(backup_bitmap)
    {
        put_bitmap(0,0,backup_bitmap,screen_buffer);
        destroy_bitmap(&backup_bitmap);
    }
    update_screen(screen_buffer);
    return (1==select)?TTN_NOT_OK:TTN_OK;
}

EXPORT  void    FNBACK  display_message16(USTR *message,SLONG flag,BMP *bitmap)
{
    SLONG   ll;
    SLONG   x,y;
    SLONG   xl,yl;
    
    ll=strlen((const char *)message);
    y=bitmap->h/2-8;
    x=bitmap->w/2-((ll+1)/2*8);
    yl=32;
    xl=(ll+1)/2*16+16;
    put_menu_box(x,y,xl,yl,bitmap);
    print16(x+8,y+8,message,PEST_PUT,bitmap);
    switch(flag)
    {
    case DISPLAY_MESSAGE_TO_SCREEN:
        update_screen(bitmap);
        break;
    case DISPLAY_MESSAGE_TO_BUFFER:
        break;
    default:
        break;
    }
}

EXPORT  void    FNBACK  redraw_current_path(BMP *bitmap)
{
    SLONG   cur_drive;
    USTR    temp_path[_MAX_PATH];

    cur_drive=_getdrive();
    _getdcwd(cur_drive,(char *)temp_path,_MAX_PATH);
    print16(0,bitmap->h-20,(USTR *)adjust_file_path((USTR *)temp_path),PEST_PUT,bitmap);
}


EXPORT  SLONG   FNBACK  select_popup_menu(SLONG sx,SLONG sy,USTR *options,BMP *bitmap)
{
    POPWIN popwin;
    MENUOPT *popwin_opt=NULL;
    SLONG choice;
    SLONG i,count,last_offset;
    
    init_popwin(&popwin);
    count=0;
    for(i=0; ; i++)
    {
        if('\0' == options[i] )
        {
            count ++;
            if('\0' == options[i+1])
                break;
        }
    }

    popwin_opt = (MENUOPT *)GlobalAlloc(GPTR, sizeof(MENUOPT)*count);
    if(popwin_opt == NULL)
        return -1;
    count = 0;
    last_offset = 0;
    for(i=0; ; i++)
    {
        if('\0' == options[i] )
        {
            if('\0' == options[i+1])
            {
                strcpy((char *)popwin_opt[count].name,(const char *)&options[last_offset]);
                popwin_opt[count].id = 1+count;
                count++;
                break;
            }
            else
            {
                strcpy((char *)popwin_opt[count].name,(const char *)&options[last_offset]);
                popwin_opt[count].id = 1+count;
                last_offset = i+1;
                count++;
            }
        }
    }

    for(i=0;i<count;++i)
        add_popwin_option(&popwin,&popwin_opt[i]);
    adjust_popwin_start(&popwin,1,sx,sy);
    reset_key();
    reset_mouse();
    choice = process_popwin_choice(&popwin);
    reset_key();
    reset_mouse();

    if(popwin_opt) GlobalFree(popwin_opt);

    //sprintf((char *)print_rec,"choice=%d",choice);
    //log_error(1,print_rec);

    return choice;
}


EXPORT  SLONG   FNBACK  load_file_to_buffer(USTR *filename, USTR **buffer)
{
    PACK_FILE  * fp = NULL;
    USTR    *file_buf;
    SLONG   file_size;

    if( NULL == ( fp = pack_fopen( ( const char* )filename, "rb" ) ) )
    {
        log_error( 1, "file %s open error", filename);
        goto error;
    }
    file_size = pack_fsize(fp);
    if(NULL == (file_buf = (USTR*)malloc(file_size)))
    {
        log_error(1, "memory alloc error");
        goto error;
    }
    pack_fread(file_buf, 1, file_size, fp);
    pack_fclose(fp);

    *buffer = file_buf;
    return  file_size;

error:
    if(fp) pack_fclose(fp);
    return  -1;
}


SLONG   get_buffer_number(USTR *buffer, SLONG *index)
{
    static  USTR    temp[256];
    SLONG   number;

    skip_compartment(buffer, index, (USTR*)" ,\x09",3);
    get_string(temp, 255, buffer, index,(USTR *)" ,\x09",3);
    number = string_2_number((char *)temp);
    return  number;
}


USTR *  get_buffer_string(USTR *buffer, SLONG *index)
{
    static  USTR    temp[256];

    skip_compartment(buffer, index, (USTR*)" ,\x09",3);
    get_string(temp, 255, buffer, index,(USTR *)" ,\x09",3);
    return  (USTR*)temp;
}


