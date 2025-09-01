/*
**      XGRAFX.CPP
**      Graphics functions.
**
**      ZJian, 2000.8.24.
**          base graphics functions.
**    
**
*/
////////////////////////////////////////////////////////////////////////////////////////////////////
//
//                   [ a d g ]
//[X Y 1] = (x y 1)* [ b e h ]  = (a*x + b*y + c, d*x + e*y + f, g*x + h*y + i)
//                   [ c f i ]
//
//[a d]
//[b e] is for scale,rotate,mirror,stagger.
//
//[c f] is for shift.
//
//[g]
//[h] is for projection.
//
//[i] is for flex.
////////////////////////////////////////////////////////////////////////////////////////////////////
#include "rays.h"
#include "winmain.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xvga.h"
#include "xtimer.h"

BMP *   screen_buffer=NULL;
BMP *   screen_channel0=NULL;
BMP *   screen_channel1=NULL;
BMP *   screen_channel2=NULL;
UHINT   (*true2hi)(ULONG c);
ULONG   (*hi2true)(UHINT c);
UCHR    (*get_r)(PIXEL c);
UCHR    (*get_g)(PIXEL c);
UCHR    (*get_b)(PIXEL c);
void    (*set_r)(UHINT *c,UCHR r);
void    (*set_g)(UHINT *c,UCHR g);
void    (*set_b)(UHINT *c,UCHR b);
UHINT   (*rgb2hi)(UCHR r,UCHR g,UCHR b);
void    (*hi2rgb)(UHINT c,UCHR *r,UCHR *g,UCHR *b);
UHINT   (*hi2fff)(UHINT c);
UHINT   (*fff2hi)(UHINT c);
void    (*oper_alpha_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
void    (*oper_additive_color) (UHINT *fore_color, UHINT *back_color);
void    (*oper_gray_color) (UHINT *fore_color, UHINT *back_color);
void    (*oper_subtractive_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_minimum_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_maximum_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_half_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_dark_color) (UHINT *fore_color, SLONG dark);
void    (*oper_red_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_green_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_blue_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_yellow_color) (UHINT *fore_color,UHINT *back_color);
void    (*oper_merge_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
void    (*oper_partial_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
void    (*oper_complement_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
void	(*oper_adulterate_color) (UHINT *fore_color, SLONG r, SLONG g, SLONG b);
void	(*oper_eclipse_color) (UHINT *fore_color, SLONG r, SLONG g, SLONG b);
void    (*oper_blue_alpha_color) (UHINT *fore_color, UHINT *back_color, SLONG alpha);
//
void    (*update_screen)(BMP *bitmap);
void    (*get_screen)(BMP *bitmap);
//
void    (*put_bitmap) (SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
void    (*alpha_put_bitmap)(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha);
void	(*half_put_bitmap)(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
void	(*gray_put_bitmap)(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp);
void	(*additive_put_bitmap) (SLONG sx,SLONG sy,BMP *src,BMP *des);
void	(*bound_put_bitmap) (SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp);
void	(*adulterate_bitmap) (SLONG r,SLONG g,SLONG b,BMP *bmp);
void	(*eclipse_bitmap) (SLONG r,SLONG g,SLONG b,BMP *bmp);
void    (*scale_put_bitmap) (SLONG sx,SLONG sy,SLONG scale_xl,SLONG scale_yl,BMP *src_bmp,BMP *dest_bmp);
//
void    (*alpha_put_bar)(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha);


PIXEL   SYSTEM_RED;
PIXEL   SYSTEM_GREEN;
PIXEL   SYSTEM_BLUE;
PIXEL   SYSTEM_YELLOW;
PIXEL   SYSTEM_CYAN;
PIXEL   SYSTEM_PINK;
PIXEL   SYSTEM_WHITE;
PIXEL   SYSTEM_BLACK;
PIXEL   SYSTEM_DARK0;
PIXEL   SYSTEM_DARK1;
PIXEL   SYSTEM_DARK2;
PIXEL   SYSTEM_DARK3;
PIXEL   SYSTEM_DARK4;
PIXEL   SYSTEM_DARK5;
PIXEL   SYSTEM_DARK6;
PIXEL   SYSTEM_PEST;




EXPORT  int     FNBACK  init_grafx(void)
{
    if(NULL==(screen_buffer=create_bitmap(g_nScreenWidth,g_nScreenHeight)))
        return -1;
    if(NULL==(screen_channel0=create_bitmap(g_nScreenWidth,g_nScreenHeight)))
        return -1;
    if(NULL==(screen_channel1=create_bitmap(g_nScreenWidth,g_nScreenHeight)))
        return -1;
    if(NULL==(screen_channel2=create_bitmap(g_nScreenWidth,g_nScreenHeight)))
        return -1;

    return 0;
}


EXPORT  void    FNBACK  free_grafx(void)
{
    destroy_bitmap(&screen_buffer);
    destroy_bitmap(&screen_channel0);
    destroy_bitmap(&screen_channel1);
    destroy_bitmap(&screen_channel2);
}


//==================================================================================

EXPORT  BMP*    FNBACK  create_bitmap(SLONG w,SLONG h)
{
    BMP *   tmp;
    PIXEL * t;
    int     i,bmpw=w;
    
    if (w&3) w=(w+3)&0xfffffffc;    // memory align(4 bytes)
    tmp=(BMP *)GlobalAlloc(GPTR, sizeof(BMP)+(h-1)*sizeof(PIXEL *)+w*h*sizeof(PIXEL));
    if (!tmp) return NULL;
    tmp->w = bmpw;
    tmp->pitch = w*sizeof(PIXEL);
    tmp->h = h;
    t=tmp->line[0]=(PIXEL *)((char *)tmp+sizeof(BMP)+(h-1)*sizeof(PIXEL *));
    for (i=1;i<h;i++)
        tmp->line[i]=(t+=w);
    return tmp;
}

EXPORT  BMP*    FNBACK  create_mirror_bitmap(SLONG w,SLONG h)
{
    BMP *   tmp;
    
    tmp=(BMP *)GlobalAlloc(GPTR, sizeof(BMP)+(h-1)*sizeof(PIXEL *));
    if(!tmp) return NULL;
    tmp->w=w,tmp->h=h;
    tmp->pitch=w*sizeof(PIXEL);
    return tmp;
}

EXPORT  BMP*    FNBACK  create_sub_bitmap(BMP *fbmp,SLONG x,SLONG y,SLONG w,SLONG h)
{
    BMP *   tmp=NULL;
    int     i;
    PIXEL * p;
    
    if (!fbmp || x<0 || y<0 || x+w>fbmp->w || y+h>fbmp->h) return NULL;
    tmp=(BMP *)GlobalAlloc(GPTR, sizeof(BMP)+(h-1)*sizeof(PIXEL *));
    if(! tmp) return NULL;
    tmp->w=w,tmp->h=h;
    tmp->pitch=fbmp->pitch;
    for (p=&fbmp->line[y][x],i=0;i<h;i++,p=(PIXEL *)((unsigned char *)p+tmp->pitch))
        tmp->line[i]=p;
    return tmp;
}

EXPORT  void    FNBACK  destroy_bitmap(BMP **bmp)
{
    if(*bmp)
    {
        GlobalFree(*bmp);
        *bmp=NULL;
    }
}

EXPORT  void    FNBACK  rotate_bitmap_left_right(BMP *bitmap)
{
    SLONG y,x;
    PIXEL c;
    for(y=bitmap->h-1;y>=0;y--)
    {
        for(x=0;x<bitmap->w/2;x++)
        {
            c=bitmap->line[y][x];
            bitmap->line[y][x]=bitmap->line[y][bitmap->w-1-x];
            bitmap->line[y][bitmap->w-1-x]=c;
        }
    }
}

EXPORT  void    FNBACK  rotate_bitmap_top_bottom(BMP *bitmap)
{
    SLONG y,x;
    PIXEL c;
    for(x=bitmap->w-1;x>=0;x--)
    {
        for(y=0;y<bitmap->h/2;y++)
        {
            c=bitmap->line[y][x];
            bitmap->line[y][x]=bitmap->line[bitmap->h-1-y][x];
            bitmap->line[bitmap->h-1-y][x]=c;
        }
    }
}

EXPORT  ABMP*   FNBACK  create_abitmap(SLONG w,SLONG h)
{
    SLONG pitch, i, abmp_w = w;
    ABMP *abmp=NULL;
    unsigned char *t=NULL;
    
    //zjian,2001.6.12. need to review and change here ???
    // pitch = w*3+(w&1);   //??
    if (w&3) w=(w+3)&0xfffffffc;    // memory align(4 bytes)
    pitch = 3 * w;

    abmp=(ABMP *)GlobalAlloc(GPTR, sizeof(ABMP)+(h-1)*sizeof(unsigned char *)+pitch*h);
    if(! abmp) return NULL;
    abmp->w = abmp_w;
    abmp->h = h;
    abmp->pitch = pitch;
    t=abmp->line[0]=(unsigned char *)((char *)abmp+sizeof(ABMP)+(h-1)*sizeof(unsigned char *));
    for (i=1;i<h;i++)
        abmp->line[i]=(t+=pitch);
    return abmp;
}

EXPORT  void    FNBACK  destroy_abitmap(ABMP **abmp)
{
    if(*abmp)
    {
        GlobalFree(*abmp);
        *abmp = NULL;
    }
}

EXPORT  void    FNBACK  rotate_abitmap_left_right(ABMP *abitmap)
{
    PIXEL pix;
    UCHR alpha;
    SLONG x,y;
    for(y=abitmap->h-1;y>=0;y--)
    {
        for(x=0;x<abitmap->w/2;x++)
        {
            pix = *( (PIXEL *)abitmap->line[y]+x );
            alpha = abitmap->line[y][abitmap->w*SIZEOFPIXEL+x];
            *( (PIXEL *)abitmap->line[y]+x ) = *( (PIXEL *)abitmap->line[y]+abitmap->w-1-x );
            abitmap->line[y][abitmap->w*SIZEOFPIXEL+x] = abitmap->line[y][abitmap->w*SIZEOFPIXEL+abitmap->w-1-x];
            *( (PIXEL *)abitmap->line[y]+abitmap->w-1-x ) = pix;
            abitmap->line[y][abitmap->w*SIZEOFPIXEL+abitmap->w-1-x] = alpha;
        }
    }
}

EXPORT  void    FNBACK  rotate_abitmap_top_bottom(ABMP *abitmap)
{
    PIXEL pix;
    UCHR alpha;
    SLONG x,y;
    for(x=abitmap->w-1;x>=0;x--)
    {
        for(y=0;y<abitmap->h/2;y++)
        {
            pix = *( (PIXEL *)abitmap->line[y]+x );
            alpha = abitmap->line[y][abitmap->w*SIZEOFPIXEL+x];
            *( (PIXEL *)abitmap->line[y]+x ) = *( (PIXEL *)abitmap->line[abitmap->h-1-y]+x );
            abitmap->line[y][abitmap->w*SIZEOFPIXEL+x] = abitmap->line[abitmap->h-1-y][abitmap->w*SIZEOFPIXEL+x];
            *( (PIXEL *)abitmap->line[abitmap->h-1-y]+x ) = pix;
            abitmap->line[abitmap->h-1-y][abitmap->w*SIZEOFPIXEL+x] = alpha;
        }
    }
}

EXPORT  void    FNBACK  put_abitmap(SLONG sx,SLONG sy,ABMP *abmp,BMP *bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;

    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+abmp->w,bmp->w)-des_sx;
    over_yl=MIN(sy+abmp->h,bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            switch(abmp->line[src_sy+y][abmp->w*sizeof(PIXEL)+src_sx+x])
            {
            case 0xff:
                bmp->line[des_sy+y][des_sx+x]=*((PIXEL *)abmp->line[src_sy+y]+src_sx+x);
                break;
            case 0x00:
                break;
            default:
                oper_alpha_color((PIXEL *)abmp->line[src_sy+y]+src_sx+x,
                    &bmp->line[des_sy+y][des_sx+x],
                    abmp->line[src_sy+y][abmp->w+abmp->w+src_sx+x]);
                break;
            }
        }
    }
}

EXPORT  UCHR    FNBACK  encode_point(SLONG x,SLONG y,SLONG left,SLONG top,SLONG right,SLONG bottom)
{
    UCHR code=0x00;
    if(x<left)
        code |= ENCODE_LEFT;
    if(x>right)
        code |= ENCODE_RIGHT;
    if(y<top)
        code |= ENCODE_TOP;
    if(y>bottom)
        code |= ENCODE_BOTTOM;
    return(code);
}

EXPORT  SLONG   FNBACK  clip_segment(SLONG *x1,SLONG *y1,SLONG *x2,SLONG *y2,SLONG left,SLONG top,SLONG right,SLONG bottom)
{
    UCHR code,code1,code2;
    SLONG x=0,y=0;
    
    code1 = encode_point(*x1,*y1,left,top,right,bottom);
    code2 = encode_point(*x2,*y2,left,top,right,bottom);
    while(code1 | code2)
    {
        if(code1 & code2)
            return 0;
        code=code1;
        if(code==0x00)
            code=code2;
        if(code==0x00)
            break;
        if(code & ENCODE_LEFT)
        {
            x=left;
            y=*y1+(SLONG)( (double)(left-*x1) * (double) (*y2-*y1) / (double)(*x2-*x1) );
        }
        else if(code & ENCODE_BOTTOM)
        {
            x=*x1+(SLONG)( (double)(bottom-*y1) * (double)(*x2-*x1) / (double)(*y2-*y1) );
            y=bottom;
        }
        else if(code & ENCODE_RIGHT)
        {
            x=right;
            y=*y1+(SLONG)( (double)(right-*x1) * (double)(*y2-*y1) / (double)(*x2-*x1) );
        }
        else if(code & ENCODE_TOP)
        {
            x=*x1+(SLONG)( (double)(top-*y1) * (double)(*x2-*x1) / (double)(*y2-*y1) );
            y=top;
        }
        if(code==code1)
        {
            *x1=x;
            *y1=y;
            code1 = encode_point(*x1,*y1,left,top,right,bottom);
        }
        else if(code==code2)
        {
            *x2=x;
            *y2=y;
            code2 = encode_point(*x2,*y2,left,top,right,bottom);
        }
    }
    return 1;
}

EXPORT  void    FNBACK  compress_line_pixels(PIXEL *des,PIXEL *src,SLONG des_len,SLONG src_len)
{
    // des[] = src[] * cos(sita) = src * (des_len*1.0/src_len)
    SLONG p=des_len,i,j=0,d=src_len-des_len;
    for(i=0;i<src_len;i++)
    {
        if(p<src_len)
            p+=des_len;
        else
            p-=d,des[j++]=src[i];
    }
}

EXPORT  void    FNBACK  expand_line_pixels(PIXEL *des,PIXEL *src,SLONG des_len,SLONG src_len)
{
    // des[] = src[] * sec(sita) = src[] * (des_len*1.0/src_len)
    SLONG p=src_len,i,j=0,d=des_len-src_len;
    for(i=0;i<des_len;i++)
    {
        if(p<des_len)
            des[i]=src[j],p+=src_len;
        else
            des[i]=src[++j],p-=d;
    }
}

EXPORT  PIXEL   FNBACK  get_pixel(SLONG x,SLONG y,BMP *bmp)
{
    return(bmp->line[y][x]);
}

EXPORT  void    FNBACK  put_pixel(SLONG x,SLONG y,PIXEL c,BMP *bmp)
{
    bmp->line[y][x]=c;
}

EXPORT  void    FNBACK  range_put_pixel(SLONG x,SLONG y,PIXEL c,BMP *bmp)
{
    if(y<0 || y>=bmp->h || x<0 || x>=bmp->w)
        return;
    bmp->line[y][x]=c;
}

EXPORT  void    FNBACK  put_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,PIXEL c,BMP *bmp)
{
    SLONG dx,dy,dm,dn,m,n,k,u,v,l,sum;
    
    if(!clip_segment(&x1,&y1,&x2,&y2,0,0,bmp->w-1,bmp->h-1))
        return;
    if((x2>x1)||(x2==x1 && y2<y1))
    {
        SWAP(x1,x2);
        SWAP(y1,y2);
    }
    dx=x2-x1, dy=y2-y1;
    if(dx==0)
    {
        u=y1,v=y2;
        if(dy<0) u=y2, v=y1;
        for (l=u; l<=v; l++)
            put_pixel(x2,l,c,bmp);
        return;
    }
    if(dy==0)
    {
        u=x1, v=x2;
        if(dx<0) u=x2,v=x1;
        for(l=u; l<=v; l++)
            put_pixel(l,y2,c,bmp);
        return;
    }
    
    dm=dn=1;
    if(dx<0) dx=-dx,dm=-1;
    if(dy<0) dy=-dy,dn=-1;
    m=dx,n=dy,k=1,u=x1,v=y1;
    if(dx<dy) m=dy,n=dx,k=dm,dm=dn,dn=k,k=0,u=y1,v=x1;
    l=0;
    sum=m;
    put_pixel(x1,y1,c,bmp);
    while (sum!=0)
    {
        sum=sum-1;
        l=l+n;
        u=u+dm;
        if(l>=m)
        {
            v=v+dn;
            l=l-m;
        }
        if(k==1)
            put_pixel(u,v,c,bmp);
        else
            put_pixel(v,u,c,bmp);
    }
    return;
}

EXPORT  void    FNBACK  put_dash_line(SLONG x1,SLONG y1,SLONG x2,SLONG y2,PIXEL c,SLONG skip,BMP *bmp)
{
    SLONG dx,dy,dm,dn,m,n,k,u,v,l,sum;
    SLONG count;
    
    if(!clip_segment(&x1,&y1,&x2,&y2,0,0,bmp->w-1,bmp->h-1))
        return;
    if((x2>x1)||(x2==x1 && y2<y1))
    {
        SWAP(x1,x2);
        SWAP(y1,y2);
    }
    dx=x2-x1, dy=y2-y1;
    if(dx==0)
    {
        u=y1,v=y2;
        if(dy<0) u=y2, v=y1;
        count=0;
        for (l=u; l<=v; l++)
        {
            if(++count >= (skip<<1) )
                count=0;
            else if(count>=skip)
                put_pixel(x2,l,c,bmp);
        }
        return;
    }
    if(dy==0)
    {
        u=x1, v=x2;
        if(dx<0) u=x2,v=x1;
        count=0;
        for(l=u; l<=v; l++)
        {
            if(++count >= (skip<<1) )
                count=0;
            else if(count>=skip)
                put_pixel(l,y2,c,bmp);
        }
        return;
    }
    
    dm=dn=1;
    if(dx<0) dx=-dx,dm=-1;
    if(dy<0) dy=-dy,dn=-1;
    m=dx,n=dy,k=1,u=x1,v=y1;
    if(dx<dy) m=dy,n=dx,k=dm,dm=dn,dn=k,k=0,u=y1,v=x1;
    l=0;
    sum=m;
    count=0;
    if(++count >= (skip<<1) )
        count=0;
    else if(count >= skip)
        put_pixel(x1,y1,c,bmp);
    while (sum!=0)
    {
        sum=sum-1;
        l=l+n;
        u=u+dm;
        if(l>=m)
        {
            v=v+dn;
            l=l-m;
        }
        if(++count >= (skip<<1) )
            count=0;
        else if(count >= skip)
        {
            if(k==1)
                put_pixel(u,v,c,bmp);
            else
                put_pixel(v,u,c,bmp);
        }
    }
    return;
}


EXPORT  void    FNBACK  put_arc(SLONG cx,SLONG cy,SLONG a,SLONG b,SLONG t1,SLONG t2,PIXEL color,BMP *bitmap)
{
    const double PI = 3.1415926535;
    double  dx,dy,f,fx,fy,x,y,j,mx,my;
    double  x1,y1,lx,ly,r,j1;
    double t;

    t=((double)t1/180.0)* PI;
    r=a;
    if(a<b) r=b;
    x1=(double)r*cos(t)+0.5;
    y1=(double)r*sin(t)+0.5;
    t=(double)t2-(double)t1;
    while (fabs(t)>360.0)
    {
        if(t>0.0)   t=t-360.0;
        else    t=t+360.0;
    }
    if((x1==0)&&(y1==0))    return;
    if(fabs(t)+1.0==1.0)    t=360.0;
    if(t>0.0)
    {
        if (y1==0)
        {
            if(x1>0)    dx=-1;
            else    dx=1;
        }
        else
        {
            if(y1>0)    dx=-1;
            else    dx=1;
        }
        if(x1==0)
        {
            if (y1>0)   dy=-1;
            else    dy=1;
        }
        else
        {
            if(x1>0)    dy=1;
            else    dy=-1;
        }
    }
    else
    {
        if(y1==0)
        {
            if(x1>0)    dx=-1;
            else    dx=1;
        }
        else
        {
            if(y1>0)    dx=1;
            else    dx=-1;
        }
        if(x1==0)
        {
            if(y1>0)    dy=-1;
            else    dy=1;
        }
        else
        {
            if(x1>0)    dy=-1;
            else    dy=1;
        }
    }
    lx=0;
    ly=0;
    j1=0;
    t2=t1;
    while(t2>=90)
    {
        j1=j1+2*r;
        t2=t2-90;
    }
    x=r*cos(t2*PI/180.0);
    y=r*sin(t2*PI/180.0);
    j1=j1+y+(r-x);
    j=0;
    t2=(SLONG)t; t=t1+t;
    if(t2>0.0)
    {
        while (t>=90.0)
        {
            j=j+2*r;
            t=t-90.0;
        }
        x=r*cos(t*PI/180.0)+0.5;
        y=r*sin(t*PI/180.0)+0.5;
        j=j+y+(r-x);
        j=j-j1;
    }
    else
    {
        if(t>=0.0)
            t2=1;
        else
        {
            t2=0;
            t=-t;
        }
        while (t>=90.0)
        {
            j=j+2*r;
            t=t-90.0;
        }
        x=r*cos(t*PI/180.0)+0.5;
        y=r*sin(t*PI/180.0)+0.5;
        j=j+y+(r-x);
        if(t2==1)
            j=j1-j;
        else
            j=j+j1;
    }
    j=j-1;
    f=0;
    fx=2*x1*dx+1;
    fy=2*y1*dy+1;
    mx=0;
    my=0;
    t=((double)t1/180.0)*PI;
    x=(int)((double)a*cos(t)+0.5);
    y=(int)((double)b*sin(t)+0.5);
    range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
    while (j!=0)
    {
        if(f>=0)
        {
            lx=lx+a;
            if(lx>=r)
            {
                if(mx!=0)
                {
                    range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
                    mx=0;
                }
                x=x+dx; lx=lx-r;
                if(my!=0)
                {
                    my=0;
                    range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
                }
                else
                    mx=1;
            }
            if(fx>0)
                f=f-fx;
            else
                f=f+fx;
            fx=fx+2;
            if((fx==0)||((fx<0)&&(fx-2>0))||((fx>0)&&(fx-2<0)))
            {
                dy=-dy;
                fy=-fy+2;
                f=-f;
            }
        }
        else
        {
            ly=ly+b;
            if(ly>=r)
            {
                if(my!=0)
                {
                    range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
                    my=0;
                }
                y=y+dy;
                ly=ly-r;
                if(mx!=0)
                {
                    mx=0;
                    range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
                }
                else
                    my=1;
            }
            if(fy>0)
                f=f+fy;
            else
                f=f-fy;
            fy=fy+2;
            if((fy==0)||((fy<0)&&(fy-2>0))||((fy>0)&&(fy-2<0)))
            {
                dx=-dx;
                fx=-fx+2;
                f=-f;
            }
        }
        j=j-1;
    }
    if((mx==0)&&(my==0))
        range_put_pixel((SLONG)(cx-y),(SLONG)(cy+x),(PIXEL)color,bitmap);
    return;
}

//---------------------------------------------------------------------------------------------------
// MidPoint put circle
//---------------------------------------------------------------------------------------------------
EXPORT  void    FNBACK  put_circle(SLONG cx,SLONG cy,SLONG r,PIXEL color,BMP *bitmap)
{
    SLONG x,y;
    SLONG delta_x,delta_y;
    SLONG d;

    x=0; y=r;
    delta_x=3; 
    delta_y=2-r-r;
    d=1-r;
    range_put_pixel(cx+x,cy+y,color,bitmap);
    range_put_pixel(cx+x,cy-y,color,bitmap);
    range_put_pixel(cx-x,cy+y,color,bitmap);
    range_put_pixel(cx-x,cy-y,color,bitmap);
    range_put_pixel(cx+y,cy+x,color,bitmap);
    range_put_pixel(cx+y,cy-x,color,bitmap);
    range_put_pixel(cx-y,cy+x,color,bitmap);
    range_put_pixel(cx-y,cy-x,color,bitmap);
    while(x<y)
    {
        if(d<0)
        {
            d+=delta_x;
            delta_x+=2;
            x++;
        }
        else
        {
            d+=(delta_x+delta_y);
            delta_x += 2;
            delta_y += 2;
            x++;
            y--;
        }
        range_put_pixel(cx+x,cy+y,color,bitmap);
        range_put_pixel(cx+x,cy-y,color,bitmap);
        range_put_pixel(cx-x,cy+y,color,bitmap);
        range_put_pixel(cx-x,cy-y,color,bitmap);
        range_put_pixel(cx+y,cy+x,color,bitmap);
        range_put_pixel(cx+y,cy-x,color,bitmap);
        range_put_pixel(cx-y,cy+x,color,bitmap);
        range_put_pixel(cx-y,cy-x,color,bitmap);
    }
}

//---------------------------------------------------------------------------------------------------
// MidPoint put ellipse
//---------------------------------------------------------------------------------------------------
EXPORT  void    FNBACK  put_ellipse(SLONG cx,SLONG cy,SLONG a,SLONG b,PIXEL color,BMP *bitmap)
{
    int x,y;
    double d1,d2;
    x=0; y=b;
    d1=b*b+a*a*(-b+0.25);
    range_put_pixel(cx+x,cy+y,color,bitmap);
    range_put_pixel(cx+x,cy-y,color,bitmap);
    range_put_pixel(cx-x,cy+y,color,bitmap);
    range_put_pixel(cx-x,cy-y,color,bitmap);
    while( b*b*(x+1) < a*a*(y-0.5) )
    {
        if(d1<0)
        {
            d1 += b*b*(2*x+3);
            x++;
        }
        else
        {
            d1 += (b*b*(2*x+3)+a*a*(-2*y+2));
            x++; y--;
        }
        range_put_pixel(cx+x,cy+y,color,bitmap);
        range_put_pixel(cx+x,cy-y,color,bitmap);
        range_put_pixel(cx-x,cy+y,color,bitmap);
        range_put_pixel(cx-x,cy-y,color,bitmap);
    }
    d2=sqrt(b*(x+0.5)) + sqrt(a*(y-1)) - sqrt(a*b);
    while(y>0)
    {
        if(d2 < 0)
        {
            d2 += b*b*(2*x+2) + a*a*(-2*y+3);
            x++; y--;
        }
        else
        {
            d2 += a*a*(-2*y+3);
            y--;
        }
        range_put_pixel(cx+x,cy+y,color,bitmap);
        range_put_pixel(cx+x,cy-y,color,bitmap);
        range_put_pixel(cx-x,cy+y,color,bitmap);
        range_put_pixel(cx-x,cy-y,color,bitmap);
    }
}


EXPORT  void    FNBACK  put_cross(SLONG x,SLONG y,PIXEL c,BMP *bmp)
{
    put_line(x-3,y,x+3,y,c,bmp);
    put_line(x,y-3,x,y+3,c,bmp);
}

EXPORT  void    FNBACK  put_xcross(SLONG x,SLONG y,PIXEL c,BMP *bmp)
{
    put_line(x-3,y-3,x+3,y+3,c,bmp);
    put_line(x+3,y-3,x-3,y+3,c,bmp);
}

EXPORT  void    FNBACK  put_box(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    put_line(sx,sy,sx+xl-1,sy,c,bmp);
    put_line(sx,sy+yl-1,sx+xl-1,sy+yl-1,c,bmp);
    put_line(sx,sy,sx,sy+yl-1,c,bmp);
    put_line(sx+xl-1,sy,sx+xl-1,sy+yl-1,c,bmp);
}

EXPORT  void    FNBACK  put_section(SLONG x,SLONG y,SLONG half_size,PIXEL outline_color,PIXEL inline_color,BMP *bmp)
{
    SLONG size=half_size<<1;

    put_box(x-(half_size+2),y-(half_size+2),size+4,size+4,outline_color,bmp);
    put_box(x-(half_size+1),y-(half_size+1),size+2,size+2,inline_color,bmp);
    put_box(x-half_size,y-half_size,size,size,outline_color,bmp);
}

EXPORT  void    FNBACK  put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            bmp->line[y][x]=c;
        }
    }
}

EXPORT  void    FNBACK  put_menu_hibar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp)
{
    put_bar(sx+1,sy+1,xl-2,yl-2,SYSTEM_DARK4,bmp);
    //alpha_put_bar(sx+1,sy+1,xl-2,yl-2,SYSTEM_DARK4,bmp,128);
    put_line(sx,sy,sx+xl-2,sy,SYSTEM_WHITE,bmp);
    put_line(sx,sy+1,sx,sy+yl-2,SYSTEM_WHITE,bmp);
    put_line(sx+xl-1,sy,sx+xl-1,sy+yl-1,SYSTEM_BLACK,bmp);
    put_line(sx,sy+yl-1,sx+xl-1,sy+yl-1,SYSTEM_BLACK,bmp);
    put_line(sx+xl-2,sy+1,sx+xl-2,sy+yl-2,SYSTEM_DARK2,bmp);
    put_line(sx+1,sy+yl-2,sx+xl-2,sy+yl-2,SYSTEM_DARK2,bmp);
}

EXPORT  void    FNBACK  put_menu_lobar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp)
{
    put_bar(sx+1,sy+1,xl-2,yl-2,SYSTEM_DARK4,bmp);
    put_line(sx,sy,sx+xl-2,sy,SYSTEM_BLACK,bmp);
    put_line(sx,sy,sx,sy+yl-2,SYSTEM_BLACK,bmp);
    put_line(sx+xl-1,sy,sx+xl-1,sy+yl-1,SYSTEM_WHITE,bmp);
    put_line(sx,sy+yl-1,sx+xl-1,sy+yl-1,SYSTEM_WHITE,bmp);
    put_line(sx+1,sy+1,sx+xl-3,sy+1,SYSTEM_DARK2,bmp);
    put_line(sx+1,sy+1,sx+1,sy+yl-3,SYSTEM_DARK2,bmp);
}

EXPORT  void    FNBACK  put_menu_box(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp)
{
    SLONG   ex,ey;
    SLONG   edge_width;
    PIXEL   in_color,mid_color;
    PIXEL   up_color,down_color;
    
    in_color=SYSTEM_DARK3;
    mid_color=SYSTEM_DARK5;
    down_color=SYSTEM_BLACK;
    up_color=SYSTEM_WHITE;
    edge_width=4;
    ex=sx+xl-1;
    ey=sy+yl-1;
    put_bar(sx,sy,xl,yl,in_color,bmp);
    put_bar(sx-edge_width,sy-edge_width,edge_width,yl+2*edge_width,mid_color,bmp);
    put_bar(ex,sy-edge_width,edge_width,yl+2*edge_width,mid_color,bmp);
    put_bar(sx,sy-edge_width,xl,edge_width,mid_color,bmp);
    put_bar(sx,ey,xl,edge_width,mid_color,bmp);
    put_bar(sx,sy,1,yl,down_color,bmp);
    put_bar(sx,sy,xl,1,down_color,bmp);
    put_bar(sx,ey,xl,1,up_color,bmp);
    put_bar(ex,sy,1,yl,up_color,bmp);
    put_bar(sx-edge_width,sy-edge_width,1,yl+2*edge_width,up_color,bmp);
    put_bar(sx-edge_width,sy-edge_width,xl+2*edge_width,1,up_color,bmp);
    put_bar(sx-edge_width,ey+edge_width,xl+2*edge_width,1,down_color,bmp);
    put_bar(ex+edge_width,sy-edge_width,1,yl+2*edge_width,down_color,bmp);
}

EXPORT  void    FNBACK  put_minimize_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        put_line(sx+4,sy+11,sx+10,sy+11,SYSTEM_BLACK,bmp);
        put_line(sx+4,sy+12,sx+10,sy+12,SYSTEM_BLACK,bmp);
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        put_line(sx+5,sy+12,sx+11,sy+12,SYSTEM_BLACK,bmp);
        put_line(sx+5,sy+13,sx+11,sy+13,SYSTEM_BLACK,bmp);
    }
}

EXPORT  void    FNBACK  put_maximize_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        put_box(sx+3,sy+3,8,8,SYSTEM_BLACK,bmp);
        put_line(sx+3,sy+4,sx+3+7,sy+4,SYSTEM_BLACK,bmp);
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        put_box(sx+3,sy+3,12,12,SYSTEM_BLACK,bmp);
        put_line(sx+3,sy+4,sx+3+6,sy+4,SYSTEM_BLACK,bmp);
    }
}

EXPORT  void    FNBACK  put_closing_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        put_line(sx+3,sy+3,sx+15-4,sy+15-4,SYSTEM_BLACK,bmp);
        put_line(sx+15-4,sy+3,sx+3,sy+15-4,SYSTEM_BLACK,bmp);
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        put_line(sx+3+1,sy+3+1,sx+15-3+1,sy+15-4+1,SYSTEM_BLACK,bmp);
        put_line(sx+15-4+1,sy+3+1,sx+3+1,sy+15-4+1,SYSTEM_BLACK,bmp);
    }
}

EXPORT  void    FNBACK  put_up_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    SLONG i;
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+7-i,sy+5+i,sx+7+i,sy+5+i,SYSTEM_BLACK,bmp);
        }
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+7-i,sy+5+i,sx+7+i,sy+5+i,SYSTEM_BLACK,bmp);
        }
    }
}

EXPORT  void    FNBACK  put_down_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    SLONG i;
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+7-i,sy+10-i,sx+7+i,sy+10-i,SYSTEM_BLACK,bmp);
        }
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+7-i,sy+10-i,sx+7+i,sy+10-i,SYSTEM_BLACK,bmp);
        }
    }
}

EXPORT  void    FNBACK  put_left_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    SLONG i;
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+4+i,sy+7-i,sx+4+i,sy+7+i,SYSTEM_BLACK,bmp);
        }
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+4+i,sy+7-i,sx+4+i,sy+7+i,SYSTEM_BLACK,bmp);
        }
    }
}

EXPORT  void    FNBACK  put_right_scroll_button(SLONG sx,SLONG sy,SLONG active,BMP *bmp)
{
    SLONG i;
    if(active)
    {
        put_menu_hibar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+10-i,sy+7-i,sx+10-i,sy+7+i,SYSTEM_BLACK,bmp);
        }
    }
    else
    {
        put_menu_lobar(sx,sy,16,16,bmp);
        for(i=0;i<5;i++)
        {
            put_line(sx+10-i,sy+7-i,sx+10-i,sy+7+i,SYSTEM_BLACK,bmp);
        }
    }
}

EXPORT  void    FNBACK  put_vertical_scroll_bar(SLONG sx,SLONG sy,SLONG yl,BMP *bmp)
{
    put_menu_hibar(sx,sy,16,yl,bmp);
}

EXPORT  void    FNBACK  put_horizontal_scroll_bar(SLONG sx,SLONG sy,SLONG xl,BMP *bmp)
{
    put_menu_hibar(sx,sy,xl,16,bmp);
}


EXPORT  void    FNBACK  put_triangle_bar(SLONG sx, SLONG sy, PIXEL color, BMP *bmp)
{
    SLONG   i;

    for(i=0; i<4; i++)
    {
        put_line(sx - i, sy - 2 + i, sx + i, sy - 2 + i, color, bmp);
    }
}


EXPORT  void    FNBACK  clear_bitmap(BMP *bmp)
{
    if(NULL==bmp) return;
    for(SLONG y=bmp->h-1; y>=0; y--)
    {
        memset(bmp->line[y],0x00,bmp->pitch);
    }
}


EXPORT  void    FNBACK  clear_abitmap(ABMP *abmp)
{
    if(NULL == abmp) return;
    for(SLONG y=abmp->h-1; y>=0; y--)
    {
        memset(abmp->line[y],0x00,abmp->pitch);
    }
}


EXPORT  void    FNBACK  clear_abitmap_image(ABMP *abmp)
{
    SLONG   y;

    for(y = 0; y < abmp->h; y ++)
    {
        memset(abmp->line[y], 0x00, abmp->w * SIZEOFPIXEL);
    }
}


EXPORT  void    FNBACK  fill_abitmap_image(ABMP *abmp, PIXEL c)
{
    SLONG   y, x;

    for(y = 0; y < abmp->h; y ++)
    {
        for(x = 0; x < abmp->w; x++ )
        {
            *(PIXEL*)&abmp->line[y][x * SIZEOFPIXEL] = c;
        }
    }
}


EXPORT  void    FNBACK  clear_abitmap_alpha(ABMP *abmp)
{
    SLONG   y;

    for(y = 0; y < abmp->h; y ++)
    {
        memset(&abmp->line[y][abmp->w * SIZEOFPIXEL], 0x00, abmp->w);
    }
}


EXPORT  void    FNBACK  fill_bitmap(BMP *bmp,PIXEL c)
{
    if(NULL==bmp) return;
    for(SLONG y=0;y<bmp->h;y++)
    {
        for(SLONG x=0;x<bmp->w;x++)
            bmp->line[y][x]=c;
    }
}

EXPORT BMP *    FNBACK  copy_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *src_bmp)
{
    BMP *des_bmp;

    if(NULL==(des_bmp=create_bitmap(xl,yl)))
        return(NULL);
    for(SLONG y=0;y<yl;y++)
    {
        memcpy(des_bmp->line[y],&src_bmp->line[sy+y][sx],des_bmp->pitch);
    }
    return(des_bmp);
}


EXPORT ABMP *   FNBACK  copy_abitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,ABMP *src_abmp)
{
    ABMP *des_abmp = NULL;

    if(NULL==(des_abmp=create_abitmap(xl,yl)))
        return(NULL);
    for(SLONG y=0;y<yl;y++)
    {
        memcpy(&des_abmp->line[y][0],&src_abmp->line[sy+y][sx*SIZEOFPIXEL],des_abmp->w*SIZEOFPIXEL);
        memcpy(&des_abmp->line[y][des_abmp->w*SIZEOFPIXEL],&src_abmp->line[sy+y][src_abmp->w*SIZEOFPIXEL+sx],des_abmp->w);
    }
    return(des_abmp);
}



EXPORT  void    FNBACK  get_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *des_bmp,BMP *src_bmp)
{
    if (xl&3) xl=(xl+3)&0xfffffffc;    // memory align(4 bytes)
    des_bmp->w=xl;
    des_bmp->h=yl;
	//geaan, 2001.12.21.
    //des_bmp->pitch=sizeof(PIXEL)*xl;
    for(SLONG y=0;y<yl;y++)
    {
        //memcpy(des_bmp->line[y],&src_bmp->line[sy+y][sx],des_bmp->pitch);
        memcpy(des_bmp->line[y],&src_bmp->line[sy+y][sx],xl * sizeof(PIXEL) );
    }
}

EXPORT  void    FNBACK  mirror_bitmap(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *des_bmp,BMP *src_bmp)
{
    des_bmp->w=xl;
    des_bmp->h=yl;
	//geaan, 2001.12.21.
    //des_bmp->pitch=sizeof(PIXEL)*xl;
    for(SLONG y=0;y<yl;y++)
    {
        des_bmp->line[y]=src_bmp->line[sy+y]+sx;
    }
}

EXPORT  void    FNBACK  pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            if(src_bmp->line[src_sy+y][src_sx+x])
                dest_bmp->line[des_sy+y][des_sx+x]=src_bmp->line[src_sy+y][src_sx+x];
        }
    }
}

EXPORT  void    FNBACK  mesh_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y,x_flag;
    
    x_flag = ((x1 & 1)+(y1 & 1)) & 1;
    for(y=y1;y<=y2;y++)
    {
        for(x=x1+x_flag;x<=x2;x+=2)
        {
            bmp->line[y][x]=c;
        }
        x_flag = 1-x_flag;
    }
}

EXPORT  void    FNBACK  mesh_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    SLONG x_flag;
   
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    x_flag = ((src_sx & 1)+(src_sy & 1)) & 1 ;
    for(y=0;y<over_yl;y++)
    {
        for(x=x_flag;x<over_xl;x+=2)
        {
            dest_bmp->line[des_sy+y][des_sx+x]=src_bmp->line[src_sy+y][src_sx+x];
        }
        x_flag=1-x_flag;
    }
}

EXPORT  void    FNBACK  mesh_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    SLONG x_flag;
   
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    x_flag = ((src_sx & 1)+(src_sy & 1)) & 1 ;
    for(y=0;y<over_yl;y++)
    {
        for(x=x_flag;x<over_xl;x+=2)
        {
            if(src_bmp->line[src_sy+y][src_sx+x])
                dest_bmp->line[des_sy+y][des_sx+x]=src_bmp->line[src_sy+y][src_sx+x];
        }
        x_flag=1-x_flag;
    }
}

EXPORT  void    FNBACK  mask_pest_bitmap(SLONG sx,SLONG sy,PIXEL mask_color,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            if(src_bmp->line[src_sy+y][src_sx+x])
                dest_bmp->line[des_sy+y][des_sx+x]=mask_color;
        }
    }
}

EXPORT  void    FNBACK  mask_edge_pest_bitmap(SLONG sx,SLONG sy,PIXEL mask_color,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=mask_color;
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  alpha_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_alpha_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x], alpha);
            }
        }
    }
}

EXPORT  void    FNBACK  alpha_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_alpha_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x], alpha);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  gray_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            if(src_bmp->line[src_sy+y][src_sx+x])
            {
                oper_gray_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  gray_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_gray_color(&fore,&dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  additive_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_additive_color(&fore, &bmp->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  additive_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_additive_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  additive_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_additive_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}



EXPORT  void    FNBACK  subtractive_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_subtractive_color(&fore, &bmp->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  subtractive_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_subtractive_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}

EXPORT  void    FNBACK  subtractive_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_subtractive_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  subtractive_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_subtractive_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  minimum_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_minimum_color(&fore, &bmp->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  minimum_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_minimum_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}

EXPORT  void    FNBACK  minimum_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_minimum_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  minimum_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_minimum_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  maximum_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_maximum_color(&fore, &bmp->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  maximum_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_maximum_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}

EXPORT  void    FNBACK  maximum_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_maximum_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  maximum_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_maximum_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  half_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_half_color(&fore, &bmp->line[y][x]);
        }
    }
}


EXPORT  void    FNBACK  half_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    PIXEL fore;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                oper_half_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
            }
        }
    }
}

EXPORT  void    FNBACK  half_edge_pest_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y,ay,ax;
    ULONG pest_flag=0;
    PIXEL back;
    PIXEL fore;
    
    // encode pest_flag:
    //
    //  0x400 0x200 0x100
    //  0x040   #   0x010
    //  0x004 0x002 0x001
    //
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            fore=src_bmp->line[src_sy+y][src_sx+x];
            if(fore)
            {
                back=dest_bmp->line[des_sy+y][des_sx+x];
                //  0x400 0x200 0x100
                //  0x040   #   0x010
                //  0x004 0x002 0x001
                pest_flag=0x757;
                ay=src_sy+y-1;
                if(ay>=0)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x400;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x200;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x100;
                    }
                }

                ay=src_sy+y+1;
                if(ay<src_bmp->h)
                {
                    if(src_sx+x-1>=0)
                    {
                        if(src_bmp->line[ay][src_sx+x-1])
                            pest_flag &= ~0x004;
                    }
                    if(src_bmp->line[ay][src_sx+x])
                        pest_flag &= ~0x002;
                    if(src_sx+x+1<src_bmp->w)
                    {
                        if(src_bmp->line[ay][src_sx+x+1])
                            pest_flag &= ~0x001;
                    }
                }
                ax=src_sx+x-1;
                if(ax>=0)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x040;
                }
                ax=src_sx+x+1;
                if(ax<src_bmp->w)
                {
                    if(src_bmp->line[src_sy+y][ax])
                        pest_flag &= ~0x010;
                }
                if(pest_flag)
                {
                    oper_half_color(&fore, &dest_bmp->line[des_sy+y][des_sx+x]);
                }
                else
                {
                    dest_bmp->line[des_sy+y][des_sx+x]=fore;
                }
            }
        }
    }
}


EXPORT  void    FNBACK  dark_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,BMP *bmp,SLONG dark)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_dark_color(&bmp->line[y][x], dark);
        }
    }
}


EXPORT  void    FNBACK  scale_pest_bitmap(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG   stx,sty ;
    SLONG   edx,edy ;
    SLONG   sxl,syl ;       // source xl,yl
    SLONG   txl,tyl ;       // target xl,yl
    SLONG   x,y ;
    SLONG   kdx,kdy;
    SLONG   mxl,myl;

    //ZJian need to optimize
    //optimize: precheck the overlapped range of the two bitmaps
    if((scalex<=0)||(scaley<=0))
        return ;
    sxl = (SLONG)src_bmp->w;
    syl = (SLONG)src_bmp->h;
    txl = scalex ;
    tyl = scaley ;
    stx = sx;
    sty = sy;
    edx = stx + txl ;
    edy = sty + tyl ;
    mxl = sxl*scalex;
    myl = syl*scaley;
    kdy = 0;
    for( y=sty; y<edy; y++ )
    {
        kdy += syl;
        if(kdy >= myl) // myl = syl*scaley
            break;
        kdx = 0;
        for( x=stx; x<edx; x++ )
        {
            kdx += sxl;
            if(kdx >= mxl)  // mxl = sxl*scalex
                break;
            if( (x>=0)&&(x<dest_bmp->w)&&(y>=0)&&(y<dest_bmp->h) )
            {
                if(src_bmp->line[kdy/scaley][kdx/scalex])
                    dest_bmp->line[y][x] = src_bmp->line[kdy/scaley][kdx/scalex];
            }
        }
    }
}


EXPORT  void    FNBACK  set_system_color(void)
{
    SYSTEM_RED    =true2hi(U4_SYSTEM_RED);
    SYSTEM_GREEN  =true2hi(U4_SYSTEM_GREEN);
    SYSTEM_BLUE   =true2hi(U4_SYSTEM_BLUE);
    SYSTEM_YELLOW =true2hi(U4_SYSTEM_YELLOW);
    SYSTEM_CYAN   =true2hi(U4_SYSTEM_CYAN);
    SYSTEM_PINK   =true2hi(U4_SYSTEM_PINK);
    SYSTEM_WHITE  =true2hi(U4_SYSTEM_WHITE);
    SYSTEM_BLACK  =true2hi(U4_SYSTEM_BLACK);
    SYSTEM_DARK0  =true2hi(U4_SYSTEM_DARK0);
    SYSTEM_DARK1  =true2hi(U4_SYSTEM_DARK1);
    SYSTEM_DARK2  =true2hi(U4_SYSTEM_DARK2);
    SYSTEM_DARK3  =true2hi(U4_SYSTEM_DARK3);
    SYSTEM_DARK4  =true2hi(U4_SYSTEM_DARK4);
    SYSTEM_DARK5  =true2hi(U4_SYSTEM_DARK5);
    SYSTEM_DARK6  =true2hi(U4_SYSTEM_DARK6);
    SYSTEM_PEST   =true2hi(U4_SYSTEM_PEST);
}

EXPORT  void    FNBACK  convert_buffer_hi2fff(PIXEL *buffer,SLONG size)
{
    for(SLONG i=size-1;i>=0;i--)
        buffer[i]=hi2fff(buffer[i]);
}

EXPORT  void    FNBACK  convert_buffer_fff2hi(PIXEL *buffer,SLONG size)
{
    for(SLONG i=size-1;i>=0;i--)
        buffer[i]=fff2hi(buffer[i]);
}

EXPORT  void    FNBACK  convert_bitmap_hi2fff(BMP *bitmap)
{
    SLONG x,y;
    for(y=bitmap->h-1;y>=0;--y)
    {
        for(x=bitmap->w-1;x>=0;--x)
        {
            bitmap->line[y][x] = hi2fff(bitmap->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  convert_bitmap_fff2hi(BMP *bitmap)
{
    SLONG x,y;
    for(y=bitmap->h-1;y>=0;--y)
    {
        for(x=bitmap->w-1;x>=0;--x)
        {
            bitmap->line[y][x] = fff2hi(bitmap->line[y][x]);
        }
    }
}

EXPORT  void    FNBACK  destroy_bitmap_ani(BITMAP_ANI **bitmap_ani)
{
    BITMAP_ANI *p=NULL,*q=NULL;
    p=(*bitmap_ani);
    while(p)
    {
        q=p;
        p=p->next;
        if(q)
        {
            if(q->bitmap) destroy_bitmap(&(q->bitmap));
            GlobalFree(q);
            q=NULL;
        }
    }
    // all data are free,so we just need to ...
    (*bitmap_ani)=NULL;
}


//zjian,2001.5.22.
//PS: to use this function, you must backup old screen_buffer to screen_channel2
EXPORT  void    FNBACK  fade_effect(SLONG tick)
{
    SLONG alpha=0;
    BMP *back_bmp=NULL;

    back_bmp = create_bitmap(screen_buffer->w, screen_buffer->h);
    if(back_bmp)
    {
        //get_screen(back_bmp);
        get_bitmap(0,0,screen_buffer->w,screen_buffer->h,back_bmp,screen_buffer);
        timer_tick00=0;
        for( alpha=0; alpha<256;  )
        {
            idle_loop();
            clear_bitmap(screen_buffer);
            put_bitmap(0,0,screen_channel2,screen_buffer);
            alpha_put_bitmap(0,0,back_bmp,screen_buffer,alpha);
            update_screen(screen_buffer);
            if(timer_tick00 > (ULONG)tick)
            {
                alpha+=15;
                timer_tick00=0;
            }
        }
        clear_bitmap(screen_buffer);
        put_bitmap(0,0,back_bmp,screen_buffer);
        destroy_bitmap(&back_bmp);
    }
    update_screen(screen_buffer);
}


EXPORT  ALF *   FNBACK  create_alf(SLONG w,SLONG h)
{
    ALF *   tmp;
    USTR * t;
    int     i,alfw=w;
    
    if (w&3) w=(w+3)&0xfffffffc;    // memory align(4 bytes)
    tmp=(ALF *)GlobalAlloc(GPTR, sizeof(ALF)+(h-1)*sizeof(USTR *)+w*h*sizeof(USTR));
    if (!tmp) return NULL;
    tmp->w=alfw;
    tmp->h=h;
    t=tmp->line[0]=(USTR *)((char *)tmp+sizeof(ALF)+(h-1)*sizeof(USTR *));
    for (i=1;i<h;i++)
        tmp->line[i]=(t+=w);
    return tmp;
}

EXPORT  void    FNBACK  destroy_alf(ALF **alf)
{
    if(*alf)
    {
        GlobalFree(*alf);
        *alf=NULL;
    }
}

EXPORT  IMG *   FNBACK  create_img(SLONG w,SLONG h)
{
    IMG *   tmp;
    USTR * t;
    int     i,imgw=w;
    
    if (w&3) w=(w+3)&0xfffffffc;    // memory align(4 bytes)
    tmp=(IMG *)GlobalAlloc(GPTR, sizeof(IMG)+(h-1)*sizeof(USTR *)+w*h*sizeof(USTR));
    if (!tmp) return NULL;
    tmp->w=imgw;
    tmp->h=h;
    t=tmp->line[0]=(USTR *)((char *)tmp+sizeof(IMG)+(h-1)*sizeof(USTR *));
    for (i=1;i<h;i++)
        tmp->line[i]=(t+=w);
    return tmp;
}

EXPORT  void    FNBACK  destroy_img(IMG **img)
{
    if(*img) 
    {
        GlobalFree(*img);
        *img=NULL;
    }
}


EXPORT  void    FNBACK  clear_img(IMG *img)
{
    SLONG   y;

    if(NULL==img) return;
    for(y=img->h-1; y>=0; y--)
    {
        memset(img->line[y],0x00,img->w * sizeof(UCHR) );
    }
}


EXPORT  void    FNBACK  get_img(SLONG sx,SLONG sy,SLONG xl,SLONG yl,IMG *des_img,IMG *src_img)
{
    SLONG   y;

    des_img->w = xl;
    des_img->h = yl;
    for(y=0;y<yl;y++)
    {
        memcpy(des_img->line[y],&src_img->line[sy+y][sx], xl * sizeof(UCHR) );
    }
}



EXPORT  void    FNBACK  oper_image_size(SLONG *sx,SLONG *sy,SLONG *ex,SLONG *ey,BMP *bitmap)
{
    SLONG image_sx,image_sy,image_ex,image_ey;
    SLONG x,y;
    
    if( (*ex) > bitmap->w ) (*ex)=bitmap->w;
    if( (*ey) > bitmap->h ) (*ey)=bitmap->h;
    if( (*sx) < 0 ) (*sx)=0;
    if( (*sy) < 0 ) (*sy)=0;
    image_sx = (*ex);
    image_sy = (*ey);
    image_ex = (*sx);
    image_ey = (*sy);
    for(y=(*sy);y<(*ey);++y)
    {
        for(x=(*sx);x<(*ex);++x)
        {
            if(bitmap->line[y][x])
            {
                if(x<image_sx) image_sx=x;
                if(x>image_ex) image_ex=x;
                if(y<image_sy) image_sy=y;
                if(y>image_ey) image_ey=y;
            }
        }
    }
    (*sx)=image_sx;
    (*sy)=image_sy;
    (*ex)=image_ex;
    (*ey)=image_ey;
}



EXPORT  BMP *   FNBACK  copy_bitmap_by_image(BMP *bmp,SLONG *sx,SLONG *sy)
{
    SLONG xbmp_sx,xbmp_sy;
    SLONG xbmp_ex,xbmp_ey;
    BMP *xbmp;
    
    xbmp_sx=xbmp_sy=0;
    xbmp_ex=bmp->w;
    xbmp_ey=bmp->h;
    oper_image_size(&xbmp_sx,&xbmp_sy,&xbmp_ex,&xbmp_ey,bmp);
    if(xbmp_sy>xbmp_ey || xbmp_sx>xbmp_ex)
    {
        xbmp=create_bitmap(2,2);
        clear_bitmap(xbmp);
        (*sx) = 0;
        (*sy) = 0;
    }
    else
    {
        xbmp=copy_bitmap(xbmp_sx,xbmp_sy,xbmp_ex-xbmp_sx+1,xbmp_ey-xbmp_sy+1,bmp);
        (*sx) = xbmp_sx;
        (*sy) = xbmp_sy;
    }
    return xbmp;
}


EXPORT  double  FNBACK  analyse_bitmap(BMP *src_bmp)
{
    ULONG normal_pixes;
    ULONG total_pixes;
    SLONG x,y;
    double trans;

    if(!src_bmp) return 0;

    normal_pixes = 0;
    total_pixes = 0;
    total_pixes = (ULONG)( src_bmp->w * src_bmp->h );
    for(y=0;y<src_bmp->h;++y)
    {
        for(x=0;x<src_bmp->w;++x)
        {
            if(src_bmp->line[y][x])
                normal_pixes ++;
        }
    }
    trans = (double)(total_pixes-normal_pixes)/(double)total_pixes;
    return trans;
}


EXPORT  void    FNBACK  convert_buffer_to_bitmap(PIXEL *buffer,SLONG w,SLONG h,BMP *dest_bmp)
{
    SLONG y;
    SLONG offset;

    dest_bmp->w=w;
    if (dest_bmp->w&3) dest_bmp->w=(dest_bmp->w+3)&0xfffffffc;    // memory align(4 bytes)
    dest_bmp->h=h;
	//geaan, 2001.12.21.
    //dest_bmp->pitch=sizeof(PIXEL)*dest_bmp->w;
    offset=0;
    for(y=0;y<h;y++)
    {
        memcpy(dest_bmp->line[y],&buffer[offset],sizeof(PIXEL)*w);
        offset += w;
    }
}


EXPORT  void    FNBACK  find_bitmap_barycenter(SLONG *bx, SLONG *by, BMP *src_bmp)
{
    SLONG x,y;
    SLONG total_y;
    SLONG bottom_y;
    SLONG average_y;
    double square_def;

    total_y=0;
    for(x=0;x<src_bmp->w;++x)
    {
        bottom_y = src_bmp->h-1;
        for(y=src_bmp->h-1;y>=0;--y)
        {
            if(src_bmp->line[y][x])
            {
                bottom_y = y;
                break;
            }
        }
        total_y += bottom_y;
    }
    average_y = (total_y+src_bmp->w/2)/src_bmp->w;

    square_def=0.0;
    for(x=0;x<src_bmp->w;++x)
    {
        bottom_y = src_bmp->h-1;
        for(y=src_bmp->h-1;y>=0;--y)
        {
            if(src_bmp->line[y][x])
            {
                bottom_y = y;
                break;
            }
        }
        square_def += (double)( (bottom_y - average_y) * (bottom_y - average_y) );
    }
    square_def = sqrt((double)square_def);

    (*bx) = src_bmp->w/2;
    (*by) = average_y;
}


EXPORT  void    FNBACK  oper_abitmap_image_size(SLONG *sx,SLONG *sy,SLONG *ex,SLONG *ey,ABMP *abitmap)
{
    SLONG image_sx,image_sy,image_ex,image_ey;
    SLONG x,y;
    
    if( (*ex) > abitmap->w ) (*ex)=abitmap->w;
    if( (*ey) > abitmap->h ) (*ey)=abitmap->h;
    if( (*sx) < 0 ) (*sx)=0;
    if( (*sy) < 0 ) (*sy)=0;
    image_sx = (*ex);
    image_sy = (*ey);
    image_ex = (*sx);
    image_ey = (*sy);
    for(y=(*sy);y<(*ey);y++)
    {
        for(x=(*sx);x<(*ex);x++)
        {
            if(abitmap->line[y][abitmap->w*SIZEOFPIXEL+x])
            {
                if(x<image_sx) image_sx=x;
                if(x>image_ex) image_ex=x;
                if(y<image_sy) image_sy=y;
                if(y>image_ey) image_ey=y;
            }
        }
    }
    (*sx)=image_sx;
    (*sy)=image_sy;
    (*ex)=image_ex;
    (*ey)=image_ey;
}


EXPORT  ABMP *  FNBACK  copy_abitmap_by_image(ABMP *abmp,SLONG *sx,SLONG *sy)
{
    SLONG xabmp_sx,xabmp_sy;
    SLONG xabmp_ex,xabmp_ey;
    ABMP *xabmp = NULL;
    
    xabmp_sx=xabmp_sy=0;
    xabmp_ex=abmp->w;
    xabmp_ey=abmp->h;
    oper_abitmap_image_size(&xabmp_sx,&xabmp_sy,&xabmp_ex,&xabmp_ey,abmp);

    if(xabmp_sy>xabmp_ey || xabmp_sx>xabmp_ex)
    {
        xabmp=create_abitmap(2,2);
        clear_abitmap(xabmp);
        (*sx) = 0;
        (*sy) = 0;
    }
    else
    {
        xabmp = copy_abitmap(xabmp_sx,xabmp_sy,xabmp_ex-xabmp_sx+1,xabmp_ey-xabmp_sy+1,abmp);
        (*sx) = xabmp_sx;
        (*sy) = xabmp_sy;
    }
    return xabmp;
}


EXPORT  ABMP *  FNBACK  make_abitmap_from_buffer(SLONG w,SLONG h,PIXEL *pix_buffer,USTR *alpha_buffer)
{
    ABMP *abmp=NULL;
    SLONG y,offset;

    if(NULL == (abmp = create_abitmap(w,h)) )
        return NULL;
    offset=0;
    for(y=0;y<abmp->h;y++)
    {
        memcpy(&abmp->line[y][0],pix_buffer+offset,SIZEOFPIXEL*w);
        memcpy(&abmp->line[y][w*SIZEOFPIXEL],alpha_buffer+offset,w);
        offset += w;
    }
    return (ABMP*)abmp;
}


EXPORT  void    FNBACK  put_abitmap_to_abitmap(SLONG sx,SLONG sy,ABMP *src_abmp,ABMP *dest_abmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG y;
    //SLONG x;

    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_abmp->w,dest_abmp->w)-des_sx;
    over_yl=MIN(sy+src_abmp->h,dest_abmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        memcpy(&dest_abmp->line[des_sy+y][des_sx*SIZEOFPIXEL],
            &src_abmp->line[src_sy+y][src_sx*SIZEOFPIXEL],
            over_xl*SIZEOFPIXEL);
        memcpy(&dest_abmp->line[des_sy+y][dest_abmp->w*SIZEOFPIXEL+des_sx],
            &src_abmp->line[src_sy+y][src_abmp->w*SIZEOFPIXEL+src_sx],
            over_xl);
    }
}

EXPORT  ABMP *  FNBACK  make_abitmap_from_bitmap(BMP *src_bmp)
{
    ABMP *abmp=NULL;
    SLONG x,y;

    if(NULL == (abmp = create_abitmap(src_bmp->w,src_bmp->h)) )
        return NULL;
    for(y=0;y<src_bmp->h;y++)
    {
        for(x=0;x<src_bmp->w;x++)
        {
            *(PIXEL*)&abmp->line[y][x*SIZEOFPIXEL] = src_bmp->line[y][x];
            if(src_bmp->line[y][x])
                abmp->line[y][abmp->w*SIZEOFPIXEL+x] = 0xff;
            else
                abmp->line[y][abmp->w*SIZEOFPIXEL+x] = 0x00;
        }
    }
    return(abmp);
}



EXPORT  BMP *   FNBACK  make_shadow_bitmap(BMP *src_bitmap, SLONG *stagger_rx, SLONG *stagger_ry)
{
    BMP *dest_bitmap=NULL;
    SLONG dest_sx,dest_sy,dest_ox;
    double dest_dx;
    SLONG src_sx,src_sy;
    double src_dy;

    SLONG w,h;
    SLONG x,y,src_y,src_x;

    //
    //
    //  |
    //  +--------+
    //  |        | 
    //  |        |
    //  |  R-----|--+
    //  | /      | /
    //  |/       |/
    //--O--------+----------------------------> X
    //  |
    //  | 
    //  |
    // \|/
    //  *
    //  Y
    //
    w = max( src_bitmap->w, src_bitmap->w+(*stagger_rx) ) 
        - min( 0, (*stagger_rx) );
    h = abs( *stagger_ry );

    if( (*stagger_rx)*(*stagger_ry) == 0) 
        return NULL;
    else if( (*stagger_rx)*(*stagger_ry) > 0)
        dest_sx = 0;
    else
        dest_sx = abs(*stagger_rx);

    dest_sy = 0;
    dest_dx = (double)(*stagger_rx) / (double)(*stagger_ry);

    src_sx = 0;
    src_dy = - (double)src_bitmap->h / (double)(*stagger_ry);
    if( *stagger_ry<0)
        src_sy = 0;
    else
        src_sy = src_bitmap->h-1;

    if(NULL==(dest_bitmap=create_bitmap(w,h)))
        return NULL;
    clear_bitmap(dest_bitmap);

    for(y=0;y<dest_bitmap->h;y++)
    {
        dest_ox = (SLONG)( dest_sx + dest_dx * y);
        src_y = (SLONG)(src_sy + src_dy * y);
        for(x=dest_ox; x<dest_ox+src_bitmap->w; x++)
        {
            src_x = x-dest_ox;
            if(src_bitmap->line[src_y][src_x])
                dest_bitmap->line[y][x] = SYSTEM_RED;
        }
    }

    // make this so we have an easy form to show shadow
    (*stagger_rx) = min( 0, (*stagger_rx) );
    (*stagger_ry) = src_bitmap->h + min( 0, (*stagger_ry) );

    return (BMP*)dest_bitmap;
}


EXPORT  ABMP *  FNBACK  make_shadow_abitmap(ABMP *src_abitmap, SLONG *stagger_rx, SLONG *stagger_ry)
{
    ABMP *dest_abitmap=NULL;
    SLONG dest_sx,dest_sy,dest_ox;
    double dest_dx;
    SLONG src_sx,src_sy;
    double src_dy;

    SLONG w,h;
    SLONG x,y,src_y,src_x;
    UCHR alpha;

    //
    //
    //  |
    //  +--------+
    //  |        | 
    //  |        |
    //  |  R-----|--+
    //  | /      | /
    //  |/       |/
    //--O--------+----------------------------> X
    //  |
    //  | 
    //  |
    // \|/
    //  *
    //  Y
    //
    w = max( src_abitmap->w, src_abitmap->w+(*stagger_rx) ) 
        - min( 0, (*stagger_rx) );
    h = abs( *stagger_ry );

    if( (*stagger_rx)*(*stagger_ry) == 0) 
        return NULL;
    else if( (*stagger_rx)*(*stagger_ry) > 0)
        dest_sx = 0;
    else
        dest_sx = abs(*stagger_rx);

    dest_sy = 0;
    dest_dx = (double)(*stagger_rx) / (double)(*stagger_ry);

    src_sx = 0;
    src_dy = - (double)src_abitmap->h / (double)(*stagger_ry);
    if( *stagger_ry<0)
        src_sy = 0;
    else
        src_sy = src_abitmap->h-1;

    if(NULL==(dest_abitmap=create_abitmap(w,h)))
        return NULL;
    clear_abitmap(dest_abitmap);

    for(y=0;y<dest_abitmap->h;y++)
    {
        dest_ox = (SLONG)( dest_sx + dest_dx * y);
        src_y = (SLONG)(src_sy + src_dy * y);
        for(x=dest_ox; x<dest_ox+src_abitmap->w; x++)
        {
            src_x = x-dest_ox;
            alpha = src_abitmap->line[src_y][src_abitmap->w*SIZEOFPIXEL+src_x];
            if(alpha)
            {
                //if(*(PIXEL*)&src_abitmap->line[src_y][src_x*SIZEOFPIXEL])
                *(PIXEL*)&dest_abitmap->line[y][x*SIZEOFPIXEL] = SYSTEM_RED;
                dest_abitmap->line[y][dest_abitmap->w*SIZEOFPIXEL+x] = alpha;
            }
        }
    }

    // make this so we have an easy form to show shadow
    (*stagger_rx) = min( 0, (*stagger_rx) );
    (*stagger_ry) = src_abitmap->h + min( 0, (*stagger_ry) );

    return (ABMP*)dest_abitmap;
}


EXPORT  void    FNBACK  change_color(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL src_color,PIXEL des_color,BMP *bitmap)
{
    SLONG x,y;
    SLONG ey,ex;

    ex = min(sx+xl,bitmap->w);
    ey = min(sy+yl,bitmap->h);
    for(y=sy; y<ey; y++)
    {
        for(x=sx; x<ex; x++)
        {
            if(bitmap->line[y][x] == src_color)
                bitmap->line[y][x] = des_color;
        }
    }
}



//-------------------------------------------------------------------------------------------------------------
// 256 color compatible functions
//-------------------------------------------------------------------------------------------------------------
EXPORT  void    FNBACK  make_indexed_color_table(USTR *palette, PIXEL *color_table)
{
    SLONG   i;
    SLONG   offset;
    UCHR    r,g,b;

    offset = 0;
    for(i=0; i<256; i++)
    {
        r = palette[offset+0];
        g = palette[offset+1];
        b = palette[offset+2];

        color_table[i] = rgb2hi(r,g,b);
        offset += 3;
    }
}


//------------------------------------------------------------------------------------------------------------
//与颜色相关的函数, 目前没有作整数型优化
//------------------------------------------------------------------------------------------------------------
#define UNDEFINED       0

//转换 RGB(红绿蓝) 到 HLS(Hue, Lightness, Saturation)(色度,亮度,饱和度)
//known:    r,g,b all in[0,1]
//find:     h in [0,360], l and s in [0,1],
//          except if s=0, then h=undefined
void    float_rgb2hls(float r, float g, float b, float *h, float *l, float *s)
{
    float   m,n;
    float   delta;

    m = r;
    if(m < g) m = g;
    if(m < b) m = b;

    n = r;
    if(n > g) n = g;
    if(n > b) n = b;

    (*l) = (m + n) / 2;
    if(m == n)
    {
        *s = 0;
        *h = UNDEFINED;
    }
    else
    {
        if( *l <= 0.5)
        {
            *s = (m - n) / (m + n);
        }
        else
        {
            *s = (m - n) / (2 - m - n);
        }

        delta = m - n;
        if(r == m)
            *h = (g - b) / delta;
        else if(g == m)
            *h = 2 + (b - r) / delta;
        else if(b == m)
            *h = 4 + (r - g) / delta;
        *h = *h * 60;
        if( *h < 0.0 )
            *h = *h + 360;
    }
}

//转换 RGB(红绿蓝) 到 HSV(Hue, Saturation, Value)(色度,亮度,饱和度)
//known:    r,g,b all in[0,1]
//find:     h in [0,360], s and v in [0,1]
void    float_rgb2hsv(float r, float g, float b, float *h, float *s, float *v)
{
    float   m,n;
    float   delta;

    m = r;
    if(m < g) m = g;
    if(m < b) m = b;

    n = r;
    if(n > g) n = g;
    if(n > b) n = b;

    (*v) = m;
    if(m != 0)
    {
        *s = (m - n) / m;
    }
    else
    {
        *s = 0;
    }

    if(*s == 0)
    {
        *h = UNDEFINED;
    }
    else
    {
        delta = m - n;
        if(r == m)
            *h = (g - b) / delta;
        else if(g == m)
            *h = 2 + (b - r) / delta;
        else if(b == m)
            *h = 4 + (r - g) / delta;
        *h = *h * 60;
        if( *h < 0 )
            *h = *h + 360;
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////
// LOCAL CPP FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////
void    _put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
   
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    over_xl *= SIZEOFPIXEL;
    over_yl = over_yl-1;
    do 
    {
        memcpy(&dest_bmp->line[des_sy+over_yl][des_sx],&src_bmp->line[src_sy+over_yl][src_sx],over_xl);
    } while(--over_yl>=0);
}


void    _alpha_put_bar(SLONG sx,SLONG sy,SLONG xl,SLONG yl,PIXEL c,BMP *bmp,SLONG alpha)
{
    SLONG x1=MAX(0,sx),y1=MAX(0,sy);
    SLONG x2=MIN(bmp->w-1,sx+xl-1),y2=MIN(bmp->h-1,sy+yl-1);
    SLONG x,y;
    PIXEL fore=c;
    
    for(y=y1;y<=y2;y++)
    {
        for(x=x1;x<=x2;x++)
        {
            oper_alpha_color(&fore, &bmp->line[y][x], alpha);
        }
    }
}


void    _alpha_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp,SLONG alpha)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_alpha_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x], alpha);
        }
    }
}


void    _half_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_half_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}


void    _gray_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_gray_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}


void    _additive_put_bitmap(SLONG sx,SLONG sy,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
    SLONG x,y;
    
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    src_sx=MAX(-sx,0);
    src_sy=MAX(-sy,0);
    for(y=0;y<over_yl;y++)
    {
        for(x=0;x<over_xl;x++)
        {
            oper_additive_color(&src_bmp->line[src_sy+y][src_sx+x], &dest_bmp->line[des_sy+y][des_sx+x]);
        }
    }
}


void	_bound_put_bitmap (SLONG sx, SLONG sy, SLONG bound_sx, SLONG bound_sy, SLONG bound_xl, SLONG bound_yl, BMP *src_bmp, BMP *dest_bmp)
{
    SLONG over_xl,over_yl;
    SLONG des_sx,des_sy;
    SLONG src_sx,src_sy;
   
    des_sx=MAX(sx,0);
    des_sy=MAX(sy,0);
    //over_xl=MIN(sx+src_bmp->w,dest_bmp->w)-des_sx;
    //over_yl=MIN(sy+src_bmp->h,dest_bmp->h)-des_sy;
    over_xl=MIN(sx+bound_xl,dest_bmp->w)-des_sx;
    over_yl=MIN(sy+bound_yl,dest_bmp->h)-des_sy;
    if(over_yl<=0 || over_xl<=0)
        return;
    //src_sx=MAX(-sx,0);
    //src_sy=MAX(-sy,0);
    src_sx=MAX(-sx+bound_sx,0+bound_sx);
    src_sy=MAX(-sy+bound_sy,0+bound_sy);
    over_xl *= SIZEOFPIXEL;
    over_yl = over_yl-1;
    do 
    {
        memcpy(&dest_bmp->line[des_sy+over_yl][des_sx],&src_bmp->line[src_sy+over_yl][src_sx],over_xl);
    } while(--over_yl>=0);
}



void	_adulterate_bitmap(SLONG r, SLONG g, SLONG b, BMP *bitmap)
{
    SLONG x, y;

    for(y = bitmap->h-1; y >= 0; --y)
    {
        for(x = bitmap->w-1; x >= 0; --x)
        {
			oper_adulterate_color( &bitmap->line[y][x], r, g, b);
        }
    }
}


void	_eclipse_bitmap(SLONG r,SLONG g,SLONG b,BMP *bitmap)
{
    SLONG x, y;

    for(y = bitmap->h-1; y >= 0; --y)
    {
        for(x = bitmap->w-1; x >= 0; --x)
        {
			oper_eclipse_color( &bitmap->line[y][x], r, g, b);
        }
    }
}


void    _scale_put_bitmap(SLONG sx,SLONG sy,SLONG scalex,SLONG scaley,BMP *src_bmp,BMP *dest_bmp)
{
    SLONG   stx,sty ;
    SLONG   edx,edy ;
    SLONG   sxl,syl ;       // source xl,yl
    SLONG   txl,tyl ;       // target xl,yl
    SLONG   x,y ;
    SLONG   kdx,kdy;
    SLONG   mxl,myl;

    //ZJian need to optimize
    //optimize: precheck the overlapped range of the two bitmaps
    if((scalex<=0)||(scaley<=0))
        return ;
    sxl = (SLONG)src_bmp->w;
    syl = (SLONG)src_bmp->h;
    txl = scalex ;
    tyl = scaley ;
    stx=sx;
    sty=sy;
    edx = stx + txl ;
    edy = sty + tyl ;
    mxl = sxl*scalex;
    myl = syl*scaley;
    kdy = 0;
    for( y=sty; y<edy; y++ )
    {
        kdy += syl;
        if(kdy >= myl) // myl = syl*scaley
            break;
        kdx = 0;
        for( x=stx; x<edx; x++ )
        {
            kdx += sxl;
            if(kdx >= mxl)  // mxl = sxl*scalex
                break;
            if( (x>=0)&&(x<dest_bmp->w)&&(y>=0)&&(y<dest_bmp->h) )
            {
                dest_bmp->line[y][x] = src_bmp->line[kdy/scaley][kdx/scalex];
            }
        }
    }
}



//==========================================================================
