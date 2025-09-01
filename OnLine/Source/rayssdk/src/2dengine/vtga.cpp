/*
**      VTGA.CPP
**      TGA file functions.
**
**      Dixon,  2000.6.12.  
**          TGA file format & decode function.
**      ZJian,  2000.10.19.
**          Optimized.
*/
#include "rays.h"
#include "xsystem.h"
#include "vtga.h"
#include "packfile.h"

#define TGA_UNCOMPRESS        2
#define TGA_RLE_COMPRESS      10
#define TGA16_R(c)          ((UCHR)((((c)&0x7c00)>>7)&0xff))
#define TGA16_G(c)          ((UCHR)((((c)&0x03e0)>>2)&0xff))
#define TGA16_B(c)          ((UCHR)((((c)&0x001f)<<3)&0xff))

/*
#pragma pack(push)
#pragma pack(1)
typedef struct tagTGA_HEAD
{
   UCHR  bIdSize;
   UCHR  bColorMapType;
   UCHR  bImageType;
   UHINT iColorMapStart;
   UHINT iColorMapLength;
   UCHR  bColorMapBits;

   UHINT ixStart;
   UHINT iyStart;
   UHINT iWidth;
   UHINT iHeight;

   UCHR  bBitsPerPixel;
   UCHR  bDescriptor;
} TGA_HEAD,*LPTGA_HEAD;;
#pragma pack(pop)
*/


static  UCHR    tga_trans_level = 128;

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EXPORT  void    FNBACK  TGA_set_transparency_level(SLONG level)
{
    if(level>=0 && level<255)
        tga_trans_level = (UCHR)level;
    else
        tga_trans_level = (UCHR)128;
}


EXPORT ABMP* FNBACK TGA_load_file_with_alpha(char *filename)
{
    TGA_HEAD tga_head;
    PACK_FILE *f=NULL;
    ABMP *abmp=NULL;
    UCHR r=0,g=0,b=0,a=0xff,t[6]={0};
    UHINT x,y,cc;
    UCHR count,flag;
    SLONG i,offset,size;
    PIXEL color;
    
    if(NULL==(f=pack_fopen(filename,"rb")))
    {
        sprintf((char *)print_rec,"Can't open tga file %s",filename);
        log_error(1,print_rec);
        goto error;
    }
    // read file head
    pack_fread(&tga_head,1,sizeof(TGA_HEAD),f);
    if(NULL==(abmp=create_abitmap(tga_head.iWidth,tga_head.iHeight)))
    {
        sprintf((char *)print_rec,"%s","memory alloc error");
        log_error(1,print_rec);
        goto error;
    }
    // skip to file data 
    pack_fseek(f,tga_head.bIdSize,SEEK_CUR);
    pack_fseek(f,tga_head.iColorMapLength,SEEK_CUR);
    switch(tga_head.bImageType)
    {
    case TGA_UNCOMPRESS:
        {
            switch(tga_head.bBitsPerPixel)
            {
            case 32: //AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        pack_fread(&a,1,1,f);
                        *((PIXEL *)abmp->line[abmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        abmp->line[abmp->h-1-y][abmp->w*SIZEOFPIXEL+x]=a;
                    }
                }
                break;
            case 24: //RRRRRRRRGGGGGGGGBBBBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        *((PIXEL *)abmp->line[abmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        abmp->line[abmp->h-1-y][abmp->w*SIZEOFPIXEL+x]=a;
                    }
                }
                break;
            case 15:
            case 16://ARRRRRGG GGGBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&t[0],1,1,f);
                        pack_fread(&t[1],1,1,f);
                        cc=*(UHINT *)&t[0];
                        r=TGA16_R(cc);
                        g=TGA16_G(cc);
                        b=TGA16_B(cc);
                        *((PIXEL *)abmp->line[abmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        abmp->line[abmp->h-1-y][abmp->w*sizeof(PIXEL)+x]=a;
                    }
                }
                break;
            default:
                sprintf((char *)print_rec,"%s","tga image type not support");
                log_error(1,print_rec);
                goto error;
                break;
            }
        }
        break;
    case TGA_RLE_COMPRESS:
        {
            offset=0;
            size=tga_head.iHeight*tga_head.iWidth;
            while(offset<size)
            {
                pack_fread(&count,1,1,f);
                flag = (UCHR)(count & 0x80);
                if(0==flag)
                {
                    flag = (UCHR)(count+1);
                    for(i=0;i<flag;i++)
                    {
                        switch(tga_head.bBitsPerPixel)
                        {
                        case 32:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            pack_fread(&a,1,1,f);
                            break;
                        case 24:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            a=255;
                            break;
                        case 16:
                        case 15:
                            pack_fread(&t[0],1,1,f);
                            pack_fread(&t[1],1,1,f);
                            cc=*(UHINT *)&t[0];
                            r=TGA16_R(cc);
                            g=TGA16_G(cc);
                            b=TGA16_B(cc);
                            a=255;
                            break;
                            
                        }
                        y=(UHINT)(offset/tga_head.iWidth);
                        x=(UHINT)(offset-y*tga_head.iWidth);
                        *((PIXEL *)abmp->line[abmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        abmp->line[abmp->h-1-y][abmp->w*SIZEOFPIXEL+x]=a;

                        if(offset>=size)
                            continue;
                        offset++;
                    }
                }
                else
                {
                    flag=(UCHR)(count-127);
                    switch(tga_head.bBitsPerPixel)
                    {
                    case 32:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        pack_fread(&a,1,1,f);
                        break;
                    case 24:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        a=255;
                        break;
                    case 16:
                    case 15:
                        pack_fread(&t[0],1,1,f);
                        pack_fread(&t[1],1,1,f);
                        cc=*(UHINT *)&t[0];
                        r=TGA16_R(cc);
                        g=TGA16_G(cc);
                        b=TGA16_B(cc);
                        a=255;
                        break;
                        
                    }
                    color=true2hi((r<<16)|(g<<8)|b);
                    for(i=0;i<flag;i++)
                    {
                        if(offset>=size)
                            continue;
                        y=(UHINT)(offset/tga_head.iWidth);
                        x=(UHINT)(offset-y*tga_head.iWidth);
                        *((PIXEL *)abmp->line[abmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        abmp->line[abmp->h-1-y][abmp->w*sizeof(PIXEL)+x]=a;
                        offset++;
                    }
                }
            }
        }
        break;
    default:
        sprintf((char *)print_rec,"%s","tga image type not support");
        log_error(1,print_rec);
        goto error;
        break;
    }
    if(f) pack_fclose(f);

    if(tga_head.bDescriptor  & 0x20)
    {
        rotate_abitmap_top_bottom(abmp);
    }
    if(tga_head.bDescriptor  & 0x10)
    {
        rotate_abitmap_left_right(abmp);
    }

    return(abmp);
error:
    if(f) pack_fclose(f);
    if(abmp) destroy_abitmap(&abmp);
    return(NULL);
}



EXPORT BMP* FNBACK TGA_load_file(char *filename)
{
    TGA_HEAD tga_head;
    PACK_FILE *f=NULL;
    BMP *bmp=NULL;
    PIXEL color;
    UCHR r=0,g=0,b=0,a=0xff,t[6]={0};
    UHINT x,y,cc;
    UCHR count,flag;
    SLONG offset,size,i;
    
    if(NULL==(f=pack_fopen(filename,"rb")))
    {
        sprintf((char *)print_rec,"Can't open tga file %s",filename);
        log_error(1,print_rec);
        goto error;
    }
    // read file head
    pack_fread(&tga_head,1,sizeof(TGA_HEAD),f);
    if(NULL==(bmp=create_bitmap(tga_head.iWidth,tga_head.iHeight)))
    {
        sprintf((char *)print_rec,"%s","memory alloc error");
        log_error(1,print_rec);
        goto error;
    }
    // skip to file data 
    pack_fseek(f,tga_head.bIdSize,SEEK_CUR);
    pack_fseek(f,tga_head.iColorMapLength,SEEK_CUR);
    switch(tga_head.bImageType)
    {
    case TGA_UNCOMPRESS:
        {
            switch(tga_head.bBitsPerPixel)
            {
            case 32: //AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        pack_fread(&a,1,1,f);
                        if(a>=tga_trans_level)
                        {
                            if(r<8 && g<8 && b<8)
                                b=8;
                        }
                        else
                        {
                            r=g=b=0;
                        }
                        *((PIXEL *)bmp->line[bmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                    }
                }
                break;
            case 24: //RRRRRRRRGGGGGGGGBBBBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        a=255;
                        if(a>=tga_trans_level)
                        {
                            if(r<8 && g<8 && b<8)
                                b=8;
                        }
                        else
                        {
                            r=g=b=0;
                        }
                        *((PIXEL *)bmp->line[bmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                    }
                }
                break;
            case 15:
            case 16://ARRRRRGG GGGBBBBB
                for(y=0;y<tga_head.iHeight;y++)
                {
                    for(x=0;x<tga_head.iWidth;x++)
                    {
                        pack_fread(&t[0],1,1,f);
                        pack_fread(&t[1],1,1,f);
                        cc=*(UHINT *)&t[0];
                        r=TGA16_R(cc);
                        g=TGA16_G(cc);
                        b=TGA16_B(cc);
                        a=255;
                        if(a>=tga_trans_level)
                        {
                            if(r<8 && g<8 && b<8)
                                b=8;
                        }
                        else
                        {
                            r=g=b=0;
                        }
                        *((PIXEL *)bmp->line[bmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                    }
                }
                break;
            default:
                sprintf((char *)print_rec,"%s","tga image type not support");
                log_error(1,print_rec);
                goto error;
                break;
            }
        }
        break;
    case TGA_RLE_COMPRESS:
        {
            offset=0;
            size=tga_head.iHeight*tga_head.iWidth;
            while(offset<size)
            {
                pack_fread(&count,1,1,f);
                flag = (UCHR)(count & 0x80);
                if(0==flag)
                {
                    flag = (UCHR)(count+1);
                    for(i=0;i<flag;i++)
                    {
                        switch(tga_head.bBitsPerPixel)
                        {
                        case 32:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            pack_fread(&a,1,1,f);
                            break;
                        case 24:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            a=255;
                            break;
                        case 16:
                        case 15:
                            pack_fread(&t[0],1,1,f);
                            pack_fread(&t[1],1,1,f);
                            cc=*(UHINT *)&t[0];
                            r=TGA16_R(cc);
                            g=TGA16_G(cc);
                            b=TGA16_B(cc);
                            a=255;
                            break;
                            
                        }
                        if(a>=tga_trans_level)
                        {
                            if(r<8 && g<8 && b<8)
                                b=8;
                        }
                        else
                        {
                            r=g=b=0;
                        }
                        y=(UHINT)(offset/tga_head.iWidth);
                        x=(UHINT)(offset-y*tga_head.iWidth);
                        *((PIXEL *)bmp->line[bmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        if(offset>=size)
                            continue;
                        offset++;
                    }
                }
                else
                {
                    flag=(UCHR)(count-127);
                    switch(tga_head.bBitsPerPixel)
                    {
                    case 32:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        pack_fread(&a,1,1,f);
                        break;
                    case 24:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        a=255;
                        break;
                    case 16:
                    case 15:
                        pack_fread(&t[0],1,1,f);
                        pack_fread(&t[1],1,1,f);
                        cc=*(UHINT *)&t[0];
                        r=TGA16_R(cc);
                        g=TGA16_G(cc);
                        b=TGA16_B(cc);
                        a=255;
                        break;
                        
                    }
                    if(a>=tga_trans_level)
                    {
                        if(r<8 && g<8 && b<8)
                            b=8;
                    }
                    else
                    {
                        r=g=b=0;
                    }
                    color=true2hi((r<<16)|(g<<8)|b);
                    for(i=0;i<flag;i++)
                    {
                        if(offset>=size)
                            continue;
                        y=(UHINT)(offset/tga_head.iWidth);
                        x=(UHINT)(offset-y*tga_head.iWidth);
                        *((PIXEL *)bmp->line[bmp->h-1-y]+x)=true2hi((r<<16)|(g<<8)|b);
                        offset++;
                    }
                }
            }
        }
        break;
    default:
        sprintf((char *)print_rec,"%s","tga image type not support");
        log_error(1,print_rec);
        goto error;
        break;
    }
    if(f) pack_fclose(f);
    if(tga_head.bDescriptor & 0x20)
    {
        rotate_bitmap_top_bottom(bmp);
    }
    if(tga_head.bDescriptor & 0x10)
    {
        rotate_bitmap_left_right(bmp);
    }
    return(bmp);
error:
    if(f) pack_fclose(f);
    destroy_bitmap(&bmp);
    return(NULL);
}


EXPORT  SLONG   FNBACK  TGA_load_file_image(USTR *filename, SLONG *image_width, SLONG *image_height, 
                                            USTR **image_buffer, USTR **alpha_buffer)
{
    TGA_HEAD tga_head;
    PACK_FILE *  f=NULL;
    UCHR    r=0, g=0, b=0, a=0xff, t[6]={0};
    UHINT   x,y,cc;
    UCHR    count,flag;
    SLONG   i,offset,size;
    USTR    *image = NULL, *alpha = NULL;
    SLONG   pix_offset, pix_offset2;
    
    if(NULL==(f = pack_fopen((const char *)filename,"rb")) )
    {
        sprintf((char *)print_rec,"Can't open tga file %s",filename);
        log_error(1,print_rec);
        goto error;
    }
    // read file head
    pack_fread(&tga_head, 1, sizeof(TGA_HEAD), f);
    if(NULL == (image = (USTR *)malloc(tga_head.iWidth * tga_head.iHeight * 3)) )
    {
        sprintf((char *)print_rec,"memory alloc error");
        log_error(1, print_rec);
        goto error;
    }
    if(NULL == (alpha = (USTR *)malloc(tga_head.iWidth * tga_head.iHeight)) )
    {
        sprintf((char *)print_rec,"memory alloc error");
        log_error(1, print_rec);
        goto error;
    }
    memset(image, 0, tga_head.iWidth * tga_head.iHeight * 3);
    memset(alpha, 0xff, tga_head.iWidth * tga_head.iHeight);

    // skip to file data 
    pack_fseek(f, tga_head.bIdSize, SEEK_CUR);
    pack_fseek(f, tga_head.iColorMapLength, SEEK_CUR);
    switch(tga_head.bImageType)
    {
    case TGA_UNCOMPRESS:
        {
            switch(tga_head.bBitsPerPixel)
            {
            case 32: //AAAAAAAARRRRRRRRGGGGGGGGBBBBBBBB
                {
                    for(y=0;y<tga_head.iHeight;y++)
                    {
                        for(x=0;x<tga_head.iWidth;x++)
                        {
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            pack_fread(&a,1,1,f);

                            pix_offset = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;
                            image[3 * pix_offset + 0] = r;
                            image[3 * pix_offset + 1] = g;
                            image[3 * pix_offset + 2] = b;
                            alpha[pix_offset] = a;
                        }
                    }
                    break;
                }
            case 24: //RRRRRRRRGGGGGGGGBBBBBBBB
                {
                    for(y=0;y<tga_head.iHeight;y++)
                    {
                        for(x=0;x<tga_head.iWidth;x++)
                        {
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);

                            pix_offset = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;
                            image[3 * pix_offset + 0] = r;
                            image[3 * pix_offset + 1] = g;
                            image[3 * pix_offset + 2] = b;
                            alpha[pix_offset] = a;
                        }
                    }
                    break;
                }
            case 15:
            case 16://ARRRRRGG GGGBBBBB
                {
                    for(y=0;y<tga_head.iHeight;y++)
                    {
                        for(x=0;x<tga_head.iWidth;x++)
                        {
                            pack_fread(&t[0],1,1,f);
                            pack_fread(&t[1],1,1,f);
                            cc = *(UHINT *)&t[0];
                            r = TGA16_R(cc);
                            g = TGA16_G(cc);
                            b = TGA16_B(cc);

                            pix_offset = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;
                            image[3 * pix_offset + 0] = r;
                            image[3 * pix_offset + 1] = g;
                            image[3 * pix_offset + 2] = b;
                            alpha[pix_offset] = a;
                        }
                    }
                    break;
                }
            default:
                {
                    sprintf((char *)print_rec,"%s","tga image type not support");
                    log_error(1,print_rec);
                    goto error;
                    break;
                }
            }
        }
        break;
    case TGA_RLE_COMPRESS:
        {
            offset = 0;
            size = tga_head.iHeight*tga_head.iWidth;
            while(offset<size)
            {
                pack_fread(&count,1,1,f);
                flag = (UCHR)(count & 0x80);
                if(0==flag)
                {
                    flag = (UCHR)(count+1);
                    for(i=0;i<flag;i++)
                    {
                        switch(tga_head.bBitsPerPixel)
                        {
                        case 32:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            pack_fread(&a,1,1,f);
                            break;
                        case 24:
                            pack_fread(&b,1,1,f);
                            pack_fread(&g,1,1,f);
                            pack_fread(&r,1,1,f);
                            a = 0xff;
                            break;
                        case 16:
                        case 15:
                            pack_fread(&t[0],1,1,f);
                            pack_fread(&t[1],1,1,f);
                            cc = *(UHINT *)&t[0];
                            r = TGA16_R(cc);
                            g = TGA16_G(cc);
                            b = TGA16_B(cc);
                            a = 0xff;
                            break;
                            
                        }
                        y = (UHINT)(offset/tga_head.iWidth);
                        x = (UHINT)(offset-y*tga_head.iWidth);
                        pix_offset = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;
                        image[3 * pix_offset + 0] = r;
                        image[3 * pix_offset + 1] = g;
                        image[3 * pix_offset + 2] = b;
                        alpha[pix_offset] = a;

                        if(offset>=size)
                            continue;
                        offset++;
                    }
                }
                else
                {
                    flag=(UCHR)(count-127);
                    switch(tga_head.bBitsPerPixel)
                    {
                    case 32:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        pack_fread(&a,1,1,f);
                        break;
                    case 24:
                        pack_fread(&b,1,1,f);
                        pack_fread(&g,1,1,f);
                        pack_fread(&r,1,1,f);
                        a = 0xff;
                        break;
                    case 16:
                    case 15:
                        pack_fread(&t[0],1,1,f);
                        pack_fread(&t[1],1,1,f);
                        cc = *(UHINT *)&t[0];
                        r = TGA16_R(cc);
                        g = TGA16_G(cc);
                        b = TGA16_B(cc);
                        a = 0xff;
                        break;
                        
                    }
                    for(i=0; i<flag; i++)
                    {
                        if(offset >= size)
                            continue;
                        y=(UHINT)(offset/tga_head.iWidth);
                        x=(UHINT)(offset-y*tga_head.iWidth);
                        pix_offset = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;
                        image[3 * pix_offset + 0] = r;
                        image[3 * pix_offset + 1] = g;
                        image[3 * pix_offset + 2] = b;
                        alpha[pix_offset] = a;

                        offset++;
                    }
                }
            }
            break;
        }
    default:
        {
            sprintf((char *)print_rec,"%s","tga image type not support");
            log_error(1,print_rec);
            goto error;
            break;
        }
    }
    if(f) pack_fclose(f);

    if(tga_head.bDescriptor  & 0x20)
    {
        //rotate image & alpha top vs bottom
        for(x = tga_head.iWidth - 1; x >= 0; x--)
        {
            for( y = 0; y < tga_head.iHeight / 2; y ++)
            {
                pix_offset = y * tga_head.iWidth + x;
                pix_offset2 = (tga_head.iHeight - 1 - y) * tga_head.iWidth + x;

                r = image[3 * pix_offset + 0];
                g = image[3 * pix_offset + 1];
                b = image[3 * pix_offset + 2];
                a = alpha[pix_offset];

                image[3 * pix_offset + 0] = image[3 * pix_offset2 + 0];
                image[3 * pix_offset + 1] = image[3 * pix_offset2 + 1];
                image[3 * pix_offset + 2] = image[3 * pix_offset2 + 2];
                alpha[pix_offset] = alpha[pix_offset2];

                image[3 * pix_offset2 + 0] = r;
                image[3 * pix_offset2 + 1] = g;
                image[3 * pix_offset2 + 2] = b;
                alpha[pix_offset2] = a;
            }
        }
    }
    if(tga_head.bDescriptor  & 0x10)
    {
        //rotate image & alpha left vs right
        for(y = tga_head.iHeight - 1; y >= 0; y --)
        {
            for(x = 0; x < tga_head.iWidth/2; x ++)
            {
                pix_offset = y * tga_head.iWidth + x;
                pix_offset2 = y * tga_head.iWidth + tga_head.iWidth - 1 - x;

                r = image[3 * pix_offset + 0];
                g = image[3 * pix_offset + 1];
                b = image[3 * pix_offset + 2];
                a = alpha[pix_offset];

                image[3 * pix_offset + 0] = image[3 * pix_offset2 + 0];
                image[3 * pix_offset + 1] = image[3 * pix_offset2 + 1];
                image[3 * pix_offset + 2] = image[3 * pix_offset2 + 2];
                alpha[pix_offset] = alpha[pix_offset2];

                image[3 * pix_offset2 + 0] = r;
                image[3 * pix_offset2 + 1] = g;
                image[3 * pix_offset2 + 2] = b;
                alpha[pix_offset2] = a;
            }
        }
    }

    (*image_width) = tga_head.iWidth;
    (*image_height) = tga_head.iHeight;
    (*image_buffer) = image;
    (*alpha_buffer) = alpha;
    return  TTN_OK;
error:
    if(f) pack_fclose(f);
    if(image) { free(image); image = NULL; }
    if(alpha) { free(alpha); alpha = NULL; }
    return  TTN_ERROR;
}


//============================================================================
