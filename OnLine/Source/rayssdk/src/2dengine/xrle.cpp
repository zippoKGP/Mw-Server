/*
**  RLE.CPP
**  RLE compressed image functions.
**
**  haha, let's study some words, publish or perish(:die)!
**
**  ZJian,2000.12.12.
*/
#include "rays.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xinput.h"
#include "xfont.h"
#include "xrle.h"

#define RLE_SHADOW_DARK     22

void	(*put_rle) (SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);

static  void    _range_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _direct_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _range_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha);
static  void    _direct_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha);
static  void    _range_gray_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _direct_gray_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _range_noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _direct_noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _range_additive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);
static  void    _direct_additive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap);

// F U N C T I O N S ////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  RLE*    FNBACK  encode_rle(BMP *merge_bmp, IMG *info_img)
{
    RLE     *rle;
    SLONG   w, h, data_size, x, y, data_index, i;
    USTR    *data = NULL;
    SLONG   *offset = NULL;
    SLONG   encode_count;
    UCHR    encode_info;
    PIXEL   encode_pix;
    SLONG   encode_flag;
    SLONG   is_empty;

    if(! (merge_bmp && info_img) )
        return  NULL;
    w = merge_bmp->w;
    h = merge_bmp->h;
    if(w != info_img->w || h != info_img->h)
        return  NULL;

    //if it is empty(no image), we will make a simply rle which just contain "end image" data.
    is_empty = 1;
    for(y = info_img->h-1; y>=0; y--)
    {
        for(x = info_img->w-1; x>=0; x--)
        {
            if(info_img->line[y][x])
            {
                is_empty = 0;
                x = -100;
                y = -100;
            }
        }
    }
    if(is_empty)
    {
        USTR    spec_data[16];    

        //encode end image
        data_index = 0;
        spec_data[data_index++]=0;
        spec_data[data_index++]=1;

        if(NULL == ( rle = create_rle(w, h, data_index)) )
            goto some_error;
        for(y=1; y<h; y++)
            rle->line[y] = (USTR*)rle->line[0];
        memcpy(rle->line[0],spec_data,data_index);
        return  (RLE *)rle;
    }


    // alloc a buffer for the encoded data, w+16 for encode the end of line codes
    // 256 for ending of image codes.
    data_size = (w + 16) * h * sizeof(USTR) * 4 + 256;
    if(NULL == (data = (USTR*)GlobalAlloc(GPTR, data_size)) )
        goto some_error;

    // alloc a temp rle and we want to save the start offset to it's line[]
    if(NULL == (offset = (SLONG*)GlobalAlloc(GPTR, sizeof(SLONG)*h)) )
        goto some_error;

    for(y=0;y<h;y++)
        offset[y] = 0;

    // encode data format:
    // count(1byte) + info(1 byte) + datas(2 bytes or more)
    {
        encode_count=0;
        encode_info=0;
        encode_pix=SYSTEM_BLACK;
        data_index = 0;
        
        for(y=0;y<h;y++)
        {
            offset[y] = data_index;
            encode_count = 0;
            encode_pix=SYSTEM_BLACK;

            for(x=0;x<w;x++)
            {
                encode_info = info_img->line[y][x];
                switch(encode_info)
                {
                case 0x00://nothing, will skip it
                    {
                        //check if the rear datas are all empty(0).
                        is_empty = 1;
                        for(i=x; i<w; i++)
                        {
                            if(info_img->line[y][i])
                            {
                                is_empty = 0;
                                i = w + 100; //break
                            }
                        }

                        if(is_empty)
                        {/* rear datas are all empty(0) */

                            // break for the following's encode EOL(end of line).
                            x = w + 100; //break
                        }
                        else
                        {/* some rear data(s) are not empty, we must encode the current */
                            encode_count++;
                            
                            // encode skip(transfer meaning)
                            if(encode_count==0xff)
                            {
                                data[data_index++]=0;
                                data[data_index++]=2;
                                data[data_index++]=(UCHR)0xff;  //skip x
                                data[data_index++]=0;   //skip y
                                encode_count -= 0xff;
                            }
                            encode_flag = 0;
                            if(x+1 >= w) encode_flag=1;
                            else if(info_img->line[y][x+1] != encode_info) encode_flag=1;
                            
                            if(encode_flag)
                            {
                                // encode skip(transfer meaning)
                                data[data_index++]=0;
                                data[data_index++]=2;
                                data[data_index++]=(UCHR)encode_count;  //skip x
                                data[data_index++]=0;   //skip y
                                
                                encode_count = 0;
                            }
                        }
                    }
                    break;
                case RLE_IMAGE://only image 
                    {
                        encode_count++;
                        //encode the image
                        if(encode_count==0xff)
                        {
                            data[data_index++]=0;
                            data[data_index++]=(UCHR)0xff;
                            memcpy(&data[data_index],&merge_bmp->line[y][x-0xff+1], 0xff*SIZEOFPIXEL);
                            data_index+=0xff*SIZEOFPIXEL;
                            encode_count-=0xff;
                        }

                        encode_flag=0;
                        if(x+1>=w) encode_flag=1;
                        else if(info_img->line[y][x+1] != encode_info) encode_flag=1;

                        if(encode_flag)
                        {
                            //encode the image
                            if(encode_count<3)
                            {
                                for(i=0;i<encode_count;i++)
                                {
                                    data[data_index++]=1;
                                    data[data_index++]=encode_info;
                                    *(PIXEL*)&data[data_index] = merge_bmp->line[y][x-encode_count+1+i];
                                    data_index+=SIZEOFPIXEL;
                                }
                            }
                            else
                            {
                                // continous pixes
                                data[data_index++]=0;
                                data[data_index++]=(UCHR)encode_count;
                                memcpy(&data[data_index],&merge_bmp->line[y][x-encode_count+1],encode_count*SIZEOFPIXEL);
                                data_index+=encode_count*SIZEOFPIXEL;
                            }
                            
                            //prepare for the next encode
                            encode_count = 0;
                        }
                    }
                    break;
                case RLE_SHADOW://only shadow
                    {
                        encode_count++;
                        //encode the shadow
                        if(encode_count==0xff)
                        {
                            data[data_index++]=(UCHR)0xff;
                            data[data_index++]=encode_info;
                            *(PIXEL*)&data[data_index] = SYSTEM_BLACK;
                            data_index += SIZEOFPIXEL;
                            encode_count-=0xff;
                        }

                        encode_flag=0;
                        if(x+1>=w) encode_flag=1;
                        else if(info_img->line[y][x+1] != encode_info) encode_flag=1; 

                        if(encode_flag)
                        {
                            //encode the shadow
                            data[data_index++]=(UCHR)encode_count;
                            data[data_index++]=encode_info;
                            *(PIXEL*)&data[data_index] = SYSTEM_BLACK;
                            data_index += SIZEOFPIXEL;
                            
                            //prepare for the next encode
                            encode_count = 0;
                        }
                    }
                    break;
                default://edge or alpha image, encode 1 by 1
                    {
                        // encode others
                        data[data_index++]=(UCHR)1;//here encode_count++ is 1
                        data[data_index++]=encode_info;
                        *(PIXEL*)&data[data_index] = merge_bmp->line[y][x];
                        data_index += SIZEOFPIXEL;

                        encode_count = 0;
                    }
                    break;
                }
            }
            //encode end of line
            data[data_index++] = 0;
            data[data_index++] = 0;
        }
    }
    //encode end image
    data[data_index++]=0;
    data[data_index++]=1;
    //ok,finished encode

    //create rle and fill data ---------------------------------------------------------------------------
    if(NULL == ( rle = create_rle(w, h, data_index)) )
        goto some_error;
    for(y=1; y<h; y++)
        rle->line[y] = (USTR*)(rle->line[0] + offset[y]);
    memcpy(rle->line[0],data,data_index);

    if(data) {GlobalFree(data); data=NULL;}
    if(offset) {GlobalFree(offset); offset=NULL;}
    return  (RLE*)rle;

some_error:
    if(data) {GlobalFree(data); data=NULL;}
    if(offset) {GlobalFree(offset); offset=NULL;}
    return  NULL;
}



EXPORT  RLE *   FNBACK  make_rle(ABMP *image_abmp,ABMP *shadow_abmp)
{
    RLE *rle=NULL;
    SLONG w,h;
    IMG *info_img=NULL;
    USTR *p=NULL;
    SLONG edge_mask;// for check image edge

    BMP *merge_bmp=NULL;
    SLONG *offset=NULL;

    USTR *data=NULL;
    SLONG x,y;
    UCHR image_alpha;
    UCHR shadow_alpha;

    UCHR encode_info;
    UCHR alpha;

    if(NULL==image_abmp) return NULL;
    w = image_abmp->w;
    h = image_abmp->h;

    if(shadow_abmp)
    {
        if( (w!=shadow_abmp->w) || (h!=shadow_abmp->h) )
            goto error;
    }

    // create & init info img -------------------------------------------------------------------------------------
    // create & init merge bmp
    // the info img is the 1byte array[][] for save the image's info mask.
    // the merge bmp is the merged bitmap for image_abmp & shadow_abmp
    if(NULL==(info_img=create_img(w, h)))
        goto error;
    if(NULL==(merge_bmp=create_bitmap(w,h)))
        goto error;
    clear_bitmap(merge_bmp);
    clear_img(info_img);

    //TRACE(make_rle#1);

    shadow_alpha = 0x00;
    for(y=0;y<h;y++)
    {
        for(x=0;x<w;x++)
        {
            encode_info = 0x00;
            image_alpha = image_abmp->line[y][image_abmp->w*SIZEOFPIXEL+x];
            if(shadow_abmp)
                shadow_alpha = shadow_abmp->line[y][shadow_abmp->w*SIZEOFPIXEL+x];
            else
                shadow_alpha = 0x00;
            switch(image_alpha)
            {
            case 0x00://no image, check if shadow
                {
                    if(shadow_alpha>=0x80)
                        encode_info |= RLE_SHADOW;

                    merge_bmp->line[y][x] = SYSTEM_BLACK;
                }
                break;
            default:
                {
                    //check if it is edge ?
                    //
                    //encode edge mask:
                    //  0x400 0x200 0x100
                    //  0x040   #   0x010
                    //  0x004 0x002 0x001
                    edge_mask = 0x757;  // we think there is no pix around this...

                    if((y-1)>=0 && (x-1)>=0)
                    {
                        if(image_abmp->line[(y-1)][w*SIZEOFPIXEL+(x-1)])
                            edge_mask &= ~0x400;
                    }
                    if((y-1)>=0 && (x-0)>=0)
                    {
                        if(image_abmp->line[(y-1)][w*SIZEOFPIXEL+(x-0)])
                            edge_mask &= ~0x200;
                    }
                    if((y-1)>=0 && (x+1)<w)
                    {
                        if(image_abmp->line[(y-1)][w*SIZEOFPIXEL+(x+1)])
                            edge_mask &= ~0x100;
                    }

                    if((y+1)<h && (x-1)>=0)
                    {
                        if(image_abmp->line[(y+1)][w*SIZEOFPIXEL+(x-1)])
                            edge_mask &= ~0x004;
                    }
                    if((y+1)<h && (x-0)>=0)
                    {
                        if(image_abmp->line[(y+1)][w*SIZEOFPIXEL+(x-0)])
                            edge_mask &= ~0x002;
                    }
                    if((y+1)<h && (x+1)<w)
                    {
                        if(image_abmp->line[(y+1)][w*SIZEOFPIXEL+(x+1)])
                            edge_mask &= ~0x001;
                    }
                    if((y-0)>=0 && (x-1)>=0)
                    {
                        if(image_abmp->line[(y-0)][w*SIZEOFPIXEL+(x-1)])
                            edge_mask &= ~0x040;
                    }
                    if((y-0)>=0 && (x+1)<w)
                    {
                        if(image_abmp->line[(y-0)][w*SIZEOFPIXEL+(x+1)])
                            edge_mask &= ~0x010;
                    }

                    if(edge_mask)
                        encode_info |= RLE_EDGE;

                    if(image_alpha == 0xff)
                    {
                        encode_info |= RLE_IMAGE;
                    }
                    else
                    {
                        alpha = (UCHR)( image_alpha & RLE_ALPHA );
                        if(alpha < RLE_LO_ALPHA) 
                            alpha = RLE_LO_ALPHA;
                        encode_info |= alpha;
                    }

                    merge_bmp->line[y][x] = *(PIXEL *)&image_abmp->line[y][x*SIZEOFPIXEL];
                }
                break;
            }
            info_img->line[y][x] = encode_info;
        }
    }

    /*************
    UCHR gray;
    PIXEL fore_color;
    PIXEL back_color;
    SLONG test_xl,test_yl;

    fill_bitmap(screen_buffer,rgb2hi(180,180,180));
    test_xl = min(screen_buffer->w, merge_bmp->w);
    test_yl = min(screen_buffer->h, merge_bmp->h);
    for(y=0;y<test_yl;y++)
    {
        for(x=0;x<test_xl;x++)
        {
            gray = info_img->line[y][x];
            switch(gray)
            {
            case 0x00:
                break;
            case RLE_SHADOW:
                break;
            default:
                if(gray & RLE_IMAGE)
                {
                    //screen_buffer->line[y][x] = rgb2hi(255,255,255);
                    screen_buffer->line[y][x] = merge_bmp->line[y][x];
                }
                else
                {
                    gray &= RLE_ALPHA;
                    //screen_buffer->line[y][x] = rgb2hi(gray,gray,gray);
                    back_color = screen_buffer->line[y][x];
                    fore_color = merge_bmp->line[y][x];

                    oper_alpha_color(&fore_color, &screen_buffer->line[y][x], gray);
                }
                break;
            }
        }
    }
    print16(4,4,(USTR*)"~C0~O3MAKE RLE...(1)~C0~O0",PEST_PUT,screen_buffer);
    update_screen(screen_buffer);
    wait_any_key();
    ****************************/
    //TRACE(make_rle#2);

    // encode rle data --------------------------------------------------------------------------------------------
    rle = encode_rle(merge_bmp, info_img);

    /***************************************
    SLONG disp_x,disp_y;
    SLONG sx,sy;
    UCHR count,info;
    PIXEL pix;
    BMP *dest_bitmap;

    dest_bitmap = screen_buffer;
    sx=sy=0;

    fill_bitmap(dest_bitmap,rgb2hi(180,180,180));
    data_index=0;
    data_size=rle->size;

    disp_x=sx;
    disp_y=sy;
    while(data_index<data_size)
    {

        count = data[data_index++];
        info = data[data_index++];

        //      sprintf((char *)print_rec,"COUNT=0X%02X(%d),    INFO=0X%02X(%d)        ",count,count,info,info);
        //      print16(4,dest_bitmap->h-20,(USTR*)print_rec,COPY_PUT,dest_bitmap);
        //      update_screen(dest_bitmap);
        //      wait_any_key();

        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            switch(info)
            {
            case RLE_SHADOW:
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        if( (disp_x & 1) ^ (disp_y & 1) )
                            dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                    }
                    disp_x++;
                }
                break;
            default://RLE_ALPHA,RLE_EDGE,RLE_IMAGE
                if(info & RLE_IMAGE)
                {
                    for(i=0;i<count;i++)
                    {
                        if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            dest_bitmap->line[disp_y][disp_x] = pix;
                        disp_x++;
                    }
                }
                else
                {
                    for(i=0;i<count;i++)
                    {
                        if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        {
                            info &= RLE_ALPHA;

                            oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                        }
                        disp_x++;
                    }
                }
                break;
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
    print16(4,4,(USTR*)"~C0~O3MAKE RLE...2~C0~O0",PEST_PUT,dest_bitmap);
    update_screen(dest_bitmap);
    wait_any_key();
    ****************************************/

error:
    destroy_img(&info_img);
    destroy_bitmap(&merge_bmp);
    return (RLE*)rle;
}



EXPORT  RLE *   FNBACK  create_rle(SLONG w,SLONG h,SLONG data_size)
{
    RLE *tmp=NULL;
    SLONG y;
    USTR *t=NULL;
    SLONG size;

    size = sizeof(RLE) + sizeof(USTR*)*(h-1) + data_size;
    size = (size + 3) & 0xfffffffc;    // memory align(4 bytes)

    if(NULL == (tmp=(RLE*)GlobalAlloc(GPTR, size) ) )
        return NULL;
    tmp->w = w;
    tmp->h = h;
    tmp->size = data_size;
    t = tmp->line[0] = (USTR *)( (USTR*)tmp + sizeof(RLE) + sizeof(USTR*)*(h-1) );
    for(y=1;y<h;y++)
        tmp->line[y] = t;
    return (RLE*)tmp;
}



EXPORT  void    FNBACK  destroy_rle(RLE **rle)
{
    if(*rle)
    {
        GlobalFree(*rle);
        (*rle)=NULL;
    }
}


EXPORT  RLE *   FNBACK  duplicate_rle(RLE *rle)
{
    RLE *tmp = NULL;
    SLONG y;
    SLONG offset;

    if(! rle)
        return NULL;

    tmp = create_rle(rle->w, rle->h, rle->size);
    if( !tmp)
        return NULL;
    for(y=1; y<tmp->h; y++)
    {
        offset = rle->line[y] - rle->line[0];
        tmp->line[y] = (USTR*)(tmp->line[0] + offset);
    }
    memcpy(tmp->line[0], rle->line[0], rle->size);
    return (RLE *)tmp;
}



EXPORT  void    FNBACK  filter_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG filter)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {

                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 1) ^ (disp_y & 1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(info >= filter)
                    {
                        if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        {
                            //dest_bitmap->line[disp_y][disp_x] = pix;
                            oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                        }
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}



EXPORT  void    FNBACK  put_rle_to_buffer(SLONG sx,SLONG sy,RLE *rle,char *pbuffer,long pitch, long w, long h)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    SLONG offset;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {

                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<w && disp_y>=0 && disp_y<h)
                    {
                        if( (disp_x & 1) ^ (disp_y & 1) )
                        {
                            offset = disp_y * pitch + disp_x * SIZEOFPIXEL;
                            //*(PIXEL *)(pbuffer + offset ) = SYSTEM_BLACK;//pix;
                            oper_dark_color((PIXEL *)(pbuffer + offset ), RLE_SHADOW_DARK);
                        }
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<w && disp_y>=0 && disp_y<h)
                    {
                        offset = disp_y * pitch + disp_x * SIZEOFPIXEL;
                        * (PIXEL *)(pbuffer + offset) = pix;
                    }
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<w && disp_y>=0 && disp_y<h)
                    {
                        offset = disp_y * pitch + disp_x * SIZEOFPIXEL;
                        oper_alpha_color(&pix, (PIXEL *)(pbuffer + offset),(SLONG)info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<w && disp_y>=0 && disp_y<h)
                    {
//						if (pitch / w == 2)
						{
							offset = disp_y * pitch + disp_x * SIZEOFPIXEL;
							*(PIXEL *)(pbuffer + offset) = *(PIXEL*)&data[data_index];							
						}
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}



EXPORT  void    FNBACK  convert_rle_hi2fff(RLE *rle)
{
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = hi2fff( *(PIXEL*)&data[data_index] );
            *(PIXEL*)&data[data_index] = pix;
            data_index += SIZEOFPIXEL;
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                data_index++;
                data_index++;
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    pix = hi2fff( *(PIXEL*)&data[data_index] );
                    *(PIXEL*)&data[data_index] = pix;
                    data_index += SIZEOFPIXEL;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  convert_rle_fff2hi(RLE *rle)
{
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = fff2hi( *(PIXEL*)&data[data_index] );
            *(PIXEL*)&data[data_index] = pix;
            data_index += SIZEOFPIXEL;
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                data_index++;
                data_index++;
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    pix = fff2hi( *(PIXEL*)&data[data_index] );
                    *(PIXEL*)&data[data_index] = pix;
                    data_index += SIZEOFPIXEL;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  scale_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bitmap)
{
    SLONG disp_x, disp_y, x, y, count, info;
    PIXEL pix;
    USTR *data=NULL;
    float zoom_x,zoom_y;
    //SLONG block_xl, block_yl;
    //SLONG block_sx, block_sy;
    SLONG block_i,block_j;
    SLONG *block_sx_table = NULL;
    SLONG *block_sy_table = NULL;
    SLONG *block_xl_table = NULL;
    SLONG *block_yl_table = NULL;
    PIXEL *line = NULL;

    if( sx + scalex < 0) return;
    if( sy + scaley < 0) return;
    if( sx >= dest_bitmap->w) return;
    if( sy >= dest_bitmap->h) return;
    if( !src_rle ) return;
    if( !(src_rle->w && src_rle->h) ) return;

    zoom_x = (float)scalex/(float)src_rle->w;
    zoom_y = (float)scaley/(float)src_rle->h;

    //create tables for optimize ------------------------------------------------------------------------
    if(NULL == (block_sx_table = (SLONG*)GlobalAlloc(GPTR, sizeof(SLONG)*src_rle->w)))
        goto some_error;
    if(NULL == (block_sy_table = (SLONG*)GlobalAlloc(GPTR, sizeof(SLONG)*src_rle->h)))
        goto some_error;
    if(NULL == (block_xl_table = (SLONG*)GlobalAlloc(GPTR, sizeof(SLONG)*src_rle->w)))
        goto some_error;
    if(NULL == (block_yl_table = (SLONG*)GlobalAlloc(GPTR, sizeof(SLONG)*src_rle->h)))
        goto some_error;

    //init block_sx_table ---------------------
    for(x=0; x<src_rle->w; x++)
    {
        block_sx_table[x] = sx + (SLONG)(x * zoom_x);
    }
    //init block_sy_table ---------------------
    for(y=0; y<src_rle->h; y++)
    {
        block_sy_table[y] = sy + (SLONG)(y * zoom_y);
    }
    //init block_xl_table ---------------------
    for(x=0; x<src_rle->w-1; x++)
    {
        block_xl_table[x] = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
    }
    //now x = (src_rle->w-1)
    block_xl_table[x] = (SLONG)scalex - (SLONG)( x * zoom_x );

    //init block_yl_table ----------------------
    for(y=0; y<src_rle->h-1; y++)
    {
        block_yl_table[y] = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
    }
    //now y = (src_rle->h-1)
    block_yl_table[y] = (SLONG)scaley - (SLONG)(y * zoom_y);


    //start decode rle ------------------------------------------------------------------------
    y = (sy < 0) ? (SLONG) ((-sy) / zoom_y ) : 0;
    while( y < src_rle->h )
    {
        if(block_yl_table[y] <= 0)
            goto _end_of_line;
        if(block_sy_table[y] >= dest_bitmap->h)
            goto _end_of_image;

        x = 0;
        data = src_rle->line[y];
        while(x < src_rle->w)
        {
            count = data[0];
            info = data[1];
            data += 2;
            if(count) // continuous pixes
            {
                pix = *(PIXEL*)data;
                data += SIZEOFPIXEL;
                if(info & RLE_SHADOW)
                {
                    while(--count >= 0)
                    {
                        disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                        block_j = block_yl_table[y];
                        while(--block_j >= 0)
                        {
                            if(disp_y < 0)
                                break;
                            if(disp_y < dest_bitmap->h)
                            {
                                disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                                block_i = block_xl_table[x];
                                while(--block_i >= 0)
                                {
                                    if(disp_x < 0)
                                        break;
                                    if(disp_x < dest_bitmap->w)
                                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                                    disp_x --;
                                }
                            }
                            disp_y --;
                        }
                        x++;
                    }
                }
                else if(info & RLE_IMAGE)
                {
                    while(--count >= 0)
                    {
                        disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                        block_j = block_yl_table[y];
                        while(--block_j >= 0)
                        {
                            if(disp_y < 0)
                                break;
                            if(disp_y < dest_bitmap->h)
                            {
                                disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                                block_i = block_xl_table[x];
                                while(--block_i >= 0)
                                {
                                    if(disp_x < 0)
                                        break;
                                    if(disp_x < dest_bitmap->w)
                                        dest_bitmap->line[disp_y][disp_x] = pix;
                                    disp_x --;
                                }
                            }
                            disp_y --;
                        }
                        x++;
                    }
                }
                else
                {
                    info &= RLE_ALPHA;
                    while(--count >= 0)
                    {
                        disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                        block_j = block_yl_table[y];
                        while(--block_j >= 0)
                        {
                            if(disp_y < 0)
                                break;
                            if(disp_y < dest_bitmap->h)
                            {
                                disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                                block_i = block_xl_table[x];
                                while(--block_i >= 0)
                                {
                                    if(disp_x < 0)
                                        break;
                                    if(disp_x<dest_bitmap->w)
                                    {
                                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                                    }
                                    disp_x --;
                                }
                            }
                            disp_y --;
                        }
                        x++;
                    }
                }
            }
            else    // count=0, is a flag
            {
                switch(info)
                {
                case 0://end of line
                    goto _end_of_line;

                case 1://end of image
                    goto _end_of_image;

                case 2://transfered meaning
                    x += data[0];
                    y += data[1];
                    data += 2;
                    break;

                default:// copy pixels
                    while(--info >= 0)
                    {
                        disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                        block_j = block_yl_table[y];
                        while(--block_j >= 0)
                        {
                            if(disp_y < 0)
                                break;
                            if(disp_y < dest_bitmap->h)
                            {
                                disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                                block_i = block_xl_table[x];
                                while(--block_i >= 0)
                                {
                                    if(disp_x < 0)
                                        break;
                                    if(disp_x < dest_bitmap->w)
                                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)data;
                                    disp_x --;
                                }
                            }
                            disp_y --;
                        }
                        
                        data += SIZEOFPIXEL;
                        x ++;
                    }
                    break;
                }
            }
        }
_end_of_line:
        y ++;
    }
_end_of_image:
    /*
    SLONG data_index = 0;
    SLONG i, data_size;

    data = src_rle->line[0];
    data_size = src_rle->size;
    x = 0;
    y = 0;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                    for(block_j = block_yl_table[y]-1; block_j>=0; block_j--)
                    {
                        disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                        for(block_i = block_xl_table[x]-1; block_i>=0; block_i--)
                        {
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                            }
                            disp_x --;
                        }
                        disp_y --;
                    }
                    x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                    for(block_j = block_yl_table[y]-1; block_j>=0; block_j--)
                    {
                        disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                        for(block_i = block_xl_table[x]-1; block_i>=0; block_i--)
                        {
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                dest_bitmap->line[disp_y][disp_x] = pix;//SYSTEM_GREEN;//pix;
                            }
                            disp_x --;
                        }
                        disp_y --;
                    }
                    x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                    for(block_j = block_yl_table[y]-1; block_j>=0; block_j--)
                    {
                        disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                        for(block_i = block_xl_table[x]-1; block_i>=0; block_i--)
                        {
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                            }
                            disp_x --;
                        }
                        disp_y --;
                    }

                    x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                {
                    x=0;
                    y++;
                    break;
                }
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                x += data[data_index++];
                y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    disp_y = block_sy_table[y] + block_yl_table[y] - 1;
                    for(block_j = block_yl_table[y]-1; block_j>=0; block_j--)
                    {
                        disp_x = block_sx_table[x] + block_xl_table[x] - 1;
                        for(block_i = block_xl_table[x]-1; block_i>=0; block_i--)
                        {
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                            }
                            disp_x --;
                        }
                        disp_y --;
                    }

                    data_index += SIZEOFPIXEL;
                    x ++;
                }
                break;
            }
        }
    }
    */

some_error:
    if(block_sx_table) { GlobalFree(block_sx_table); block_sx_table = NULL; }
    if(block_sy_table) { GlobalFree(block_sy_table); block_sy_table = NULL; }
    if(block_xl_table) { GlobalFree(block_xl_table); block_xl_table = NULL; }
    if(block_yl_table) { GlobalFree(block_yl_table); block_yl_table = NULL; }

    return;
}


EXPORT  void    FNBACK  scale_blue_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    SLONG x,y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    double zoom_x,zoom_y;
    SLONG block_xl, block_yl;
    SLONG block_sx, block_sy;
    SLONG block_i,block_j;

    if( !src_rle ) return;
    if( !(src_rle->w && src_rle->h) ) return;

    zoom_x = (double)scalex/(double)src_rle->w;
    zoom_y = (double)scaley/(double)src_rle->h;

    data = src_rle->line[0];
    data_size = src_rle->size;
    data_index=0;
    x = 0;
    y = 0;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            switch(info)
            {
            case RLE_SHADOW:
                for(i=0;i<count;i++)
                {
                    block_sx = sx + (SLONG)(x * zoom_x);
                    block_sy = sy + (SLONG)(y * zoom_y);
                    block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                    block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                    for(block_j = 0; block_j<block_yl; block_j++)
                    {
                        for(block_i = 0; block_i<block_xl; block_i++)
                        {
                            disp_x = block_sx + block_i;
                            disp_y = block_sy + block_j;
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                                //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                                oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                            }
                        }
                    }
                    x++;
                }
                break;
            default:
                if(info & RLE_IMAGE)
                {
                    for(i=0;i<count;i++)
                    {
                        block_sx = sx + (SLONG)(x * zoom_x);
                        block_sy = sy + (SLONG)(y * zoom_y);
                        block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                        block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                        for(block_j = 0; block_j<block_yl; block_j++)
                        {
                            for(block_i = 0; block_i<block_xl; block_i++)
                            {
                                disp_x = block_sx + block_i;
                                disp_y = block_sy + block_j;
                                if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                                {
									oper_blue_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                                }
                            }
                        }
                        x++;
                    }
                }
                else
                {
                    info &= RLE_ALPHA;
                    for(i=0;i<count;i++)
                    {
                        block_sx = sx + (SLONG)(x * zoom_x);
                        block_sy = sy + (SLONG)(y * zoom_y);
                        block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                        block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                        for(block_j = 0; block_j<block_yl; block_j++)
                        {
                            for(block_i = 0; block_i<block_xl; block_i++)
                            {
                                disp_x = block_sx + block_i;
                                disp_y = block_sy + block_j;
                                if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                                {
									oper_blue_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                                }
                            }
                        }
                        x++;
                    }
                }
                break;
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                {
                    x=0;
                    y++;
                    break;
                }
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                x += data[data_index++];
                y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    block_sx = sx + (SLONG)(x * zoom_x);
                    block_sy = sy + (SLONG)(y * zoom_y);
                    block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                    block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                    for(block_j = 0; block_j<block_yl; block_j++)
                    {
                        for(block_i = 0; block_i<block_xl; block_i++)
                        {
                            disp_x = block_sx + block_i;
                            disp_y = block_sy + block_j;
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
								oper_blue_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                            }
                        }
                    }
                    data_index += SIZEOFPIXEL;
                    x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  scale_gray_put_rle(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,RLE *src_rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    SLONG x,y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    double zoom_x,zoom_y;
    SLONG block_xl, block_yl;
    SLONG block_sx, block_sy;
    SLONG block_i,block_j;

    if( !src_rle ) return;
    if( !(src_rle->w && src_rle->h) ) return;

    zoom_x = (double)scalex/(double)src_rle->w;
    zoom_y = (double)scaley/(double)src_rle->h;

    data = src_rle->line[0];
    data_size = src_rle->size;
    data_index=0;
    x = 0;
    y = 0;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            switch(info)
            {
            case RLE_SHADOW:
                for(i=0;i<count;i++)
                {
                    block_sx = sx + (SLONG)(x * zoom_x);
                    block_sy = sy + (SLONG)(y * zoom_y);
                    block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                    block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                    for(block_j = 0; block_j<block_yl; block_j++)
                    {
                        for(block_i = 0; block_i<block_xl; block_i++)
                        {
                            disp_x = block_sx + block_i;
                            disp_y = block_sy + block_j;
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
                                //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                                //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                                oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                            }
                        }
                    }
                    x++;
                }
                break;
            default:
                if(info & RLE_IMAGE)
                {
                    for(i=0;i<count;i++)
                    {
                        block_sx = sx + (SLONG)(x * zoom_x);
                        block_sy = sy + (SLONG)(y * zoom_y);
                        block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                        block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                        for(block_j = 0; block_j<block_yl; block_j++)
                        {
                            for(block_i = 0; block_i<block_xl; block_i++)
                            {
                                disp_x = block_sx + block_i;
                                disp_y = block_sy + block_j;
                                if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                                {
									oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                                }
                            }
                        }
                        x++;
                    }
                }
                else
                {
                    info &= RLE_ALPHA;
                    for(i=0;i<count;i++)
                    {
                        block_sx = sx + (SLONG)(x * zoom_x);
                        block_sy = sy + (SLONG)(y * zoom_y);
                        block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                        block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                        for(block_j = 0; block_j<block_yl; block_j++)
                        {
                            for(block_i = 0; block_i<block_xl; block_i++)
                            {
                                disp_x = block_sx + block_i;
                                disp_y = block_sy + block_j;
                                if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                                {
									oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
									oper_gray_color(&dest_bitmap->line[disp_y][disp_x],&dest_bitmap->line[disp_y][disp_x]);
                                }
                            }
                        }
                        x++;
                    }
                }
                break;
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                {
                    x=0;
                    y++;
                    break;
                }
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                x += data[data_index++];
                y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    block_sx = sx + (SLONG)(x * zoom_x);
                    block_sy = sy + (SLONG)(y * zoom_y);
                    block_xl = (SLONG)((x+1) * zoom_x) - (SLONG)(x * zoom_x);
                    block_yl = (SLONG)((y+1) * zoom_y) - (SLONG)(y * zoom_y);
                    for(block_j = 0; block_j<block_yl; block_j++)
                    {
                        for(block_i = 0; block_i<block_xl; block_i++)
                        {
                            disp_x = block_sx + block_i;
                            disp_y = block_sy + block_j;
                            if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                            {
								oper_gray_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                            }
                        }
                    }
                    data_index += SIZEOFPIXEL;
                    x ++;
                }
                break;
            }
        }
    }
}



EXPORT  void    FNBACK  mask_edge_put_rle(SLONG sx,SLONG sy,PIXEL mask_color,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  gray_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else //alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  alpha_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  additive_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    // Jack, todo [12:00,10/9/2002]
}


EXPORT  void    FNBACK  subtractive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            /*
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            */
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_subtractive_color( (PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  subtractive_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
}


EXPORT  void    FNBACK  minimum_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            /*
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            */
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_minimum_color( (PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  minimum_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    // Jack, todo [12:00,10/9/2002]
}


EXPORT  void    FNBACK  maximum_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            /*
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            */
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_maximum_color( (PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  maximum_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    // Jack, todo [12:00,10/9/2002]
}


EXPORT  void    FNBACK  half_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            /*
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            */
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //dest_bitmap->line[disp_y][disp_x] = SYSTEM_RED;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_half_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_half_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  half_edge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    // Jack, todo [12:01,10/9/2002]
}


EXPORT  void    FNBACK  mesh_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            /*
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;//SYSTEM_GREEN;//pix;
                    disp_x++;
                }
            }
            else if(info & RLE_SHADOW)
            */
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(info & 0x80)
                    {
                        if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        {
                            if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                                dest_bitmap->line[disp_y][disp_x] = pix;
                        }
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                            dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  mask_put_rle(SLONG sx,SLONG sy,PIXEL mask_color,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                disp_x += count;
                /*
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        if( (disp_x & 1) ^ (disp_y & 1) )
                            dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                    }
                    disp_x++;
                }
                */
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  mask_put_rle_edge(SLONG sx,SLONG sy,PIXEL mask_color,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_EDGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = mask_color;
                    disp_x++;
                }
            }
            else
            {
				disp_x += count;
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
				data_index += SIZEOFPIXEL * info;
				disp_x += info;
                break;
            }
        }
    }
}



EXPORT  double  FNBACK  analyse_rle(RLE *rle)
{
    return 0;
}


EXPORT  void    FNBACK  clip_direct_put_rle(SLONG sx, SLONG sy, SLONG clip_sx, SLONG clip_sy, SLONG clip_xl, SLONG clip_yl, RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    SLONG clip_ex = clip_sx + clip_xl;
    SLONG clip_ey = clip_sy + clip_yl;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {

                for(i=0;i<count;i++)
                {
                    if(disp_x>=clip_sx && disp_x<clip_ex && disp_y>=clip_sy && disp_y<clip_ey)
                    {
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=clip_sx && disp_x<clip_ex && disp_y>=clip_sy && disp_y<clip_ey)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=clip_sx && disp_x<clip_ex && disp_y>=clip_sy && disp_y<clip_ey)
                    {
                        oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=clip_sx && disp_x<clip_ex && disp_y>=clip_sy && disp_y<clip_ey)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  red_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //we use dark color for shadow
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                oper_red_color(&pix, &pix2);
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = pix2;
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //oper_partial_color(&pix, &pix, info);
                        //oper_complement_color(&pix, &pix, info);
                        oper_red_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_red_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  green_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //we use dark color for shadow
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                oper_green_color(&pix, &pix2);
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = pix2;
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //oper_partial_color(&pix, &pix, info);
                        //oper_complement_color(&pix, &pix, info);
                        oper_green_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_green_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  blue_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //we use dark color for shadow
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                oper_blue_color(&pix, &pix2);
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = pix2;
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //oper_partial_color(&pix, &pix, info);
                        //oper_complement_color(&pix, &pix, info);
                        oper_blue_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_blue_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  blue_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap, SLONG alpha)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    //PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //we use dark color for shadow
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_blue_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha);
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_blue_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info * alpha / 255);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_blue_alpha_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x], alpha);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  yellow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //we use dark color for shadow
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                oper_yellow_color(&pix, &pix2);
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = pix2;
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //oper_partial_color(&pix, &pix, info);
                        //oper_complement_color(&pix, &pix, info);
                        oper_yellow_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_yellow_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void    FNBACK  merge_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    //PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    PIXEL black = SYSTEM_BLACK;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //geaan, 2001.10.30.
                        //dest_bitmap->line[disp_y][disp_x] = pix;
                        oper_merge_color(&pix, &dest_bitmap->line[disp_y][disp_x], 255);
                    }
                    disp_x++;
                }
            }
            else // alpha
            {
                // we simply process alpha...
                for(i=0;i<count;i++)
                {
                    if(info & 0x80)
                    {
                        if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        {
                            oper_merge_color(&pix, &dest_bitmap->line[disp_y][disp_x], (SLONG)info);
                        }
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x = sx;
                disp_y++;
                break;
            case 1://end of image
                data_index = data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //geaan, 2001.10.30.
                        oper_merge_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x], 255);
                        //dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


EXPORT  void	FNBACK  remake_rle_line_pointer(RLE *rle)
{
    UCHR	count,info;
    USTR *	data=NULL;
    SLONG	data_size;
    SLONG	data_index;
	SLONG	disp_x, disp_y;
	SLONG*	line_offset = NULL;
	SLONG	i;

    if(!rle) return;

	line_offset = (SLONG *)malloc(sizeof(SLONG) * rle->h ); 
	if(! line_offset) return;
	for(i=0; i<rle->h; i++)
	{
		line_offset[i] = 0;
	}
	disp_x = disp_y = 0;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    while(data_index<data_size)
    {
		//get line pointer offset
		if( (disp_x == 0) && (disp_y < rle->h) )
		{
			line_offset[disp_y] = data_index;
		}

        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            data_index += SIZEOFPIXEL;	//skip read pixel
			disp_x += count;
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x = 0;
                disp_y ++;
                break;
            case 1://end of image
                data_index = data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                data_index += info * SIZEOFPIXEL;
                disp_x += info;
                break;
            }
        }
    }
	for(i=1; i<rle->h; i++)
	{
		rle->line[i] = rle->line[0] + line_offset[i];
	}

	if(line_offset) free(line_offset);
}


EXPORT  SLONG   FNBACK  naked_write_rle(FILE *fp, RLE *rle)
{
    USTR    *t=NULL;
    SLONG   y, offset;
    
    convert_rle_hi2fff(rle);
    fwrite(rle, 1, sizeof(RLE) - sizeof(USTR*), fp);
    t = rle->line[0];
    for(y=0;y<rle->h;++y)
    {
        offset = (SLONG)(rle->line[y] - t);
        fwrite(&offset, 1, sizeof(SLONG), fp);
    }
    fwrite(rle->line[0], 1, rle->size, fp);
    convert_rle_fff2hi(rle);

    return  TTN_OK;
}


EXPORT  SLONG   FNBACK  naked_read_rle(PACK_FILE *fp, RLE **rle)
{
    RLE     head, *tmp = NULL;
    SLONG   y, offset;

    pack_fread(&head, 1, sizeof(RLE) - sizeof(USTR*), fp);
    if(NULL == (tmp = create_rle(head.w, head.h, head.size)))
    {
        log_error(1, "memory alloc error when read rle");
        return  TTN_ERROR;
    }
    for(y=0; y<tmp->h; ++y)
    {
        pack_fread(&offset, 1, sizeof(SLONG), fp);
        tmp->line[y] = (USTR*)(tmp->line[0] + offset);
    }
    pack_fread(tmp->line[0], 1, head.size, fp);
    convert_rle_fff2hi(tmp);
    *rle = tmp;
    return  TTN_OK;
}



////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OPTIMIZED FUNCTIONS
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  void    FNBACK  _put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    if( !rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;
    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_put_rle(sx,sy,rle,dest_bitmap);
    else
        _range_put_rle(sx,sy,rle,dest_bitmap);
}

EXPORT  void    FNBACK  alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha)
{
    if( !rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;

    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_alpha_put_rle(sx,sy,rle,dest_bitmap,alpha);
    else
        _range_alpha_put_rle(sx,sy,rle,dest_bitmap,alpha);
}


EXPORT  void    FNBACK  gray_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    if( !rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;

    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_gray_put_rle(sx,sy,rle,dest_bitmap);
    else
        _range_gray_put_rle(sx,sy,rle,dest_bitmap);
}


EXPORT  void	FNBACK  noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    if( !rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;

    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_noshadow_put_rle(sx,sy,rle,dest_bitmap);
    else
        _range_noshadow_put_rle(sx,sy,rle,dest_bitmap);
}


EXPORT  void    FNBACK  additive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    if( !rle) return;
    if(sx <= -rle->w) return;
    if(sy <= -rle->h) return;
    if(sx >= dest_bitmap->w) return;
    if(sy >= dest_bitmap->h) return;

    if(sx>=0 && sy>=0 && sx+rle->w < dest_bitmap->w && sy+rle->h < dest_bitmap->h)
        _direct_additive_put_rle(sx,sy,rle,dest_bitmap);
    else
        _range_additive_put_rle(sx,sy,rle,dest_bitmap);
}



/******************************************************************************************************************/
/* STATIC FUNCTIONS                                                                                               */
/******************************************************************************************************************/
static  void    _range_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    //TODO: can be optimized(just process overlapped lines, rle->line[y]...)
    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 1) ^ (disp_y & 1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _direct_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    //SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
	PIXEL *dest_pix = NULL;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
				/*
                for(i=0;i<count;i++)
                {
					oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    disp_x++;
                }
				*/
				dest_pix = &dest_bitmap->line[disp_y][disp_x];
				disp_x += count;
				while(count--)
				{
					oper_dark_color(dest_pix, RLE_SHADOW_DARK);
					dest_pix++;
				}
            }
            else if(info & RLE_IMAGE)
            {
				/*
                for(i=0;i<count;i++)
                {
					dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
				*/
				dest_pix = &dest_bitmap->line[disp_y][disp_x];
				disp_x += count;
				while(count--)
				{
					*dest_pix = pix;
					dest_pix++;
				}
            }
            else
            {
                info &= RLE_ALPHA;
				dest_pix = &dest_bitmap->line[disp_y][disp_x];
				disp_x += count;
				while(count--)
				{
					oper_alpha_color(&pix, dest_pix, (SLONG)info);
					dest_pix++;
				}
				/*
                for(i=0;i<count;i++)
                {
					oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                    disp_x++;
                }
				*/
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                /*
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                */
                memcpy(&dest_bitmap->line[disp_y][disp_x], (PIXEL*)&data[data_index], info * SIZEOFPIXEL);
                data_index += info * SIZEOFPIXEL;
                disp_x += info;
                break;
            }
        }
    }
}


static  void    _range_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha)
{

    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    PIXEL black = SYSTEM_BLACK;
    SLONG alpha2;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&black, &dest_bitmap->line[disp_y][disp_x], alpha);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha);
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                info &= RLE_ALPHA;
                alpha2 = (SLONG)info * alpha / 255;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha2);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x], alpha);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _direct_alpha_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap,SLONG alpha)
{

    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;
    PIXEL black = SYSTEM_BLACK;
    SLONG alpha2;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    oper_alpha_color(&black, &dest_bitmap->line[disp_y][disp_x], alpha);
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha);
                    disp_x++;
                }
            }
            else //if(info & RLE_ALPHA)
            {
                info &= RLE_ALPHA;
                alpha2 = (SLONG)info * alpha / 255;
                for(i=0;i<count;i++)
                {
                    oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], alpha2);
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    oper_alpha_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x], alpha);
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _range_gray_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
						oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                        oper_gray_color(&dest_bitmap->line[disp_y][disp_x],&dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_gray_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _direct_gray_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    oper_gray_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    oper_alpha_color(&pix, &dest_bitmap->line[disp_y][disp_x], info);
                    oper_gray_color(&dest_bitmap->line[disp_y][disp_x],&dest_bitmap->line[disp_y][disp_x]);
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    oper_gray_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _range_noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
				disp_x += count;
				/*
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 1) ^ (disp_y & 1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
				*/
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _direct_noshadow_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix;
//  PIXEL pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
				disp_x += count;
				/*
                for(i=0;i<count;i++)
                {
                    //if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 1) ^ (disp_y & 1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
				*/
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    //if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                        dest_bitmap->line[disp_y][disp_x] = pix;
                    disp_x++;
                }
            }
            else
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    //if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_alpha_color(&pix,&dest_bitmap->line[disp_y][disp_x],(SLONG)info);
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                /*
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        dest_bitmap->line[disp_y][disp_x] = *(PIXEL*)&data[data_index];
                        //dest_bitmap->line[disp_y][disp_x] = rgb2hi(255,255,255);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                */
                memcpy(&dest_bitmap->line[disp_y][disp_x], (PIXEL*)&data[data_index], info * SIZEOFPIXEL);
                data_index += info * SIZEOFPIXEL;
                disp_x += info;
                break;
            }
        }
    }
}


static  void    _range_additive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        //if( (disp_x & 0x1) ^ (disp_y & 0x1) )
                        //  dest_bitmap->line[disp_y][disp_x] = SYSTEM_BLACK;//pix;
                        oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    }
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_additive_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    }
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        // Jack. [15:32,9/19/2002]
                        //*
                        UCHR    r,g,b;
                        hi2rgb(pix, &r, &g, &b);
                        r = (UCHR)( (SLONG)r * info / 255 );
                        g = (UCHR)( (SLONG)g * info / 255 );
                        b = (UCHR)( (SLONG)b * info / 255 );
                        pix2 = rgb2hi(r, g, b);
                        oper_additive_color(&pix2, &dest_bitmap->line[disp_y][disp_x]);
                        //*/

                        //oper_additive_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                        /*
                        pix2 = dest_bitmap->line[disp_y][disp_x];
                        oper_additive_color(&dest_bitmap->line[disp_y][disp_x], &pix2);
                        oper_alpha_color(&pix2, &dest_bitmap->line[disp_y][disp_x], info);
                        */

                        /*
                        pix2 = dest_bitmap->line[disp_y][disp_x];
                        oper_alpha_color(&pix, &pix2, info);
                        oper_additive_color(&pix2, &dest_bitmap->line[disp_y][disp_x]);
                        */
                    }
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    if(disp_x>=0 && disp_x<dest_bitmap->w && disp_y>=0 && disp_y<dest_bitmap->h)
                    {
                        oper_additive_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    }
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


static  void    _direct_additive_put_rle(SLONG sx,SLONG sy,RLE *rle,BMP *dest_bitmap)
{
    SLONG disp_x,disp_y;
    UCHR count,info;
    PIXEL pix,pix2;
    SLONG i;
    USTR *data=NULL;
    SLONG data_size;
    SLONG data_index;

    if(!rle) return;

    data = rle->line[0];
    data_size = rle->size;
    data_index=0;
    disp_x = sx;
    disp_y = sy;
    while(data_index<data_size)
    {
        count = data[data_index++];
        info = data[data_index++];
        if(count) // continuous pixes
        {
            pix = *(PIXEL*)&data[data_index];
            data_index += SIZEOFPIXEL;
            if(info & RLE_SHADOW)
            {
                for(i=0;i<count;i++)
                {
                    oper_dark_color(&dest_bitmap->line[disp_y][disp_x], RLE_SHADOW_DARK);
                    disp_x++;
                }
            }
            else if(info & RLE_IMAGE)
            {
                for(i=0;i<count;i++)
                {
                    oper_additive_color(&pix, &dest_bitmap->line[disp_y][disp_x]);
                    disp_x++;
                }
            }
            else//alpha
            {
                info &= RLE_ALPHA;
                for(i=0;i<count;i++)
                {
                    UCHR    r,g,b;
                    hi2rgb(pix, &r, &g, &b);
                    r = (UCHR)( (SLONG)r * info / 255 );
                    g = (UCHR)( (SLONG)g * info / 255 );
                    b = (UCHR)( (SLONG)b * info / 255 );
                    pix2 = rgb2hi(r, g, b);
                    oper_additive_color(&pix2, &dest_bitmap->line[disp_y][disp_x]);
                    disp_x++;
                }
            }
        }
        else    // count=0, is a flag
        {
            switch(info)
            {
            case 0://end of line
                disp_x=sx;
                disp_y++;
                break;
            case 1://end of image
                data_index=data_size+100;   //break
                break;
            case 2://transfered meaning
                disp_x += data[data_index++];
                disp_y += data[data_index++];
                break;
            default:// copy pixels
                for(i=0;i<info;i++)
                {
                    oper_additive_color((PIXEL*)&data[data_index], &dest_bitmap->line[disp_y][disp_x]);
                    data_index += SIZEOFPIXEL;
                    disp_x ++;
                }
                break;
            }
        }
    }
}


