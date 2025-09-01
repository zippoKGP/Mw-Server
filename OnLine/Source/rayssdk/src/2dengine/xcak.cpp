/*
**      XCAK.CPP
**      grafx CAK disk i/o functions. 
**
**      ZJian, 2000.10.19.
**          New file format "CAK": 
**              make most grafx image struct a "cake" to disk file.
**
**
*/
#include "rays.h"
#include "xsystem.h"
#include "xfont.h"
#include "xcak.h"
#include "packfile.h"

static  CAKE_ID         cake_id;
static  CAKE_LEN        cake_len;
static  HEAD_CAKE       head_cake;
static  COMMON_CAKE     common_cake;
static  OFFSET_CAKE     offset_cake;
static  FRAME_CAKE      frame_cake;
static  END_CAKE        end_cake;
static  DUMMY_CAKE      dummy_cake;
static  INFO_CAKE       info_cake;

//Jack, 2002.4.26.
static  SLONG   naked_read_cake(PACK_FILE *fp, ULONG *id, void **p);



OFFSET_CAKE *   create_offset_cake(SLONG count)
{
    OFFSET_CAKE *tmp = NULL;
    SLONG i;
    tmp=(OFFSET_CAKE *)GlobalAlloc(GPTR, sizeof(OFFSET_CAKE)+(count-1)*sizeof(SLONG));
    if(!tmp) return NULL;
    tmp->count=count;
    for(i=0;i<count;i++)
        tmp->offset[i]=0;
    return tmp;
}

void    destroy_offset_cake(OFFSET_CAKE *oc)
{
    if(oc) {GlobalFree(oc); oc=NULL; }
}

SLONG   compute_cake_data_length(ULONG cake_id,void *p)
{
    SLONG data_size;
    data_size=0;
    switch(cake_id)
    {
    case CAKE_ID_HEAD:
        {
            HEAD_CAKE *hc=(HEAD_CAKE *)p;
            if(hc) data_size = sizeof(HEAD_CAKE);
        }
        break;
    case CAKE_ID_COMMON:
        {
            COMMON_CAKE *cc=(COMMON_CAKE *)p;
            if(cc) data_size = sizeof(COMMON_CAKE);
        }
        break;
    case CAKE_ID_OFFSET:
        {
            OFFSET_CAKE *oc=(OFFSET_CAKE *)p;
            if(oc) data_size = sizeof(OFFSET_CAKE) - sizeof(SLONG) + oc->count * sizeof(SLONG);
        }
        break;
    case CAKE_ID_FRAME:
        {
            FRAME_CAKE *fc=(FRAME_CAKE *)p;
            if(fc) data_size = sizeof(FRAME_CAKE);
        }
        break;
    case CAKE_ID_END:
        {
            END_CAKE *ec=(END_CAKE *)p;
            if(ec) data_size = sizeof(END_CAKE);
        }
        break;
    case CAKE_ID_DUMMY:
        {
            DUMMY_CAKE *dc=(DUMMY_CAKE *)p;
            if(dc) data_size = sizeof(DUMMY_CAKE);
        }
        break;
    case CAKE_ID_INFO:
        {
            INFO_CAKE *ic=(INFO_CAKE *)p;
            if(ic) data_size = sizeof(INFO_CAKE);
        }
        break;
    case CAKE_ID_ALF:
        {
            ALF *ac=(ALF *)p;
            if(ac) data_size = sizeof(ALF) - sizeof(USTR *) + ac->w * ac->h ;
        }
        break;
    case CAKE_ID_BMP:
        {
            BMP *bc=(BMP *)p;
            if(bc) data_size = sizeof(BMP) - sizeof(PIXEL *) + bc->h * bc->pitch;
        }
        break;
    case CAKE_ID_RLE:
        {
            RLE *rc=(RLE*)p;
            if(rc) data_size = sizeof(RLE) + sizeof(USTR*)*(rc->h-1) + rc->size;
        }
        break;
    case CAKE_ID_IMG:
        {
            IMG *ic=(IMG *)p;
            if(ic) data_size = sizeof(IMG) - sizeof(USTR *) + ic->w * ic->h ;
        }
        break;
    case CAKE_ID_PAL:
        {
            PAL *pc=(PAL *)p;
            if(pc) data_size = sizeof(PAL);
        }
        break;
    case CAKE_ID_POLY:
        {
            POLY *pc=(POLY *)p;
            if(pc) data_size = sizeof(POLY);
        }
        break;
    default:
        data_size=0;
        break;
    }
    return (data_size);
}

SLONG   write_cake_to_file(FILE *fp,ULONG id,void *p)
{
    SLONG data_size;
    SLONG write_size;

    write_size=0;

    cake_id.id = id;
    fwrite(&cake_id,1,sizeof(CAKE_ID),fp);
    write_size += sizeof(CAKE_ID);

    cake_len.length=compute_cake_data_length(id,p);
    fwrite(&cake_len,1,sizeof(CAKE_LEN),fp);
    data_size=cake_len.length;
    write_size += sizeof(CAKE_LEN);

    // in the following codes, data_size = cake_len.length
    switch(id)
    {
    case CAKE_ID_HEAD:
        if(cake_len.length && p)
        {
            HEAD_CAKE *hc=(HEAD_CAKE *)p;
            data_size = sizeof(HEAD_CAKE);
            fwrite(hc,1,data_size,fp);
        }
        break;
    case CAKE_ID_COMMON:
        if(cake_len.length && p)
        {
            COMMON_CAKE *cc=(COMMON_CAKE *)p;
            data_size = sizeof(COMMON_CAKE);
            fwrite(cc,1,data_size,fp);
        }
        break;
    case CAKE_ID_OFFSET:
        if(cake_len.length && p)
        {
            OFFSET_CAKE *oc=(OFFSET_CAKE *)p;
            data_size = sizeof(OFFSET_CAKE) - sizeof(SLONG) + oc->count * sizeof(SLONG) ;
            fwrite(oc,1,data_size,fp);
        }
        break;
    case CAKE_ID_FRAME:
        if(cake_len.length && p)
        {
            FRAME_CAKE *fc=(FRAME_CAKE *)p;
            data_size = sizeof(FRAME_CAKE);
            fwrite(fc,1,data_size,fp);
        }
        break;
    case CAKE_ID_END:
        if(cake_len.length && p)
        {
            END_CAKE *ec=(END_CAKE *)p;
            data_size = sizeof(END_CAKE);
            fwrite(ec,1,data_size,fp);
        }
        break;
    case CAKE_ID_DUMMY:
        if(cake_len.length && p)
        {
            DUMMY_CAKE *dc=(DUMMY_CAKE *)p;
            data_size = sizeof(DUMMY_CAKE);
            fwrite(dc,1,data_size,fp);
        }
        break;
    case CAKE_ID_INFO:
        if(cake_len.length && p)
        {
            INFO_CAKE *ic=(INFO_CAKE *)p;
            data_size = sizeof(INFO_CAKE);
            fwrite(ic,1,data_size,fp);
        }
        break;
    case CAKE_ID_ALF:
        if(cake_len.length && p)
        {
            ALF *ac=(ALF *)p;
            SLONG y;

            data_size = sizeof(ALF) - sizeof(USTR *) + ac->w * ac->h ;
            //fwrite(&ac->w,1,sizeof(SLONG),fp);
            //fwrite(&ac->h,1,sizeof(SLONG),fp);
            fwrite(ac,1,sizeof(ALF)-sizeof(USTR *),fp);
            for(y=0;y<ac->h;++y)
                fwrite(ac->line[y],1,ac->w,fp);
        }
        break;
    case CAKE_ID_BMP:
        if(cake_len.length && p)
        {
            BMP *bc=(BMP *)p;
            SLONG y;

            convert_bitmap_hi2fff(bc);
            data_size = sizeof(BMP) - sizeof(PIXEL *) + bc->h * bc->pitch;
            //fwrite(&bc->w,1,sizeof(SLONG),fp);
            //fwrite(&bc->h,1,sizeof(SLONG),fp);
            //fwrite(&bc->pitch,1,sizeof(SLONG),fp);
            fwrite(bc,1,sizeof(BMP)-sizeof(PIXEL *),fp);
            for(y=0;y<bc->h;++y)
                fwrite(bc->line[y],1,bc->pitch,fp);
            convert_bitmap_fff2hi(bc);
        }
        break;
    case CAKE_ID_IMG:
        if(cake_len.length && p)
        {
            IMG *ic=(IMG *)p;
            SLONG y;

            data_size = sizeof(IMG) - sizeof(USTR *) + ic->w * ic->h ;
            //fwrite(&ic->w,1,sizeof(SLONG),fp);
            //fwrite(&ic->h,1,sizeof(SLONG),fp);
            fwrite(ic,1,sizeof(IMG)-sizeof(USTR *),fp);
            for(y=0;y<ic->h;++y)
                fwrite(ic->line[y],1,ic->w,fp);
        }
        break;
    case CAKE_ID_PAL:
        if(cake_len.length && p)
        {
            PAL *pc=(PAL *)p;

            convert_buffer_hi2fff(pc->data,256);
            data_size = sizeof(PAL);
            fwrite(pc,1,data_size,fp);
            convert_buffer_fff2hi(pc->data,256);
        }
        break;
    case CAKE_ID_POLY:
        if(cake_len.length && p)
        {
            POLY *pc=(POLY *)p;
            data_size = sizeof(POLY);
            fwrite(pc,1,data_size,fp);
        }
        break;
    case CAKE_ID_RLE:
        if(cake_len.length && p)
        {
            RLE *rc=(RLE*)p;
            USTR *t=NULL;
            SLONG y,offset;

            convert_rle_hi2fff(rc);
            data_size = sizeof(RLE) - sizeof(USTR *)*(rc->h-1) + rc->size;
            //fwrite(&rc->w,1,sizeof(SLONG),fp);
            //fwrite(&rc->h,1,sizeof(SLONG),fp);
            //fwrite(&rc->size,1,sizeof(SLONG),fp);
            fwrite(rc,1,sizeof(RLE)-sizeof(USTR*),fp);
            t = rc->line[0];
            for(y=0;y<rc->h;++y)
            {
                offset = (SLONG)(rc->line[y] - t);
                fwrite(&offset,1,sizeof(SLONG),fp);
            }
            fwrite(rc->line[0],1,rc->size,fp);
            convert_rle_fff2hi(rc);
        }
        break;
    default:
        data_size=0;
        break;
    }
    write_size += data_size;
    return write_size;
}

SLONG   read_cake_from_file(FILE *fp,ULONG *id,void **p)
{
    SLONG data_size;
    SLONG read_size;

    read_size=0;

    fread(&cake_id,1,sizeof(CAKE_ID),fp);
    (*id)=cake_id.id;
    read_size += sizeof(CAKE_ID);

    fread(&cake_len,1,sizeof(CAKE_LEN),fp);
    data_size=cake_len.length;
    read_size += sizeof(CAKE_LEN);

    // in the following, data_size = cake_len.length
    (*p)=NULL;
    switch( (*id) )
    {
    case CAKE_ID_HEAD:
        if(cake_len.length )
        {
            HEAD_CAKE *hc;
            //data_size=sizoef(HEAD_CAKE);
            if( NULL== ( hc=(HEAD_CAKE *)GlobalAlloc(GPTR, cake_len.length) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read head_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(hc,1,cake_len.length,fp);
            (*p)=hc;
        }
        break;
    case CAKE_ID_COMMON:
        if(cake_len.length )
        {
            COMMON_CAKE *cc;
            //data_size=sizeof(COMMON_CAKE);
            if(NULL== ( cc=(COMMON_CAKE *)GlobalAlloc(GPTR, cake_len.length) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read common_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(cc,1,cake_len.length,fp);
            (*p)=cc;
        }
        break;
    case CAKE_ID_OFFSET:
        if(cake_len.length )
        {
            OFFSET_CAKE *oc;
            //data_size = sizeof(OFFSET_CAKE) - sizeof(SLONG) + oc->count * sizeof(SLONG) ;
            if(NULL==(oc=(OFFSET_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read offset_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(oc,1,cake_len.length,fp);
            (*p)=oc;
        }
        break;
    case CAKE_ID_FRAME:
        if(cake_len.length )
        {
            FRAME_CAKE *fc;
            //data_size = sizeof(FRAME_CAKE);
            if(NULL==(fc=(FRAME_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read frame_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(fc,1,cake_len.length,fp);
            (*p)=fc;
        }
        break;
    case CAKE_ID_END:
        if(cake_len.length )
        {
            END_CAKE *ec;
            //data_size = sizeof(END_CAKE);
            if(NULL==(ec=(END_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read end_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(ec,1,cake_len.length,fp);
            (*p)=ec;
        }
        break;
    case CAKE_ID_DUMMY:
        if(cake_len.length )
        {
            DUMMY_CAKE *dc;
            //data_size = sizeof(DUMMY_CAKE);
            if(NULL==(dc=(DUMMY_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read dummy_cake)");
                log_error(1,print_rec);
                goto error;
            }
            fread(dc,1,cake_len.length,fp);
            (*p)=dc;
        }
        break;
    case CAKE_ID_INFO:
        if(cake_len.length )
        {
            INFO_CAKE *ic;
            if(NULL==(ic=(INFO_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read info_cake)");
                log_error(1,print_rec);
                goto error;
            }
            //data_size = sizeof(INFO_CAKE);
            fread(ic,1,cake_len.length,fp);
            (*p)=ic;
        }
        break;
    case CAKE_ID_ALF:
        if(cake_len.length )
        {
            ALF *ac,tac;
            SLONG y;
            //data_size = sizeof(ALF) - sizeof(USTR *) + ac->w * ac->h ;
            fread(&tac,1,sizeof(ALF)-sizeof(USTR *),fp);
            if(NULL==(ac=create_alf(tac.w, tac.h)))
            {
                sprintf((char *)print_rec,"memory alloc error(read ALF)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<ac->h;++y)
                fread(ac->line[y],1,ac->w,fp);
            (*p)=ac;
        }
        break;
    case CAKE_ID_BMP:
        if(cake_len.length )
        {
            BMP *bc,tbc;
            SLONG y;

            //data_size = sizeof(BMP) - sizeof(PIXEL *) + bc->h * bc->pitch;
            fread(&tbc,1,sizeof(BMP)-sizeof(PIXEL *),fp);
            if(NULL==(bc=(BMP *)create_bitmap(tbc.w, tbc.h) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read BMP)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<bc->h;++y)
                fread(bc->line[y],1,bc->pitch,fp);

            convert_bitmap_fff2hi(bc);

            (*p)=bc;
        }
        break;
    case CAKE_ID_RLE:
        if(cake_len.length )
        {
            RLE *rc,trc;
            SLONG y,offset;

            //data_size = sizeof(RLE) + sizeof(USTR *)*(rc->h-1) + rc->size;
            fread(&trc,1,sizeof(RLE)-sizeof(USTR *),fp);
            if(NULL==(rc=(RLE *)create_rle(trc.w, trc.h, trc.size) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read RLE)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<rc->h;++y)
            {
                fread(&offset,1,sizeof(SLONG),fp);
                rc->line[y] = (USTR*)(rc->line[0]+offset);
            }
            fread(rc->line[0],1,rc->size,fp);

            convert_rle_fff2hi(rc);

            (*p)=rc;
        }
        break;
    case CAKE_ID_IMG:
        if(cake_len.length )
        {
            IMG *ic,tic;
            SLONG y;

            //data_size = sizeof(IMG) - sizeof(USTR *) + ic->w * ic->h ;
            fread(&tic,1,sizeof(IMG)-sizeof(USTR *),fp);
            if(NULL==(ic=create_img(tic.w, tic.h)))
            {
                sprintf((char *)print_rec,"memory alloc error(read IMG)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<ic->h;++y)
                fread(ic->line[y],1,ic->w,fp);
            (*p)=ic;
        }
        break;
    case CAKE_ID_PAL:
        if(cake_len.length )
        {
            PAL *pc;
            //data_size = sizeof(PAL);
            if(NULL==(pc=(PAL *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read PAL)");
                log_error(1,print_rec);
                goto error;
            }
            fread(pc,1,cake_len.length,fp);
            convert_buffer_fff2hi(pc->data,256);
            (*p)=pc;
        }
        break;
    case CAKE_ID_POLY:
        if(cake_len.length )
        {
            POLY *pc;
            //data_size = sizeof(POLY);
            if(NULL==(pc=(POLY *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read poly)");
                log_error(1,print_rec);
                goto error;
            }
            fread(pc,1,cake_len.length,fp);
            (*p)=pc;
        }
        break;
    default:
        (*p)=NULL;
        break;
    }
    read_size += data_size;
    return read_size;
error:
    (*p)=NULL;
    return -read_size;
}


CAKE_FRAME *    create_cake_frame(void)
{
    CAKE_FRAME *p = NULL;

    p = (CAKE_FRAME *)GlobalAlloc(GPTR, sizeof(CAKE_FRAME));
    if(!p) return NULL;
    p->rle=NULL;
    p->poly=NULL;
    p->frame_rx=0;
    p->frame_ry=0;
    p->poly_rx=0;
    p->poly_ry=0;
    p->poly_rz=0;
    return p;
}

void    destroy_cake_frame(CAKE_FRAME **p)
{
    if(*p)
    {
        if((*p)->rle) {destroy_rle( &(*p)->rle ); }
        if((*p)->poly) {GlobalFree((*p)->poly); (*p)->poly=NULL; }
        GlobalFree(*p);
        (*p)=NULL;
    }
}


CAKE_FRAME *        duplicate_cake_frame(CAKE_FRAME *cf)
{
    CAKE_FRAME  *tmp = NULL;

    if(!cf)
        return NULL;

    tmp = create_cake_frame();
    tmp->frame_rx = cf->frame_rx;
    tmp->frame_ry = cf->frame_ry;
    tmp->poly_rx = cf->poly_rx;
    tmp->poly_ry = cf->poly_ry;
    tmp->poly_rz = cf->poly_rz;
    if(cf->poly)
    {
        tmp->poly = D3_create_poly();
        D3_copy_poly(tmp->poly, cf->poly);
    }
    if(cf->rle)
    {
        tmp->rle = duplicate_rle(cf->rle);
    }
    return (CAKE_FRAME *)tmp;
}


CAKE_FRAME_ANI *    create_cake_frame_ani(void)
{
    CAKE_FRAME_ANI *cfa = NULL;

    cfa = (CAKE_FRAME_ANI *)GlobalAlloc(GPTR, sizeof(CAKE_FRAME_ANI));
    if(!cfa) 
        goto error;
    cfa->cake_frame = create_cake_frame();    // for store common data
    if(!cfa->cake_frame)
        goto error;
    cfa->last=NULL;
    cfa->next=NULL;
    return (CAKE_FRAME_ANI *)cfa;
error:
    if(cfa)
    {
        if(cfa->cake_frame) GlobalFree(cfa->cake_frame);
        GlobalFree(cfa);
    }
    return NULL;
}

void    destroy_cake_frame_ani(CAKE_FRAME_ANI **cfa)
{
    CAKE_FRAME_ANI *p,*q;
    p=(*cfa);
    while(p)
    {
        q=p;
        p=p->next;
        if(q)
        {
            destroy_cake_frame(&q->cake_frame);
            GlobalFree(q);
            q=NULL;
        }
    }
    (*cfa)=NULL;
}

SLONG   count_cake_frames(CAKE_FRAME_ANI *cfa)
{
    SLONG frames = 0;
    CAKE_FRAME_ANI *p;
    
    if( !cfa ) return 0;
    p = cfa->next;
    while(p)
    {
        ++frames;
        p=p->next;
    }
    return(frames);
}

SLONG   get_cake_frame_no(CAKE_FRAME_ANI *p,CAKE_FRAME_ANI *cfa)
{
    SLONG frame_no;
    CAKE_FRAME_ANI *q;
    
    if(!(p && cfa)) return 0;
    frame_no=0;
    q=(cfa)->next;
    while(q)
    {
        if(p==q) break;
        ++frame_no;
        q=q->next;
    }
    return(frame_no);
}



SLONG   save_cake_frame_ani_to_file(USTR *filename,CAKE_FRAME_ANI *cfa)
{
    FILE *fp=NULL;
    void *cp=NULL;
    OFFSET_CAKE *poc=NULL;
    SLONG back_file_offset;
    SLONG back_file_offset2;
    SLONG total_frames;
    SLONG now_frame;
    CAKE_FRAME_ANI *p;
    CAKE_FRAME *f;
    char    lower_filename[_MAX_FNAME];

    strcpy(lower_filename, (const char *)filename);
    CharLower((LPSTR)lower_filename);
    if(NULL==(fp=fopen((const char *)lower_filename,"wb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }

    //[HEAD_CAKE]
    head_cake.version = 0x00010000;
    cp=&head_cake;
    write_cake_to_file(fp,CAKE_ID_HEAD,cp);

    //[COMMON_CAKE]
    common_cake.flag = 0;
    cp=&common_cake;
    write_cake_to_file(fp,CAKE_ID_COMMON,cp);

    // Jack, 注释 [19:21,10/12/2002]
    // 如果cfa头中含有rle和poly(比如共用poly), 则直接将它们先写入文件中.
    // 这时, 在读取cak的时候, 我们需要将未出现[FRAME]之前的RLE和POLY赋给CFA的头中.
    //[COMMON RLE] & [COMMON POLY]
    p=cfa;
    f=p->cake_frame;
    if(f->rle) { write_cake_to_file(fp,CAKE_ID_RLE,f->rle); }
    if(f->poly) { write_cake_to_file(fp,CAKE_ID_POLY,f->poly); }

    //[OFFSET_CAKE]
    // prepair offset:
    // we alloc memory & write the offset cake first.
    // when we write each frame datas to the file, update offset.
    // when all frame datas are written ok, we rewrite the offset cake.
    back_file_offset=ftell(fp);
    total_frames=count_cake_frames(cfa);
    poc = create_offset_cake(total_frames);
    if(!poc) 
    {
        sprintf((char *)print_rec,"create offset cake error in save_cake_frame_ani_to_file()");
        log_error(1, (USTR*)print_rec);
        goto error;
    }
    cp=poc;
    write_cake_to_file(fp,CAKE_ID_OFFSET,cp);

    // [FRMAES ... ] 
    p=cfa->next;
    now_frame=0;
    while(p)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        f=p->cake_frame;
        if(f)
        {
            //[FRAME_CAKE]
            poc->offset[now_frame] = ftell(fp);

            frame_cake.frame_rx=f->frame_rx;
            frame_cake.frame_ry=f->frame_ry;
            frame_cake.poly_rx=f->poly_rx;
            frame_cake.poly_ry=f->poly_ry;
            frame_cake.poly_rz=f->poly_rz;
            write_cake_to_file(fp,CAKE_ID_FRAME,&frame_cake);

            //[FRAME datas...]
            if(f->rle) { write_cake_to_file(fp,CAKE_ID_RLE,f->rle); }
            if(f->poly) { write_cake_to_file(fp,CAKE_ID_POLY,f->poly); }
        }
        p=p->next;
        now_frame++;
    }

    //[END_CAKE]
    end_cake.flag = 0;
    write_cake_to_file(fp,CAKE_ID_END,&end_cake);

    // rewrite the offset cake
    back_file_offset2=ftell(fp);
    fseek(fp,back_file_offset,SEEK_SET);
    cp=poc;
    write_cake_to_file(fp,CAKE_ID_OFFSET,cp);
    fseek(fp,back_file_offset2,SEEK_SET);

    // finished anything
    if(fp) fclose(fp);
    destroy_offset_cake(poc);
    return TTN_OK;
error:
    if(fp) fclose(fp);
    destroy_offset_cake(poc);
    return TTN_NOT_OK;
}



SLONG   save_cake_frame_ani_to_opened_file(FILE *fp,CAKE_FRAME_ANI *cfa)
{
    /*
    FILE *fp=NULL;
    */
    void *cp=NULL;
    OFFSET_CAKE *poc=NULL;
    SLONG back_file_offset;
    SLONG back_file_offset2;
    SLONG total_frames;
    SLONG now_frame;
    CAKE_FRAME_ANI *p;
    CAKE_FRAME *f;

    /*
    if(NULL==(fp=fopen((const char *)filename,"wb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }
    */

    //[HEAD_CAKE]
    head_cake.version = 0x00010000;
    cp=&head_cake;
    write_cake_to_file(fp,CAKE_ID_HEAD,cp);

    //[COMMON_CAKE]
    common_cake.flag = 0;
    cp=&common_cake;
    write_cake_to_file(fp,CAKE_ID_COMMON,cp);

    p=cfa;
    f=p->cake_frame;
    if(f->rle) { write_cake_to_file(fp,CAKE_ID_RLE,f->rle); }
    if(f->poly) { write_cake_to_file(fp,CAKE_ID_POLY,f->poly); }

    //[OFFSET_CAKE]
    // prepair offset:
    // we alloc memory & write the offset cake first.
    // when we write each frame datas to the file, update offset.
    // when all frame datas are written ok, we rewrite the offset cake.

    back_file_offset=ftell(fp);
    total_frames=count_cake_frames(cfa);
    poc = create_offset_cake(total_frames);
    cp=poc;
    write_cake_to_file(fp,CAKE_ID_OFFSET,cp);

    // [FRMAES ... ] 
    p=cfa->next;
    now_frame=0;
    while(p)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        f=p->cake_frame;
        if(f)
        {
            //[FRAME_CAKE]
            poc->offset[now_frame] = ftell(fp);

            frame_cake.frame_rx=f->frame_rx;
            frame_cake.frame_ry=f->frame_ry;
            frame_cake.poly_rx=f->poly_rx;
            frame_cake.poly_ry=f->poly_ry;
            frame_cake.poly_rz=f->poly_rz;
            write_cake_to_file(fp,CAKE_ID_FRAME,&frame_cake);

            //[FRAME datas...]
            if(f->rle) { write_cake_to_file(fp,CAKE_ID_RLE,f->rle); }
            if(f->poly) { write_cake_to_file(fp,CAKE_ID_POLY,f->poly); }
        }
        p=p->next;
        now_frame++;
    }

    //[END_CAKE]
    end_cake.flag = 0;
    write_cake_to_file(fp,CAKE_ID_END,&end_cake);

    // rewrite the offset cake
    back_file_offset2=ftell(fp);
    fseek(fp,back_file_offset,SEEK_SET);
    cp=poc;
    write_cake_to_file(fp,CAKE_ID_OFFSET,cp);
    fseek(fp,back_file_offset2,SEEK_SET);

    // finished anything
    destroy_offset_cake(poc);
    return TTN_OK;
    /*
    if(fp) fclose(fp);
    return TTN_OK;
error:
    destroy_offset_cake(poc);
    if(fp) fclose(fp);
    return TTN_NOT_OK;
    */
}



SLONG   load_cake_frame_ani_from_opened_file(FILE *fp,CAKE_FRAME_ANI **cfa)
{
    /*
    FILE *fp;
    */
    CAKE_FRAME_ANI *p=NULL,*s=NULL;
    CAKE_FRAME *f=NULL;
    ULONG id;
    void *vp=NULL;
    SLONG main_pass;

    /*
    if(NULL==(fp=fopen((const char *)filename,"rb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }
    */
    (*cfa) = create_cake_frame_ani();
    p= (*cfa);
    f=p->cake_frame;
    main_pass=0;
    while(main_pass==0)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        read_cake_from_file(fp,&id,&vp);
        switch(id)
        {
        case CAKE_ID_HEAD:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_COMMON:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_OFFSET: // we load the frames sequencely,so skip it.
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_FRAME: // new frame begin
            {
                FRAME_CAKE *fc=(FRAME_CAKE *)vp;

                s=create_cake_frame_ani();
                s->last = p;
                p->next = s;

                p=p->next;
                f=p->cake_frame;

                f->frame_rx = fc->frame_rx;
                f->frame_ry = fc->frame_ry;
                f->poly_rx = fc->poly_rx;
                f->poly_ry = fc->poly_ry;
                f->poly_rz = fc->poly_rz;
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_RLE:   //current cake_frame(maybe cfa->cake_frame)'s rle
            {
                f->rle = (RLE *)vp;
            }
            break;
        case CAKE_ID_POLY:  //current cake_frame(maybe cfa->cake_frame)'s poly
            {
                f->poly = (POLY *)vp;
            }
            break;
        case CAKE_ID_END:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
                main_pass=1;
            }
            break;
        default:
            break;
        }
    }
    /*
    if(fp) fclose(fp);
    */
    return TTN_OK;
    /*
error:
    if(fp) fclose(fp);
    return TTN_NOT_OK;
    */
}



SLONG   load_cake_frame_ani_from_file(USTR *filename,CAKE_FRAME_ANI **cfa)
{
    PACK_FILE *fp = NULL;
    CAKE_FRAME_ANI *p=NULL,*s=NULL;
    CAKE_FRAME *f=NULL;
    ULONG id;
    void *vp=NULL;
    SLONG main_pass;

    if(NULL== (fp = pack_fopen((const char *)filename,"rb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }
    (*cfa) = create_cake_frame_ani();
    p= (*cfa);
    f=p->cake_frame;
    main_pass=0;
    while(main_pass==0)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        //Jack, 2002.4.26.
        //read_cake_from_file(fp,&id,&vp);
        naked_read_cake(fp, &id, &vp);

        switch(id)
        {
        case CAKE_ID_HEAD:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_COMMON:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_OFFSET: // we load the frames sequencely,so skip it.
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_FRAME: // new frame begin
            {
                FRAME_CAKE *fc=(FRAME_CAKE *)vp;

                s=create_cake_frame_ani();
                s->last = p;
                p->next = s;

                p=p->next;
                f=p->cake_frame;

                f->frame_rx = fc->frame_rx;
                f->frame_ry = fc->frame_ry;
                f->poly_rx = fc->poly_rx;
                f->poly_ry = fc->poly_ry;
                f->poly_rz = fc->poly_rz;
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_RLE:   //current cake_frame(maybe cfa->cake_frame)'s rle
            {
                f->rle = (RLE *)vp;
            }
            break;
        case CAKE_ID_POLY:  //current cake_frame(maybe cfa->cake_frame)'s poly
            {
                f->poly = (POLY *)vp;
            }
            break;
        case CAKE_ID_END:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
                main_pass=1;
            }
            break;
        default:
            break;
        }
    }
    if(fp) pack_fclose(fp);
    return TTN_OK;
error:
    if(fp) pack_fclose(fp);
    return TTN_NOT_OK;
}


SLONG   load_cake_frame_from_file(USTR *filename, SLONG frame_index, CAKE_FRAME **cf)
{
    PACK_FILE *fp = NULL;
    OFFSET_CAKE *poc;
    FRAME_CAKE *pfc;
    ULONG id;
    void *vp=NULL;
    SLONG   found_offset, i;
    RLE *common_rle = NULL;
    POLY *common_poly = NULL;

    if(frame_index < 0)
        return  TTN_ERROR;

    if(NULL==(fp=pack_fopen((const char *)filename,"rb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }

    //每个cak文件中, 起始的数据块一定是[HEAD],[COMMON],然后可能会有COMMON的
    //[RLE]和[POLY], 然后一定是[OFFSET], 之后是每帧的数据, 包含[FRAME], 
    //FRAME的[RLE]和[POLY].

    //[HEAD_CAKE]
    naked_read_cake(fp, &id, &vp);
    memcpy(&head_cake, vp, sizeof(HEAD_CAKE));
    GlobalFree(vp);
    vp = NULL;

    //[COMM_CAKE]
    naked_read_cake(fp, &id, &vp);
    memcpy(&common_cake, vp, sizeof(COMMON_CAKE));
    GlobalFree(vp);
    vp = NULL;

    //读取[COMMON_CAKE]以后的第一个数据块, 可能是[COMMON RLE], [COMMON_POLY] 或者 [OFFSET_CAKE]
    found_offset = FALSE;
    naked_read_cake(fp, &id, &vp);
    switch(id)
    {
    case CAKE_ID_RLE:
        common_rle = (RLE*)vp;
        break;
    case CAKE_ID_POLY:
        common_poly = (POLY *)vp;
        break;
    case CAKE_ID_OFFSET:
        poc = (OFFSET_CAKE *)vp;
        found_offset = TRUE;
        break;
    }

    //如果没有读取到[OFFSET_CAKE], 则继续读取下一个数据块.
    if(!found_offset)
    {
        naked_read_cake(fp, &id, &vp);
        switch(id)
        {
        case CAKE_ID_RLE:
            common_rle = (RLE*)vp;
            break;
        case CAKE_ID_POLY:
            common_poly = (POLY *)vp;
            break;
        case CAKE_ID_OFFSET:
            poc = (OFFSET_CAKE *)vp;
            found_offset = TRUE;
            break;
        }
    }

    //如果仍然没有读取到[OFFSET_CAKE], 则刚刚读取到的是[RLE]和[POLY], 而且这个数据块应该是[OFFSET_CAKE]
    if(!found_offset)
    {
        naked_read_cake(fp, &id, &vp);
        if(id != CAKE_ID_OFFSET)
        {
            log_error(1, "can't find offset cake from the file %s", filename);
            goto error;
        }
        else
        {
            if(frame_index >= poc->count)
            {
                log_error(1, "frame out range from file %s", filename);
                goto error;
            }
            poc = (OFFSET_CAKE *)vp;
            found_offset = TRUE;
        }
    }



    //依据读取到的[OFFSET_CAKE], 定位文件指标, 以备后面读取[FRAME]资料.
    pack_fseek(fp, poc->offset[frame_index], SEEK_SET);

    //[FRAMES...]
    //[FRAME_CAKE]
    naked_read_cake(fp, &id, &vp);
    if(id == CAKE_ID_FRAME)
    {
        pfc = (FRAME_CAKE *)vp;

        (*cf) = create_cake_frame();
        (*cf)->frame_rx = pfc->frame_rx;
        (*cf)->frame_ry = pfc->frame_ry;
        (*cf)->poly_rx = pfc->poly_rx;
        (*cf)->poly_ry = pfc->poly_ry;
        (*cf)->poly_rz = pfc->poly_rz;
        (*cf)->rle = NULL;
        (*cf)->poly = NULL;

        GlobalFree(vp);
        vp = NULL;
    }
    else
    {
        log_error(1, "can't find frame %d data of file %s", frame_index, filename);
        goto error;
    }

    //[FRAME datas...], 包含最多两个数据块, [RLE]和[POLY].
    for(i = 0; i < 2; i++)
    {
        naked_read_cake(fp, &id, &vp);
        switch(id)
        {
        case CAKE_ID_RLE:
            (*cf)->rle = (RLE*)vp;
            vp = NULL;
            break;
        case CAKE_ID_POLY:
            (*cf)->poly = (POLY*)vp;
            vp = NULL;
            break;
        default:
            GlobalFree(vp);
            vp = NULL;
            break;
        }
    }

    //如果COMMON存在[RLE]([POLY]), 而[FRAME datas...]中没有, 则作对应添加, 否则, 释放COMMON的[RLE]([POLY]).
    if(common_rle)
    {
        if(!(*cf)->rle) 
            (*cf)->rle = common_rle;
        else 
            destroy_rle(&common_rle);
    }
    if(common_poly)
    {
        if(!(*cf)->poly) 
            (*cf)->poly = common_poly;
        else
        {
            D3_destroy_poly(common_poly);
            common_poly = NULL;
        }
    }

    if(fp) pack_fclose(fp);
    return  TTN_OK;

error:
    if(common_rle) 
        destroy_rle(&common_rle);
    if(common_poly) 
    {
        D3_destroy_poly(common_poly);
        common_poly = NULL;
    }
    if(vp) GlobalFree(vp);
    if(fp) pack_fclose(fp);
    return  TTN_ERROR;
}


void    redraw_cake_frame(SLONG px, SLONG py, SLONG pz, CAKE_FRAME *cf, BMP *bitmap)
{
    SLONG x,y;
    if(! cf) return;
    // we think the (px,py) is the frame's left-top point in the screen position
    // so pz is reserved ???
    x=px + cf->frame_rx;
    y=py + cf->frame_ry;
    if(cf->rle)
        put_rle(x,y,cf->rle,bitmap);

}


void    scale_redraw_cake_frame_ani(SLONG sx,SLONG sy,SLONG scale_xl,SLONG scale_yl,CAKE_FRAME_ANI *cfa,BMP *bitmap)
{
    CAKE_FRAME_ANI *p=NULL;
    CAKE_FRAME *f=NULL;
    RLE *rle;
    double zoomx,zoomy,zoom;
    USTR info[48];

    if( !(cfa && bitmap) ) return;
    p=cfa->next;
    if(!p) return;
    f=p->cake_frame;
    if(!f) return;
    rle=f->rle;
    if(!rle) return;
    zoomx=zoomy=1.0;
    if(scale_xl)
        zoomx = (double)((scale_xl*1.0)/(rle->w*1.0));
    if(scale_yl)
        zoomy = (double)((scale_yl*1.0)/(rle->h*1.0));
    zoom=min(zoomx,zoomy);
    scale_put_rle(sx,sy,(SLONG)(rle->w*zoom),(SLONG)(rle->h*zoom),rle,bitmap);
    sprintf((char *)info,"%.3f",zoom);
    sprintf((char *)print_rec,"~C0~O3%s~C0~O0",info);
    print16(sx+scale_xl-(8*strlen((const char *)info)+4),sy+scale_yl-18,(USTR *)print_rec,PEST_PUT,bitmap);
}


CAKE_FRAME_ANI *    get_cake_frame_ani(CAKE_FRAME_ANI *cfa,CAKE_FRAME_ANI *p,SLONG get_flag)
{
    CAKE_FRAME_ANI *q;

    q=p;
    switch(get_flag)
    {
    case GET_HOME_FRAME:
        q=cfa->next;
        break;
    case GET_END_FRAME:
        q=cfa;
        while(q->next) q=q->next;
        break;
    case GET_LAST_FRAME:
        if(q->last) q=q->last;
        if(q == cfa) q=cfa->next;
        break;
    case GET_NEXT_FRAME:
        if(q->next) q=q->next;
        break;
    default:
        break;
    }
    return(q);
}

CAKE_FRAME_ANI *    get_specify_cake_frame_ani(CAKE_FRAME_ANI *cfa,SLONG frame)
{
    CAKE_FRAME_ANI *p;

    p=cfa;
    frame=frame+1;
    while(p && frame)
    {
        p=p->next;
        frame--;
    }
    return p;
}



SLONG   get_cak_frames(USTR *filename)
{
    FILE *fp = NULL;
    ULONG id;
    void *vp=NULL;
    SLONG main_pass;
    SLONG total_frames = 0;

    if(NULL==(fp=fopen((const char *)filename,"rb")))
    {
        sprintf((char *)print_rec,"file %s open error",filename);
        log_error(1,print_rec);
        goto error;
    }
    main_pass=0;
    while(main_pass==0)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        read_cake_from_file(fp,&id,&vp);
        switch(id)
        {
        case CAKE_ID_OFFSET: // we need known the frames
            {
                OFFSET_CAKE *poc = NULL;
                if(vp)
                {
                    poc = (OFFSET_CAKE *)vp;
                    total_frames = poc->count;
                    GlobalFree(vp);
                    vp = NULL;
                    main_pass = 1;
                }
            }
            break;
        default:
            if(vp) { GlobalFree(vp); vp=NULL; }
            break;
        }
    }
error:
    if(fp) fclose(fp);
    return total_frames;
}


SLONG   get_cake_frame_ani_size(CAKE_FRAME_ANI *cfa, SLONG *sx, SLONG *sy, SLONG *ex, SLONG *ey)
{
    CAKE_FRAME_ANI *p = NULL;
    CAKE_FRAME  *cf = NULL;
    RLE *rle = NULL;
    SLONG   g_sx, g_sy, g_ex, g_ey;
    SLONG   f_sx, f_sy, f_ex, f_ey;
    SLONG   first_frame = TRUE;

    if(!cfa)
        return TTN_ERROR;
    g_sx = g_sy = 0;
    g_ex = g_ey = 0;
    p = cfa->next;
    while(p)
    {
        cf = p->cake_frame;
        if(cf)
        {
            rle = cf->rle;
            if(rle)
            {
                f_sx = -cf->poly_rx;
                f_sy = -cf->poly_ry;
                f_ex = g_sx + rle->w;
                f_ey = g_sy + rle->h;
                if(first_frame)
                {
                    g_sx = f_sx;
                    g_sy = f_sy;
                    g_ex = f_ex;
                    g_ey = f_ey;
                    first_frame = FALSE;
                }
                else
                {
                    if(f_sx < g_sx) g_sx = f_sx;
                    if(f_sy < g_sy) g_sy = f_sy;
                    if(f_ex > g_ex) g_ex = f_ex;
                    if(f_ey > g_ey) g_ey = f_ey;
                }
            }
        }
        p=p->next;
    }
    (*sx) = g_sx;
    (*sy) = g_sy;
    (*ex) = g_ex;
    (*ey) = g_ey;
    return TTN_OK;
}



//////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS FOR PACKFILE(Jack, 2002.4.26.)                                                 //
//////////////////////////////////////////////////////////////////////////////////////////////
static  SLONG   naked_read_cake(PACK_FILE *fp, ULONG *id, void **p)
{
    SLONG data_size;
    SLONG read_size;

    read_size=0;

    pack_fread(&cake_id,1,sizeof(CAKE_ID),fp);
    (*id)=cake_id.id;
    read_size += sizeof(CAKE_ID);

    pack_fread(&cake_len,1,sizeof(CAKE_LEN),fp);
    data_size=cake_len.length;
    read_size += sizeof(CAKE_LEN);

    // in the following, data_size = cake_len.length
    (*p)=NULL;
    switch( (*id) )
    {
    case CAKE_ID_HEAD:
        if(cake_len.length )
        {
            HEAD_CAKE *hc;
            //data_size=sizoef(HEAD_CAKE);
            if( NULL== ( hc=(HEAD_CAKE *)GlobalAlloc(GPTR, cake_len.length) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read head_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(hc,1,cake_len.length,fp);
            (*p)=hc;
        }
        break;
    case CAKE_ID_COMMON:
        if(cake_len.length )
        {
            COMMON_CAKE *cc;
            //data_size=sizeof(COMMON_CAKE);
            if(NULL== ( cc=(COMMON_CAKE *)GlobalAlloc(GPTR, cake_len.length) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read common_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(cc,1,cake_len.length,fp);
            (*p)=cc;
        }
        break;
    case CAKE_ID_OFFSET:
        if(cake_len.length )
        {
            OFFSET_CAKE *oc;
            //data_size = sizeof(OFFSET_CAKE) - sizeof(SLONG) + oc->count * sizeof(SLONG) ;
            if(NULL==(oc=(OFFSET_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read offset_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(oc,1,cake_len.length,fp);
            (*p)=oc;
        }
        break;
    case CAKE_ID_FRAME:
        if(cake_len.length )
        {
            FRAME_CAKE *fc;
            //data_size = sizeof(FRAME_CAKE);
            if(NULL==(fc=(FRAME_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read frame_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(fc,1,cake_len.length,fp);
            (*p)=fc;
        }
        break;
    case CAKE_ID_END:
        if(cake_len.length )
        {
            END_CAKE *ec;
            //data_size = sizeof(END_CAKE);
            if(NULL==(ec=(END_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read end_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(ec,1,cake_len.length,fp);
            (*p)=ec;
        }
        break;
    case CAKE_ID_DUMMY:
        if(cake_len.length )
        {
            DUMMY_CAKE *dc;
            //data_size = sizeof(DUMMY_CAKE);
            if(NULL==(dc=(DUMMY_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read dummy_cake)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(dc,1,cake_len.length,fp);
            (*p)=dc;
        }
        break;
    case CAKE_ID_INFO:
        if(cake_len.length )
        {
            INFO_CAKE *ic;
            if(NULL==(ic=(INFO_CAKE *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read info_cake)");
                log_error(1,print_rec);
                goto error;
            }
            //data_size = sizeof(INFO_CAKE);
            pack_fread(ic,1,cake_len.length,fp);
            (*p)=ic;
        }
        break;
    case CAKE_ID_ALF:
        if(cake_len.length )
        {
            ALF *ac,tac;
            SLONG y;

            //data_size = sizeof(ALF) - sizeof(USTR *) + ac->w * ac->h ;
            pack_fread(&tac,1,sizeof(ALF)-sizeof(USTR *),fp);
            if(NULL==(ac=create_alf(tac.w, tac.h)))
            {
                sprintf((char *)print_rec,"memory alloc error(read ALF)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<ac->h;++y)
                pack_fread(ac->line[y],1,ac->w,fp);
            (*p)=ac;
        }
        break;
    case CAKE_ID_BMP:
        if(cake_len.length )
        {
            BMP *bc,tbc;
            SLONG y;

            //data_size = sizeof(BMP) - sizeof(PIXEL *) + bc->h * bc->pitch;
            pack_fread(&tbc,1,sizeof(BMP)-sizeof(PIXEL *),fp);
            if(NULL==(bc=(BMP *)create_bitmap(tbc.w, tbc.h) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read BMP)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<bc->h;++y)
                pack_fread(bc->line[y],1,bc->pitch,fp);

            convert_bitmap_fff2hi(bc);

            (*p)=bc;
        }
        break;
    case CAKE_ID_RLE:
        if(cake_len.length )
        {
            RLE *rc,trc;
            SLONG y,offset;

            //data_size = sizeof(RLE) + sizeof(USTR *)*(rc->h-1) + rc->size;
            pack_fread(&trc,1,sizeof(RLE)-sizeof(USTR *),fp);
            if(NULL==(rc=(RLE *)create_rle(trc.w, trc.h, trc.size) ) )
            {
                sprintf((char *)print_rec,"memory alloc error(read RLE)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<rc->h;++y)
            {
                pack_fread(&offset,1,sizeof(SLONG),fp);
                rc->line[y] = (USTR*)(rc->line[0]+offset);
            }
            pack_fread(rc->line[0],1,rc->size,fp);

            convert_rle_fff2hi(rc);

            (*p)=rc;
        }
        break;
    case CAKE_ID_IMG:
        if(cake_len.length )
        {
            IMG *ic,tic;
            SLONG y;

            //data_size = sizeof(IMG) - sizeof(USTR *) + ic->w * ic->h ;
            pack_fread(&tic,1,sizeof(IMG)-sizeof(USTR *),fp);
            if(NULL==(ic=create_img(tic.w, tic.h)))
            {
                sprintf((char *)print_rec,"memory alloc error(read IMG)");
                log_error(1,print_rec);
                goto error;
            }
            for(y=0;y<ic->h;++y)
                pack_fread(ic->line[y],1,ic->w,fp);
            (*p)=ic;
        }
        break;
    case CAKE_ID_PAL:
        if(cake_len.length )
        {
            PAL *pc;
            //data_size = sizeof(PAL);
            if(NULL==(pc=(PAL *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read PAL)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(pc,1,cake_len.length,fp);
            convert_buffer_fff2hi(pc->data,256);
            (*p)=pc;
        }
        break;
    case CAKE_ID_POLY:
        if(cake_len.length )
        {
            POLY *pc;
            //data_size = sizeof(POLY);
            if(NULL==(pc=(POLY *)GlobalAlloc(GPTR, cake_len.length)))
            {
                sprintf((char *)print_rec,"memory alloc error(read poly)");
                log_error(1,print_rec);
                goto error;
            }
            pack_fread(pc,1,cake_len.length,fp);
            (*p)=pc;
        }
        break;
    default:
        (*p)=NULL;
        break;
    }
    read_size += data_size;
    return read_size;
error:
    (*p)=NULL;
    return -read_size;
}



SLONG   naked_read_cake_frame_ani(PACK_FILE *fp, CAKE_FRAME_ANI **cfa)
{
    CAKE_FRAME_ANI *p=NULL,*s=NULL;
    CAKE_FRAME *f=NULL;
    ULONG id;
    void *vp=NULL;
    SLONG main_pass;

    (*cfa) = create_cake_frame_ani();
    p= (*cfa);
    f=p->cake_frame;
    main_pass=0;
    while(main_pass==0)
    {
        // Jack, for SeaAge, Event engine. [09:33,10/7/2002]
        //idle_loop();

        naked_read_cake(fp,&id,&vp);
        switch(id)
        {
        case CAKE_ID_HEAD:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_COMMON:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_OFFSET: // we load the frames sequencely,so skip it.
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_FRAME: // new frame begin
            {
                FRAME_CAKE *fc=(FRAME_CAKE *)vp;

                s=create_cake_frame_ani();
                s->last = p;
                p->next = s;

                p=p->next;
                f=p->cake_frame;

                f->frame_rx = fc->frame_rx;
                f->frame_ry = fc->frame_ry;
                f->poly_rx = fc->poly_rx;
                f->poly_ry = fc->poly_ry;
                f->poly_rz = fc->poly_rz;
                if(vp) { GlobalFree(vp); vp=NULL; }
            }
            break;
        case CAKE_ID_RLE:
            {
                f->rle = (RLE *)vp;
            }
            break;
        case CAKE_ID_POLY:
            {
                f->poly = (POLY *)vp;
            }
            break;
        case CAKE_ID_END:
            {
                if(vp) { GlobalFree(vp); vp=NULL; }
                main_pass=1;
            }
            break;
        default:
            break;
        }
    }
    return TTN_OK;
}


