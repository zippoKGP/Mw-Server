/*
**  xrla.cpp
**  rle array functions.
**
**  Jack, 2002.3.27.
*/
#include "rays.h"
#include "xgrafx.h"
#include "xsystem.h"
#include "xinput.h"
#include "xfont.h"
#include "xrle.h"
#include "xrla.h"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  RLA*    FNBACK  create_rla(SLONG w, SLONG h, SLONG rw, SLONG rh)
{
    RLA*    tmp = NULL;
    SLONG   cw, ch, size, i;

    cw = (w + rw - 1) / rw;
    ch = (h + rh - 1) / rh;
    size = sizeof(RLA) + (cw * ch - 1) * sizeof(RLE *);
    if(NULL == (tmp = (RLA*)malloc(size)))
        return  NULL;
    tmp->w = w;
    tmp->h = h;
    tmp->rw = rw;
    tmp->rh = rh;
    tmp->cw = cw;
    tmp->ch = ch;
    for(i = cw*ch-1; i>=0; i--)
    {
        tmp->rle[i] = NULL;
    }
    return  (RLA*)tmp;
}



EXPORT  void    FNBACK  destroy_rla(RLA **rla)
{
    SLONG   i;

    if(*rla)
    {
        for(i=(*rla)->cw * (*rla)->ch - 1; i>=0; i--)
        {
            destroy_rle(&(*rla)->rle[i]);
        }
        free(*rla);
        (*rla) = NULL;
    }
}


EXPORT  void    FNBACK  convert_rla_hi2fff(RLA *rla)
{
    SLONG   m, n, index;

    for(n = 0; n < rla->ch; n ++)
    {
        for(m = 0; m < rla->cw; m ++)
        {
            index = n * rla->cw + m;
            if(rla->rle[index])
            {
                convert_rle_hi2fff(rla->rle[index]);
            }
        }
    }
}


EXPORT  void    FNBACK  convert_rla_fff2hi(RLA *rla)
{
    SLONG   m, n, index;

    for(n = 0; n < rla->ch; n ++)
    {
        for(m = 0; m < rla->cw; m++)
        {
            index = n * rla->cw + m;
            if(rla->rle[index])
            {
                convert_rle_fff2hi(rla->rle[index]);
            }
        }
    }
}


EXPORT  void    FNBACK  put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            //if(rle)
            {
                put_rle(x, y, rle, bitmap);
                //put_box(x, y, rle->w, rle->h, SYSTEM_BLUE, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  mask_edge_put_rla(SLONG sx, SLONG sy, PIXEL mask_color, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                mask_edge_put_rle(x, y, mask_color, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  gray_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                gray_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  gray_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                gray_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  alpha_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap, SLONG alpha)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                alpha_put_rle(x, y, rle, bitmap, alpha);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  alpha_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap, SLONG alpha)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                alpha_edge_put_rle(x, y, rle, bitmap, alpha);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  additive_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                additive_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  additive_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                additive_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  subtractive_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                subtractive_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  subtractive_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                subtractive_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  minimum_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                minimum_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  minimum_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                minimum_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  maximum_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                maximum_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  maximum_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                maximum_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  half_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                half_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  half_edge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                half_edge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  mesh_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                mesh_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  mask_put_rla(SLONG sx, SLONG sy, PIXEL mask_color, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                mask_put_rle(x, y, mask_color, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  red_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                red_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  green_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                green_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  blue_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                blue_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  yellow_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                yellow_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  merge_put_rla(SLONG sx, SLONG sy, RLA *rla, BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                merge_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void    FNBACK  scale_put_rla(SLONG sx, SLONG sy, SLONG scalex, SLONG scaley, RLA *rla, BMP *bitmap)
{
    SLONG   m, n, index;
    double  zoom_x, zoom_y;
    SLONG   x, y, scale_xl2, scale_yl2;
    RLE *rle;

    if(! (rla->w && rla->h) )
        return;
    zoom_x = (double)scalex / rla->w;
    zoom_y = (double)scaley / rla->h;

    for(n = 0; n < rla->ch; n++)
    {
        y = (SLONG)(sy + rla->rh * n * zoom_y);

        if(n == rla->ch - 1)
            scale_yl2 = (SLONG)(sy + scaley) - y;
        else
            scale_yl2 = (SLONG)(sy + rla->rh * (n+1) * zoom_y) - y;

        for(m = 0; m < rla->cw; m++)
        {
            x = (SLONG)(sx + rla->rw * m * zoom_x);

            if(m == rla->cw - 1)
                scale_xl2 = (SLONG)(sx + scalex) - x;
            else
                scale_xl2 = (SLONG)(sx + rla->rw * (m+1) * zoom_x) - x;

            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                scale_put_rle(x, y, scale_xl2, scale_yl2, rle, bitmap);
            }
        }
    }
}


EXPORT  void    FNBACK  mask_put_rla_edge(SLONG sx,SLONG sy,PIXEL mask_color,RLA *rla,BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                mask_put_rle_edge(x, y, mask_color, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}


EXPORT  void	FNBACK  noshadow_put_rla(SLONG sx,SLONG sy,RLA *rla,BMP *bitmap)
{
    SLONG over_xl, over_yl;
    SLONG des_sx, des_sy;
    SLONG src_sx, src_sy;
    
    des_sx = MAX(sx,0);
    des_sy = MAX(sy,0);
    over_xl = MIN(sx + rla->w, bitmap->w) - des_sx;
    over_yl = MIN(sy + rla->h, bitmap->h) - des_sy;
    if(over_yl <= 0 || over_xl <= 0)
        return;
    src_sx = MAX(-sx, 0);
    src_sy = MAX(-sy, 0);

    //overlapped area:
    //src_sy -> src_sy + over_yl
    //src_sx -> src_sx + over_xl
    //des_sy -> des_sy + over_yl
    //des_sx -> des_sx + over_xl
    SLONG   start_m, start_n, end_m, end_n, m, n, index;
    SLONG   start_x, start_y, x, y;
    RLE     *rle;

    start_m = src_sx / rla->rw;
    start_n = src_sy / rla->rh;
    end_m = (src_sx + over_xl-1) / rla->rw;
    end_n = (src_sy + over_yl-1) / rla->rh;
    start_x = des_sx + (start_m * rla->rw - src_sx);
    start_y = des_sy + (start_n * rla->rh - src_sy);
    y = start_y;
    for(n = start_n; n <= end_n; n++)
    {
        x = start_x;
        for(m = start_m; m <= end_m; m++)
        {
            index = n * rla->cw + m;
            rle = rla->rle[index];
            if(rle)
            {
                noshadow_put_rle(x, y, rle, bitmap);
            }
            x += rla->rw;
        }
        y += rla->rh;
    }
}



EXPORT  RLA *   FNBACK  make_rla(ABMP *image_abmp, ABMP *shadow_abmp)
{
    RLA *rla = NULL;
    IMG *info_img=NULL;
    BMP *merge_bmp=NULL;
    SLONG w, h, merge_w, merge_h, w_sects, h_sects, x, y;
    SLONG edge_mask;// for check image edge
    UCHR image_alpha, shadow_alpha, encode_info, alpha;

    SLONG   m, n;
    IMG *sect_img = NULL;
    BMP *sect_bmp = NULL;
    SLONG sect_w, sect_h;


    //(1) safe check --------------------------------------------------------------------------
    if(NULL==image_abmp) return NULL;
    w = image_abmp->w;
    h = image_abmp->h;

    if(shadow_abmp)
    {
        if( (w!=shadow_abmp->w) || (h!=shadow_abmp->h) )
            goto error;
    }

    //-----------------------------------------------------------------------------------------
    //(PART I) Merge image_abmp & alpha_abmp to info_img & merge bmp.
    //  create merge info img & merge bmp
    //  the info img is the 1byte array[][] for save the image's info mask.
    //  the merge bmp is the merged bitmap for image_abmp & shadow_abmp.
    //-----------------------------------------------------------------------------------------
    merge_w = w;
    if (merge_w & 3) merge_w = (merge_w+3) & 0xfffffffc;    // memory align(4 bytes)
    merge_h = h;

    if(NULL==(info_img = create_img(merge_w, merge_h)))
        goto error;
    if(NULL==(merge_bmp = create_bitmap(merge_w, merge_h)))
        goto error;
    clear_img(info_img);
    clear_bitmap(merge_bmp);

    shadow_alpha = 0x00;
    for(y=0; y<h; y++)
    {
        for(x=0; x<w; x++)
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
                    if(shadow_alpha >= 0x80)
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

    //-----------------------------------------------------------------------------------------
    //(PART II) Encode rla data.
    //-----------------------------------------------------------------------------------------
    if(NULL == (rla = create_rla(w, h, SECT_RLE_WIDTH, SECT_RLE_HEIGHT)))
        goto error;
    if(NULL == (sect_img = create_img(rla->rw, rla->rh)))
        goto error;
    if(NULL == (sect_bmp = create_bitmap(rla->rw, rla->rh)))
        goto error;
    w_sects = rla->cw;
    h_sects = rla->ch;

    for(n=0; n<h_sects; n++)
    {
        y = n * rla->rh;
        for(m=0; m<w_sects; m++)
        {
            x = m * rla->rw;
            clear_bitmap(sect_bmp);
            clear_img(sect_img);
            sect_w = rla->rw;
            sect_h = rla->rh;

            if(m == w_sects-1) // right edges
            {
                sect_w = merge_w - (w_sects - 1) * rla->rw;
            }
            if(n == h_sects-1) //bottom edge
            {
                sect_h = merge_h - (h_sects - 1) * rla->rh;
            }
            get_bitmap(x, y, sect_w, sect_h, sect_bmp, merge_bmp);
            get_img(x, y, sect_w, sect_h, sect_img, info_img);
            rla->rle[n * rla->cw + m] = encode_rle(sect_bmp, sect_img);
        }
    }

    //OK, rla created. free temp memory.
    destroy_img(&info_img);
    destroy_bitmap(&merge_bmp);
    destroy_img(&sect_img);
    destroy_bitmap(&sect_bmp);

    //--------------------------------------------------------------
    //now test show rla.
    //--------------------------------------------------------------
    SLONG   main_pass;
    SLONG   disp_sx, disp_sy;
    UCHR    ch;

    //Jack, 2002.4.2. remark for cancel the test.
    main_pass = 1;
    //main_pass = 0;
    disp_sx = 0;
    disp_sy = 0;
    while(0 == main_pass)
    {
        idle_loop();
        ch = read_data_key();
        if(ch) reset_data_key();

        fill_bitmap(screen_buffer, rgb2hi(180,180,180));
        put_rla(disp_sx, disp_sy, rla, screen_buffer);
        sprintf((char *)print_rec, "TEST PUT RLA");
        print16(4, screen_buffer->h - 20, (USTR*)print_rec, PEST_PUT, screen_buffer);
        update_screen(screen_buffer);

        switch(ch)
        {
        case S_Left: disp_sx += 20; break;
        case S_Right:disp_sx -= 20; break;
        case S_Up:  disp_sy += 20;  break;
        case S_Dn:  disp_sy -= 20;  break;
        case S_Esc: main_pass = 1;  break;
        }
    }

    return  (RLA*)rla;

error:
    destroy_img(&info_img);
    destroy_bitmap(&merge_bmp);
    destroy_img(&sect_img);
    destroy_bitmap(&sect_bmp);
    destroy_rla(&rla);
    return NULL;
}


EXPORT  SLONG   FNBACK  naked_write_rla(FILE *fp, RLA *rla)
{
    SLONG   i, result;

    fwrite(rla, 1, sizeof(RLA) - sizeof(RLE *), fp);
    for(i=0; i<rla->cw * rla->ch; i++)
    {
        result = naked_write_rle(fp, rla->rle[i]);
        if(TTN_OK != result)
        {
            log_error(1, "write rle error when naked write rla");
            return  TTN_ERROR;
        }
    }

    return  TTN_OK;
}


EXPORT  SLONG   FNBACK  naked_read_rla(PACK_FILE *fp, RLA **rla)
{
    RLA     head, *tmp = NULL;
    RLE     *rle = NULL;
    SLONG   i;

    pack_fread(&head, 1, sizeof(RLA) - sizeof(RLE *), fp);
    if(NULL == (tmp = create_rla(head.w, head.h, head.rw, head.rh)))
    {
        log_error(1, "memory alloc error");
        return  TTN_ERROR;
    }
    for(i=0; i<head.cw * head.ch; i++)
    {
        naked_read_rle(fp, &rle);
        tmp->rle[i] = rle;
    }
    *rla = tmp;
    return  TTN_OK;
}


