/*
**      PCX.CPP
**      PCX functions.
**
**      ZJian, 2000.7.16.
**          256 color PCX support.
**      Dixon, 2000.9.1.
**          16M color PCX file format & decode.
**      ZJian, 2000.9.2.
**          16M color PCX support. 
**   
*/
#include "rays.h"
#include "xsystem.h"
#include "vpcx.h"
#include "packfile.h"



/*
#pragma pack(push)
#pragma pack(1)
typedef struct tagPCX_HEAD
{
   UCHR  manufacturer;
   UCHR  version;
   UCHR  encoding;
   UCHR  bitsPerPixel;
   SHINT xMin;
   SHINT yMin;
   SHINT xMax;
   SHINT yMax;
   SHINT hResolution;
   SHINT vResolution;
   UCHR  palette[48];
   UCHR  videoMode;
   UCHR  colorPlanes;
   SHINT bytesPerLines;
   SHINT paletteType;
   SHINT shResolution;
   SHINT svResolution;
   UCHR  filler[54];
} PCX_HEAD,*LPPCX_HEAD;
#pragma pack(pop)
*/


static PCX_HEAD pcx_head;
static USTR *pcx_file_buffer=NULL;
static USTR  pcx_palette[768];

static void decode_true_color_pcx_image(BMP *bmp);
static void decode_256_color_pcx_image(BMP *bmp);


EXPORT BMP* FNBACK PCX_load_file(char *filename)
{
    PACK_FILE *f=NULL;
    BMP *bmp=NULL;
    UCHR ch;
    SLONG pcx_file_size;
    
    if(NULL==(f=pack_fopen(filename,"rb")))
    {
        sprintf((char *)print_rec,"ERROR:can't open pcx file %s",filename);
        log_error(1,print_rec);
        goto error;
    }
    pack_fread(&pcx_head,1,sizeof(PCX_HEAD),f);
    if(0x0a != pcx_head.manufacturer)
    {
        sprintf((char *)print_rec,"ERROR:pcx file %s head check error",filename);
        log_error(1,print_rec);
        goto error;
    }
    if(1 != pcx_head.encoding)
    {
        sprintf((char *)print_rec,"ERROR:pcx file %s format not support",filename);
        log_error(1,print_rec);
        goto error;
    }
    if(8 != pcx_head.bitsPerPixel)
    {
        sprintf((char *)print_rec,"ERROR:pcx file %s format not support",filename);
        log_error(1,print_rec);
        goto error;
    }
    if(NULL==(bmp=create_bitmap(pcx_head.xMax-pcx_head.xMin+1,pcx_head.yMax-pcx_head.yMin+1)))
    {
        sprintf((char *)print_rec,"ERROR:create bitmap error while reading pcx file %s",filename);
        log_error(1,print_rec);
        goto error;
    }
    pcx_file_size=pack_fsize(f);
    pcx_file_buffer=(UCHR *)GlobalAlloc(GPTR, pcx_file_size);
    if(NULL==pcx_file_buffer)
    {
        sprintf((char *)print_rec,"ERROR:memory alloc error");
        log_error(1,print_rec);
        goto error;
    }
    pack_fread(pcx_file_buffer,1,pcx_file_size,f);
    switch(pcx_head.bitsPerPixel*pcx_head.colorPlanes)
    {
    case 8:// 256 colors 
        pack_fseek(f,-769L,SEEK_END);
        pack_fread(&ch,1,1,f);
        if(0x0c != ch)
        {
            sprintf((char *)print_rec,"ERROR:pcx file %s palette check error",filename);
            log_error(1,print_rec);
            goto error;
        }
        pack_fread(&pcx_palette,1,768,f);
        if(f) pack_fclose(f);
        decode_256_color_pcx_image(bmp);
        break;
    case 24:// true color
        if(f) pack_fclose(f);
        decode_true_color_pcx_image(bmp);
        break;
    default:
        sprintf((char *)print_rec,"ERROR:pcx file %s format not support",filename);
        log_error(1,print_rec);
        goto error;
        break;
    }
    if(pcx_file_buffer)
    {
        GlobalFree(pcx_file_buffer);
        pcx_file_buffer = NULL;
    }
    return(bmp);
error:
    if(pcx_file_buffer)
    {
        GlobalFree(pcx_file_buffer);
        pcx_file_buffer = NULL;
    }
    if(f) pack_fclose(f);
    destroy_bitmap(&bmp);
    return(NULL);
}



static void decode_256_color_pcx_image(BMP *bmp)
{
    SLONG source_idx=0,target_idx=0;
    UCHR data=0,count=0,r,g,b,c;
    UCHR *decode_buffer=NULL;
    SLONG i,decode_y=0;
    
    decode_buffer=(UCHR *)GlobalAlloc(GPTR, bmp->w*3+64);
    while(decode_y<bmp->h)
    {
        data=pcx_file_buffer[source_idx++];
        if(0xc0 != (data&0xc0))
        {
            decode_buffer[target_idx++]=data;
        }
        else
        {
            count=(unsigned char)(data&0x3f);
            data=pcx_file_buffer[source_idx++];
            memset(&decode_buffer[target_idx],data,count);
            target_idx=target_idx+count;
        }
        if(target_idx>=bmp->w)  // RGB decode 1 line OK
        {
            target_idx=0;
            for(i=0;i<bmp->w;i++)
            {
                c=decode_buffer[i];
                r=pcx_palette[c+c+c+0];
                g=pcx_palette[c+c+c+1];
                b=pcx_palette[c+c+c+2];
                bmp->line[decode_y][i]=true2hi((r<<16)|(g<<8)|b);
            }
            decode_y++;
        }
    }
    if(decode_buffer)
    {
        GlobalFree(decode_buffer);
        decode_buffer = NULL;
    }
}



static void decode_true_color_pcx_image(BMP *bmp)
{
    SLONG source_idx=0,target_idx=0;
    UCHR data=0,count=0,r,g,b;
    UCHR *decode_buffer=NULL;
    SLONG i,decode_y=0;
    
    decode_buffer=(UCHR *)GlobalAlloc(GPTR, bmp->w*3+64);
    while(decode_y<bmp->h)
    {
        data=pcx_file_buffer[source_idx++];
        if(0xc0 != (data&0xc0))
        {
            decode_buffer[target_idx++]=data;
        }
        else
        {
            count=(unsigned char)(data&0x3f);
            data=pcx_file_buffer[source_idx++];
            memset(&decode_buffer[target_idx],data,count);
            target_idx=target_idx+count;
        }
        if(target_idx>=bmp->w*3)  // RGB decode 1 line OK
        {
            target_idx=0;
            for(i=0;i<bmp->w;i++)
            {
                r=decode_buffer[i];
                g=decode_buffer[i+bmp->w];
                b=decode_buffer[i+bmp->w+bmp->w];
                bmp->line[decode_y][i]=true2hi((r<<16)|(g<<8)|b);
            }
            decode_y++;
        }
    }
    if(decode_buffer)
    {
        GlobalFree(decode_buffer);
        decode_buffer = NULL;
    }
}


//=====================================================================
