/*
**      MENUTREE.CPP
**      MenuTree functions.
**
**      ZJian, 2000.08.17.
*/
#include "rays.h"
#include "menutree.h"
#include "xsystem.h"
#include "xgrafx.h"
#include "xfont.h"
#include "xinput.h"

#define MT_STACK_SIZE       4096
#define MENU_TEXT_SIZE      (16/2)

struct tagMT_STACK
{
    MENU_TREE   *tree[MT_STACK_SIZE];
    SLONG       flag[MT_STACK_SIZE];
    SLONG       top;
}mt_stack;

static  void        (*_draw_menutree_background_func)(BMP *bit_map)=NULL;
static  SLONG       (*_exit_menutree_func)(void)=NULL;
static  MENU_TREE * _mt_father(MENU_TREE *mt,MENU_TREE *p,SLONG flag);
static  MENU_TREE * _mt_rightest(MENU_TREE *p);
static  MENU_TREE * _mt_toppest(MENU_TREE *p);
static  MENU_TREE * _mt_first(MENU_TREE *mt,MENU_TREE *p);
static  MENU_TREE * _mt_find(MENU_TREE *mt,SLONG self_id);
static  void        _mt_list(MENU_TREE *mt);
static  void        _mt_append(MENU_TREE *mt,MENU_TREE *p,MENU_CELL *mc);
static  void        _mt_stack_clear(void);
static  void        _mt_stack_push(MENU_TREE *p,SLONG flag);
static  MENU_TREE * _mt_stack_pop(SLONG *flag);
static  SLONG       _mt_is_stack_empty(void);
static  void        _mt_dummy_redraw_background(BMP *bit_map);
static  MENU_TREE * _mt_key_down(MENU_TREE *p,UCHR key);
static  MENU_TREE * _mt_mouse_move(MENU_TREE *mt,MENU_TREE *p,SLONG mx,SLONG my);
static  MENU_TREE * _mt_mouse_left_down(MENU_TREE *mt,MENU_TREE *p,SLONG mx,SLONG my);

EXPORT  void    FNBACK  init_menutree(MENU_TREE **mt);
EXPORT  void    FNBACK  destroy_menutree(MENU_TREE **mt);
EXPORT  void    FNBACK  redraw_menutree(MENU_TREE *mt,MENU_TREE *p,BMP *bit_map);
EXPORT  void    FNBACK  setup_menutree(MENU_TREE *mt,MENU_CELL *mc);
EXPORT  void    FNBACK  exec_menutree(MENU_TREE *mt);
EXPORT  void    FNBACK  set_menutree_redraw(void (*func)(BMP *bit_map));
EXPORT  void    FNBACK  set_menutree_exit(SLONG (*func)(void));
EXPORT  SLONG   FNBACK  exit_menutree(void);

//===========================================================================================
EXPORT  void    FNBACK  init_menutree(MENU_TREE **mt)
{
    (*mt)=(MENU_TREE *)GlobalAlloc(GPTR, sizeof(MENU_TREE));
    (*mt)->father=NULL;
    (*mt)->left=NULL;
    (*mt)->right=NULL;
    (*mt)->mc=(MENU_CELL *)GlobalAlloc(GPTR, sizeof(MENU_CELL));
    (*mt)->mc->father_id=-1;
    (*mt)->mc->self_id=0;
    strcpy((char *)(*mt)->mc->name,(const char *)"ROOT");
    (*mt)->mc->sx=0;
    (*mt)->mc->sy=0;
    (*mt)->mc->xl=0;
    (*mt)->mc->yl=0;
    _draw_menutree_background_func=_mt_dummy_redraw_background;
}

EXPORT  void    FNBACK  destroy_menutree(MENU_TREE **mt)
{
    MENU_TREE *p;
    SLONG      sign=0;
    
    _mt_stack_clear();
    p=(*mt);
    while(!((p==NULL)&&(_mt_is_stack_empty())))
    {
        idle_loop();
        if(p) // search left
        {
            _mt_stack_push(p,1);
            p=p->left;
        }
        else
        {
            p=_mt_stack_pop(&sign);
            if(sign==1) // search right
            {
                _mt_stack_push(p,2);
                p=p->right;
            }
            else    // ok,visit p
            {
                if(p->mc)
                {
                    GlobalFree(p->mc);
                    p->mc=NULL;
                }
                if(p)
                {
                    GlobalFree(p);
                    p=NULL; // activate next loop
                }
            }
        }
    }
    // all data are GlobalFree, so we just need to ...
    (*mt)=NULL;
}

EXPORT  void    FNBACK  redraw_menutree(MENU_TREE *mt,MENU_TREE *p,BMP *bit_map)
{
    MENU_TREE *s,*t,*x=NULL;
    MENU_CELL *c;
    SLONG   main_pass;
    SLONG   left,top,right,bottom;
    SLONG   sign;
    
    main_pass=0;
    s=p;
    _mt_stack_clear();
    while((s!=mt)&&(s)) // backfind the active menutree node & push it to stack
    {
        _mt_stack_push(s,0);
        t=_mt_toppest(s);
        _mt_stack_push(t,0);
        s=t->father;
    }
    while(NULL!=(s=_mt_stack_pop(&sign)))
    {
        t=s;                    // toppest node
        x=_mt_stack_pop(&sign);  // active submenu node

        // put menu bar
        if(t->father==mt)
        {
            left=0;
            top=0;
            right=bit_map->w;
            bottom=20;
            put_menu_hibar(left,top,right-left,bottom-top,bit_map);
            //alpha_put_bar(left, top, right-left, bottom-top, SYSTEM_DARK2, bit_map, 128 );
        }
        else
        {
            left=bit_map->w;
            right=0;
            top=bit_map->h;
            bottom=0;
            while(t)
            {
                c=t->mc;
                if(left>c->sx) left=c->sx;
                if(top>c->sy) top=c->sy;
                if(right<c->sx+c->xl) right=c->sx+c->xl;
                if(bottom<c->sy+c->yl) bottom=c->sy+c->yl;
                t=t->right;
            }
            put_menu_hibar(left,top,right-left,bottom-top,bit_map);
            //alpha_put_bar(left, top, right-left, bottom-top, SYSTEM_DARK2, bit_map, 128 );
        }

        t=s;
        while(t)
        {
            c=t->mc;
            if(x==t)
            {
                put_bar(c->sx+2,c->sy+1,c->xl-4,c->yl-2,SYSTEM_BLUE,bit_map);
                //put_menu_hibar(c->sx+2,c->sy+1,c->xl-4,c->yl-2,bit_map);

                // ~C0 SYSTEM_WHITE
                sprintf((char *)print_rec,"~C0%s~C0",c->name);
                print16(c->sx+4,c->sy+2,(USTR *)print_rec,PEST_PUT,bit_map);
            }
            else
            {
                // ~C8 SYSTEM_BLACK
                sprintf((char *)print_rec,"~C8%s~C0",c->name);
                print16(c->sx+4,c->sy+2,(USTR *)print_rec,PEST_PUT,bit_map);
            }
            t=t->right;
        }
    }
}

EXPORT  void    FNBACK  setup_menutree(MENU_TREE *mt,MENU_CELL *mc)
{
    MENU_CELL *pmc=mc,*c=NULL;
    MENU_TREE *pf,*p=mt,*top=NULL;
    SLONG father_id=0;
    SLONG type=MENU_TITLE,flag;

    // add menucells to menutree ==============================
    while(pmc->self_id>=0)
    {
        idle_loop();
        father_id=pmc->father_id;
        if(father_id>=0)
        {
            pf=_mt_find(mt,father_id);  // find father
            if(pf)
            {
                _mt_append(mt,pf,pmc);
            }
        }
        pmc++;
    }

    // setup menutree positions ================================
    p=mt;
    _mt_stack_clear();
    while( p || !_mt_is_stack_empty() )
    {
        idle_loop();
        if(p)
        {
            // visit p first
            switch(type)
            {
            case MENU_TITLE:
                pf=p->father;
                c=p->mc;
                if(pf && c)
                {
                    top=_mt_toppest(pf);
                    if(pf->left==p) // sub menu
                    {
                        if(pf==mt)
                        {
                            c->sx = pf->mc->sx;
                            c->sy = pf->mc->sy;
                            c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                            c->yl = 20;
                        }
                        else if(top->father==mt)
                        {
                            c->sx = pf->mc->sx;
                            c->sy = pf->mc->sy+20;
                            c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                            c->yl = 20;
                        }
                        else
                        {
                            c->sx = pf->mc->sx+pf->mc->xl+1;
                            c->sy = pf->mc->sy;
                            c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                            c->yl = 20;
                        }
                    }
                    else //if(pf->right==p)   // brother menu
                    {
                        if(top->father==mt)
                        {
                            c->sx = pf->mc->sx+pf->mc->xl;
                            c->sy = pf->mc->sy;
                            c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                            c->yl = 20;
                        }
                        else
                        {
                            c->sx = pf->mc->sx;
                            c->sy = pf->mc->sy+20;
                            c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                            c->yl = 20;
                        }
                    }
                }
                break;
            case MENU_POPUP:
                pf=p->father;
                c=p->mc;
                if(pf && c)
                {
                    if(pf->left==p) // sub menu
                    {
                        c->sx = pf->mc->sx+pf->mc->xl+3;
                        c->sy = pf->mc->sy;
                        c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                        c->yl = 20;
                    }
                    else //if(pf->right==p)   // brother menu
                    {
                        c->sx = pf->mc->sx;
                        c->sy = pf->mc->sy+20;
                        c->xl = MENU_TEXT_SIZE*strlen((const char *)c->name)+8;
                        c->yl = 20;
                    }
                }
                break;
            }
            _mt_stack_push(p,1);
            p=p->right;
        }
        else
        {
            p=_mt_stack_pop(&flag);
            p=p->left;
        }
    }
}

EXPORT  void    FNBACK  exec_menutree(MENU_TREE *mt)
{
    MENU_TREE   *p=NULL,*t=NULL,*f=NULL,*s=NULL;
    SLONG   main_pass,mx,my;
    UCHR    ch;
    
    main_pass=0;
    p=mt->left;
    while(main_pass==0)
    {
        idle_loop();
        if(_draw_menutree_background_func)
        {
            _draw_menutree_background_func(screen_buffer);
        }
        redraw_menutree(mt,p,screen_buffer);
        update_screen(screen_buffer);
        ch = (UCHR) toupper( read_data_key() );
        if(ch!=(UCHR)NULL)
            reset_key();
        else
            ch=get_mouse_key();
        switch(ch)
        {
        case S_Left:
            t=_mt_toppest(p);
            if(t->father==mt)      // main menu
            {
                if(NULL!=(f=_mt_father(mt,p,MENU_BROTHER)))
                    p=f;
                else
                    p=_mt_rightest(p);
            }
            else                   // sub menus
            {
                p=t->father;
                t=_mt_toppest(p);
                if(t->father==mt)
                {
                    if(NULL!=(f=_mt_father(mt,p,MENU_BROTHER)))
                        p=f;
                    else
                        p=_mt_rightest(p);
                    if(p->left)
                        p=p->left;
                }
            }
            break;
        case S_Right:
            t=_mt_toppest(p);
            if(t->father==mt)      // main menu
            {
                if(p->right)
                    p=p->right;
                else
                    p=t;
            }
            else                   // sub menus
            {
                if(p->left)
                    p=p->left;
                else
                {
                    p=_mt_first(mt,p);
                    if(p->right)
                        p=p->right;
                    else
                        p=_mt_toppest(p);
                    if(p->left)
                        p=p->left;
                }
            }
            break;
        case S_Up:
            if(NULL!=(f=_mt_father(mt,p,MENU_BROTHER)))
                p=f;
            else
                p=_mt_rightest(p);
            break;
        case S_Dn:
            t=_mt_toppest(p);
            if(t->father==mt)
                p=p->left;
            else
            {
                if(p->right)
                    p=p->right;
                else
                    p=_mt_toppest(p);
            }
            break;
        case S_Enter:case ' ':
            if(p->left)
            {
                p=p->left;
            }
            else
            {
                if(p->mc->func!=NULL) 
                {
                    if(p->mc->func==exit_menutree)
                        main_pass=p->mc->func();
                    else
                        p->mc->func();
                }
                p=_mt_first(mt,p);
            }
            break;
        case S_Esc:
            p=_mt_toppest(p);
            if(p->father==mt)
            {
                //main_pass=1;
            }
            else
                p=p->father;
            break;
        case MS_Move:   // process mouse move control
            get_mouse_position(&mx,&my);
            s=_mt_mouse_move(mt,p,mx,my);
            if(s) p=s;
            break;
        case MS_LDn:    // process mouse down control
            get_mouse_position(&mx,&my);
            s=_mt_mouse_left_down(mt,p,mx,my);
            if(s)
            {
                p=s;
                if(p->left)
                {
                    p=p->left;
                }
                else 
                {
                    if(p->mc->func!=NULL) 
                    {
                        if(p->mc->func==exit_menutree)
                            main_pass=p->mc->func();
                        else
                            p->mc->func();
                    }
                    p=_mt_first(mt,p);
                }
            }
            else
            {
                p=_mt_first(mt,p);
            }
            break;
        default:    // process key control
            s=_mt_key_down(p,ch);
            if(s)
            {
                p=s;
                if(p->left) 
                {
                    p=p->left;
                }
                else 
                {
                    if(p->mc->func!=NULL) 
                    {
                        if(p->mc->func==exit_menutree)
                            main_pass=p->mc->func();
                        else
                            p->mc->func();
                    }
                    p=_mt_first(mt,p);
                }
            }
            break;
        }
    }
}

EXPORT  void    FNBACK  set_menutree_redraw(void (*func)(BMP *bit_map))
{
    _draw_menutree_background_func=func;
}

EXPORT  void    FNBACK  set_menutree_exit(SLONG (*func)(void))
{
    _exit_menutree_func=func;
}


EXPORT  SLONG   FNBACK  exit_menutree(void)
{
    if(_exit_menutree_func)
        return _exit_menutree_func();
    else
        return 1;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// STATIC FUNCTIONS
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
static  void        _mt_stack_clear(void)
{
    int i;
    
    for(i=0;i<MT_STACK_SIZE;i++)
    {
        mt_stack.flag[i]=0;
        mt_stack.tree[i]=NULL;
    }
    mt_stack.top=-1;
}

static  void        _mt_stack_push(MENU_TREE *p,SLONG flag)
{
    if(mt_stack.top+1<MT_STACK_SIZE)
    {
        mt_stack.top++;
        mt_stack.tree[mt_stack.top]=p;
        mt_stack.flag[mt_stack.top]=flag;
    }
}

static  MENU_TREE * _mt_stack_pop(SLONG *flag)
{
    MENU_TREE *tree=NULL;
    if(mt_stack.top>=0)
    {
        *flag=mt_stack.flag[mt_stack.top];
        tree=mt_stack.tree[mt_stack.top];
        mt_stack.top--;
    }
    return(tree);
}

static  SLONG       _mt_is_stack_empty(void)
{
    if(mt_stack.top<0)
        return(1);
    else
        return(0);
}


static  void        _mt_dummy_redraw_background(BMP *bit_map)
{
    clear_bitmap(bit_map);
}


static  void    _mt_append(MENU_TREE *mt,MENU_TREE *p,MENU_CELL *mc)
{
    MENU_TREE *pf=NULL;
    MENU_TREE *s=NULL;
    MENU_CELL *c=NULL;

    if(!(mt && p && mc)) return;
    // create menu cell in memory to store mc data
    s=(MENU_TREE *)GlobalAlloc(GPTR, sizeof(MENU_TREE));
    s->left=NULL;
    s->right=NULL;
    s->father=NULL;
    c=(MENU_CELL *)GlobalAlloc(GPTR, sizeof(MENU_CELL));
    memcpy((char *)c,mc,sizeof(MENU_CELL));
    s->mc=c;
    if(p->left) // if had submenu(son),append as brother 
    {
        pf=p->left;
        while(pf->right)
            pf=pf->right;
        s->father=pf;
        pf->right=s;
    }
    else // else append as submenu(son)
    {
        pf=p;
        s->father=pf;
        pf->left=s;
    }
}

static  MENU_TREE * _mt_father(MENU_TREE *mt,MENU_TREE *p,SLONG flag)
{
    if(!(mt && p)) return NULL;
    if(p->father)
    {
        if(((flag==MENU_SON)&&(p->father->left==p))||((flag==MENU_BROTHER)&&(p->father->right==p)))
            return(p->father);
    }
    return(NULL);
}

static  MENU_TREE * _mt_rightest(MENU_TREE *p)
{
    MENU_TREE *s=p;
    if(!p) return NULL;
    while(s->right)
        s=s->right;
    return(s);
} 

static  MENU_TREE * _mt_toppest(MENU_TREE *p)
{
    MENU_TREE       *s;
    
    s=p;
    while(s->father)
    {
        if(s->father->right==s)
            s=s->father;
        else
            break;
    }
    return(s);
}


static  MENU_TREE * _mt_find(MENU_TREE *mt,SLONG self_id)
{
    MENU_TREE *find;
    MENU_TREE *p=mt;
    MENU_CELL *mc=NULL;
    SLONG flag=0;
    
    _mt_stack_clear();
    find=NULL;
    while( p || !_mt_is_stack_empty() )
    {
        idle_loop();
        if(p)   // search for left
        {
            _mt_stack_push(p,1);
            p=p->left;
        }
        else    // no left now 
        {
            p=_mt_stack_pop(&flag);
            if(1==flag) // search for right
            {
                _mt_stack_push(p,2);
                p=p->right;
            }
            else    // have searched left and right,visit it,then set to NULL to continue
            {
                mc=p->mc;
                if(mc)
                {
                    if(mc->self_id==self_id)
                    {
                        find=p;
                        return(find);
                    }
                }
                p=NULL;
            }
        }
    }
    return(find);
}

static  MENU_TREE * _mt_first(MENU_TREE *mt,MENU_TREE *p)
{
    MENU_TREE *s,*t;
    int pass=0;

    s=NULL;
    t=p;
    while(pass==0)
    {
        s=t;
        t=_mt_toppest(t);
        if(t->father)
        {
            if(t->father!=mt)
            {
                t=t->father;
            }
            else
            {
                pass=1;
            }
        }
    }
    return(s);
}

static  void        _mt_list(MENU_TREE *mt)
{
    MENU_TREE *p=mt;
    MENU_CELL *mc=NULL;
    SLONG flag=0;
    
    _mt_stack_clear();
    while( p || !_mt_is_stack_empty() )
    {
        idle_loop();
        if(p)   // search for left
        {
            _mt_stack_push(p,1);
            p=p->left;
        }
        else    // no left now 
        {
            p=_mt_stack_pop(&flag);
            if(1==flag) // search for right
            {
                _mt_stack_push(p,2);
                p=p->right;
            }
            else    // have searched left and right,visit it,then set to NULL to continue
            {
                mc=p->mc;
                if(mc)
                {
                    sprintf((char *)print_rec,"SID=%d  FID=%d  SX=%d  SY=%d  XL=%d  YL=%d  NAME=%s  KEY=%c",
                        mc->self_id,mc->father_id,mc->sx,mc->sy,mc->xl,mc->yl,mc->name,mc->key);
                    log_error(1,print_rec);
                }
                p=NULL;
            }
        }
    }
}

static  MENU_TREE * _mt_key_down(MENU_TREE *p,UCHR key)
{
    MENU_TREE *s;
    MENU_CELL *c=NULL;

    s=_mt_toppest(p);
    while(s)
    {
        c=s->mc;
        if(c)
        {
            if(c->key==toupper(key))
                return(s);
        }
        s=s->right;
    }
    return(NULL);
}

static  MENU_TREE * _mt_mouse_move(MENU_TREE *mt,MENU_TREE *p,SLONG mx,SLONG my)
{
    MENU_TREE *s;
    MENU_TREE *t=NULL;
    MENU_CELL *c=NULL;

    t=p;
    while(t!=mt)
    {
        idle_loop();
        s=_mt_toppest(t);
        while(s)
        {
            idle_loop();
            if(NULL!=(c=s->mc))
            {
                if(mx>=c->sx && mx<c->sx+c->xl && my>=c->sy && my<=c->sy+c->yl)
                {
                    if(s->left) s=s->left;  // expand sub menu options 
                    return(s);
                }
            }
            s=s->right;
        }
        t=t->father;
    }
    return(NULL);
}

static  MENU_TREE * _mt_mouse_left_down(MENU_TREE *mt,MENU_TREE *p,SLONG mx,SLONG my)
{
    MENU_TREE *s;
    MENU_TREE *t=NULL;
    MENU_CELL *c=NULL;

    t=p;
    while(t!=mt)
    {
        idle_loop();
        s=_mt_toppest(t);
        while(s)
        {
            idle_loop();
            if(NULL!=(c=s->mc))
            {
                if(mx>=c->sx && mx<c->sx+c->xl && my>=c->sy && my<=c->sy+c->yl)
                {
                    return(s);
                }
            }
            s=s->right;
        }
        t=t->father;
    }
    return(NULL);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// SAMPLE FOR USE MENUTREE FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
void    test_menutree(void)
{
    MENU_TREE *my_mt;
    MENU_CELL menu_data[]=
    {
        { 1     ,0      ,0,0,0,0,"文件(F)"         ,'F'        ,NULL   },
        { 2     ,0      ,0,0,0,0,"编辑(E)"         ,'E'        ,NULL   },
        { 3     ,0      ,0,0,0,0,"地表(C)"         ,'C'        ,NULL   },   
        { 4     ,0      ,0,0,0,0,"物件(W)"         ,'W'        ,NULL   },   
        { 5     ,0      ,0,0,0,0,"人物(R)"         ,'R'        ,NULL   },
        { 6     ,0      ,0,0,0,0,"设置(S)"         ,'S'        ,NULL   },
        { 7     ,0      ,0,0,0,0,"工具(T)"         ,'T'        ,NULL   },

        { 8     ,1      ,0,0,0,0,"新建地图(N)"     ,'N'        ,NULL   },
        { 9     ,1      ,0,0,0,0,"打开地图(O)"     ,'O'        ,NULL   },
        {10     ,1      ,0,0,0,0,"存储地图(S)"     ,'S'        ,NULL   },
        {11     ,1      ,0,0,0,0,"另存地图(A)"     ,'A'        ,NULL   },
        {12     ,1      ,0,0,0,0,"修改参数(M)"     ,'M'        ,NULL   },
        {13     ,1      ,0,0,0,0,"退出系统(X)"     ,'X'        ,NULL   },

        {14     ,2      ,0,0,0,0,"编辑地表(C)"     ,'C'        ,NULL   },
        {15     ,2      ,0,0,0,0,"编辑物件(W)"     ,'W'        ,NULL   },
        {16     ,2      ,0,0,0,0,"编辑人物(R)"     ,'R'        ,NULL   },
        {17     ,2      ,0,0,0,0,"编辑地形(S)"     ,'S'        ,NULL   },
        {18     ,2      ,0,0,0,0,"编辑事件(P)"     ,'P'        ,NULL   },

        {19     ,3      ,0,0,0,0,"浏览地表(B)"     ,'B'        ,NULL   },
        {20     ,3      ,0,0,0,0,"添加地表(A)"     ,'A'        ,NULL   },
        {21     ,3      ,0,0,0,0,"删除地表(D)"     ,'D'        ,NULL   },
        {22     ,3      ,0,0,0,0,"修改地表(E)"     ,'E'        ,NULL   },

        {23     ,4      ,0,0,0,0,"浏览物件(B)"     ,'B'        ,NULL   },
        {24     ,4      ,0,0,0,0,"添加物件(A)"     ,'A'        ,NULL   },
        {25     ,4      ,0,0,0,0,"删除物件(D)"     ,'D'        ,NULL   },
        {26     ,4      ,0,0,0,0,"修改物件(E)"     ,'E'        ,NULL   },

        {27     ,5      ,0,0,0,0,"浏览人物(B)"     ,'B'        ,NULL   },
        {28     ,5      ,0,0,0,0,"添加人物(A)"     ,'A'        ,NULL   },
        {29     ,5      ,0,0,0,0,"删除人物(D)"     ,'D'        ,NULL   },
        {30     ,5      ,0,0,0,0,"修改人物(E)"     ,'E'        ,NULL   },
        
        {31     ,6      ,0,0,0,0,"地图卷动(S)"     ,'S'        ,NULL   },
        {32     ,6      ,0,0,0,0,"人物移动(M)"     ,'M'        ,NULL   },
        {33     ,6      ,0,0,0,0,"地图显示(V)"     ,'V'        ,NULL   },
        
        {34     ,7      ,0,0,0,0,"文件浏览(B)"     ,'B'        ,NULL   },
        {35     ,7      ,0,0,0,0,"动画转换(C)"     ,'C'        ,NULL   },
        {36     ,7      ,0,0,0,0,"系统帮助(A)"     ,'A'        ,NULL   },

        {37     ,8      ,0,0,0,0,"1 x 1 (1)"        ,'1'        ,NULL   },
        {38     ,8      ,0,0,0,0,"2 x 2 (2)"        ,'2'        ,NULL   },
        {39     ,8      ,0,0,0,0,"3 x 3 (3)"        ,'3'        ,NULL   },
        {40     ,8      ,0,0,0,0,"4 x 4 (4)"        ,'4'        ,NULL   },
        {41     ,8      ,0,0,0,0,"5 x 5 (5)"        ,'5'        ,NULL   },
        {42     ,8      ,0,0,0,0,"6 x 6 (6)"        ,'6'        ,NULL   },
        {43     ,8      ,0,0,0,0,"7 x 7 (7)"        ,'7'        ,NULL   },
        {44     ,8      ,0,0,0,0,"8 x 8 (8)"        ,'8'        ,NULL   },
        {45     ,8      ,0,0,0,0,"9 x 9 (9)"        ,'9'        ,NULL   },
        {46     ,8      ,0,0,0,0,"10x10 (A)"        ,'A'        ,NULL   },
        {47     ,8      ,0,0,0,0,"11x11 (B)"        ,'B'        ,NULL   },
        {-1     ,-1     ,0,0,0,0,""             ,0x00       ,NULL   }
    };
    
    init_menutree(&my_mt);
    setup_menutree(my_mt,menu_data);
    exec_menutree(my_mt);
    destroy_menutree(&my_mt);
}

**************************************************************************************************************
*/

