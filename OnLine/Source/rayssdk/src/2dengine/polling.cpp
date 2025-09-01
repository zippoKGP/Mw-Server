/*
**      POLLING.CPP
**      Polling funcions and Routines.
**
**      Stephen,1999.5.10.
**          Principle of polling task functions.
**      ZJian, 1999.5.12.
**          Polling task functions.
**      ZJian, 2000.9.3.
**          Change function name to lower case.
**
*/
#include "rays.h"
#include "xsystem.h"

#define MAX_POLLING     16

typedef struct  tagPOLLING  POLLING,*LPPOLLING;
struct  tagPOLLING
{
    POLLING *last,*next;
    void (*func)();
};

static  POLLING *head_polling_task[MAX_POLLING] = {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL};

EXPORT  SLONG   FNBACK  init_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  append_polling_task(SLONG index,void (*func)());
EXPORT  SLONG   FNBACK  delete_polling_task(SLONG index,void (*func)());
EXPORT  SLONG   FNBACK  exec_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  clear_polling_task(SLONG index);
EXPORT  SLONG   FNBACK  insert_polling_task(SLONG index,void (*func)(),void (*pos_func)());

/******************************************************************************/

EXPORT  SLONG   FNBACK  init_polling_task(SLONG index)
{
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    head_polling_task[index]=(POLLING *)GlobalAlloc(GPTR, sizeof(POLLING));
    if(head_polling_task[index]==NULL)
    {
        sprintf((char *)print_rec,"init_polling_task(%d) error",index);
        log_error(1,print_rec);
        return(TTN_NOT_OK);
    }
    head_polling_task[index]->last=NULL;
    head_polling_task[index]->next=NULL;
    head_polling_task[index]->func=NULL;
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  append_polling_task(SLONG index,void (*func)())
{
    POLLING *p;
    
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    p=head_polling_task[index];
    while(p->next!=NULL)
        p=p->next;
    p->func=func;
    p->next=(POLLING *)GlobalAlloc(GPTR, sizeof(POLLING));
    p->next->last=p;
    p->next->next=NULL;
    p->next->func=NULL;
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  delete_polling_task(SLONG index,void (*func)())
{
    POLLING *p;
    
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    p=head_polling_task[index];
    while(p->next!=NULL)
    {
        if(p->func==func)
        {
            if(p->last!=NULL)
            {
                p->last->next=p->next;
                p->next->last=p->last;
                if(p!=NULL)
                    GlobalFree(p);
                return(TTN_OK);
            }
            else
            {
                p->next->last=NULL;
                head_polling_task[index]=p->next;
                if(p!=NULL)
                    GlobalFree(p);
                return(TTN_OK);
            }
        }
        else
            p=p->next;
    }
    return(TTN_NOT_OK);
}

EXPORT  SLONG   FNBACK  clear_polling_task(SLONG index)
{
    POLLING *p,*s;
    
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    p=head_polling_task[index];
    while(p->next!=NULL)
        p=p->next;
    while(p->last!=NULL)
    {
        s=p;
        p=s->last;
        s->func=NULL;
        s->next=NULL;
        if(s!=NULL)
            GlobalFree(s);
        p->next=NULL;
    }
    if(head_polling_task[index]!=NULL)
    {
        GlobalFree(head_polling_task[index]);
        head_polling_task[index]=NULL;
    }
    return(TTN_OK);
}

EXPORT  SLONG   FNBACK  exec_polling_task(SLONG index)
{
    POLLING *p;
    
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    p=head_polling_task[index];
    while(p!=NULL)
    {
        if(p->func != NULL)
            (p->func)();
        p=p->next;
    }
    return(TTN_OK);
}


EXPORT  SLONG   FNBACK  insert_polling_task(SLONG index,void (*func)(),void (*pos_func)())
{
    POLLING *p,*s;
    SLONG   find;
    
    if((index<0)||(index>MAX_POLLING-1))
        return(TTN_NOT_OK);
    p=head_polling_task[index];
    find=0;
    while(p->next!=NULL)
    {
        p=p->next;
        if(p->func == pos_func)
        {
            find=1;
            s=(POLLING *)GlobalAlloc(GPTR, sizeof(POLLING));
            s->func=func;
            s->last=p;
            s->next=p->next;
            p->next=s;
        }
        if(find==1)
            break;
    }
    if(find==1)
        return(TTN_OK);
    else
        return(TTN_NOT_OK);
}
