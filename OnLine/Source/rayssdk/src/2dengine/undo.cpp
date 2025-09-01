/*
**      UNDO.CPP
**      undo functions.
**
**      There are three aciton types for support undo,as the following:
**      (1) DO (do something for normal action)
**      (2) UNDO (undo the last n do-actions)
**      (3) REDO (redo the last n undo-actions)
**
**      ZJian, 2000.9.28.
*/

#include "rays.h"
#include "xsystem.h"
#include "xkiss.h"
#include "undo.h"


EXPORT  void    FNBACK  init_undo_head(UNDO_HEAD *undo_head)
{
    undo_head->copyright = MAKEFOURCC('3','S','S',0x1a);
    undo_head->id = MAKEFOURCC('U','N','D','O');
    undo_head->version = 0x00010000;
    undo_head->head_size=sizeof(UNDO_HEAD);
    undo_head->data_size=0;
    undo_head->read_index=0;
    undo_head->write_index=0;
    for(SLONG i=0;i<MAX_UNDO_RECORDS;i++)
        undo_head->offset[i]=sizeof(UNDO_HEAD);
}


EXPORT  SLONG   FNBACK  create_undo_file(USTR *filename)
{
    FILE *fp=NULL;
    UNDO_HEAD undo_head;
    if(NULL==(fp=fopen((const char *)filename,"wb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        return TTN_ERROR;
    }
    init_undo_head(&undo_head);
    fwrite(&undo_head,1,sizeof(UNDO_HEAD),fp);
    if(fp) fclose(fp);
    return TTN_OK;
}


EXPORT  SLONG   FNBACK  flush_undo_file(USTR *filename)
{
    remove((const char *)filename);
    create_undo_file((USTR *)filename);
    return TTN_OK;
}

EXPORT  SLONG   FNBACK  process_undo_file_with_do(USTR *filename,UNDO_RECORD *undo_record)
{
    // (1)if exist undo actions (redo actions) , clear the undo_records after read_index
    //    and set write_index to read_index
    // (2)append 
    FILE *fp=NULL,*tfp=NULL;
    UNDO_HEAD undo_head;
    USTR temp_filename[_MAX_PATH+_MAX_FNAME],*buffer=NULL;
    SLONG i,offset,undo_file_data_size,remain_size,write_index,record_size;

    if(NULL==undo_record)
    {
        sprintf((char *)print_rec,"add null undo record error");
        log_error(1,print_rec);
        goto error;
    }
    // get read_index for check if it is overflow of MAX_UNDO_RECORDS
    if(NULL==(fp=fopen((const char *)filename,"rb")))
    {
        sprintf((char *)print_rec,"open file %s error",filename);
        log_error(1,print_rec);
        goto error;
    }
    fread(&undo_head,1,sizeof(UNDO_HEAD),fp);
    // if the undo records index is overflow ,remake the undo file
    if(undo_head.read_index>=MAX_UNDO_RECORDS) 
    {
        // seek read pointer of undo file 
        undo_file_data_size=undo_head.data_size+sizeof(UNDO_HEAD);
        offset=undo_head.offset[MAX_UNDO_RECORDS-MAX_UNDO_TIMES];
        fseek(fp,offset,SEEK_SET);
        remain_size=undo_file_data_size-offset;

        // remain the last MAX_UNDO_RECORDS
        sprintf((char *)temp_filename,"%s\\$UNDO$.$$$",get_file_path((USTR *)filename));
        remove((const char *)temp_filename);
        if(NULL==(tfp=fopen((const char *)temp_filename,"wb")))
        {
            sprintf((char *)print_rec,"open file %s error",temp_filename);
            log_error(1,print_rec);
            goto error;
        }
        fwrite(&undo_head,1,sizeof(UNDO_HEAD),tfp);
        if(NULL==(buffer=(USTR *)GlobalAlloc(GPTR, 20000)))
        {
            sprintf((char *)print_rec,"memory alloc error");
            log_error(1,print_rec);
            goto error;
        }
        for(i=0;i<remain_size/20000;i++)
        {
            fread(buffer,1,20000,fp);
            fwrite(buffer,1,20000,tfp);
        }
        if(remain_size%20000)
        {
            fread(buffer,1,remain_size%20000,fp);
            fwrite(buffer,1,remain_size%20000,tfp);
        }
        if(buffer) {GlobalFree(buffer); buffer=NULL;}
        for(i=0;i<MAX_UNDO_TIMES;i++)
        {
            undo_head.offset[i]=undo_head.offset[MAX_UNDO_RECORDS-MAX_UNDO_TIMES+i]
                -offset+sizeof(UNDO_HEAD);
        }
        for(i=MAX_UNDO_TIMES;i<MAX_UNDO_RECORDS;i++)
        {
            undo_head.offset[i]=remain_size;
        }
        undo_head.data_size=remain_size;
        undo_head.write_index-=MAX_UNDO_RECORDS-MAX_UNDO_TIMES;
        undo_head.read_index-=MAX_UNDO_RECORDS-MAX_UNDO_TIMES;
        fseek(tfp,0L,SEEK_SET);
        fwrite(&undo_head,1,sizeof(UNDO_HEAD),tfp);
        if(tfp) fclose(tfp);
        if(fp) fclose(fp);
        remove((const char *)filename);
        rename((const char *)temp_filename,(const char *)filename);
        sprintf((char *)print_rec,"REMAKE UNDO FILE %s !",filename);
        log_error(1,print_rec);
    }
    else
    {
        if(fp) fclose(fp);
    }

    // process do action to undo file
    if(NULL==(fp=fopen((const char *)filename,"r+b")))
    {
        sprintf((char *)print_rec,"file %s (re)open error",filename);
        log_error(1,print_rec);
        goto error;
    }
    fread(&undo_head,1,sizeof(UNDO_HEAD),fp);

    // write this undo_record to undo file from read_index
    remain_size=sizeof(UNDO_HEAD)+undo_head.offset[undo_head.read_index];
    record_size=undo_record->size+sizeof(UNDO_RECORD)-4;
    fseek(fp,remain_size,SEEK_SET);
    fwrite(undo_record,1,record_size,fp);

    // update undo_head
    write_index=undo_head.read_index;
    undo_head.offset[write_index]=remain_size;
    write_index++;
    undo_head.write_index=write_index;
    undo_head.read_index=write_index;
    undo_head.data_size=remain_size-sizeof(UNDO_HEAD)+record_size;
    for(i=write_index;i<MAX_UNDO_RECORDS;i++)
        undo_head.offset[i]=undo_head.data_size+sizeof(UNDO_HEAD);
    fseek(fp,0L,SEEK_SET);
    fwrite(&undo_head,1,sizeof(UNDO_HEAD),fp);
    if(fp) fclose(fp);
    return TTN_OK;
error:
    if(fp) fclose(fp);
    if(tfp) fclose(tfp);
    if(buffer) {GlobalFree(buffer);buffer=NULL;}
    return TTN_ERROR;
}


EXPORT  SLONG   FNBACK  process_undo_file_with_undo(USTR *filename,UNDO_RECORD **undo_record)
{
    FILE *fp=NULL;
    UNDO_HEAD undo_head;
    UNDO_RECORD temp_undo_record;
    SLONG offset,size;
    SLONG ret;

    ret = TTN_NOT_OK;
    if(NULL==(fp=fopen((const char *)filename,"r+b")))
    {
        sprintf((char *)print_rec,"open file %s error",filename);
        log_error(1,print_rec);
        goto error;
    }
    // free the memory of (*undo_record)
    if(*undo_record) { GlobalFree(*undo_record); (*undo_record)=NULL; }
    fread(&undo_head,1,sizeof(UNDO_HEAD),fp);
    if(undo_head.read_index>MAX(undo_head.write_index-MAX_UNDO_TIMES,0))
    {
        // get undo record
        undo_head.read_index--;
        offset=undo_head.offset[undo_head.read_index];
        fseek(fp,offset,SEEK_SET);
        fread(&temp_undo_record,1,sizeof(UNDO_RECORD),fp);
        fseek(fp,-1L*((SLONG)sizeof(UNDO_RECORD)),SEEK_CUR);
        size=temp_undo_record.size+sizeof(UNDO_RECORD)-4;
        if(NULL==((*undo_record)=(UNDO_RECORD *)GlobalAlloc(GPTR, size)))
        {
            sprintf((char *)print_rec,"memory alloc error for redo");
            log_error(1,print_rec);
            goto error;
        }
        fread((*undo_record),1,size,fp);
        // update undo_head
        fseek(fp,0L,SEEK_SET);
        fwrite(&undo_head,1,sizeof(UNDO_HEAD),fp);
        ret = TTN_OK;
    }
error:
    if(fp) fclose(fp);
    return ret;
}


EXPORT  SLONG   FNBACK  process_undo_file_with_redo(USTR *filename,UNDO_RECORD **undo_record)
{
    FILE *fp=NULL;
    UNDO_HEAD undo_head;
    UNDO_RECORD temp_undo_record;
    SLONG offset,size;
    SLONG ret;

    ret = TTN_NOT_OK;
    if(NULL==(fp=fopen((const char *)filename,"r+b")))
    {
        sprintf((char *)print_rec,"open file %s error",filename);
        log_error(1,print_rec);
        goto error;
    }
    // free the memory of (*undo_record)
    if(*undo_record) { GlobalFree(*undo_record); (*undo_record)=NULL; }
    fread(&undo_head,1,sizeof(UNDO_HEAD),fp);
    // exist redo record(read_index<write_index) so process redo
    if(undo_head.read_index<undo_head.write_index)
    {
        // get redo record 
        offset=undo_head.offset[undo_head.read_index];
        fseek(fp,offset,SEEK_SET);
        fread(&temp_undo_record,1,sizeof(UNDO_RECORD),fp);
        fseek(fp,-1L*((SLONG)sizeof(UNDO_RECORD)),SEEK_CUR);
        size=temp_undo_record.size+sizeof(UNDO_RECORD)-4;
        if(NULL==((*undo_record)=(UNDO_RECORD *)GlobalAlloc(GPTR, size)))
        {
            sprintf((char *)print_rec,"memory alloc error for redo");
            log_error(1,print_rec);
            goto error;
        }
        fread((*undo_record),1,size,fp);
        // read_index++ & update undo_head
        fseek(fp,0L,SEEK_SET);
        undo_head.read_index++;
        fwrite(&undo_head,1,sizeof(UNDO_HEAD),fp);
        ret = TTN_OK;
    }
error:
    if(fp) fclose(fp);
    return ret;
}


EXPORT  UNDO_RECORD *   FNBACK  create_undo_record(SLONG data_size)
{
    UNDO_RECORD *tmp;

    if(NULL == (tmp = (UNDO_RECORD *)GlobalAlloc(GPTR, sizeof(UNDO_RECORD)-4+data_size) ) )
        return NULL;
    tmp->size = data_size;

    return (UNDO_RECORD *)tmp;
}



EXPORT  void    FNBACK  destroy_undo_record(UNDO_RECORD **ur)
{
    if(*ur) {GlobalFree(*ur); *ur=NULL; }
}

