/*
**      MENUWIN.CPP
**      Menuwindows functions.
**
**      ZJian,2000.09.07.
**          created.
**      ZJian,2000.10.22.
**          update process_popwin_choice():
**              added ready_active & end_active which making our choice selected by 
**              mouse left down & left up in the same area.
**      ZJian,2000.11.3.
**          added key_choice functions.
** 
*/
#include "rays.h"
#include "winmain.h"
#include "menutree.h"
#include "xsystem.h"
#include "xgrafx.h"
#include "xfont.h"
#include "xinput.h"
#include "menuwin.h"

static  MENU_RECT   menu_rect[MAX_MENU_RECT];
static  KEY_CHOICE  key_choice[MAX_KEY_CHOICE];


//
// KEY CHOICE FUNCTIONS ========================================================================
//
EXPORT  void    FNBACK  clear_key_choice(void)
{
    int i;

    for(i=0;i<MAX_KEY_CHOICE;i++)
    {
        key_choice[i].key=0;
    }
}



EXPORT  void    FNBACK  register_key_choice(KEY_CHOICE kc)
{
    SLONG i;

    for(i=0;i<MAX_KEY_CHOICE;i++)
    {
        if(key_choice[i].key==0)
        {
            key_choice[i] = kc;
            break;
        }
    }
}



EXPORT  void    FNBACK  kill_key_choice(UCHR key)
{
    int i;
    for(i=0;i<MAX_KEY_CHOICE;i++)
    {
        if(key_choice[i].key==key)
            key_choice[i].key=0;
    }
}



EXPORT  void    FNBACK  setup_key_choice(int nn,KEY_CHOICE *kc)
{
    int i;
    for(i=0;i<nn;i++)
    {
        register_key_choice(kc[i]);
    }
}



EXPORT  SLONG   FNBACK  get_key_choice(UCHR key)
{
    SLONG i;
    SLONG choice;

    choice=0;
    for(i=0;i<MAX_KEY_CHOICE;i++)
    {
        if(key_choice[i].key)
        {
            if(key_choice[i].key == key)
            {
                choice=key_choice[i].choice;
                break;
            }
        }
    }
    return choice;
}



//
// MENU RECT FUNCTIONS =========================================================================
//
EXPORT  void    FNBACK  register_menu_rect(MENU_RECT mr)
{
    for(SLONG i=0;i<MAX_MENU_RECT;i++)
    {
        if(menu_rect[i].id==0)
        {
            menu_rect[i].id=mr.id;
            menu_rect[i].sx=mr.sx;
            menu_rect[i].sy=mr.sy;
            menu_rect[i].xl=mr.xl;
            menu_rect[i].yl=mr.yl;
            break;
        }
    }
}

EXPORT  void    FNBACK  clear_menu_rect(void)
{
    int i;
    for(i=0;i<MAX_MENU_RECT;i++)
    {
        menu_rect[i].id=0;
    }
}

EXPORT  void    FNBACK  kill_menu_rect(ULONG id)
{
    int i;
    for(i=0;i<MAX_MENU_RECT;i++)
    {
        if(menu_rect[i].id==id)
            menu_rect[i].id=0;
    }
}

EXPORT  void    FNBACK  setup_menu_rect(int nn,MENU_RECT *mr)
{
    int i;
    for(i=0;i<nn;i++)
    {
        register_menu_rect(mr[i]);
    }
}

EXPORT  ULONG   FNBACK  get_menu_id(SLONG x,SLONG y)
{
    for(int i=0;i<MAX_MENU_RECT;i++)
    {
        if(menu_rect[i].id)
        {
            if(x>=menu_rect[i].sx && x<menu_rect[i].sx+menu_rect[i].xl
                && y>=menu_rect[i].sy && y<menu_rect[i].sy+menu_rect[i].yl)
            {
                return(menu_rect[i].id);
            }
        }
    }
    return(0);
}


//
// MENU WINDOW FUNCTIONS =======================================================================
//
EXPORT  void    FNBACK  init_menuwin(MENUWIN *mw)
{
    mw->flag=MENUWIN_FLAG_ACTIVE;
    mw->show=MENUWIN_SHOW_NORMAL;
    strcpy((char *)mw->title,"Ñ¡µ¥");
    //mw->option[MAX_MENUWIN_OPTION];
    mw->total_option=0;
    mw->start_option=0;
    mw->active_option=0;
    mw->win_sx=g_nScreenWidth/2;
    mw->win_sy=g_nScreenHeight/2;
    mw->win_xl=40;
    mw->win_yl=24+2;
    mw->opt_sx=3;
    mw->opt_sy=22;
    mw->opt_xl=mw->win_xl-6;
    mw->opt_yl=mw->win_yl-24;
    mw->org_sx=0;
    mw->org_sy=0;
    mw->action=MENUWIN_ACTION_STAND;
}

EXPORT  void    FNBACK  redraw_menuwin(MENUWIN *mw,BMP *bitmap)
{
    SLONG   i,len;
    SLONG   disp_x,disp_y;

    // redraw background bar
    put_menu_hibar(mw->win_sx,mw->win_sy,mw->win_xl,mw->win_yl,bitmap);

    // redraw title
    // ~C0 SYSTEM_WHITE
    if(mw->flag & MENUWIN_FLAG_ACTIVE)
        put_bar(mw->win_sx+2,mw->win_sy+2,mw->win_xl-4,18,SYSTEM_BLUE,bitmap);
    else
        put_bar(mw->win_sx+2,mw->win_sy+2,mw->win_xl-4,18,SYSTEM_DARK3,bitmap);

    sprintf((char *)print_rec,"~C0%s~C0",mw->title);
    print16(mw->win_sx+3,mw->win_sy+3,(USTR *)print_rec,PEST_PUT,bitmap);
    // redraw minimize button
    if(mw->show == MENUWIN_SHOW_NORMAL)
        put_minimize_button(mw->win_sx+mw->win_xl-20,mw->win_sy+3,1,bitmap);
    else
        put_maximize_button(mw->win_sx+mw->win_xl-20,mw->win_sy+3,1,bitmap);

    if(mw->show==MENUWIN_SHOW_NORMAL)
    {
        // redraw options
        // ~C8 SYSTEM_BLACK
        for(i=0;i<mw->total_option;i++)
        {
            disp_x=mw->win_sx+mw->opt_sx;
            disp_y=mw->win_sy+mw->opt_sy+i*20;
            if(mw->active_option==i)
            {
                half_put_bar(disp_x,disp_y,mw->opt_xl,20,SYSTEM_BLUE,bitmap);
                len=(SLONG)strlen((const char *)mw->option[i].name)*8;
                sprintf((char *)print_rec,"~C0%s~C0",mw->option[i].name);
                print16(disp_x+(mw->opt_xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
            }
            else
            {
                len=(SLONG)strlen((const char *)mw->option[i].name)*8;
                sprintf((char *)print_rec,"~C8%s~C0",mw->option[i].name);
                print16(disp_x+(mw->opt_xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
            }
        }
        // redraw vscroll bar
        // redraw hscroll bar
    }
}

EXPORT  void    FNBACK  set_menuwin_title(MENUWIN *mw,USTR *title)
{
    SLONG   len;
    strcpy((char *)mw->title,(const char *)title);
    len=strlen((const char *)mw->title)*8;
    len+=24+8;
    if(mw->win_xl<len)
    {
        mw->win_xl=len;
        mw->opt_xl=mw->win_xl-mw->opt_sx*2;
    }
}

EXPORT  SLONG   FNBACK  add_menuwin_option(MENUWIN *mw,MENUOPT *opt)
{
    SLONG   len;
    if(mw->total_option>=MAX_MENUWIN_OPTION-1)
        return(TTN_NOT_OK);
    memcpy(&mw->option[mw->total_option],opt,sizeof(MENUOPT));
    len=strlen((const char *)&mw->option[mw->total_option])*8;
    mw->total_option++;
    len+=16;
    if(mw->opt_xl<len)
    {
        mw->opt_xl=len;
        mw->win_xl=mw->opt_xl+mw->opt_sx*2;
    }
    if(mw->show==MENUWIN_SHOW_NORMAL)
    {
        mw->win_yl=24+mw->total_option*20+2;
    }
    mw->opt_yl=mw->total_option*20;
    return(TTN_OK);
}

EXPORT  void    FNBACK  clear_menuwin_option(MENUWIN *mw)
{
    mw->total_option=0;
    mw->opt_yl=mw->total_option*20;
    mw->win_yl=24+mw->total_option*20+2;
    mw->active_option=0;
    mw->start_option=0;
}

EXPORT  void    FNBACK  set_menuwin_show(MENUWIN *mw,SLONG show)
{
    mw->show=show;
    switch(mw->show)
    {
    case MENUWIN_SHOW_NORMAL:
        mw->win_yl=24+mw->total_option*20+2;
        break;
    case MENUWIN_SHOW_MINIMIZE:
        mw->win_yl=24+2;
        break;
    }
}

EXPORT  void    FNBACK  adjust_menuwin_start(MENUWIN *mw,SLONG adjust_flag,SLONG sx,SLONG sy)
{
    if(adjust_flag)
    {
        mw->win_sx=sx;
        mw->win_sy=sy;
    }
    else
    {
        mw->win_sx=(g_nScreenWidth-mw->win_xl)/2;
        mw->win_sy=(g_nScreenHeight-mw->win_yl)/2;
    }
}

EXPORT  SLONG   FNBACK  process_menuwin_choice(MENUWIN *mw)
{
    SLONG   process_x,process_y;
    SLONG   choice=-1,mx,my;
    UCHR    ch;

    // process keyboard response
    if(mw->flag & MENUWIN_FLAG_ACTIVE)
    {
        if(mw->show == MENUWIN_SHOW_NORMAL)
        {
            ch = (UCHR) toupper( read_data_key() );
            switch(ch)
            {
            case S_Up:
                if(mw->active_option>0)
                    mw->active_option--;
                reset_key();
                break;
            case S_Dn:
                if(mw->active_option<mw->total_option-1)
                    mw->active_option++;
                reset_key();
                break;
            case S_Enter:
                choice=mw->option[mw->active_option].id;
                reset_key();
                break;
            }
        }
    }
    // process mouse response
    ch=get_mouse_key();
    get_mouse_position(&mx,&my);
    switch(ch)
    {
    case MS_Move:
        if(mw->flag & MENUWIN_FLAG_ACTIVE)
        {
            if(mw->show == MENUWIN_SHOW_NORMAL)
            {
                process_x=mx-mw->win_sx;
                process_y=my-mw->win_sy;
                if(process_x>=mw->opt_sx && process_x<mw->opt_xl+mw->opt_sx && 
                    process_y>=mw->opt_sy && process_y<mw->opt_yl+mw->opt_sy)
                {
                    mw->active_option=(process_y-mw->opt_sy)/20;
                }
            }
            mw->action=MENUWIN_ACTION_STAND;
        }
        break;
    case MS_LDn:
        process_x=mx-mw->win_sx;
        process_y=my-mw->win_sy;
        if(process_x>=0 && process_x<mw->win_xl && process_y>=0 && process_y<mw->win_yl)
        {
            mw->flag |= MENUWIN_FLAG_ACTIVE;
        }
        else
        {
            mw->flag &= ~MENUWIN_FLAG_ACTIVE;
        }
        if(mw->flag & MENUWIN_FLAG_ACTIVE)
        {
            if(process_x>=0 && process_x<mw->win_xl-20 && process_y>=0 && process_y<22)
            {
                mw->org_sx=mw->win_sx-mx;
                mw->org_sy=mw->win_sy-my;
                mw->action=MENUWIN_ACTION_MOVE;
            }
            else if(process_x>=mw->opt_sx && process_x<mw->opt_xl+mw->opt_sx && 
                    process_y>=mw->opt_sy && process_y<mw->opt_yl+mw->opt_sy)
            {
                if(mw->show == MENUWIN_SHOW_NORMAL)
                {
                    mw->active_option=(process_y-mw->opt_sy)/20;
                }
            }
        }
        break;
    case MS_LDrag:
        if(mw->flag & MENUWIN_FLAG_ACTIVE)
        {
            if(mw->action==MENUWIN_ACTION_MOVE)
            {
                mw->win_sx=mw->org_sx+mx;
                mw->win_sy=mw->org_sy+my;
            }
        }
        break;
    case MS_LUp:
        if(mw->flag & MENUWIN_FLAG_ACTIVE)
        {
            process_x=mx-mw->win_sx;
            process_y=my-mw->win_sy;
            if(process_x>=0 && process_x<mw->win_xl-20 && process_y>=0 && process_y<22)
            {
                mw->action=MENUWIN_ACTION_STAND;
                reset_mouse();
            }
            else if(process_x>=mw->win_xl-20 && process_x<mw->win_xl && process_y>=0 && process_y<22)
            {
                if(mw->show==MENUWIN_SHOW_NORMAL)
                    set_menuwin_show(mw,MENUWIN_SHOW_MINIMIZE);
                else
                    set_menuwin_show(mw,MENUWIN_SHOW_NORMAL);
                reset_mouse();
            }
            else if(process_x>=mw->opt_sx && process_x<mw->opt_xl+mw->opt_sx && 
                    process_y>=mw->opt_sy && process_y<mw->opt_yl+mw->opt_sy)
            {
                if(mw->show == MENUWIN_SHOW_NORMAL)
                {
                    mw->active_option=(process_y-mw->opt_sy)/20;
                    choice=mw->option[mw->active_option].id;
                    reset_mouse();
                }
            }
        }
        break;
    }
    return(choice);
}

EXPORT  void    FNBACK  set_menuwin_active_option(MENUWIN *p,SLONG option_id)
{
    SLONG i;
    if( !(p && option_id) ) return;
    for(i=0;i<p->total_option;++i)
    {
        if(p->option[i].id == option_id)
            p->active_option = i;
    }
}


//
// EDIT WINDOW FUNCTIONS =====================================================================
//
EXPORT  void    FNBACK  init_editwin(EDITWIN *ew)
{
    ew->flag=MENUWIN_FLAG_ACTIVE;
    ew->show=MENUWIN_SHOW_NORMAL;
    strcpy((char *)ew->title,"ÊäÈë");
    memset((char *)ew->string,0x00,MAX_EDIT_LENGTH);
    ew->string_index=0;
    ew->string_max=0;
    ew->edit_flag=EDITWIN_EDIT_INS;
    ew->cursor_tick=0;
    ew->win_sx=40;
    ew->win_sy=g_nScreenHeight-80;
    ew->win_xl=240;
    ew->win_yl=24+2+20;
    ew->opt_sx=3;
    ew->opt_sy=22;
    ew->opt_xl=ew->win_xl-6;
    ew->opt_yl=ew->win_yl-24;
    ew->action=MENUWIN_ACTION_STAND;
    ew->org_sx=0;
    ew->org_sy=0;
}

EXPORT  void    FNBACK  redraw_editwin(EDITWIN *ew,BMP *bitmap)
{
    SLONG   disp_x,disp_y;
    USTR    temp[MAX_EDIT_LENGTH];

    // redraw background bar
    put_menu_hibar(ew->win_sx,ew->win_sy,ew->win_xl,ew->win_yl,bitmap);

    // redraw title
    // ~C0 SYSTEM_WHITE
    if(ew->flag & MENUWIN_FLAG_ACTIVE)
        put_bar(ew->win_sx+2,ew->win_sy+2,ew->win_xl-4,18,SYSTEM_BLUE,bitmap);
    else
        put_bar(ew->win_sx+2,ew->win_sy+2,ew->win_xl-4,18,SYSTEM_DARK3,bitmap);
    sprintf((char *)print_rec,"~C0%s~C0",ew->title);
    print16(ew->win_sx+3,ew->win_sy+3,(USTR *)print_rec,PEST_PUT,bitmap);

    // redraw minimize button
    if(ew->show == MENUWIN_SHOW_NORMAL)
        put_minimize_button(ew->win_sx+ew->win_xl-20,ew->win_sy+3,1,bitmap);
    else
        put_maximize_button(ew->win_sx+ew->win_xl-20,ew->win_sy+3,1,bitmap);

    // redraw string
    // ~C8 SYSTEM_BLACK
    memcpy(temp,ew->string,MAX_EDIT_LENGTH);
    temp[ew->string_max]=0x00;
    sprintf((char *)print_rec,"~C8%s~C0",temp);
    disp_x=ew->win_sx+ew->opt_sx;
    disp_y=ew->win_sy+ew->opt_sy;
    print16(disp_x,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);

    // redraw cursor
    ew->cursor_tick++;
    if(ew->cursor_tick>30)
    {
        ew->cursor_tick=0;
    }
    else if(ew->cursor_tick>10)
    {
        disp_x=ew->win_sx+ew->opt_sx;
        disp_y=ew->win_sy+ew->opt_sy;
        disp_x += ew->string_index*8+1;
        if(ew->edit_flag==EDITWIN_EDIT_OVER)
            put_bar(disp_x,disp_y+2,8,16,SYSTEM_YELLOW,bitmap);
        else
            put_bar(disp_x,disp_y+2,2,16,SYSTEM_YELLOW,bitmap);
    }
}

EXPORT  void    FNBACK  set_editwin_title(EDITWIN *ew,USTR *title)
{
    if(strlen((const char *)title)<MAX_TITLE_LENGTH)
    {
        strcpy((char *)ew->title,(const char *)title);
    }
}

EXPORT  void    FNBACK  set_editwin_string(EDITWIN *ew,USTR *string)
{
    SLONG   len;
    if(strlen((const char *)string)<MAX_EDIT_LENGTH)
    {
        len=strlen((const char *)string);
        strcpy((char *)ew->string,(const char *)string);
        ew->string_max=len;
        ew->string_index=len;
    }
}

EXPORT  void    FNBACK  adjust_editwin_start(EDITWIN *ew,SLONG adjust_flag,SLONG sx,SLONG sy)
{
    if(adjust_flag)
    {
        ew->win_sx=sx;
        ew->win_sy=sy;
    }
    else
    {
        ew->win_sx=(g_nScreenWidth-ew->win_xl)/2;
        ew->win_sy=(g_nScreenHeight-ew->win_yl)/2;
    }
}

EXPORT  SLONG   FNBACK  process_editwin_input(EDITWIN *ew)
{
    SLONG   process_x,process_y;
    SLONG   choice=-1;
    SLONG   i,mx,my;
    UCHR    ch;

    ch=read_data_key();
    if(ch)
    {
        if(ew->flag & MENUWIN_FLAG_ACTIVE)
        {
            switch(ch)
            {
            case S_Left:
                ew->string_index--;
                if(ew->string_index<0)
                    ew->string_index=0;
                reset_key();
                break;
            case S_Right:
                ew->string_index++;
                if(ew->string_index>ew->string_max)
                    ew->string_index=ew->string_max;
                if(ew->string_index>28)
                    ew->string_index=28;
                reset_key();
                break;
            case S_Enter:
                choice=1;
                reset_key();
                break;
            case S_Del:
                if(ew->string_index<ew->string_max)
                {
                    for(i=ew->string_index;i<27;i++)
                        ew->string[i]=ew->string[i+1];
                    ew->string_max--;
                    if(ew->string_max<0)
                        ew->string_max=0;
                }
                reset_key();
                break;
            case S_Backspace:
                if(ew->string_index>0)
                {
                    for(i=ew->string_index-1;i<27;i++)
                        ew->string[i]=ew->string[i+1];
                    ew->string[27]=' ';
                    ew->string_index--;
                    if(ew->string_max>0)
                        ew->string_max--;
                }
                reset_key();
                break;
            case S_Ins:
                if(ew->edit_flag == EDITWIN_EDIT_OVER)
                    ew->edit_flag = EDITWIN_EDIT_INS;
                else
                    ew->edit_flag = EDITWIN_EDIT_OVER;
                reset_key();
                break;
            case S_Home:
                ew->string_index=0;
                break;
            case S_End:
                ew->string_index=ew->string_max;
                break;
            case ' ':
                break;
            default:
                // we can use a parameter to check valid string for this ...
                if( (ch>='0' && ch<='9') || ( ch>='A' && ch<='Z') || (ch>='a' && ch<='z')
                    || (ch=='.') || (ch=='_') )
                {
                    if(ew->edit_flag == EDITWIN_EDIT_OVER)
                    {
                        ew->string[ew->string_index]=ch;
                        if(ew->string_index<28)
                            ew->string_index++;
                        if(ew->string_index>ew->string_max)
                            ew->string_max=ew->string_index;
                    }
                    else if(ew->edit_flag == EDITWIN_EDIT_INS)
                    {
                        for(i=27;i>=ew->string_index;i--)
                            ew->string[i+1]=ew->string[i];
                        ew->string[ew->string_index]=ch;
                        if(ew->string_max<28)
                            ew->string_max++;
                        if(ew->string_index<28)
                            ew->string_index++;
                    }
                    reset_data_key();
                }
                break;
            }
        }
    }
    else
    {
        ch=get_mouse_key();
        get_mouse_position(&mx,&my);
        switch(ch)
        {
        case MS_Move:
            if(ew->flag & MENUWIN_FLAG_ACTIVE)
            {
                ew->action=MENUWIN_ACTION_STAND;
            }
            break;
        case MS_LDn:
            process_x=mx-ew->win_sx;
            process_y=my-ew->win_sy;
            if(process_x>=0 && process_x<ew->win_xl && process_y>=0 && process_y<ew->win_yl)
            {
                ew->flag |= MENUWIN_FLAG_ACTIVE;
            }
            else
            {
                ew->flag &= ~MENUWIN_FLAG_ACTIVE;
            }
            if(ew->flag & MENUWIN_FLAG_ACTIVE)
            {
                if(process_x>=0 && process_x<ew->win_xl-20 && process_y>=0 && process_y<22)
                {
                    ew->org_sx=ew->win_sx-mx;
                    ew->org_sy=ew->win_sy-my;
                    ew->action=MENUWIN_ACTION_MOVE;
                }
            }
            break;
        case MS_LDrag:
            if(ew->flag & MENUWIN_FLAG_ACTIVE)
            {
                if(ew->action==MENUWIN_ACTION_MOVE)
                {
                    ew->win_sx=ew->org_sx+mx;
                    ew->win_sy=ew->org_sy+my;
                }
            }
            break;
        case MS_LUp:
            if(ew->flag & MENUWIN_FLAG_ACTIVE)
            {
                process_x=mx-ew->win_sx;
                process_y=my-ew->win_sy;
                if(process_x>=0 && process_x<ew->win_xl-20 && process_y>=0 && process_y<22)
                {
                    ew->action=MENUWIN_ACTION_STAND;
                    reset_mouse();
                }
            }
            break;
        }
    }
    return(choice);
}

EXPORT  USTR *  FNBACK  get_editwin_string(EDITWIN *ew)
{
    static  USTR    temp[MAX_EDIT_LENGTH];
    memcpy(temp,ew->string,MAX_EDIT_LENGTH);
    temp[ew->string_max]=0x00;
    return((USTR *)temp);
}

// POP WIN FUNCTIONS ===========================================================
EXPORT  void    FNBACK  init_popwin(POPWIN *p)
{
    p->option[MAX_MENUWIN_OPTION];
    p->total_option=0; 
    p->active_option=0;
    p->win_sx=g_nScreenWidth/2;
    p->win_sy=g_nScreenHeight/2;
    p->win_xl=2;
    p->win_yl=2;
    p->opt_sx=2;
    p->opt_sy=2;
    p->opt_xl=p->win_xl;
    p->opt_yl=p->win_yl;
}

EXPORT  void    FNBACK  redraw_popwin(POPWIN *p,BMP *bitmap)
{
    SLONG   i,len;
    SLONG   disp_x,disp_y;

    // redraw background bar
    put_menu_hibar(p->win_sx,p->win_sy,p->win_xl,p->win_yl,bitmap);

    // redraw options
    // ~C8 SYSTEM_BLACK
    for(i=0;i<p->total_option;i++)
    {
        disp_x=p->win_sx+p->opt_sx;
        disp_y=p->win_sy+p->opt_sy+i*20;
        if(p->active_option==i)
        {
            half_put_bar(disp_x,disp_y,p->opt_xl,20,SYSTEM_BLUE,bitmap);
            len=(SLONG)strlen((const char *)p->option[i].name)*8;
            sprintf((char *)print_rec,"~C0%s~C0",p->option[i].name);
            print16(disp_x+(p->opt_xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
        }
        else
        {
            len=(SLONG)strlen((const char *)p->option[i].name)*8;
            sprintf((char *)print_rec,"~C8%s~C0",p->option[i].name);
            print16(disp_x+(p->opt_xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
        }
    }
}

EXPORT  SLONG   FNBACK  add_popwin_option(POPWIN *p,MENUOPT *opt)
{
    SLONG   len;
    if(p->total_option>=MAX_MENUWIN_OPTION-1)
        return(TTN_NOT_OK);
    p->option[p->total_option]=*opt;
    len=strlen((const char *)&p->option[p->total_option])*8;
    p->total_option++;
    len+=16;
    if(p->opt_xl<len)
    {
        p->opt_xl=len;
        p->win_xl=p->opt_xl+p->opt_sx*2;
    }
    p->win_yl=p->total_option*20+2*p->opt_sy;
    p->opt_yl=p->total_option*20;
    return(TTN_OK);
}

EXPORT  void    FNBACK  adjust_popwin_start(POPWIN *p,SLONG adjust_flag,SLONG sx,SLONG sy)
{
    if(adjust_flag)
    {
        p->win_sx=sx;
        if(p->win_sx+p->win_xl>=g_nScreenWidth)
            p->win_sx=g_nScreenWidth-p->win_xl;
        if(p->win_sx<0)
            p->win_sx=0;
        p->win_sy=sy;
        if(p->win_sy+p->win_yl>=g_nScreenHeight)
            p->win_sy=g_nScreenHeight-p->win_yl;
        if(p->win_sy<0)
            p->win_sy=0;
    }
    else
    {
        p->win_sx=(g_nScreenWidth-p->win_xl)/2;
        p->win_sy=(g_nScreenHeight-p->win_yl)/2;
    }
}

EXPORT  SLONG   FNBACK  process_popwin_choice(POPWIN *p)
{
    SLONG   main_pass=0,choice=0;
    SLONG   process_x,process_y;
    SLONG   mx,my,old_mx,old_my;
    SLONG   ready_active;
    SLONG   end_active;
    UCHR    ch;
    BMP *   backup_bitmap = NULL;

    reset_mouse();
    get_mouse_position(&old_mx,&old_my);
    reset_key();
    backup_bitmap = copy_bitmap(0,0,screen_buffer->w,screen_buffer->h,screen_buffer);
    ready_active=0;
    end_active=-1;
    while(0==main_pass)
    {
        idle_loop();
        if(backup_bitmap)
            put_bitmap(0,0,backup_bitmap,screen_buffer);
        redraw_popwin(p,screen_buffer);
        update_screen(screen_buffer);
        ch=read_data_key();
        if(ch) reset_key();
        else ch=get_mouse_key();
        get_mouse_position(&mx,&my);
        switch(ch)
        {
        case S_Up:
            if(p->active_option>0)
                p->active_option--;
            break;
        case S_Dn:
            if(p->active_option<p->total_option-1)
                p->active_option++;
            break;
        case S_Enter:case ' ':
            choice=p->option[p->active_option].id;
            main_pass=1;
            break;
        case S_Esc:
            choice=0;
            main_pass=1;
            break;
        case MS_Move://case MS_LDrag:case MS_RDrag:
            process_x=mx-p->win_sx;
            process_y=my-p->win_sy;
            if(process_x>=p->opt_sx && process_x<p->opt_xl+p->opt_sx && 
                process_y>=p->opt_sy && process_y<p->opt_yl+p->opt_sy)
            {
                p->active_option=(process_y-p->opt_sy)/20;
            }
            break;
        case MS_LDn:
            process_x=mx-p->win_sx;
            process_y=my-p->win_sy;
            if(process_x>=p->opt_sx && process_x<p->opt_xl+p->opt_sx && 
                process_y>=p->opt_sy && process_y<p->opt_yl+p->opt_sy)
            {
                p->active_option=(process_y-p->opt_sy)/20;
                ready_active=p->active_option;
            }
            else
            {
                ready_active=-1;
            }
            reset_mouse();
            break;
        case MS_LUp:
            process_x=mx-p->win_sx;
            process_y=my-p->win_sy;
            if(process_x>=p->opt_sx && process_x<p->opt_xl+p->opt_sx && 
                process_y>=p->opt_sy && process_y<p->opt_yl+p->opt_sy)
            {
                p->active_option=(process_y-p->opt_sy)/20;
                end_active=p->active_option;
            }
            else
            {
                end_active=-1;
            }
            if(ready_active == end_active)
            {
                if(end_active>=0)
                    choice=p->option[p->active_option].id;
                else
                    choice=0;
                main_pass=1;
                break;
            }
            reset_mouse();
            break;
        default:
            break;
        }
    }
    if(backup_bitmap)
    {
        put_bitmap(0,0,backup_bitmap,screen_buffer);
        destroy_bitmap(&backup_bitmap);
    }
    reset_key();
    reset_mouse();
    //set_mouse_position(old_mx,old_my);
    return choice;
}


EXPORT  void    FNBACK  set_popwin_active_option(POPWIN *p,SLONG option_id)
{
    SLONG i;
    if( !(p && option_id) ) return;
    for(i=0;i<p->total_option;++i)
    {
        if(p->option[i].id == option_id)
            p->active_option = i;
    }
}


// BAR WINDOW FUNCTIONS =======================================================
EXPORT  void    FNBACK  init_barwin(BARWIN *p)
{
    p->total_option=0;
    p->active_option=0;
    p->win_sx=0;
    p->win_sy=g_nScreenHeight-BARWIN_HEIGHT;
    p->win_xl=g_nScreenWidth;
    p->win_yl=BARWIN_HEIGHT;
    p->opt_sx[0]=p->win_sx+4;
    for(SLONG i=0;i<MAX_MENUWIN_OPTION;i++)
    {
        p->opt_sy[i]=(BARWIN_HEIGHT-20)/2;
        p->opt_yl[i]=20;
    }
}

EXPORT  void    FNBACK  redraw_barwin(BARWIN *p,BMP *bitmap)
{
    SLONG   i,len;
    SLONG   disp_x,disp_y,xl,yl;

    // redraw background bar
    put_menu_hibar(p->win_sx,p->win_sy,p->win_xl,p->win_yl,bitmap);
    // redraw options
    // ~C8 SYSTEM_BLACK
    for(i=0;i<p->total_option;i++)
    {
        disp_x=p->win_sx+p->opt_sx[i];
        disp_y=p->win_sy+p->opt_sy[i];
        xl=p->opt_xl[i];
        yl=p->opt_yl[i];
        put_menu_hibar(disp_x,disp_y,xl,yl,bitmap);
        if(p->active_option==i)
        {
            half_put_bar(disp_x+1,disp_y+1,xl-2,yl-2,SYSTEM_BLUE,bitmap);
            len=(SLONG)strlen((const char *)p->option[i].name)*8;
            sprintf((char *)print_rec,"~C0%s~C0",p->option[i].name);
            print16(disp_x+(xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
        }
        else
        {
            len=(SLONG)strlen((const char *)p->option[i].name)*8;
            sprintf((char *)print_rec,"~C8%s~C0",p->option[i].name);
            print16(disp_x+(xl-len)/2,disp_y+2,(USTR *)print_rec,PEST_PUT,bitmap);
        }
    }
}

EXPORT  SLONG   FNBACK  add_barwin_option(BARWIN *p,MENUOPT *opt)
{
    SLONG   len;
    if(p->total_option>=MAX_MENUWIN_OPTION-1)
        return(TTN_NOT_OK);
    p->option[p->total_option]=*opt;
    len=strlen((const char *)&p->option[p->total_option])*8+16;
    p->opt_xl[p->total_option]=len;
    if(p->total_option+1<MAX_MENUWIN_OPTION)
        p->opt_sx[p->total_option+1]=p->opt_sx[p->total_option]+p->opt_xl[p->total_option]+4;
    p->total_option++;
    return(TTN_OK);
}

EXPORT  void    FNBACK  change_barwin_option_string(BARWIN *p,SLONG index,USTR *string)
{
    if(index>=0 && index<MAX_MENUWIN_OPTION-1)
    {
        strcpy((char *)p->option[index].name,(const char *)string);
        SLONG len=strlen((const char *)&p->option[index])*8+16;
        for(SLONG i=index;i<p->total_option;i++)
        {
            p->opt_xl[index]=len;
            if(index+1<MAX_MENUWIN_OPTION)
            {
                p->opt_sx[index+1]=p->opt_sx[index]+p->opt_xl[index]+4;
            }
        }
    }
}

EXPORT  void    FNBACK  adjust_barwin_start(BARWIN *p,SLONG adjust_flag,SLONG sx,SLONG sy)
{
    if(adjust_flag)
    {
        p->win_sx=sx;
        if(p->win_sx+p->win_xl>=g_nScreenWidth)
            p->win_sx=g_nScreenWidth-p->win_xl;
        if(p->win_sx<0)
            p->win_sx=0;
        p->win_sy=sy;
        if(p->win_sy+p->win_yl>=g_nScreenHeight)
            p->win_sy=g_nScreenHeight-p->win_yl;
        if(p->win_sy<0)
            p->win_sy=0;
    }
    else
    {
        p->win_sx=0;
        p->win_sy=g_nScreenHeight-BARWIN_HEIGHT;
    }
}

EXPORT  SLONG   FNBACK  process_barwin_choice(BARWIN *p)
{
    SLONG mx,my,i,choice=0;
    UCHR ch=get_mouse_key();

    get_mouse_position(&mx,&my);
    if(mx>=p->win_sx && mx<p->win_sx+p->win_xl && my>=p->win_sy && my<p->win_sy+p->win_yl )
    {
        switch(ch)
        {
        case MS_LDn:
            mx-=p->win_sx;
            my-=p->win_sy;
            for(i=0;i<p->total_option;i++)
            {
                if(mx>=p->opt_sx[i] && mx<p->opt_sx[i]+p->opt_xl[i] && my>=p->opt_sy[i] 
                    && my<p->opt_sy[i]+p->opt_yl[i])
                {
                    p->active_option=i;
                    choice=p->option[i].id;
                }
            }
            reset_mouse();
            break;
        default:
            break;
        }
    }
    return choice;
}

EXPORT  void    FNBACK  set_barwin_active_option(BARWIN *p,SLONG option_id)
{
    SLONG i;
    if( !(p && option_id) ) return;
    for(i=0;i<p->total_option;++i)
    {
        if(p->option[i].id == option_id)
            p->active_option = i;
    }
}


