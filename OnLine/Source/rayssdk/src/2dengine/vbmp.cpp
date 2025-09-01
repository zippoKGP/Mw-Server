/*
**      BMP.CPP
**      BMP file functions.
**
**      ZJian,2000.7.11
**
*/
#include "rays.h"
#include "xsystem.h"
#include "vbmp.h"
#include "packfile.h"

/*
#pragma pack (push)
#pragma pack (2)
typedef struct tagBITMAPHEADER
{
    UHINT type;
    ULONG size;
    ULONG reserved;
    ULONG off_bits;
    ULONG head_size;
    ULONG width;
    ULONG height;
    UHINT planes;
    UHINT bit; 
} BITMAPHEADER,*LPBITMAPHEADER;;
#pragma pack (pop)
*/


EXPORT  BMP*    FNBACK  BMP_load_file(char *filename)
{
    BITMAPHEADER bmph;
    PACK_FILE *f=NULL;
    BMP *bmp=NULL;
    char *tmp=NULL,*line=NULL;
    int i,pitch;
    unsigned j;
    long size;
    
    if (NULL==(f=pack_fopen(filename,"rb"))) 
    {
        sprintf((char *)print_rec,"Can't open bmp file %s",filename);
        log_error(1,print_rec);
        return NULL;
    }
    pack_fread(&bmph,1,sizeof(bmph),f);
    if (bmph.type != 19778 || (bmph.bit!=0x18 && bmph.bit !=8)) 
    {
        if(f) pack_fclose(f);
        return NULL;
    }
    
    if(8==bmph.bit)   // Load 8bit BMP
    {
        BITMAPINFOHEADER bitinfo;
        RGBQUAD pal[256];
        unsigned short p[256];
        int palcount=0,size,i,j,linesize;
        BYTE * buffer;
        
        pack_fseek(f,sizeof(BITMAPFILEHEADER),SEEK_SET);
        pack_fread(&bitinfo,1,sizeof(bitinfo),f);
        
        if(bitinfo.biClrUsed)
        {
            size=bitinfo.biClrUsed*sizeof(RGBQUAD);
            palcount=bitinfo.biClrUsed;
        }
        else
        {
            if(bitinfo.biBitCount==8)
            {
                size=256*sizeof(RGBQUAD);
                palcount=256;
            }
            else
            {
                //ZJian add 2000.09.21
                size=0;
                palcount=0;
            }
        }
        pack_fread(&pal,1,size,f);
        for(i=0;i<palcount;i++)
            p[i]=true2hi(*((unsigned long *)(pal+i)));
        //      p[0]=MASKCOLOR;
        bmp=create_bitmap(bitinfo.biWidth,bitinfo.biHeight);
        linesize=(bitinfo.biWidth+3)&0xfffffffc;
        buffer=(BYTE*)GlobalAlloc(GPTR, linesize);
        for(i=bitinfo.biHeight-1;i>=0;i--)
        {
            pack_fread(buffer,1,linesize,f);
            for(j=0;j<bitinfo.biWidth;j++)
                bmp->line[i][j]=p[buffer[j]];
        }
        GlobalFree(buffer);
        if(f) pack_fclose(f);
        return bmp;
    }
    
    pack_fseek(f,bmph.off_bits,SEEK_SET);
    size=(bmph.width*3+3)/4*4*bmph.height;
    if ((line=tmp=(char *)GlobalAlloc(GPTR, size))==NULL) 
    {
        if(f) pack_fclose(f);
        return NULL;
    }
    pack_fread(tmp,1,size,f);
    if(f) pack_fclose(f);
    bmp=create_bitmap(bmph.width,bmph.height);
    pitch=(bmph.width*3+3)&0xfffffffc;
    
    for (i=bmph.height-1;i>=0;i--,line+=pitch) 
    {
        for (j=0;j<bmph.width;j++) 
            bmp->line[i][j]=true2hi(*((unsigned long *)(line+j*3)));
    }
    GlobalFree(tmp);
    return bmp;
}


EXPORT  SLONG   FNBACK  BMP_save_file(BMP *bmp,char *filename)
{
    FILE *f=NULL;
    BITMAPFILEHEADER bithead;
    BITMAPINFOHEADER bitinfo;
    int headsize,linesize,i,j,s;
    unsigned long color;
    
    sprintf((char *)print_rec,"Can't save null bmp to file %s",filename);
    log_error(!bmp,print_rec);
    
    linesize=(bmp->w*3+3)&0xfffffffc;
    s=linesize-bmp->w*3;
    if((f=fopen(filename,"wb"))==NULL) 
    {
        sprintf((char *)print_rec,"Can't open bmp file %s",filename);
        log_error(1,print_rec);
        return -1;
    }
    headsize=sizeof(bithead)+sizeof(bitinfo);
    bithead.bfType='MB';
    bithead.bfSize=headsize+linesize*bmp->h;
    bithead.bfReserved1=bithead.bfReserved2=0;
    bithead.bfOffBits=headsize;
    fwrite(&bithead,1,sizeof(bithead),f);
    bitinfo.biSize=sizeof(bitinfo);
    bitinfo.biWidth=bmp->w;
    bitinfo.biHeight=bmp->h;
    bitinfo.biPlanes=1;
    bitinfo.biBitCount=24;
    bitinfo.biCompression=BI_RGB;
    bitinfo.biSizeImage=0;
    bitinfo.biXPelsPerMeter=72;
    bitinfo.biYPelsPerMeter=72;
    bitinfo.biClrUsed=0;
    bitinfo.biClrImportant=0;
    fwrite(&bitinfo,1,sizeof(bitinfo),f);
    for(i=bmp->h-1;i>=0;i--)
    {
        for(j=0;j<bmp->w;j++)
        {
            color=hi2true(bmp->line[i][j]);
            fwrite(&color,1,3,f);
        }
        fwrite(&color,1,s,f);
    }
    if(f) fclose(f);
    return 0;
}


EXPORT  SLONG   FNBACK  BMP_save_file(UHINT *buffer,SLONG xl,SLONG yl,char *filename)
{
    FILE *f=NULL;
    BITMAPFILEHEADER bithead;
    BITMAPINFOHEADER bitinfo;
    int headsize,linesize,i,j,s;
    unsigned long color;
    unsigned long offset;
    
    if(!buffer) return TTN_NOT_OK;
    
    linesize=(xl*3+3)&0xfffffffc;
    s=linesize-xl*3;
    if((f=fopen(filename,"wb"))==NULL) 
    {
        sprintf((char *)print_rec,"Can't open bmp file %s",filename);
        log_error(1,print_rec);
        return -1;
    }
    headsize=sizeof(bithead)+sizeof(bitinfo);
    bithead.bfType='MB';
    bithead.bfSize=headsize+linesize*yl;
    bithead.bfReserved1=bithead.bfReserved2=0;
    bithead.bfOffBits=headsize;
    fwrite(&bithead,1,sizeof(bithead),f);
    bitinfo.biSize=sizeof(bitinfo);
    bitinfo.biWidth=xl;
    bitinfo.biHeight=yl;
    bitinfo.biPlanes=1;
    bitinfo.biBitCount=24;
    bitinfo.biCompression=BI_RGB;
    bitinfo.biSizeImage=0;
    bitinfo.biXPelsPerMeter=72;
    bitinfo.biYPelsPerMeter=72;
    bitinfo.biClrUsed=0;
    bitinfo.biClrImportant=0;
    fwrite(&bitinfo,1,sizeof(bitinfo),f);
    for(i=yl-1;i>=0;i--)
    {
        for(j=0;j<xl;j++)
        {
            offset = i * xl + j;
            color=hi2true(buffer[offset]);
            fwrite(&color,1,3,f);
        }
        fwrite(&color,1,s,f);
    }
    if(f) fclose(f);
    return 0;
}
//==================================================================
