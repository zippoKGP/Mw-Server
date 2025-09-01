/*
**      POLY.CPP
**      Poly functions.
**
**      ZJian, 1999.11.11.
**          Poly functions for 3D.
**      ZJian, 2000.7.25.
**          Rewrite the functions for 2D and 3D because some functions have too more parameters
**              in last version.
**
**
*/
#include "rays.h"
#include "xsystem.h"
#include "xfont.h"
#include "xpoly.h"

EXPORT  SLONG   FNBACK  compute_greatest_common_divisor(SLONG mm,SLONG nn)
{
    SLONG   m,n,left,pass=0;
    
    m=max(abs(mm),abs(nn));
    n=min(abs(mm),abs(nn));
    if(!n) return 1;
    while(pass==0)
    {
        left=m%n;
        if(left)m=n,n=left;
        else pass=1;
    }
    return(n);
}


EXPORT  POLY *  FNBACK  D3_create_poly(void)
{
    POLY *tmp;
    if(NULL==(tmp=(POLY *)GlobalAlloc(GPTR, sizeof(POLY))))
        return NULL;
    return tmp;
}



EXPORT  void    FNBACK  D3_destroy_poly(POLY *pl)
{
    if(pl) { GlobalFree(pl); pl=NULL; }
}



EXPORT  void    FNBACK  D3_copy_poly(POLY *dest_poly,POLY *src_poly)
{
    if( !(dest_poly && src_poly) ) return;
    memcpy(dest_poly,src_poly,sizeof(POLY));
}



EXPORT  void    FNBACK  D3_set_poly_type(POLY *pl,ULONG type)
{
    if(pl) pl->type = type;
}

EXPORT  void    FNBACK  D3_set_poly_point_underside(POLY *pl,SLONG index,SLONG x,SLONG y,SLONG z,SLONG set_flag)
{
    SLONG xx,yy,zz;

    if(!pl) return;
    if(index>=pl->pn) return;
    
    if(set_flag & SET_SPECIFY_X)
        pl->px[index]=x;
    if(set_flag & SET_SPECIFY_Y)
        pl->py[index]=y;

    if(set_flag & SET_SPECIFY_Z)
    {
        pl->pz[index]=z;
    }
    else
    {
        D3_intersect_line_surface(pl->px[index],pl->py[index],0,
            pl->px[index],pl->py[index],10,
            pl->na[COVER_SURFACE],pl->nb[COVER_SURFACE],pl->nc[COVER_SURFACE],pl->nd[COVER_SURFACE],
            &xx,&yy,&zz);
        pl->pz[index]=zz;
    }
    
    pl->px[pl->pn]=pl->px[0];
    pl->py[pl->pn]=pl->py[0];
    pl->pz[pl->pn]=pl->pz[0];
    pl->pu[pl->pn]=pl->pu[0];
}

EXPORT  void    FNBACK  D3_move_poly_by_barycenter(POLY *pl,SLONG new_bx,SLONG new_by)
{
    SLONG dx,dy;
    SLONG i;

    dx=new_bx-pl->bx;
    dy=new_by-pl->by;

    pl->bx+=dx;
    pl->by+=dy;
    for(i=0;i<=pl->pn;i++)
    {
        pl->px[i] += dx;
        pl->py[i] += dy;
    }
}


EXPORT  void    FNBACK  D3_translate_poly_world_coordinates(POLY *pl,SLONG px,SLONG py,SLONG pz)
{
    SLONG i;
    for(i=0;i<=pl->pn;++i)
    {
        pl->px[i] += px;
        pl->py[i] += py;
        pl->pz[i] += pz;
        pl->pu[i] += pz;
    }
    pl->bx += px;
    pl->by += py;
    pl->bz += pz;
    pl->bu += pz;
}

//==================================================================================
// 2 Demension functions
//==================================================================================
//FUNCTION: Check the two points are at the same side of a line
//ARGUMENT: (px1,py1),(px2,py2)  -> point1 and point2
//          (lx1,ly1)-(lx2,ly2)  -> line
//RETURN:   TRUE for the points are at the same side of the line,FALSE else
EXPORT SLONG FNBACK D2_is_sameside_point_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,
                                              SLONG lx1,SLONG ly1,SLONG lx2,SLONG ly2)
{
    double  dx=lx2-lx1,dy=ly2-ly1;
    double  dx1=px1-lx1,dy1=py1-ly1;
    double  dx2=px2-lx1,dy2=py2-ly1;
    double  fork1,fork2;

    fork1 = (dx*dy1)-(dx1*dy);
    fork2 = (dx*dy2)-(dx2*dy);
    if(fork1>=0 && fork2>=0) return TRUE;
    if(fork1<=0 && fork2<=0) return TRUE;
    return FALSE;
}

//FUNCTION: Check are the two segment intersect together
//ARGUMENT: (px1,py1)-(px2,py2)  -> segment1 P
//          (qx1,qy1)-(qx2,qy2)  -> segment2 P
//RETURN:   TRUE for cross,FALSE else
EXPORT SLONG FNBACK D2_is_intersect_segment_segment(SLONG px1,SLONG py1,SLONG px2,SLONG py2,
                                                    SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2)
{
    if( (D2_is_sameside_point_line(px1,py1,px2,py2,qx1,qy1,qx2,qy2)==FALSE)
        && (D2_is_sameside_point_line(qx1,qy1,qx2,qy2,px1,py1,px2,py2)==FALSE) )
        return TRUE;
    else
        return FALSE;
}

//FUNCTION: Check is there any intersect points of  line and poly?
//ARGUMENT: (x1,y1)-(x2,y2)   -> line
//          *pl               -> poly
//RETURN:   TRUE for the found,FALSE else
EXPORT SLONG FNBACK D2_is_intersect_line_poly(SLONG x1,SLONG y1,SLONG x2,SLONG y2,POLY *pl)
{
    double  dx=x2-x1,dy=y2-y1;
    SLONG   pn,i,flag,flag1=0;
    double  data;

    pn=pl->pn;
    for(i=0;i<pn;i++)
    {
        data=dx*(pl->py[i]-y1)-(pl->px[i]-x1)*dy;
        flag=ZSGN(data);
        if(!flag)
            return(TRUE);
        if(!flag1)
            flag1=flag;
        if(flag!=flag1)
            return(TRUE);
    }
    return FALSE;
}


//FUNCTION: Check is the point in the poly underside-shadow
//ARGUMENT: (x,y)    -> point
//          *pl      -> poly
//RETURN:   TRUE for the point is in poly,FALSE else
EXPORT SLONG FNBACK D2_is_point_in_poly(SLONG x,SLONG y,POLY *pl)
{
    SLONG flag,flag1, i;
    SLONG min_x=pl->px[pl->min_x];
    SLONG max_x=pl->px[pl->max_x];
    SLONG min_y=pl->py[pl->min_y];
    SLONG max_y=pl->py[pl->max_y];
    double data;
    
    //Jack, 2002.3.29.
    //Added for minimum edges check.
    if(pl->pn < 3) return FALSE;

    //CAUTION:  add this line to fast check include, and must use this after setup 
    //          the poly range id(such as sx,lx,sy,ly).
    if(!D2_INCLUDE(x,y,min_x,min_y,max_x,max_y)) return(FALSE);
    flag1=0;
    for(i=0;i<pl->pn;i++)
    {
        data = (double)(pl->px[i]-x) * (double)(pl->py[i+1]-y) - (double)(pl->px[i+1]-x) * (double)(pl->py[i]-y);
        flag = (data>0.0) ? 1 : ( (data < 0.0) ? -1 : 0 );
        if(flag)
        {
            if(!flag1) flag1=flag;
            if(flag!=flag1) return(FALSE);
        }
    }
    return(TRUE);
}


//FUNCTION: Check is the segment included by the poly?
//ARGUMENT: (x1,y1)-(x2,y2)   -> segment
//          *pl               -> poly
//RETURN:   1 for the segment included by the poly,0 else
EXPORT SLONG FNBACK D2_is_segment_in_poly(SLONG x1,SLONG y1,SLONG x2,SLONG y2,POLY *pl)
{
    if( (D2_is_point_in_poly(x1,y1,pl)==TRUE) 
        && (D2_is_point_in_poly(x2,y2,pl)==TRUE) )
        return TRUE;
    else
        return FALSE;
}

//FUNCTION: Check is poly1 included by poly2 ?
//ARGUMENT: *pl1  -> poly 1
//          *pl2  -> poly 2
//RETURN:   1 for poly1 is included by poly2, 0 else
EXPORT SLONG FNBACK D2_is_poly_in_poly(POLY *pl1,POLY *pl2)
{
    //Jack, 2002.3.29.
    //Added for minimum edges check.
    if(pl1->pn < 1) return FALSE;

    for(SLONG i=0;i<pl1->pn;i++)
    {
        if(FALSE== D2_is_point_in_poly(pl1->px[i],pl1->py[i],pl2) )
            return(FALSE);
    }
    return(TRUE);
}

//FUNCTION: Check are the two polyes overlapped together?
//ARGUMENT: *pl1  -> poly 1
//          *pl2  -> poly 2
//RETURN:   1 for the two polyes are overlapped,0 else
EXPORT SLONG FNBACK D2_is_overlap_poly_poly(POLY *pl1,POLY *pl2)
{
    SLONG i,j;

    if(   (pl1->px[pl1->min_x] > pl2->px[pl2->max_x])
        ||(pl1->py[pl1->min_y] > pl2->py[pl2->max_y])
        ||(pl1->px[pl1->max_x] < pl2->px[pl2->min_x])
        ||(pl1->py[pl1->max_y] < pl2->py[pl2->min_y]))
        return FALSE;
    
    i=pl1->pn;
    while(--i>=0)
    {
        if(D2_is_point_in_poly(pl1->px[i],pl1->py[i],pl2)==TRUE)
            return(TRUE);
    }
    
    i=pl2->pn;
    while(--i>=0)
    {
        if(D2_is_point_in_poly(pl2->px[i],pl2->py[i],pl1)==TRUE)
            return(TRUE);
    }
    for(i=0;i<pl1->pn;i++)
    {
        for(j=0;j<pl2->pn;j++)
        {
            //  SLONG x,y;
            //  if(INTERSECT_OK == D2_intersect_segment_segment(pl1->px[i],pl1->py[i],pl1->px[i+1],pl1->py[i+1],
            //        pl2->px[j],pl2->py[j],pl2->px[j+1],pl2->py[j+1],&x,&y) )
            //  return TRUE;
            if(D2_is_intersect_segment_segment(
                pl1->px[i],pl1->py[i],pl1->px[i+1],pl1->py[i+1],
                pl2->px[j],pl2->py[j],pl2->px[j+1],pl2->py[j+1])==TRUE)
                return(TRUE);
        }
    }
    return(FALSE);
}

//FUNCTION: Check perspective relation between segment and segment
//ARGUMENT: (x11,y11)-(x12,y12)        -> segment1
//          (x21,y21)-(x22,y22)        -> segment2
//RETURN:   DEEPLY_FIRST for segment1 is deeply(far away from our eyes) than segment2
//          DEEPLY_SECOND for segment2 is deeply than segment1
//          DEEPLY_UNKNOWN for they have no obviousely perspective relations
EXPORT SLONG FNBACK D2_check_deeply_segment_segment(SLONG x11,SLONG y11,SLONG x12,SLONG y12,
                                                    SLONG x21,SLONG y21,SLONG x22,SLONG y22)
{
    SLONG min_x1=min(x11,x12),max_x1=max(x11,x12);
    SLONG min_y1=min(y11,y12),max_y1=max(y11,y12);
    SLONG min_x2=min(x21,x22),max_x2=max(x21,x22);
    SLONG min_y2=min(y21,y22),max_y2=max(y21,y22);
    SLONG xa,xb,xa1,ya1,xa2,ya2,xb1,yb1,xb2,yb2;
    
    if((min_x1>max_x2)||(min_x2>max_x1))    // 1 & 2 no vcross
    {
        // we set the deeply order with compulsion(force) , so some object on the map will 
        // have the same order when map is moving.
		//
        if(min_y1 < min_y2) return (DEEPLY_FIRST);
        if(min_y1 > min_y2) return (DEEPLY_SECOND);
        if(x11+x12 < x21+x22 ) return (DEEPLY_FIRST);
        else return(DEEPLY_SECOND);

		// ZJian, 2002.1.31. we force it by it's min_y last, and now we use the max_y to check it(TIBET).
		//if(max_y1 > max_y2) return (DEEPLY_SECOND);
		//if(max_y2 > max_y1) return (DEEPLY_FIRST);
        //if(x11+x12 < x21+x22 ) return (DEEPLY_FIRST);
        //else return(DEEPLY_SECOND);
    }
    else if(max_y1<=min_y2)                 // vbox 1 is upper than vbox 2
        return(DEEPLY_FIRST);
    else if(max_y2<=min_y1)                 // vbox 2 is upper than vbox 1
        return(DEEPLY_SECOND);
    else
    {
        //ZJian,2000.11.4
        //TODO: we need to optimize the following codes because we just justify and mustn't seek intersect point.
        //TODO: there is a bug in the following codes: 
        //      if seg1 or seg2 is vertical segment, the result is DEEPLY_UNKNOWN(???)
        //      but in normal situation, this will never happen, so ignore it.
        xa=max(min_x1,min_x2);
        xb=min(max_x1,max_x2);
        D2_intersect_line_line(x11,y11,x12,y12,xa,0,xa,100,&xa1,&ya1);
        D2_intersect_line_line(x11,y11,x12,y12,xb,0,xb,100,&xb1,&yb1);
        D2_intersect_line_line(x21,y21,x22,y22,xa,0,xa,100,&xa2,&ya2);
        D2_intersect_line_line(x21,y21,x22,y22,xb,0,xb,100,&xb2,&yb2);
        if(ya1<=ya2 && yb1<=yb2) return(DEEPLY_FIRST);
        if(ya1>=ya2 && yb1>=yb2) return(DEEPLY_SECOND);
        return(DEEPLY_UNKNOWN);
    }
}


//FUNCTION: Compute the distance from point to point
//ARGUMENT: (x1,y1)  -> point1
//          (x2,y2)  -> point2
//RETURN:   the distance from point1 to point2
EXPORT SLONG FNBACK D2_distance_point_point(SLONG x1,SLONG y1,SLONG x2,SLONG y2)
{
    double  dx=x2-x1,dy=y2-y1;
    double  dd=dx*dx+dy*dy;
    return((SLONG)(sqrt((double)dd)+0.5));
}

//FUNCTION: Compute the distance from point to line
//ARGUMENT: (x,y)             -> point
//          (x1,y1)-(x2,y2)   -> line
//RETURN:   the distance from point to line
EXPORT SLONG FNBACK D2_distance_point_line(SLONG x,SLONG y,SLONG x1,SLONG y1,SLONG x2,SLONG y2)
{
    double  nx=x2-x1,ny=y2-y1;
    double  dx=x-x1,dy=y-y1;
    double  ndd=nx*nx+ny*ny;
    double  dd=dx*dx+dy*dy;
    double  dt=nx*dx+ny*dy;
    if(ndd) dd-=(dt*dt)*1.0/ndd;
    return((SLONG)(sqrt((double)dd)+0.5));
}

//FUNCTION: compute nipangle of line and line
//ARGUMENT: (px1,py1)-(px2,py2)  ->line1
//          (qx1,qy1)-(qx2,qy2)  ->line2
//RETURN:   nipangle of line1 and line2(0~18000)
EXPORT SLONG FNBACK D2_nipangle_line_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,
                                          SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2)
{
    double  bx=px2-px1,by=py2-py1;
    double  dx=qx2-qx1,dy=qy2-qy1;
    double  bd=bx*dx+by*dy;
    double  kk=sqrt(bx*bx+by*by)*sqrt(dx*dx+dy*dy);
    return((SLONG)(acos((kk)?bd/kk:-1.0)/acos(-1.0)*18000.0+0.5));
}

//FUNCTION: seek intersect point of two lines
//ARGUMENT: (px1,py1)-(px2,py2)  -> line1 P
//          (qx1,qy1)-(qx2,qy2)  -> line2 Q
//          (*x,*y)              -> intersect point returned
//RETURN:   INTERSECT_OK for have seek intersect point,INTERSECT_NO else
EXPORT SLONG FNBACK D2_intersect_line_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,
                                           SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2,
                                           SLONG *x,SLONG *y)
{
    double  bx=px2-px1,by=py2-py1;  // P(t) = P1 + Bt (0<=t<=1),  B = P2 - P1
    double  dx=qx2-qx1,dy=qy2-qy1;  // Q(s) = Q1 + Ds (0<=s<=1),  D = Q2 - Q1
    double  t;
    
    if(bx*dy-by*dx)
    {
        t=(((qx1*dy-qy1*dx)-(px1*dy-py1*dx))*1.0)/((bx*dy-by*dx)*1.0);
        *x=(SLONG)(px1+bx*t+0.5);
        *y=(SLONG)(py1+by*t+0.5);
        return(INTERSECT_OK);
    }
    return(INTERSECT_NO);
}

//FUNCTION: seek intersect point of two segments
//ARGUMENT: (px1,py1)-(px2,py2)  -> segment1 P
//          (qx1,qy1)-(qx2,qy2)  -> segment2 Q
//          (*x,*y)              -> intersect point returned
//RETURN:   INTERSECT_OK for have seek intersect point,INTERSECT_NO else
EXPORT SLONG FNBACK D2_intersect_segment_segment(SLONG px1,SLONG py1,SLONG px2,SLONG py2,
                                                 SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2,
                                                 SLONG *x,SLONG *y)
{
    double  bx=px2-px1,by=py2-py1;  // P(t) = P1 + Bt (0<=t<=1),  B = P2 - P1
    double  dx=qx2-qx1,dy=qy2-qy1;  // Q(s) = Q1 + Ds (0<=s<=1),  D = Q2 - Q1
    double  t,s;
    
    if(bx*dy-by*dx)
    {
        t=(((qx1*dy-qy1*dx)-(px1*dy-py1*dx))*1.0)/((bx*dy-by*dx)*1.0);
        s=(((px1*by-py1*bx)-(qx1*by-qy1*bx))*1.0)/((dx*by-dy*bx)*1.0);
        if(t>=0 && t<=1.0 && s>=0 && s<=1.0)
        {
            *x=(SLONG)(px1+bx*t+0.5);
            *y=(SLONG)(py1+by*t+0.5);
            return(INTERSECT_OK);
        }
    }
    return(INTERSECT_NO);
}

//FUNCTION: Seek intersect points of segment and poly
//ARGUMENT: (*x1,*y1)   -> segment point 1, && store returned intersect point1
//          (*x2,*y2)   -> segment point 2, && store returned intersect point2
//          *pl         -> poly
//RETURN:   find intersect points number
//          INTERSECT_NO -> no intersect point
//          INTERSECT_ONE -> find 1st intersect point, stored in (*x1,*y1)
//          (INTERSECT_ONE|INTERSECT_TWO) -> find 1st and 2nd intersect points, stored in (*x1,*y1),(*x2,*y2)
EXPORT SLONG FNBACK D2_intersect_segment_poly(SLONG *x1,SLONG *y1,SLONG *x2,SLONG *y2,POLY *pl)
{
    SLONG a1=min(*x1,*x2),b1=min(*y1,*y2);
    SLONG a2=max(*x1,*x2),b2=max(*y1,*y2);
    SLONG c1=pl->px[pl->min_x];
    SLONG d1=pl->py[pl->min_y];
    SLONG c2=pl->px[pl->max_x];
    SLONG d2=pl->py[pl->max_y];
    SLONG pn=pl->pn,find=0,i;
    SLONG px[MAX_POLY_NUM+1],py[MAX_POLY_NUM+1];
    
    if((a1>c2)||(a2<c1)||(b1>d2)||(b2<d1))
        return INTERSECT_NO;
    if(D2_is_intersect_line_poly(*x1,*y1,*x2,*y2,pl)==FALSE)
        return INTERSECT_NO;
    for(i=0;i<pn;i++)
    {
        if(D2_intersect_segment_segment(*x1,*y1,*x2,*y2,pl->px[i],pl->py[i],pl->px[i+1],
            pl->py[i+1],&px[find],&py[find]))
            find++;
        if(find>MAX_POLY_NUM)
            break;
    }
    if(find<1)
        return INTERSECT_NO;
    *x1=px[0];
    *y1=py[0];
    for(i=1;i<find;i++)
    {
        if(px[i]!=px[0] || py[i]!=py[0])
        {
            *x2=px[i];
            *y2=py[i];
            return (INTERSECT_ONE|INTERSECT_TWO);
        }
    }
    return INTERSECT_ONE;
}

//FUNCTION: Seek chip point index from point to poly gon, so the rays from the point to 
//          the found two points will include the poly
//ARGUMENT: (x,y,z)     -> point
//          *pl         -> poly
//          (*n1,*n2)   -> poly gon point index returned
//RETURN:   0 always
EXPORT SLONG FNBACK D2_chip_point_poly(SLONG x,SLONG y,POLY *pl,SLONG *n1,SLONG *n2)
{
    SLONG   pn=pl->pn,max_alpha=0,alpha;
    SLONG   i,j;
    
    *n1=*n2=0;
    for(i=0;i<pn;i++)
    {
        for(j=i+1;j<pn;j++)
        {
            alpha=D2_nipangle_line_line(x,y,pl->px[i],pl->py[i],x,y,pl->px[j],pl->py[j]);
            if(alpha>max_alpha)
            {
                max_alpha=alpha;
                *n1=i;
                *n2=j;
            }
        }
    }
    return(0);
}

//FUNCTION: fill the poly by some color
//ARGUMENT: pl    -> the poly to be filled
//          c     -> fill color
//          *bmp  -> bitmap
//RETURN:   void
EXPORT void FNBACK D2_fill_poly_ver1(POLY *pl,PIXEL c,BMP *bmp)
{
    SLONG c1=pl->px[pl->min_x];
    SLONG c2=pl->px[pl->max_x];
    SLONG x,x1,y1,x2,y2,find;
    
    for(x=c1;x<=c2;x++)
    {
        x1=x,y1=c1,x2=x,y2=c2;
        find=D2_intersect_segment_poly(&x1,&y1,&x2,&y2,pl);
        if(find==3)
        {
            put_line(x1,y1,x2,y2,c,bmp);
        }
    }
}


EXPORT  void    FNBACK  D2_fill_poly(SLONG relation_x, SLONG relation_y, POLY *poly,PIXEL color,BMP *bmp)
{   
#define INDEX_FORWORD(index)    index = (index + 1) % num;
#define INDEX_BACKWORD(index)   index = (index - 1 + num) % num;
    int lx1,ly1,lx2,ly2,rx1,ry1,rx2,ry2,ldx,ldy,rdx,rdy;
    int minY,maxY,lindex,rindex,lxinc,rxinc,lxadd,rxadd,lxsgn,rxsgn,lxsum,rxsum;
    int x1,x2;
    long width=bmp->w-1,height=bmp->h-1;
    int num;
    static POLY spl;
    POLY *pl;

    memcpy(&spl,poly,sizeof(POLY));
    pl = &spl;
    for(int i=0;i<= pl->pn;i++)
    {
        pl->px[i] -= relation_x;
        pl->py[i] -= relation_y;
    }

    num = pl->pn;
    if(num < 3)
        return;
    lindex = pl->min_y;
    minY = pl->py[lindex];
    maxY = pl->py[pl->max_y];
    if(minY == maxY)
        return;
    rindex = lindex;
    while(pl->py[lindex] == minY)
        INDEX_FORWORD(lindex);
    INDEX_BACKWORD(lindex);
    while(pl->py[rindex] == minY)
        INDEX_BACKWORD(rindex);

    INDEX_FORWORD(rindex);
    lx1 = pl->px[lindex];
    ly1 = pl->py[lindex];
    INDEX_FORWORD(lindex);
    lx2 = pl->px[lindex];
    ly2 = pl->py[lindex];

    ldx = lx2 - lx1;
    ldy = ly2 - ly1;
    lxsgn = (ldx >= 0)?1:-1;

    lxadd=0;
    lxinc=0;
    if(ldy>0)
    {   
        lxadd = ldx/ldy;
        lxinc = (ldx%ldy)*lxsgn;
    }

    rx1 = pl->px[rindex];
    ry1 = pl->py[rindex];
    INDEX_BACKWORD(rindex);
    rx2 = pl->px[rindex];
    ry2 = pl->py[rindex];

    rdx = rx2 - rx1;
    rdy = ry2 - ry1;
    rxsgn = (rdx >= 0)?1:-1;

    rxadd=0;
    rxinc=0;
    if(rdy>0)
    {   
        rxadd = rdx/rdy;
        rxinc = (rdx%rdy)*rxsgn;
    }
    lxsum = 0;
    rxsum = 0;
    minY++;
    while(minY <= maxY)
    {   
        lxsum+=lxinc;
        lx1+=lxadd;
        if(lxsum >= ldy)
        {   
            lx1+=lxsgn;
            lxsum -= ldy;
        }
        rxsum+=rxinc;
        rx1+=rxadd;
        if(rxsum >= rdy)
        {   
            rx1+=rxsgn;
            rxsum -= rdy;
        }
        x1 = lx1,x2 = rx1 - 1;
        if(minY>=0&&minY<=height&&x1<=width&&x2>=0&&x2>=x1)
        {   
            if(x1<0)
                x1=0;
            if(x2>width)
                x2=width;
            bmp->line[minY][x1] = color;
            while(x1<x2)
            {   
                bmp->line[minY][1+x1] = color;
                x1++;
            }
        }
        if(minY == ly2)
        {   
            do {    
                lx1 = pl->px[lindex];
                ly1 = pl->py[lindex];
                INDEX_FORWORD(lindex);
                lx2 = pl->px[lindex];
                ly2 = pl->py[lindex];
            }while(ly2 == ly1);

            ldx = lx2 - lx1;
            ldy = ly2 - ly1;
            lxsgn = (ldx >= 0)?1:-1;
            if(ldy>0)
            {   
                lxadd = ldx/ldy;
                lxinc = (ldx%ldy)*lxsgn;
            }
        }
        if(minY == ry2)
        {   
            do {
                rx1 = pl->px[rindex];
                ry1 = pl->py[rindex];
                INDEX_BACKWORD(rindex);
                rx2 = pl->px[rindex];
                ry2 = pl->py[rindex];
            }while(ry2 == ry1);
            rdx = rx2 - rx1;
            rdy = ry2 - ry1;
            rxsgn = (rdx >= 0)?1:-1;
            if(rdy>0)//???;
            {   
                rxadd = rdx/rdy;
                rxinc = (rdx%rdy)*rxsgn;
            }
        }
        minY++;
    }
}


//FUNCTION: find barycenter point of poly
//ARGUMENT: pl      -> the poly to be accessed
//RETURN:   void
EXPORT void FNBACK D2_barycenter_poly(POLY *pl)
{
    SLONG cx=0,cy=0;
    if(pl->pn)
    {
        for(SLONG i=0;i<pl->pn;i++)
        {
            cx+=pl->px[i];
            cy+=pl->py[i];
        }
        pl->bx=cx/pl->pn;
        pl->by=cy/pl->pn;
    }
}

//FUNCTION: adjust position to diamond box position
//AUGUMENT: (*x,*y)     -> point to be adjusted, and store the adjusted point position
//RETURN:   void
EXPORT void FNBACK D2_adjust_diamond_position(SLONG *x,SLONG *y,SLONG *cell_x,SLONG *cell_y,
                                              SLONG diamond_width,SLONG diamond_height)
{
    static  SLONG   cx,cy;
    static  SLONG   dcx[5]={0,-1, 0,-1, 0};
    static  SLONG   dcy[5]={0,-1,-1, 1, 1};
    static  SLONG   fdx[5]={0,-1, 1,-1, 1};
    static  SLONG   fdy[5]={0,-1,-1, 1, 1};
    SLONG tx,ty,xx,yy,find,i;
    SLONG map_x,map_y,diamond_x,diamond_y;
    POLY  op;
    
    if(diamond_width<=0 || diamond_height<=0)
        return;
    map_x=*x;
    map_y=*y;
    cx=(map_x+diamond_width/2)/diamond_width;
    cy=(map_y+diamond_height/2)/diamond_height;
    cy=cy*2;
    tx=cx*diamond_width;
    ty=cy*(diamond_height/2);
    find=0;
    op.pn=4;
    op.min_x=0;
    op.max_x=2;
    op.min_y=3;
    op.max_y=1;
    for(i=0;i<5;i++)
    {
        xx=tx+fdx[i]*diamond_width/2;
        yy=ty+fdy[i]*diamond_height/2;
        op.px[0]=xx-diamond_width/2;
        op.py[0]=yy;
        op.px[1]=xx;
        op.py[1]=yy+diamond_height/2;
        op.px[2]=xx+diamond_width/2;
        op.py[2]=yy;
        op.px[3]=xx;
        op.py[3]=yy-diamond_height/2;
        op.px[4]=op.px[0];
        op.py[4]=op.py[0];
        if(D2_is_point_in_poly(map_x,map_y,&op)==TRUE)
        {
            find=i;
            break;
        }
    }
    diamond_x = cx+dcx[find];
    diamond_y = cy+dcy[find];
    *cell_x=diamond_x;
    *cell_y=diamond_y;
    *x = diamond_x*diamond_width;
    if(diamond_y%2)
        *x+=diamond_width/2;
    *y=diamond_y*(diamond_height/2)-diamond_height/2;
}

//==================================================================================
// 3 Demension functions
//==================================================================================
//FUNCTION: Compute the distance from point to point
//ARGUMENT: (x1,y1,z1)  -> point 1
//          (x2,y2,z2)  -> point 2
//RETURN:   distance between the two points
EXPORT SLONG FNBACK D3_distance_point_point(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2)
{
    double  dx=x2-x1,dy=y2-y1,dz=z2-z1;
    double  dd=dx*dx+dy*dy+dz*dz;
    return((SLONG)(sqrt(dd)+0.5));
}


//FUNCTION: Compute the distance from point to line
//ARGEMENT: (x,y,z)                 -> point
//          (x1,y1,z1)-(x2,y2,z2)   -> line
//RETURN:   distance from point to line
EXPORT SLONG FNBACK D3_distance_point_line(SLONG x,SLONG y,SLONG z,SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2)
{
    double  nx = x2-x1, ny = y2-y1, nz = z2-z1;
    double  dx = x-x1, dy = y-y1,dz = z-z1;
    double  ndd = nx * nx + ny * ny + nz * nz;
    double  dd = dx * dx + dy * dy + dz * dz;
    double  dt = nx * dx + ny * dy + nz * dz;
    
    if(ndd) dd-=(dt*dt)*1.0/ndd;
    return((SLONG)(sqrt((double)dd)+0.5));
}

//FUNCTION: Compute distance from point to surface
//ARGUMENT: (x,y,z)           -> point
//          (a,b,c,d)         -> surface aX+bY+cZ+d=0
//RETURN:   distance from point to line
EXPORT SLONG FNBACK D3_distance_point_surface(SLONG x,SLONG y,SLONG z,SLONG a,SLONG b,SLONG c,SLONG d)
{
    double sdd=a*a+b*b+c*c;        // surface : aX+bY+cZ+d=0
    double dd=(sdd)?(abs(a*x+b*y+c*z+d)*1.0)/(sqrt(sdd)):sqrt(x*x+y*y+z*z);
    return((SLONG)(dd+0.5));
}

//FUNCTION: Compute distance between two lines
//ARGUMENT: (x11,y11,z11)-(x12,y12,z12)   -> line1
//          (x21,y21,z21)-(x22,y22,z22) -> line2
//RETURN:   distance from line to line
EXPORT SLONG FNBACK D3_distance_line_line(SLONG x11,SLONG y11,SLONG z11,SLONG x12,SLONG y12,SLONG z12,
                                          SLONG x21,SLONG y21,SLONG z21,SLONG x22,SLONG y22,SLONG z22)
{
    SLONG   flag=0;
    SLONG   dist=0;
    double  bx=x12-x11,by=y12-y11,bz=z12-z11;   // P(t) = A + B * t ( 0<=t<=1)
    double  dx=x22-x21,dy=y22-y21,dz=z22-z21;   // Q(s) = C + D * s ( 0<=s<=1)
    double  bb=bx*bx+by*by+bz*bz;
    double  dd=dx*dx+dy*dy+dz*dz;
    double  px,py,pz,ux,uy,uz;
    double  uu,ud;
    double  pdd,dt,rdd;
    
    if(!bb) flag|=1;
    if(!dd) flag|=2;
    switch(flag)
    {
    case 0:                       // P1!=P2,Q1!=Q2
        ux=by*dz-bz*dy;            // U = B X D
        uy=bz*dx-bx*dz;
        uz=bx*dy-by*dx;
        uu=ux*ux+uy*uy+uz*uz;
        if(!uu)                    // P/Q  or P=Q
        {
            px=x11-x21;
            py=y11-y21;
            pz=z11-z21;
            pdd=px*px+py*py+pz*pz;
            dt=dx*px+dy*py+dz*pz;
            rdd=pdd-((dt*dt)*1.0)/dd;
            dist=(SLONG)(sqrt(rdd)+0.5);
        }
        else
        {
            ud=-(ux*x21+uy*y21+uz*z21);       // -(U * C)
            rdd=((ux*x11+uy*y11+uz*z11+ud)*1.0)/sqrt(uu);
            dist=(SLONG)abs((SLONG)(rdd+0.5));
        }
        break;
    case 1:                      // P1=P2,Q1!=Q2
        px=x11-x21;
        py=y11-y21;
        pz=z11-z21;
        pdd=px*px+py*py+pz*pz;
        dt=dx*px+dy*py+dz*pz;
        rdd=pdd-((dt*dt)*1.0)/dd;
        dist=(SLONG)(sqrt(rdd)+0.5);
        break;
    case 2:                      // P1!=P2,Q1=Q2
        px=x21-x11;
        py=y21-y11;
        pz=z21-z11;
        pdd=px*px+py*py+pz*pz;
        dt=bx*px+by*py+bz*pz;
        rdd=pdd-((dt*dt)*1.0)/bb;
        dist=(SLONG)(sqrt(rdd)+0.5);
        break;
    case 3:                      // P1=P2,Q1=Q2
        px=x11-x21;
        py=y11-y21;
        pz=z11-z21;
        pdd=px*px+py*py+pz*pz;
        rdd=pdd;
        dist=(SLONG)(sqrt(rdd)+0.5);
        break;
    }
    return(dist);
}

//FUNCTION: Compute nipangle of two lines
//ARGUMENT: (px1,py1,pz1)-(px2,py2,pz2)   -> line1 P
//          (qx1,qy1,qz1)-(qx2,qy2,qz2)   -> line2 Q
//RETURN:   nipangle of the two lines, from 0 to 18000
EXPORT SLONG FNBACK D3_nipangle_line_line(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,
                                          SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2)
{
    double  bx=px2-px1,by=py2-py1,bz=pz2-pz1;
    double  dx=qx2-qx1,dy=qy2-qy1,dz=qz2-qz1;
    double  bd=bx*dx+by*dy+bz*dz;
    double  kk=sqrt(bx*bx+by*by+bz*bz)*sqrt(dx*dx+dy*dy+dz*dz);
    return((SLONG)(acos((kk)?bd/kk:-1.0)/acos(-1.0)*18000.0+0.5));
}

//FUNCTION: Compute normal metrix for the surface
//ARGUMENT: (x1,y1,z1)     -> surface point1
//          (x2,y2,z2)     -> surface point2
//          (x3,y3,z3)     -> surface point3
//          (*a,*b,*c,*d)  -> surface normal metrix (aX+bY+cZ+d=0)
//RETURN:   return 0 if ok, -1 else
EXPORT SLONG FNBACK D3_normal_surface(SLONG x1,SLONG y1,SLONG z1,
                                      SLONG x2,SLONG y2,SLONG z2,
                                      SLONG x3,SLONG y3,SLONG z3,
                                      SLONG *a,SLONG *b,SLONG *c,SLONG *d)
{
    //Jack, 2002.5.14.
    //TODO: May be use double ???
    SLONG dx=x2-x1,dy=y2-y1,dz=z2-z1;
    SLONG fx=x3-x1,fy=y3-y1,fz=z3-z1;
    SLONG nx=dy*fz-fy*dz,ny=dz*fx-fz*dx,nz=dx*fy-fx*dy;   
    SLONG nd=-(nx*x1+ny*y1+nz*z1);   // ZJian may be use SHYPER here
    SLONG r1=compute_greatest_common_divisor(nx,ny);
    SLONG r2=compute_greatest_common_divisor(nz,nd);
    SLONG r=compute_greatest_common_divisor(r1,r2);
    if(r)
    {
        nx/=r,ny/=r,nz/=r,nd/=r;
    }
    if(nx*nx+ny*ny+nz*nz)
    {
        *a=nx,*b=ny,*c=nz,*d=nd;
        return(TTN_OK);
    }
    else
    {
        *a=0,*b=0,*c=0,*d=0;
        return(TTN_NOT_OK);
    }
}

//FUNCTION: seek intersect point of line and surface
//ARGUMENT: (x1,y1,z1)-(x2,y2,z2)   -> line
//          (a,b,c,d)                  -> surface aX+bY+cZ+d=0
//             (*x,*y,*z)                  -> intersect point returned
//RETURN:   INTERSECT_OK if there is an intersect point,INTERSECT_NO else.
EXPORT SLONG FNBACK D3_intersect_line_surface(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,
                                              SLONG a,SLONG b,SLONG c,SLONG d,
                                              SLONG *x,SLONG *y,SLONG *z)
{
    double  bx=x2-x1,by=y2-y1,bz=z2-z1;  // LINE: L(t) = P1 + B*t
    double  f=a*x1+b*y1+c*z1+d;  // SURF: aX + bY + cZ + d = 0
    double  g=a*bx+b*by+c*bz;
    double  t=(g)?-f/g:0;
    
    *x=(SLONG)(x1+bx*t+0.5);
    *y=(SLONG)(y1+by*t+0.5);
    *z=(SLONG)(z1+bz*t+0.5);
    return(INTERSECT_OK);
}

//FUNCTION: seek intersect point of line and line
//ARGUMENT: (px1,py1,pz1)-(px2,py2,pz2)     -> line1
//             (qx1,qy1,qz1)-(qx2,qy2,qz2)      -> line2
//             (*x,*y,*z)                             -> intersect point
//RETURN:   0 if found intersect point,else -1.
EXPORT SLONG FNBACK D3_intersect_line_line(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,
                                           SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2,
                                           SLONG *x,SLONG *y,SLONG *z)
{
    double  bx=px2-px1,by=py2-py1,bz=pz2-pz1;   // P(t) = P1 + Bt (0<=t<=1),  B = P2 - P1
    double  dx=qx2-qx1,dy=qy2-qy1,dz=qz2-qz1;   // Q(s) = Q1 + Ds (0<=s<=1),  D = Q2 - Q1
    double  t,s;
    
    if(bx*dy-by*dx)
    {
        t=(((qx1*dy-qy1*dx)-(px1*dy-py1*dx))*1.0)/((bx*dy-by*dx)*1.0);
        s=(((px1*by-py1*bx)-(qx1*by-qy1*bx))*1.0)/((dx*by-dy*bx)*1.0);
        if(pz1+bz*t==qz1+dz*s)
        {
            *x=(SLONG)(px1+bx*t+0.5);
            *y=(SLONG)(py1+by*t+0.5);
            *z=(SLONG)(pz1+bz*t+0.5);
            return(INTERSECT_OK);
        }
    }
    return(INTERSECT_NO);
}

//FUNCTION: seek intersect point of segment and segment
//ARGUMENT: (px1,py1,pz1)-(px2,py2,pz2) -> segment1 P
//          (qx1,qy1,qz1)-(qx2,qy2,qz2) -> segment2 Q
//          (*x,*y,*z)                      -> intersect point
//RETURN:   INTERSECT_OK for have intersect point,INTERSECT_NO else.
EXPORT SLONG FNBACK D3_intersect_segment_segment(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,
                                                 SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2,
                                                 SLONG *x,SLONG *y,SLONG *z)
{
    double  bx=px2-px1,by=py2-py1,bz=pz2-pz1;   // P(t) = P1 + Bt (0<=t<=1),  B = P2 - P1
    double  dx=qx2-qx1,dy=qy2-qy1,dz=qz2-qz1;   // Q(s) = Q1 + Ds (0<=s<=1),  D = Q2 - Q1
    double  t,s;
    
    if(bx*dy-by*dx)
    {
        t=(((qx1*dy-qy1*dx)-(px1*dy-py1*dx))*1.0)/((bx*dy-by*dx)*1.0);
        s=(((px1*by-py1*bx)-(qx1*by-qy1*bx))*1.0)/((dx*by-dy*bx)*1.0);
        if(pz1+bz*t==qz1+dz*s && t>=0 && t<=1 && s>=0 && s<=1)
        {
            *x=(SLONG)(px1+bx*t+0.5);
            *y=(SLONG)(py1+by*t+0.5);
            *z=(SLONG)(pz1+bz*t+0.5);
            return(INTERSECT_OK);
        }
    }
    return(INTERSECT_NO);
}

//FUNCTION: seek droop point from point to line
//AUGUMENT: (px,py,pz)              -> point P
//          (x1,y1,z1,x2,y2,z2) -> line Q
//          (*x,*y,*z)              ->  droop point returned
//RETURN:   0 always
EXPORT SLONG FNBACK D3_droop_point_line(SLONG px,SLONG py,SLONG pz,
                                        SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,
                                        SLONG *x,SLONG *y,SLONG *z)
{
    double  bx=x2-x1,by=y2-y1,bz=z2-z1;  // Q(t) = Q1 + Bt (0<=t<=1),  B = Q2 - Q1
    double  bd=-(bx*px+by*py+bz*pz); // SURFACE: bx*X + by*Y + bz*Z + bd = 0
    double  f=bx*x1+by*y1+bz*z1+bd;
    double  g=bx*bx+by*by+bz*bz;
    double  t=(g)?-f/g:0;
    
    *x=(SLONG)(x1+bx*t+0.5);
    *y=(SLONG)(y1+by*t+0.5);
    *z=(SLONG)(z1+bz*t+0.5);
    return(0);
}

//FUNCTION: seek droop point of point to surface
//ARGEMENT: (px,py,pz)      -> point 
//          (a,b,c,d)       -> surface aX+bY+cZ+d=0
//          (*x,*y,*z)      -> droop point returned
//RETURN:   0 always
EXPORT SLONG FNBACK D3_droop_point_surface(SLONG px,SLONG py,SLONG pz,
                                           SLONG a,SLONG b,SLONG c,SLONG d,
                                           SLONG *x,SLONG *y,SLONG *z)
{
    double f=a*px+b*py+c*pz+d;     // LINE : P(t) = P + (-N)t
    double g=a*a+b*b+c*c;          // N = normal(SURFACE)
    double t=(g)?f/g:0;
    
    *x=(SLONG)(px+a*t+0.5);
    *y=(SLONG)(py+b*t+0.5);
    *z=(SLONG)(pz+c*t+0.5);
    return(0);
}


//FUNCTION: Make rectangle poly(4 gons)
//ARGUMENT: *pl      -> poly to make
//          (cx,cy)  -> poly center point(cx,cy,0)
//          (a,b)    -> poly x/y half-axis
//RETURN:   void
EXPORT void FNBACK D3_make_rectangle_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b)
{
    SLONG s_table[4+1]={ -1,-1, 1, 1,-1 };
    SLONG c_table[4+1]={ -1, 1, 1,-1,-1 };
    
    pl->pn=4;
    pl->type=POLY_TYPE_HORIZONTAL;
    pl->min_x=0;   // left x
    pl->max_y=1;   // bottom y
    pl->max_x=2;   // right x
    pl->min_y=3;   // top y
    pl->min_z=0;   // high z
    pl->max_z=0;   // low z
    pl->bx=cx;  // barycenter x
    pl->by=cy;  // barycenter y
    pl->bu=0;   // barycenter z0
    pl->bz=0;   // barycenter z1
    for(SLONG i=0;i<5;i++)
    {
        pl->px[i]=(SLONG)(cx+a*s_table[i]); // x
        pl->py[i]=(SLONG)(cy+b*c_table[i]); // y
        pl->pz[i]=0;   // z1
        pl->pu[i]=0;   // z0
    }
    // set the normal vetrix of the cover & under surface 
    pl->na[UNDER_SURFACE] = pl->na[COVER_SURFACE] = 0;
    pl->nb[UNDER_SURFACE] = pl->nb[COVER_SURFACE] = 0;
    pl->nc[UNDER_SURFACE] = pl->nc[COVER_SURFACE] = 1;
}


//FUNCTION: Make diamond poly(4 gons)
//ARGUMENT: *pl      -> poly to make
//          (cx,cy)  -> poly center point(cx,cy,0)
//          (a,b)    -> poly x/y half-axis
//RETURN:   void
EXPORT void FNBACK D3_make_diamond_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b)
{
    SLONG s_table[4+1]={ -1, 0, 1, 0,-1 };
    SLONG c_table[4+1]={  0, 1, 0,-1, 0 };
    
    pl->pn=4;
    pl->type=POLY_TYPE_HORIZONTAL;
    pl->min_x=0;   // left x
    pl->max_y=1;   // bottom y
    pl->max_x=2;   // right x
    pl->min_y=3;   // top y
    pl->min_z=0;   // high z
    pl->max_z=0;   // low z
    pl->bx=cx;  // barycenter x
    pl->by=cy;  // barycenter y
    pl->bu=0;   // barycenter z0
    pl->bz=0;   // barycenter z1
    for(SLONG i=0;i<5;i++)
    {
        pl->px[i]=(SLONG)(cx+a*s_table[i]); // x
        pl->py[i]=(SLONG)(cy+b*c_table[i]); // y
        pl->pz[i]=0;   // z1
        pl->pu[i]=0;   // z0
    }
    // set the normal vetrix of the cover & under surface 
    pl->na[UNDER_SURFACE] = pl->na[COVER_SURFACE] = 0;
    pl->nb[UNDER_SURFACE] = pl->nb[COVER_SURFACE] = 0;
    pl->nc[UNDER_SURFACE] = pl->nc[COVER_SURFACE] = 1;
}

//FUNCTION: Make ellips poly(12 gons)
//ARGUMENT: *pl      -> poly to make
//          (cx,cy)  -> poly center point(cx,cy,0)
//          (a,b)    -> poly x/y half-axis
//RETURN:   void
EXPORT void FNBACK D3_make_ellipse_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b)
{
    SLONG i;
    double sita=0.0;
    
    pl->pn=12;
    pl->type=POLY_TYPE_HORIZONTAL;
    pl->min_x=0;   // left x
    pl->max_y=3;   // bottom y
    pl->max_x=6;   // right x
    pl->min_y=9;   // top y
    pl->min_z=0;   // high z
    pl->max_z=0;   // low z
    pl->reserved1=0;     // reserved area
    pl->reserved2=0;     // reserved area
    pl->reserved3=0;     // reserved area
    pl->bx=cx;  // barycenter x
    pl->by=cy;  // barycenter y
    pl->bu=0;   // barycenter z0
    pl->bz=0;   // barycenter z1
    for(i=0;i<13;i++)
    {
        sita=(6+i)*3.1415927/6.0;
        pl->px[i]=(SLONG)(cx+a*cos(sita)+0.5);
        pl->py[i]=(SLONG)(cy+(-b*sin(sita))+0.5); // monitorY = - mathY
        pl->pz[i]=0;
        pl->pu[i]=0;
        pl->pa[i]=0;
    }
    // set the normal vetrix of the cover & under surface 
    pl->na[UNDER_SURFACE] = pl->na[COVER_SURFACE] = 0;
    pl->nb[UNDER_SURFACE] = pl->nb[COVER_SURFACE] = 0;
    pl->nc[UNDER_SURFACE] = pl->nc[COVER_SURFACE] = 1;
}


//FUNCTION: Neaten poly data (neaten poly gon index)
//          0 for the leftest gon point
//ARGUMENT: *pl   -> poly to be neatened
//RETURN:   void
EXPORT void FNBACK D3_neaten_poly_gon(POLY *pl)
{
    ULONG lindex[MAX_POLY_NUM+1],did_flag[MAX_POLY_NUM+1],lmax=0;
    ULONG i,pn=pl->pn;
    ULONG start=0,next=1;
    UCHR  sx=0,lx=0,sy=0,ly=0,sz=0,lz=0;
    SLONG min_angle,angle;
    SLONG x,y;
    POLY spl;
    
    // backup poly datas
    memcpy(&spl,pl,sizeof(POLY));
    
    // find leftest(start) gon point
    for(i=0;i<pn;i++)
    {
        did_flag[i]=0;
        if((pl->px[i]<pl->px[start])
            ||(pl->px[i]==pl->px[start] && pl->py[i]>pl->py[start]))
            start=i;
    }
    
    // find turn of gon point
    lindex[lmax++]=start;
    did_flag[start]=1;
    x=pl->px[start];
    y=pl->py[start]-100;
    while(lmax<pn)
    {
        min_angle=99999;
        for(i=0;i<pn;i++)
        {
            //Jack, 2002.3.29. 
            //changed for auto delete inner point.
            //if(!did_flag[i])
            if( (!did_flag[i]) || (lmax>=3 && i==lindex[0]) )
            {
                angle=D2_nipangle_line_line(x,y,pl->px[start],pl->py[start],
                    pl->px[start],pl->py[start],pl->px[i],pl->py[i]);
                if(angle<min_angle)
                {
                    min_angle=angle;
                    next=i;
                }
            }
        }
        //Jack, 2002.3.29.
        //changed for auto delete inner point.
        /*
        lindex[lmax++]=next;
        did_flag[next]=1;
        x=pl->px[start];
        y=pl->py[start];
        start=next;
        */
        if(next == lindex[0])
        {
            break;
        }
        else
        {
            lindex[lmax++]=next;
            did_flag[next]=1;
            x=pl->px[start];
            y=pl->py[start];
            start=next;
        }
    }
    lindex[lmax]=lindex[0];
    
    //Jack, 2002.3.29.
    //added for auto delete inner point.
    pl->pn = lmax;
    pn = pl->pn;

    // sort poly point by found turn(set pn' same as 0')
    for(i=0;i<=pn;i++)
    {
        start=lindex[i];
        pl->px[i]=spl.px[start];
        pl->py[i]=spl.py[start];
        pl->pz[i]=spl.pz[start];
        pl->pu[i]=spl.pu[start];
        pl->pa[i]=spl.pa[start];
    }
    
    // remake poly special data
    for(i=0;i<pn;i++)
    {
        if(pl->px[i]>pl->px[lx])
            lx=(UCHR)i;
        if(pl->py[i]<pl->py[sy])
            sy=(UCHR)i;
        if(pl->py[i]>pl->py[ly])
            ly=(UCHR)i;
        if(pl->pu[i]<pl->pu[sz])
            sz=(UCHR)i;
        if(pl->pz[i]>pl->pz[lz])
            lz=(UCHR)i;
    }
    pl->min_x=sx;
    pl->max_x=lx;
    pl->min_y=sy;
    pl->max_y=ly;
    pl->min_z=sz;
    pl->max_z=lz;
}

//FUNCTION: set poly height
//ARGUMENT: *pl      -> poly to be set
//          z1,z2    -> low(under) height & high(top) height
//RETURN:   void
EXPORT void FNBACK D3_set_poly_height(POLY *pl,SLONG under_z,SLONG top_z)
{
    SLONG i;
    SLONG add_height;

    add_height=0;
    for(i=0;i<=pl->pn;i++)
    {
        pl->pu[i] = under_z;
        pl->pz[i] = top_z;
    }
    pl->bu=under_z;
    pl->bz=top_z;
    // Normal: (0,0,1)
    pl->na[UNDER_SURFACE] = pl->na[COVER_SURFACE] = 0;
    pl->nb[UNDER_SURFACE] = pl->nb[COVER_SURFACE] = 0;
    pl->nc[UNDER_SURFACE] = pl->nc[COVER_SURFACE] = 1;
    pl->nd[UNDER_SURFACE] = -under_z;   // SURFACE: 0*X + 0*Y + 1*Z + (-under_z) = 0
    pl->nd[COVER_SURFACE] = -top_z;     // SURFACE: 0*X + 0*Y + 1*Z + (-top_z)  = 0
}

//FUNCTION: Draw 3D point on bmp
//ARGUMENT: (x,y,z)  -> point to draw
//          *bmp     -> target bitmap
//RETURN:   void
EXPORT void FNBACK D3_draw_point(SLONG x,SLONG y,SLONG z,BMP *bmp)
{
    //range_put_pixel(x,y-z,SYSTEM_WHITE,bmp);
    put_line(x,y-z,x,y,SYSTEM_DARK2,bmp);
    put_cross(x,y,SYSTEM_DARK2,bmp);
    put_cross(x,y-z,SYSTEM_WHITE,bmp);
    range_put_pixel(x,y-z,SYSTEM_YELLOW,bmp);
}

//FUNCTION: Draw 3D line on bitmap
//ARGUMENT: (x1,y1,z1,x2,y2,z2)  -> line to draw
//          *bmp                 -> target bitmap
//RETURN:   void
EXPORT void FNBACK D3_draw_line(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,BMP *bit_map)
{
    //put_line(x1,y1-z1,x2,y2-z2,SYSTEM_WHITE,bmp);
    D3_draw_point(x1,y1,z1,bit_map);
    D3_draw_point(x2,y2,z2,bit_map);
    put_line(x1,y1,x2,y2,SYSTEM_DARK2,bit_map);
    put_line(x1,y1-z1,x2,y2-z2,SYSTEM_YELLOW,bit_map);
}

//FUNCTION: Draw poly on bitmap
//ARGUMENT: (px,py,pz)     -> barycenter position
//          flag           -> draw flag
//          *pl            -> poly to draw
//          *bmp           -> target bitmap
//RETURN:   void
EXPORT void FNBACK D3_draw_poly(SLONG px,SLONG py,SLONG pz,SLONG flag,POLY *pl,BMP *bit_map)
{
    SLONG i,pn,ll,rr;
    PIXEL pcolor;
    SLONG view_x,view_y;
    
    if(!(pl && bit_map))  return;
    view_x=px;
    view_y=(py-pz);
    pcolor=SYSTEM_YELLOW;
    if(flag & PDP_COLOR)
    {
        switch(flag & PDP_COLOR)
        {
        case PDP_RED:
            pcolor = SYSTEM_RED;
            break;
        case PDP_GREEN:
            pcolor = SYSTEM_GREEN;
            break;
        case PDP_BLUE:
            pcolor = SYSTEM_BLUE;
            break;
        case PDP_YELLOW:
            pcolor = SYSTEM_YELLOW;
            break;
        case PDP_CYAN:
            pcolor = SYSTEM_CYAN;
        case PDP_PINK:
            pcolor = SYSTEM_PINK;
            break;
        default:
            break;
        }
    }
    else
    {
        if(flag & PDP_VALID)
            pcolor=SYSTEM_YELLOW;
        else
            pcolor=SYSTEM_DARK1;
    }
    pn=pl->pn;
    ll=pl->min_x;
    rr=pl->max_x;
    if(pn >= 1)
    {
        if(flag & PDP_ZHEIGHT)   // the poly height in the global world
        {
            put_line(px,py,px,py-pz,SYSTEM_CYAN,bit_map);
            if(flag & PDP_CROSS)
            {
                put_cross(px,py,SYSTEM_WHITE,bit_map);
                put_cross(px,py-pz,SYSTEM_WHITE,bit_map);
            }
        }
        if(flag & PDP_PROJECTION)   // projection
        {
            for(i=0;i<pn;i++)
            {
                if(i<rr)
                {
                    put_line(px+pl->px[i],py+pl->py[i],
                        px+pl->px[i+1],py+pl->py[i+1],
                        SYSTEM_BLUE,bit_map);
                }
                else
                {
                    put_dash_line(px+pl->px[i],py+pl->py[i],
                        px+pl->px[i+1],py+pl->py[i+1],
                        SYSTEM_BLUE,2,bit_map);
                }
            }
        }
        if(flag & PDP_BASE)  // under base lines
        {
            for(i=0;i<pn;i++)
            {
                if(i<rr)
                    put_line(px+pl->px[i],py-pz+(pl->py[i]-pl->pu[i]),px+pl->px[i+1],py-pz+(pl->py[i+1]-pl->pu[i+1]),
                    pcolor,bit_map);
                else
                    put_dash_line(px+pl->px[i],py-pz+(pl->py[i]-pl->pu[i]),px+pl->px[i+1],py-pz+(pl->py[i+1]-pl->pu[i+1]),
                    pcolor,2,bit_map);
            }
        }
        if(flag & PDP_HEIGHT) // barycenter height line
        {
            put_line(px+pl->bx,py-pz+(pl->by-pl->bu),px+pl->bx,py-pz+(pl->by-pl->bz),SYSTEM_PINK,bit_map);
            if(flag & PDP_CROSS)
            {
                put_cross(px+pl->bx,py-pz+(pl->by-pl->bu),SYSTEM_WHITE,bit_map);
                put_cross(px+pl->bx,py-pz+(pl->by-pl->bz),SYSTEM_WHITE,bit_map);
            }
        }
        if(flag & PDP_ARRIS) // arris lines
        {
            for(i=0;i<pn;i++)
            {
                if(i<=rr)
                    put_line(px+pl->px[i],py-pz+(pl->py[i]-pl->pu[i]),px+pl->px[i],py-pz+(pl->py[i]-pl->pz[i]),
                    pcolor,bit_map);
                else
                    put_dash_line(px+pl->px[i],py-pz+(pl->py[i]-pl->pu[i]),px+pl->px[i],py-pz+(pl->py[i]-pl->pz[i]),
                    pcolor,2,bit_map);
            }
        }
        if(flag & PDP_TOP)   // cover(top) lines
        {
            for(i=0;i<pn;i++)
            {
                put_line(px+pl->px[i],py-pz+pl->py[i]-pl->pz[i],px+pl->px[i+1],
                    py-pz+pl->py[i+1]-pl->pz[i+1],
                    pcolor,bit_map);
            }
        }

        if(flag & PDP_CROSSBOX)
        {
            for(i=0;i<pn;i++)
            {
                put_section(px+pl->px[i],py-pz+pl->py[i]-pl->pu[i],2,SYSTEM_BLACK,SYSTEM_YELLOW,bit_map);
                put_section(px+pl->px[i],py-pz+pl->py[i]-pl->pz[i],2,SYSTEM_BLACK,SYSTEM_YELLOW,bit_map);
            }
        }
        else if(flag & PDP_CROSS) // edge cross
        {
            for(i=0;i<pn;i++)
            {
                put_cross(px+pl->px[i],py-pz+pl->py[i]-pl->pu[i],SYSTEM_WHITE,bit_map);
                put_cross(px+pl->px[i],py-pz+pl->py[i]-pl->pz[i],SYSTEM_WHITE,bit_map);
            }
        }
        if(flag & PDP_NUMBER)
        {
            for(i=0;i<pn;i++)
            {
                sprintf((char *)print_rec,"%d",i);
                if(i==pl->min_x)
                    strcat((char *)print_rec,"x");
                if(i==pl->max_y)
                    strcat((char *)print_rec,"Y");
                if(i==pl->max_x)
                    strcat((char *)print_rec,"X");
                if(i==pl->min_y)
                    strcat((char *)print_rec,"y");
                if(i==pl->min_z)
                    strcat((char *)print_rec,"z");
                if(i==pl->max_z)
                    strcat((char *)print_rec,"Z");
                print16(px+pl->px[i]+3,py-pz+(pl->py[i]-pl->pu[i])+2,(USTR *)print_rec,PEST_PUT,bit_map);
            }
        }
    }
}

EXPORT void FNBACK D3_clear_poly(POLY *pl)
{
    SLONG i;
    pl->pn=0;
    pl->bx=pl->by=pl->bz=pl->bu=0;
    pl->max_x=pl->min_x=pl->max_y=pl->min_y=pl->max_z=pl->min_z=0;
    for(i=0;i<MAX_POLY_NUM+1;++i)
    {
        pl->px[i]=0;
        pl->py[i]=0;
        pl->pz[i]=0;
        pl->pu[i]=0;
    }
    pl->na[UNDER_SURFACE]=0;
    pl->nb[UNDER_SURFACE]=0;
    pl->nc[UNDER_SURFACE]=1;
    pl->nd[UNDER_SURFACE]=0;

    pl->na[COVER_SURFACE]=0;
    pl->nb[COVER_SURFACE]=0;
    pl->nc[COVER_SURFACE]=1;
    pl->nd[COVER_SURFACE]=0;

    pl->type=POLY_TYPE_MAST;
}


EXPORT void FNBACK D3_add_point_to_poly(POLY *pl,SLONG x,SLONG y,SLONG under_z,SLONG cover_z,SLONG add_flag)
{
    SLONG xx,yy,zz;

    if(pl->pn<MAX_POLY_NUM)
    {
        if(add_flag & ADD_SPECIFY_X)
            pl->px[pl->pn]=x;
        if(add_flag & ADD_SPECIFY_Y)
            pl->py[pl->pn]=y;

        if(add_flag & ADD_SPECIFY_UNDER_Z)
        {
            pl->pu[pl->pn]=under_z;
        }
        else
        {
            D3_intersect_line_surface(pl->px[pl->pn],pl->py[pl->pn],0,
                pl->px[pl->pn],pl->py[pl->pn],10,
                pl->na[UNDER_SURFACE],pl->nb[UNDER_SURFACE],pl->nc[UNDER_SURFACE],pl->nd[UNDER_SURFACE],
                &xx,&yy,&zz);
            pl->pu[pl->pn]=zz;
        }

        if(add_flag & ADD_SPECIFY_COVER_Z)
        {
            pl->pz[pl->pn]=cover_z;
        }
        else
        {
            D3_intersect_line_surface(pl->px[pl->pn],pl->py[pl->pn],0,
                pl->px[pl->pn],pl->py[pl->pn],10,
                pl->na[COVER_SURFACE],pl->nb[COVER_SURFACE],pl->nc[COVER_SURFACE],pl->nd[COVER_SURFACE],
                &xx,&yy,&zz);
            pl->pz[pl->pn]=zz;
        }

        pl->pn++;
        pl->px[pl->pn]=pl->px[0];
        pl->py[pl->pn]=pl->py[0];
        pl->pu[pl->pn]=pl->pu[0];
        pl->pz[pl->pn]=pl->pz[0];
    }
    D3_neaten_poly_gon(pl);
}


//
// �������������߶Ȳ���
//
EXPORT SLONG FNBACK D3_neaten_poly_height(POLY *pl,SLONG n1,SLONG n2,SLONG n3)
{
    SLONG pn,a,b,c,d,x,y,z;
    SLONG i;
    
    pn=pl->pn;
    if(pn >= RIGHTFUL_POLY_MIN_NUM)
    {
        D3_normal_surface(pl->px[n1],pl->py[n1],pl->pz[n1],
            pl->px[n2],pl->py[n2],pl->pz[n2],
            pl->px[n3],pl->py[n3],pl->pz[n3],
            &a,&b,&c,&d);
// ZJian,2000.10.26
        pl->na[COVER_SURFACE]=a;
        pl->nb[COVER_SURFACE]=b;
        pl->nc[COVER_SURFACE]=c;
        pl->nd[COVER_SURFACE]=d;

        for(i=0;i<=pn;i++)
        {
            D3_intersect_line_surface(pl->px[i],pl->py[i],0,
                pl->px[i],pl->py[i],10,
                a,b,c,d,&x,&y,&z);
            pl->pz[i]=z;
        }
        D3_intersect_line_surface(pl->bx,pl->by,0,
            pl->bx,pl->by,10,
            a,b,c,d,&x,&y,&z);
        pl->bz=z;
        return(TTN_OK);
    }
    return(TTN_NOT_OK);
}



//
// Ѱ����������(ͶӰ)�Ͼ����������̵�������
//
EXPORT  SLONG   FNBACK  D3_find_point_poly_underside(SLONG *x,SLONG *y,SLONG r,POLY *pl)
{
    SLONG px,py;
    SLONG min_dist,dist,index;
    SLONG pn;
    SLONG  i;
    
    pn=pl->pn;
    min_dist=999999;
    index=-1;
    px=*x; py=*y;
    for(i=0;i<pn;i++)
    {
        dist=(px-pl->px[i])*(px-pl->px[i])+(py-pl->py[i])*(py-pl->py[i]);
        if(dist<min_dist)
        {
            min_dist=dist;
            index=i;
        }
    }
    if(min_dist<r*r)
    {
        *x=pl->px[index];
        *y=pl->py[index];
    }
    else
    {
        index=-1;
    }
    return(index);
}


//
// Ѱ�����������ĵ�
//
EXPORT  SLONG   FNBACK  D3_find_point_poly_barycenter(SLONG *x,SLONG *y,SLONG *z,SLONG r,POLY *pl)
{
    SLONG   px, py, find;
    double  dist;
    
    px=*x;
    py=(*y)-(*z);
    find=0;
    // In the normal condition, poly cover will COVER the under, so we check for cover first.
    // we convert to 2D for compute distance.
    dist=(double)(px-pl->bx) * (double)(px-pl->bx) + (double)(py-(pl->by-pl->bz)) * (double)(py-(pl->by-pl->bz));
    if(dist<r*r)
    {
        *x=pl->bx;
        *y=pl->by;
        *z=pl->bz;
        //return FIND_BARYCENTER_COVER;
        find |= 0x1;
    }
    dist=(double)(px-pl->bx) * (double)(px-pl->bx) + (double)(py-(pl->by-pl->bu)) * (double)(py-(pl->by-pl->bu));
    if(dist<r*r)
    {
        *x=pl->bx;
        *y=pl->by;
        *z=pl->bu;
        //return FIND_BARYCENTER_UNDER;
        find |= 0x2;
    }
    if(find & 0x1)  // we need to check cover first
        return FIND_BARYCENTER_COVER;
    if(find & 0x2)
        return FIND_BARYCENTER_UNDER;
    else
        return FIND_BARYCENTER_NONE;
}


EXPORT void  FNBACK D3_adjust_poly_up_sea_level(POLY *pl)
{
//    SLONG i;
    SLONG lowest_z;
    SLONG sz_index;
    SLONG dy;

    if(!pl) return;

    lowest_z=pl->bu;
    sz_index=(SLONG)pl->min_z;
    if(pl->pu[sz_index] < lowest_z)
        lowest_z = pl->pu[sz_index];
    if(lowest_z>=0)
        return;
    dy = -lowest_z;
//    for(i=0;i<=pl->pn;i++)
//    {
//        pl->by[i] += dy;
//    }




}


EXPORT void FNBACK D3_convert_poly_relational_by_barycenter(POLY *pl)
{
    SLONG stand_x,stand_y;
    SLONG i;

    stand_x=pl->bx;
    stand_y=pl->by;

    pl->bx -= stand_x;
    pl->by -= stand_y;
    for(i=0;i<=pl->pn;i++)
    {
        pl->px[i] -= stand_x;
        pl->py[i] -= stand_y;
    }
}


EXPORT void FNBACK D3_convert_poly_relational_by_world(POLY *pl,SLONG stand_x,SLONG stand_y)
{
    SLONG i;
    pl->bx += stand_x;
    pl->by += stand_y;
    for(i=0;i<=pl->pn;i++)
    {
        pl->px[i] += stand_x;
        pl->py[i] += stand_y;
    }
}


//
// ����������ĳһ���������(����õ������������ϵֱ�߾������)������
// IN:  ͨ��(*x, *y, *z)�� kx,ky ȷ�����������ֱ��.
//
EXPORT  SLONG   FNBACK  D3_seek_relation_point_poly_ex(POLY *pl,SLONG dist,SLONG kx, SLONG ky, SLONG *x,SLONG *y,SLONG *z,SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2)
{
    SLONG pn,ret,sx,sy,sz,tx,ty,tz;
    SLONG sx1,sy1,sz1,sx2,sy2,sz2,sx3,sy3,sz3,sx4,sy4,sz4;
    SLONG dist1,dist2,min_dist1,min_dist2,find1,find2;
    SLONG i;
    
    ret=SEEK_RELATION_NONE;
    find1=-1; find2=-1;
    sx=*x; sy=*y; sz=*z;
    sx1=sx-kx; sy1=sy-ky; sz1=sz;
    sx2=sx+kx; sy2=sy+ky; sz2=sz;
    sx3=sx-kx; sy3=sy+ky; sz3=sz;
    sx4=sx+kx; sy4=sy-ky; sz4=sz;
    min_dist1=999999; min_dist2=999999;
    pn=pl->pn;
    for(i=0;i<pn;i++)
    {
        tx=pl->px[i]; ty=pl->py[i]; tz=pl->pz[i];
        dist1=D3_distance_point_line(tx,ty,tz,sx1,sy1,sz1,sx2,sy2,sz2);
        if(dist1<min_dist1)
        {
            min_dist1=dist1;
            find1=i;
        }
        dist2=D3_distance_point_line(tx,ty,tz,sx3,sy3,sz3,sx4,sy4,sz4);
        if(dist2<min_dist2)
        {
            min_dist2=dist2;
            find2=i;
        }
    }
    if(min_dist1<dist)
    {
        *x1=pl->px[find1];
        *y1=pl->py[find1];
        *z1=pl->pz[find1];
        sx1=pl->px[find1]-kx;
        sy1=pl->py[find1]-ky;
        sz1=pl->pz[find1];
        sx2=pl->px[find1]+kx;
        sy2=pl->py[find1]+ky;
        sz2=pl->pz[find1];
        ret |= SEEK_RELATION_FIRST;
    }
    if(min_dist2<dist)
    {
        *x2=pl->px[find2];
        *y2=pl->py[find2];
        *z2=pl->pz[find2];
        sx3=pl->px[find2]-kx;
        sy3=pl->py[find2]+ky;
        sz3=pl->pz[find2];
        sx4=pl->px[find2]+kx;
        sy4=pl->py[find2]-ky;
        sz4=pl->pz[find2];
        ret |= SEEK_RELATION_SECOND;
    }
    D3_intersect_line_line(sx1,sy1,sz1,sx2,sy2,sz2,sx3,sy3,sz3,sx4,sy4,sz4,&tx,&ty,&tz);
    *x=tx; *y=ty; *z=tz;
    return(ret);
}


//
// ����������ĳһ���������(����õ�������ض�ֱ�߾������)������
//
EXPORT SLONG FNBACK D3_seek_relation_point_poly(POLY *pl,SLONG dist,SLONG *x,SLONG *y,SLONG *z,
                                                SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2)
{
    SLONG pn,ret,sx,sy,sz,tx,ty,tz;
    SLONG sx1,sy1,sz1,sx2,sy2,sz2,sx3,sy3,sz3,sx4,sy4,sz4;
    SLONG dist1,dist2,min_dist1,min_dist2,find1,find2;
    SLONG i;
    
    ret=SEEK_RELATION_NONE;
    find1=-1; find2=-1;
    sx=*x; sy=*y; sz=*z;
    sx1=sx-20; sy1=sy-10; sz1=sz;
    sx2=sx+20; sy2=sy+10; sz2=sz;
    sx3=sx-20; sy3=sy+10; sz3=sz;
    sx4=sx+20; sy4=sy-10; sz4=sz;
    min_dist1=999999; min_dist2=999999;
    pn=pl->pn;
    for(i=0;i<pn;i++)
    {
        tx=pl->px[i]; ty=pl->py[i]; tz=pl->pz[i];
        dist1=D3_distance_point_line(tx,ty,tz,sx1,sy1,sz1,sx2,sy2,sz2);
        if(dist1<min_dist1)
        {
            min_dist1=dist1;
            find1=i;
        }
        dist2=D3_distance_point_line(tx,ty,tz,sx3,sy3,sz3,sx4,sy4,sz4);
        if(dist2<min_dist2)
        {
            min_dist2=dist2;
            find2=i;
        }
    }
    if(min_dist1<dist)
    {
        *x1=pl->px[find1];
        *y1=pl->py[find1];
        *z1=pl->pz[find1];
        sx1=pl->px[find1]-20;
        sy1=pl->py[find1]-10;
        sz1=pl->pz[find1];
        sx2=pl->px[find1]+20;
        sy2=pl->py[find1]+10;
        sz2=pl->pz[find1];
        ret |= SEEK_RELATION_FIRST;
    }
    if(min_dist2<dist)
    {
        *x2=pl->px[find2];
        *y2=pl->py[find2];
        *z2=pl->pz[find2];
        sx3=pl->px[find2]-20;
        sy3=pl->py[find2]+10;
        sz3=pl->pz[find2];
        sx4=pl->px[find2]+20;
        sy4=pl->py[find2]-10;
        sz4=pl->pz[find2];
        ret |= SEEK_RELATION_SECOND;
    }
    D3_intersect_line_line(sx1,sy1,sz1,sx2,sy2,sz2,sx3,sy3,sz3,sx4,sy4,sz4,&tx,&ty,&tz);
    *x=tx; *y=ty; *z=tz;
    return(ret);
}


EXPORT SLONG FNBACK D3_is_poly_rightful(POLY *pl)
{
    if(!pl) return FALSE;
    if(pl->pn<RIGHTFUL_POLY_MIN_NUM) return FALSE;
    return TRUE;
}


//
// �����������
//
EXPORT  SLONG   FNBACK  D3_fill_poly_cover(SLONG rx,SLONG ry,POLY *pl,PIXEL color,BMP *bit_map)
{
    POLY    spl;
    SLONG   pn;
    SLONG  i;
    
    memcpy(&spl,pl,sizeof(POLY));
    pn=spl.pn;
    for(i=0;i<=pn;i++)
    {
        spl.py[i]-=spl.pz[i];
        spl.pz[i]=0;
    }
    D3_neaten_poly_gon(&spl);
    D3_fill_poly_underside(rx,ry,&spl,color,bit_map);
    return(TTN_OK);
}


//FUNCTION: Check perspective relation between poly and poly
//          ��������ͶӰ������������ǰ��(͸��)��ϵ
//RETURN:   DEEPLY_FIRST for poly1(pl1) is deeply(far away from our eyes) than poly2
//          DEEPLY_SECOND for poly2(pl2) is deeply than poly1
//          DEEPLY_UNKNOWN for they have no obviousely perspective relations
EXPORT  SLONG   FNBACK  D3_check_deeply_poly_poly(POLY *pl1,SLONG px1,SLONG py1,SLONG pz1,
                                                  POLY *pl2,SLONG px2,SLONG py2,SLONG pz2)
{
    static  POLY    sp1,sp2;
    static  SLONG   x11,y11,x12,y12,x21,y21,x22,y22;
    
    // prepare datas ------------------------------------------------------------
    D3_copy_poly(&sp1,pl1);
    D3_copy_poly(&sp2,pl2);
    D3_translate_poly_world_coordinates(&sp1,px1,py1,pz1);
    D3_translate_poly_world_coordinates(&sp2,px2,py2,pz2);
    
    // poly 1 & poly 2 is shadow overlapped -------------------------------------
    if(D2_is_overlap_poly_poly(&sp1,&sp2)==TRUE)
    {
        if(sp1.bz == 0 && sp2.bz != 0)       // if POLY1 is 0 height, POLY2 is front of POLY1,return TTN_OK
            return DEEPLY_FIRST;
        else if(sp1.bz != 0 && sp2.bz == 0)  // 2 is 0 height
            return DEEPLY_SECOND;
        else
        {
            if(pz1<pz2)       // 1 is under 2
                return DEEPLY_FIRST;
            else if(pz1>pz2)  // 2 is under 1
                return DEEPLY_SECOND;
            else              // 1 & 2 cross
            {
                if(py1>py2)
                    return DEEPLY_SECOND;
                else if(py1<py2)
                    return DEEPLY_FIRST;
                else
                {
                    if(px1<px2)
                        return DEEPLY_FIRST;
                    else if(px1>px2)
                        return DEEPLY_SECOND;
                    else
                        return DEEPLY_UNKNOWN;
                }
            }
        }
    }
    
    // poly 1 & poly 2 is not overlapped -----------------------------------
    else
    {
        return(D2_check_deeply_segment_segment(
            sp1.px[sp1.min_x],sp1.py[sp1.min_x],
            sp1.px[sp1.max_x],sp1.py[sp1.max_x],
            sp2.px[sp2.min_x],sp2.py[sp2.min_x],
            sp2.px[sp2.max_x],sp2.py[sp2.max_x]));
    }
    //return DEEPLY_UNKNOWN;
}


//
// ���һ�������Ƿ��������Ķ�����
//
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_cover(POLY *pl,SLONG x,SLONG y)
{
    SLONG flag,flag1;
    double data;

    //Jack, 2002.3.29.
    //Added for minimum edges check.
    if(pl->pn < 3) return FALSE;

    // Because we dont know the min & max x/y range of the cover.
    // May be you will ask me can we use the underside min_x,min_y...,
    // the answer is NO ! for it's cover, we dont know the every z-height! 
    flag1=0;
    for(SLONG i=0;i<pl->pn;i++)
    {
        data = (double)(pl->px[i]-x) * (double)( (pl->py[i+1] - pl->pz[i+1]) - y )  - (double)(pl->px[i+1]-x) * (double)( (pl->py[i]-pl->pz[i]) - y );
        flag = (data>0.0) ? 1 : ( (data < 0.0) ? -1 : 0 );
        if(flag)
        {
            if(!flag1) flag1=flag;
            if(flag!=flag1) return(FALSE);
        }
    }
    return TRUE;
}


//
// ���һ�������Ƿ�������������
//
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_underside(POLY *pl,SLONG x,SLONG y)
{
    SLONG flag,flag1;
    double data;
    
    //Jack, 2002.3.29.
    //Added for minimum edges check.
    if(pl->pn < 3) return FALSE;

    flag1=0;
    for(SLONG i=0;i<pl->pn;i++)
    {
        data = (double)(pl->px[i]-x) * (double)( (pl->py[i+1] - pl->pu[i+1]) - y )  
            - (double)(pl->px[i+1]-x) * (double)( (pl->py[i]-pl->pu[i]) - y );
        flag = (data>0.0) ? 1 : ( (data < 0.0) ? -1 : 0 );
        if(flag)
        {
            if(!flag1) flag1=flag;
            if(flag!=flag1) return(FALSE);
        }
    }
    return TRUE;
}


//
// ���������Ƿ���������ǰ��(������)ƽ����
//
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_foreside(POLY *pl,SLONG x,SLONG y,SLONG *index)
{
    static  POLY    spl;
    SLONG   side_index;
    SLONG   pn,ll,rr;
    SLONG  i;
    
    ll=pl->min_x;
    rr=pl->max_x;
    pn=pl->pn;
    spl.pn=4;
    side_index=-1;
    for(i=ll;i<rr;i++)
    {
        spl.px[0]=pl->px[i];
        spl.py[0]=pl->py[i];
        spl.px[1]=pl->px[(i+1)%pn];
        spl.py[1]=pl->py[(i+1)%pn];
        spl.px[2]=pl->px[(i+1)%pn];
        spl.py[2]=pl->py[(i+1)%pn]-pl->pz[(i+1)%pn];
        spl.px[3]=pl->px[i];
        spl.py[3]=pl->py[i]-pl->pz[i];
        spl.px[4]=spl.px[0];    // set it same
        spl.py[4]=spl.py[0];

        if( TRUE == D3_is_point_in_poly_underside(&spl,x,y) )
        {
            side_index=i;
            i=rr+100;
        }
    }
    if(side_index>=0)
    {
        (*index) = side_index;
        return TRUE;
    }
    return FALSE;
}


//
// �����������
//
EXPORT  SLONG   FNBACK  D3_fill_poly_underside(SLONG rx,SLONG ry,POLY *pl,PIXEL color,BMP *bit_map)
{
    D2_fill_poly(rx,ry,pl,color,bit_map);
    return TTN_OK;
}


//
// ���߶�����˵�1����Ϊrr�ĵ�
//
EXPORT  SLONG   FNBACK  D3_seek_space_point_segment(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,SLONG *x,SLONG *y,SLONG *z,SLONG rr)
{
    static  double  ax,ay,az,bx,by,bz;
    static  double  bb,t;
    
    ax=x1;          // SEG :  S(t) = A + Bt(0<=t<=1)
    ay=y1;
    az=z1;
    bx=x2-x1;
    by=y2-y1;
    bz=z2-z1;
    bb=bx*bx+by*by+bz*bz;
    if(bb)
    {
        t=sqrt((rr*rr*1.0)/(bb*1.0));
        *x=(SLONG)(ax+bx*t+0.5);
        *y=(SLONG)(ay+by*t+0.5);
        *z=(SLONG)(az+bz*t+0.5);
        return(TTN_OK);
    }
    *x=(x1+x2)/2;
    *y=(y1+y2)/2;
    *z=(z1+z2)/2;
    return(TTN_NOT_OK);
}


//
// �߶βü�(�������˵�ľ���)
//
EXPORT  SLONG   FNBACK  D3_clip_segment(SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2,SLONG rr)
{
    static  SLONG   xx1,yy1,zz1,xx2,yy2,zz2;
    static  SLONG   ret1,ret2;
    
    ret1=D3_seek_space_point_segment(*x1,*y1,*z1,*x2,*y2,*z2,&xx1,&yy1,&zz1,rr);
    ret2=D3_seek_space_point_segment(*x2,*y2,*z2,*x1,*y1,*z1,&xx2,&yy2,&zz2,rr);
    *x1=xx1;
    *y1=yy1;
    *z1=zz1;
    *x2=xx2;
    *y2=yy2;
    *z2=zz2;
    if((ret1==TTN_OK)&&(ret2==TTN_OK))
        return(TTN_OK);
    return(TTN_NOT_OK);
}


//
// ��������������(ͶӰ)�ϵ�һ�㵽���������Ӧ��ľ���(�߶�)
//
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_underside(POLY *pl,SLONG x,SLONG y)
{
    static  SLONG   pn,height,a,b,c,d;
    static  SLONG   ux,uy,uz;
    
    pn=pl->pn;
    height=0;
    if(pn>2)
    {
        D3_normal_surface(
            pl->px[0],pl->py[0],pl->pz[0],
            pl->px[1],pl->py[1],pl->pz[1],
            pl->px[2],pl->py[2],pl->pz[2],
            &a,&b,&c,&d);
        D3_intersect_line_surface(x,y,0,x,y,100,a,b,c,d,&ux,&uy,&uz);
        height=uz;
    }
    return(height);
}



//
// ���������������ϵ�һ���Ӧ�ڸ������ĸ߶�
//     (�õ���������������Z��ͶӰ��֮��ľ���)
//
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_cover(POLY *pl,SLONG x,SLONG y)
{
    SLONG   a, b, c, d;
    SLONG   height;
    double  z;
    
    z=0.0;
    if(pl->pn>2)
    {
        D3_normal_surface(pl->px[0],pl->py[0],pl->pz[0],
            pl->px[1],pl->py[1],pl->pz[1],
            pl->px[2],pl->py[2],pl->pz[2],
            &a,&b,&c,&d);

        if(b+c)
        {
            z=(-(a*x+b*y+d)*1.0)/((b+c)*1.0);
        }
    }
    height=(SLONG)(z+0.5);
    return(height);
}

//
// ����������ǰ��(������)�ϵ�һ���Ӧ����������ĸ߶�
//
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_foreside(POLY *pl,SLONG side,SLONG x,SLONG y)
{
    static  SLONG   pn,height,a,b,c,d;
    static  double  z;
    
    z=0.0;
    pn=pl->pn;
    if(pn>2)
    {
        D3_normal_surface(pl->px[side],pl->py[side],0,
            pl->px[side],pl->py[side],1,
            pl->px[(side+1)%pn],pl->py[(side+1)%pn],0,
            &a,&b,&c,&d);
        if(b+c)
        {
            z=(-(a*x+b*y+d)*1.0)/((b+c)*1.0);
        }
    }
    height=(SLONG)(z+0.5);
    return(height);
}





#if 0 ////////////////////////////////////////////////////////////////////////////////////////////////






//
// У��һ����������صĸ߶�(��õ�߶Ƚӽ��������߶ȵ�ĸ߶�)
// adjust height by tali poly
//
SLONG   PL_adjust_height_point_tali_poly(POLY *pl,SLONG x,SLONG y,SLONG z)
{
    SLONG   i,pn,h[16],index,find,near_var,height;
    
    height = z;
    pn = pl->pn;
    index=0;
    for(i=0;i<pn;i++)
    {
        if(pl->pz[i] == pl->pz[(i+1)%pn])
            h[index++] = pl->pz[i];
    }
    near_var = 9999;
    find = -1;
    for(i=0;i<index;i++)
    {
        if(abs(z - h[i]) < near_var)
        {
            near_var = abs(z - h[i]);
            find = i;
        }
    }
    if(find>=0)
        height = h[find];
    return(height);
}


//
// ���һ��������һ��б��(����)�Ƿ�������ص�
// check normal-poly pl1 & tali-poly pl2 is overlapped or not ?
// if pl1 is all in pl2, they are overlapped
// else if the intersect point(s) is(are) all in the regular lines of pl2,
//      they are overlapped too
// else they are not overlapped
// PS:  pl1     is an normal poly
//      pl2     is an tali poly
//
SLONG   PL_check_overlap_poly_tali_poly(POLY *pl1,POLY *pl2)
{
    static  SLONG   x,y,z;
    register SLONG  i,j;
    SLONG   flag,level;
    
    // rectangel clipper check ---------------------------------------------
    if( pl1->px[pl1->ll] > pl2->px[pl2->rr]
        || pl1->px[pl1->rr] < pl2->px[pl2->ll]
        || pl1->py[pl1->dd] < pl2->py[pl2->uu]
        || pl1->py[pl1->uu] > pl2->py[pl2->dd] )
        return(TTN_NOT_OK);
    
    // is normal poly pl1 be included in the tali poly pl2 ? ---------------
    flag=0;
    i=pl1->pn;
    while(--i>=0)
    {
        if(PL_check_point_in_poly_underside(pl2,pl1->px[i],pl1->py[i])!=TTN_OK)
        {
            flag=1;   // some point is not in the tali poly pl2
            break;
        }
    }
    if(flag==0)
        return(TTN_OK);
    
    // is intersect point in the level line of tali poly pl2 ? -------------
    flag=0;
    for(i=0;i<pl2->pn;i++)
    {
        // check is this line a level line ? 1 for a level line, else 0
        level = ( pl2->pz[i] == pl2->pz[(i+1)%pl2->pn] ) ? 1 : 0 ;
        
        for(j=0;j<pl1->pn;j++)
        {
            if(PL_seek_intersect_point_segment_segment(
                pl1->px[j],pl1->py[j],0,pl1->px[(j+1)%pl1->pn],pl1->py[(j+1)%pl1->pn],0,
                pl2->px[i],pl2->py[i],0,pl2->px[(i+1)%pl2->pn],pl2->py[(i+1)%pl2->pn],0,
                &x,&y,&z)==TTN_OK)
            {
                if(level == 0)         // if not a level line
                    return(TTN_NOT_OK);
                else
                    flag=1;
            }
        }
    }
    if(flag==1)
        return(TTN_OK);
    
    return(TTN_NOT_OK);
}



//
// �����������ʾ������Ƿ��ǽ������(б��)
//
SLONG   PL_check_tali_poly(POLY *pl)
{
    register SLONG  i;
    static  SLONG   z;
    
    z=pl->pz[0];
    i=pl->pn;
    while(--i>=0)
    {
        if(pl->pz[i] != z)
            return(TTN_OK);
    }
    return(TTN_NOT_OK);
}

//
// ���һ��������ͶӰ�Ƿ���ȫ��������һ��������ͶӰ(PL2 �Ƿ���� PL1)
//
SLONG   PL_check_include_poly_poly(POLY *pl1,POLY *pl2)
{
    register SLONG  i;
    i=pl1->pn;
    while(--i>=0)
    {
        if(PL_check_point_in_poly_underside(pl2,pl1->px[i],pl1->py[i])!=TTN_OK)
            return(TTN_NOT_OK);
    }
    return(TTN_OK);
}




#endif///////////////////////////////////////////////////////////////////////////////////////////////////////





