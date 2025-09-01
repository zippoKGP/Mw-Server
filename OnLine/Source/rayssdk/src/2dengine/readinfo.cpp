/*
**      READINFO.CPP
**      read .ini information file functions.
**
**      ZJian,2000.10.02.
*/
#include "rays.h"
#include "xsystem.h"
#include "xkiss.h"
#include "readinfo.h"
#include "function.h"

#define LINE_BUFFER_SIZE        1024

typedef struct  tagLINE_DNODE   LINE_DNODE,*LPLINE_DNODE;
struct  tagLINE_DNODE
{
    LINE_DNODE  *last,*next;
    USTR buffer[LINE_BUFFER_SIZE];
};

static LINE_DNODE   *inport_line = NULL ;

static SLONG   find_group_section(USTR *id,LINE_DNODE *ss,LINE_DNODE **tt);

/******************************************************************************/
USTR *  get_ini_information(USTR *group,USTR *keyword,USTR *value)
{
    static USTR data[256];
    LINE_DNODE *p=NULL;
    SLONG lines,line_index=0,index,find=0;
    USTR buffer[1024];
    USTR temp[256];
    USTR temp1[256];
    USTR keyword_str[80];
    
    memset((char *)keyword_str,0x00,80);
    strcpy((char *)keyword_str,(const char *)keyword);
    strupr((char *)keyword_str);
    strcpy((char *)value,(const char *)"");
    memset(data,0x00,256);
    lines=find_group_section(group,inport_line,&p);
    if(p && lines)
    {
        while(p->next)
        {
            p=(LINE_DNODE *)p->next;
            
            index=0;
            memset(buffer,0x00,1024);
            memset(temp,0x00,256);
            strcpy((char *)buffer,(const char *)p->buffer);
            skip_compartment(buffer,&index,(USTR *)" ,=\x09",4);
            get_string(temp,255,buffer,&index,(USTR *)" ,=\x09",4);
            if((temp[0]!=0x00) && (temp[0]!=';'))
            {
                strupr((char *)temp);
                if(strcmpi((const char *)keyword,(const char *)temp)==0)
                {
                    memset(temp1,0x00,256);
                    skip_compartment(buffer,&index,(USTR *)" ,=\x09",4);
                    get_string((USTR *)temp1,255,buffer,&index,(USTR *)"\x00",1);
                    if(temp1[0]!=0x00)
                    {
                        strcpy((char *)data,(const char *)temp1);
                        find=1;
                    }
                }
            }
            line_index++;
            if(line_index>=lines)
                break;
            if(find==1)
                break;
        }
    }
    strcpy((char *)value,(const char *)data);
    return((USTR *)data);
}


SLONG load_ini_information(USTR *filename)
{
    USTR line[LINE_BUFFER_SIZE];
    LINE_DNODE *p=NULL,*s=NULL;
    SLONG   file_size;
    USTR    *file_buf = NULL;
    SLONG   buffer_index, result, pass;
    
    file_size = load_file_to_buffer(filename, &file_buf);
    if(file_size < 0)
        goto error;

    if(NULL!=inport_line)
    {
        sprintf((char *)print_rec,"info file reopen error",filename);
        log_error(1,(USTR *)print_rec);
        goto error;
    }

    if(NULL==(inport_line=(LINE_DNODE *)GlobalAlloc(GPTR, sizeof(LINE_DNODE))))
    {
        strcpy((char *)print_rec,"memory alloc error");
        log_error(1,(USTR *)print_rec);
        goto error;
    }
    inport_line->last=inport_line->next=NULL;
    memset(inport_line->buffer,0x00,LINE_BUFFER_SIZE);
    p=inport_line;

    pass = 0;
    buffer_index = 0;
    result = get_buffer_line((char*)file_buf, &buffer_index, file_size, (char*)line, LINE_BUFFER_SIZE);
    while((result == TTN_OK) && (pass == 0))
    {
        if(NULL==(s=(LINE_DNODE *)GlobalAlloc(GPTR, sizeof(LINE_DNODE))))
        {
            strcpy((char *)print_rec,"memory alloc error");
            log_error(1,(USTR *)print_rec);
            goto error;
        }
        s->last=s->next=NULL;
        strcpy((char *)s->buffer,(const char *)line);
        p->next=s;
        s->last=p;
        p=p->next;

        //read next line
        result = get_buffer_line((char*)file_buf, &buffer_index, file_size, (char*)line, LINE_BUFFER_SIZE);
    }
    if(file_buf) free(file_buf);
    return TTN_OK;

error:
    if(file_buf) free(file_buf);
    return TTN_ERROR;
}


void free_ini_information(void)
{
    LINE_DNODE *p=NULL,*s=NULL;
    if(NULL==(p=inport_line)) return;

    //Jack, 2002.3.28. 
    //found memory leak here.
    /*
    while(p->next)
    {
        s=p;
        p=p->next;
        if(s) GlobalFree(s);
    }
    */
    while(p)
    {
        s = p;
        p = p->next;
        if(s)
        {
            GlobalFree(s);
            s = NULL;
        }
    }

    inport_line=NULL;
}

static      SLONG   find_group_section(USTR *id,LINE_DNODE *ss,LINE_DNODE **tt)
{
    SLONG lines=0,find=0,index,i,len;
    USTR buffer[1024],temp[256],temp1[256],idstr[80];
    LINE_DNODE *p=ss;
    
    (*tt)=NULL;
    if(p==NULL) return(0);
    memset((char *)idstr,0x00,80);
    strcpy((char *)idstr,(const char *)id);
    strupr((char *)idstr);
    memset((char *)temp1,0x00,256);
    while(p->next!=NULL)
    {
        p=p->next;
        
        index=0;
        memset(buffer,0x00,1024);
        strcpy((char *)buffer,(const char *)p->buffer);
        skip_compartment(buffer,&index,(USTR *)" ,=\x09",4);
        get_string((USTR *)temp,255,buffer,&index,(USTR *)"\x00",1);
        if(temp[0]=='[')
        {
            strcpy((char *)temp1,(const char *)&temp[1]);
            len=(SLONG)strlen((const char *)temp1);
            for(i=0;i<len;i++)
            {
                if(temp1[i]==']')
                {
                    temp1[i]=0x00;
                    i=len+100;
                }
            }
            strupr((char *)temp1);
            if(strcmpi((const char *)idstr,(const char *)temp1)==0)
            {
                find=1;
                *tt=p;
                lines=0;
            }
            else
            {
                if(find==1)
                    break;
            }
        }
        else
        {
            if(find==1)
                lines++;
        }
    }
    return(lines);
}


void    list_ini_information(void)
{
    LINE_DNODE *p=NULL;
    SLONG lines;

    p=inport_line;
    if(!p) return;
    lines=0;
    while(p->next)
    {
        p=(LINE_DNODE *)p->next;
        sprintf((char *)print_rec,"[%08d]%s",lines,p->buffer);
        log_error(1,print_rec);
        lines++;
    }
}