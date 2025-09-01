/*
**      XPOLY.H
**      Poly functions header.
**      ZJian,2000.7.25.
*/
#ifndef  _XPOLY_H_INCLUDE_
#define  _XPOLY_H_INCLUDE_      1
#include "xgrafx.h"

typedef enum POLY_PARAMETER_ENUMS
{ MAX_POLY_NUM              = 15,   // �����̦h���I��
RIGHTFUL_POLY_MIN_NUM       = 3,    // �X�k�������ֳ̤��I��
UNDER_SURFACE               = 0,    // ������
COVER_SURFACE               = 1     // ������
} POLY_PARAMETEW;

//poly type defines -------------------------------------------------------------------
typedef enum POLY_TYPE_VALUE_ENUMS
{ POLY_TYPE_MAST            = 1,    // �W�^(���q�������)
POLY_TYPE_TRAPEZIA          = 2,    // ٱ�^(�����������ת������)
POLY_TYPE_HORIZONTAL        = 3,    // ������(�a�����������)
POLY_TYPE_VERTICAL          = 4,    // ����(�i�K���������)
POLY_TYPE_BEVEL             = 5,    // �׭�(�������������)
} POLY_TYPE_VALUE;

//check view deeply of two segments or two objects ------------------------------------
typedef enum VIEW_DEEPLY_RESULTS
{ DEEPLY_UNKNOWN    =   0,
DEEPLY_FIRST        =   1,
DEEPLY_SECOND       =   2,
} VIEW_DEEPLY_RESULTS;

//compute intersect points of two segments result -------------------------------------
typedef enum INTERSECT_POINTS_RESULT_ENUMS
{ INTERSECT_NO      =   0,
INTERSECT_OK        =   1,
INTERSECT_ONE       =   0x1,
INTERSECT_TWO       =   0x2,
} INTERSECT_POINTS_RESULT;

// D3 draw poly control flags ---------------------------------------------------------
typedef enum DRAW_POLY_CONTROL_FLAG_ENUMS
{ PDP_HEIGHT        =   0x00000001,     // barycenter z height
PDP_BASE            =   0x00000002,     // under(base) lines
PDP_TOP             =   0x00000004,     // cover(top) lines
PDP_ARRIS           =   0x00000008,     // arris
PDP_CROSS           =   0x00000010,     // line(segment) start & end point cross
PDP_FACE            =   0x00000020,     //
PDP_VALID           =   0x00000040,     // if this poly is valid
PDP_NUMBER          =   0x00000080,     // special points number(sx,lx,sy,ly,sz,lz ...)
PDP_ZHEIGHT         =   0x00000100,     // global world height
PDP_PROJECTION      =   0x00000200,     // projection to 0-height horizontal surface
PDP_CROSSBOX        =   0x00000400,     // segment start & end point crossbox
PDP_RED             =   0x00010000,     // color red
PDP_GREEN           =   0x00020000,     // color green
PDP_BLUE            =   0x00030000,     // color blue
PDP_YELLOW          =   0x00040000,     // color yellow
PDP_CYAN            =   0x00050000,     // color cyan
PDP_PINK            =   0x00060000,     // color pink
PDP_COLOR           =   0x00ff0000,     // colors mask
} DRAW_POLY_CONTROL_FLAG;

// D3 find poly barycenter results
typedef enum FIND_POLY_BARYCENTER_RESULTS_ENUMS
{ FIND_BARYCENTER_UNDER =   1,
FIND_BARYCENTER_COVER   =   2,
FIND_BARYCENTER_NONE    =   0
} FIND_POLY_BARYCENTER_RESULTS;

// D3 seek relation point poly
typedef enum SEEK_RELATION_POINT_POLY_RESULTS_ENUMS
{ SEEK_RELATION_NONE    =   0x0,
SEEK_RELATION_FIRST     =   0x1,
SEEK_RELATION_SECOND    =   0x2
} SEEK_RELATION_POINT_POLY_RESULTS;

// D3 add point to poly --- add flags
typedef enum ADD_POINT_TO_POLY_FLAGS_ENUMS
{ ADD_SPECIFY_X         =   0x1,
ADD_SPECIFY_Y           =   0x2,
ADD_SPECIFY_COVER_Z     =   0x4,
ADD_SPECIFY_UNDER_Z     =   0x8
} ADD_POINT_TO_POLY_FLAGS;

//D3 set poly point underside flags
typedef enum SET_POLY_POINY_UNDERSIDE_FLAGS_ENUMS
{ SET_SPECIFY_X         =   0x1,
SET_SPECIFY_Y           =   0x2,
SET_SPECIFY_Z           =   0x4
} SET_POLY_POINY_UNDERSIDE_FLAGS;


//--------------------------------------------------------------------------------------
#pragma pack(push)
#pragma pack(1)
typedef struct tagPOLY
{
   SLONG    pn;             // �������(������v)
   ULONG    type;           // ��������

   UCHR     min_x;          // �̥����I���ޭ�
   UCHR     max_x;          // �̥k���I���ޭ�
   UCHR     min_y;          // �̤W���I���ޭ� 
   UCHR     max_y;          // �̤U���I���ޭ� 
   UCHR     min_z;          // �̧C���I���ޭ�
   UCHR     max_z;          // �̰����I���ޭ�

   UCHR     reserved1;      // �O�d��1
   UCHR     reserved2;      // �O�d��2
   ULONG    reserved3;      // �O�d��3

   SLONG    bx;             // �����I���� X
   SLONG    by;             // �����I���� Y
   SLONG    bz;             // �����I���� Z(Z1)
   SLONG    bu;             // �����I���� Z0(under)

   SLONG    px[MAX_POLY_NUM+1]; // ���I���� X
   SLONG    py[MAX_POLY_NUM+1]; // ���I���� Y
   SLONG    pz[MAX_POLY_NUM+1]; // ���I���� Z(Z1)
   SLONG    pu[MAX_POLY_NUM+1]; // ���I���� Z0(under Z)
   SLONG    pa[MAX_POLY_NUM+1]; // ���I�ݩ�(attrib)

   SLONG    na[2];          // ��(��)�����k�V�q(X���q)
   SLONG    nb[2];          // ��(��)�����k�V�q(Y���q)
   SLONG    nc[2];          // ��(��)�����k�V�q(Z���q)
   SLONG    nd[2];          // ��(��)������{�Ѽ� D ( na*X + nb*Y + nc*Z + nd = 0)

} POLY,*LPPOLY;
#pragma pack(pop)
//-------------------------------------------------------------------------------------

#define D2_INCLUDE(x,y,x1,y1,x2,y2)             (((x)>=(x1))&&((x)<=(x2))&&((y)>=(y1))&&((y)<=(y2)))
#define D3_INCLUDE(x,y,z,x1,y1,z1,x2,y2,z2)     (((x)>=(x1))&&((x)<=(x2))&&((y)>=(y1))&&((y)<=(y2))&&((z)>=(z1))&&((z)<=(z2)))

//==================================================================================
// 2 Demensions functions
//==================================================================================
EXPORT  SLONG   FNBACK  D2_is_sameside_point_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,SLONG lx1,SLONG ly1,SLONG lx2,SLONG ly2);
EXPORT  SLONG   FNBACK  D2_is_intersect_segment_segment(SLONG px1,SLONG py1,SLONG px2,SLONG py2,SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2);
EXPORT  SLONG   FNBACK  D2_is_intersect_line_poly(SLONG x1,SLONG y1,SLONG x2,SLONG y2,POLY *pl);
EXPORT  SLONG   FNBACK  D2_is_point_in_poly(SLONG x,SLONG y,POLY *pl);
EXPORT  SLONG   FNBACK  D2_is_segment_in_poly(SLONG x1,SLONG y1,SLONG x2,SLONG y2,POLY *pl);
EXPORT  SLONG   FNBACK  D2_is_poly_in_poly(POLY *pl1,POLY *pl2);
EXPORT  SLONG   FNBACK  D2_is_overlap_poly_poly(POLY *pl1,POLY *pl2);
EXPORT  SLONG   FNBACK  D2_check_deeply_segment_segment(SLONG x11,SLONG y11,SLONG x12,SLONG y12,SLONG x21,SLONG y21,SLONG x22,SLONG y22);
EXPORT  SLONG   FNBACK  D2_distance_point_point(SLONG x1,SLONG y1,SLONG x2,SLONG y2);
EXPORT  SLONG   FNBACK  D2_distance_point_line(SLONG x,SLONG y,SLONG x1,SLONG y1,SLONG x2,SLONG y2);
EXPORT  SLONG   FNBACK  D2_nipangle_line_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2);
EXPORT  SLONG   FNBACK  D2_intersect_line_line(SLONG px1,SLONG py1,SLONG px2,SLONG py2,SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2,SLONG *x,SLONG *y);
EXPORT  SLONG   FNBACK  D2_intersect_segment_segment(SLONG px1,SLONG py1,SLONG px2,SLONG py2,SLONG qx1,SLONG qy1,SLONG qx2,SLONG qy2,SLONG *x,SLONG *y);
EXPORT  SLONG   FNBACK  D2_intersect_segment_poly(SLONG *x1,SLONG *y1,SLONG *x2,SLONG *y2,POLY *pl);
EXPORT  SLONG   FNBACK  D2_chip_point_poly(SLONG x,SLONG y,POLY *pl,SLONG *n1,SLONG *n2);
EXPORT  void    FNBACK  D2_fill_poly(SLONG relation_x, SLONG relation_y, POLY *poly,PIXEL color,BMP *bmp);
EXPORT  void    FNBACK  D2_barycenter_poly(POLY *pl);
EXPORT  void    FNBACK  D2_adjust_diamond_position(SLONG *x,SLONG *y,SLONG *cell_x,SLONG *cell_y,SLONG diamond_width,SLONG diamond_height);

//==================================================================================
// 3 Demensions functions
//==================================================================================
EXPORT  SLONG   FNBACK  D3_distance_point_point(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2);
EXPORT  SLONG   FNBACK  D3_distance_point_line(SLONG x,SLONG y,SLONG z,SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2);
EXPORT  SLONG   FNBACK  D3_distance_point_surface(SLONG x,SLONG y,SLONG z,SLONG a,SLONG b,SLONG c,SLONG d);
EXPORT  SLONG   FNBACK  D3_distance_line_line(SLONG x11,SLONG y11,SLONG z11,SLONG x12,SLONG y12,SLONG z12,SLONG x21,SLONG y21,SLONG z21,SLONG x22,SLONG y22,SLONG z22);
EXPORT  SLONG   FNBACK  D3_nipangle_line_line(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2);
EXPORT  SLONG   FNBACK  D3_normal_surface(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,SLONG x3,SLONG y3,SLONG z3,SLONG *a,SLONG *b,SLONG *c,SLONG *d);
EXPORT  SLONG   FNBACK  D3_intersect_line_surface(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,SLONG a,SLONG b,SLONG c,SLONG d,SLONG *x,SLONG *y,SLONG *z);
EXPORT  SLONG   FNBACK  D3_intersect_line_line(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2,SLONG *x,SLONG *y,SLONG *z);
EXPORT  SLONG   FNBACK  D3_intersect_segment_segment(SLONG px1,SLONG py1,SLONG pz1,SLONG px2,SLONG py2,SLONG pz2,SLONG qx1,SLONG qy1,SLONG qz1,SLONG qx2,SLONG qy2,SLONG qz2,SLONG *x,SLONG *y,SLONG *z);
EXPORT  SLONG   FNBACK  D3_droop_point_line(SLONG px,SLONG py,SLONG pz,SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,SLONG *x,SLONG *y,SLONG *z);
EXPORT  SLONG   FNBACK  D3_droop_point_surface(SLONG px,SLONG py,SLONG pz,SLONG a,SLONG b,SLONG c,SLONG d,SLONG *x,SLONG *y,SLONG *z);
EXPORT  void    FNBACK  D3_make_rectangle_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b);
EXPORT  void    FNBACK  D3_make_diamond_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b);
EXPORT  void    FNBACK  D3_make_ellipse_poly(POLY *pl,SLONG cx,SLONG cy,SLONG a,SLONG b);
EXPORT  void    FNBACK  D3_set_poly_height(POLY *pl,SLONG under_z,SLONG top_z);
EXPORT  void    FNBACK  D3_neaten_poly_gon(POLY *pl);
EXPORT  void    FNBACK  D3_draw_point(SLONG x,SLONG y,SLONG z,BMP *bmp);
EXPORT  void    FNBACK  D3_draw_line(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,BMP *bmp);
EXPORT  void    FNBACK  D3_draw_poly(SLONG px,SLONG py,SLONG pz,SLONG flag,POLY *pl,BMP *bmp);
EXPORT  void    FNBACK  D3_clear_poly(POLY *pl);
EXPORT  void    FNBACK  D3_add_point_to_poly(POLY *pl,SLONG x,SLONG y,SLONG z,SLONG z2,SLONG add_flag);
EXPORT  SLONG   FNBACK  D3_find_point_poly_underside(SLONG *x,SLONG *y,SLONG r,POLY *pl);
EXPORT  SLONG   FNBACK  D3_find_point_poly_barycenter(SLONG *x,SLONG *y,SLONG *z,SLONG r,POLY *pl);
EXPORT  void    FNBACK  D3_adjust_poly_up_sea_level(POLY *pl);
EXPORT  void    FNBACK  D3_convert_poly_relational_by_barycenter(POLY *pl);
EXPORT  void    FNBACK  D3_convert_poly_relational_by_world(POLY *pl,SLONG stand_x,SLONG stand_y);
EXPORT  SLONG   FNBACK  D3_neaten_poly_height(POLY *pl,SLONG n1,SLONG n2,SLONG n3);
EXPORT  SLONG   FNBACK  D3_seek_relation_point_poly(POLY *pl,SLONG dist,SLONG *x,SLONG *y,SLONG *z,SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2);
EXPORT  SLONG   FNBACK  D3_is_poly_rightful(POLY *pl);
EXPORT  SLONG   FNBACK  D3_fill_poly_underside(SLONG rx,SLONG ry,POLY *pl,PIXEL color,BMP *bit_map);
EXPORT  SLONG   FNBACK  D3_fill_poly_cover(SLONG rx,SLONG ry,POLY *pl,PIXEL color,BMP *bit_map);
EXPORT  SLONG   FNBACK  D3_check_deeply_poly_poly(POLY *pl1,SLONG px1,SLONG py1,SLONG pz1,POLY *pl2,SLONG px2,SLONG py2,SLONG pz2);
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_cover(POLY *pl,SLONG x,SLONG y);
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_underside(POLY *pl,SLONG x,SLONG y);
EXPORT  SLONG   FNBACK  D3_is_point_in_poly_foreside(POLY *pl,SLONG x,SLONG y,SLONG *index);
EXPORT  SLONG   FNBACK  D3_seek_space_point_segment(SLONG x1,SLONG y1,SLONG z1,SLONG x2,SLONG y2,SLONG z2,SLONG *x,SLONG *y,SLONG *z,SLONG rr);
EXPORT  SLONG   FNBACK  D3_clip_segment(SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2,SLONG rr);
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_underside(POLY *pl,SLONG x,SLONG y);
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_cover(POLY *pl,SLONG x,SLONG y);
EXPORT  SLONG   FNBACK  D3_compute_height_point_poly_foreside(POLY *pl,SLONG side,SLONG x,SLONG y);
EXPORT  SLONG   FNBACK  D3_seek_relation_point_poly_ex(POLY *pl,SLONG dist,SLONG kx, SLONG ky, SLONG *x,SLONG *y,SLONG *z,SLONG *x1,SLONG *y1,SLONG *z1,SLONG *x2,SLONG *y2,SLONG *z2);


EXPORT  POLY *  FNBACK  D3_create_poly(void);
EXPORT  void    FNBACK  D3_destroy_poly(POLY *poly);
EXPORT  void    FNBACK  D3_copy_poly(POLY *dest_poly,POLY *src_poly);
EXPORT  void    FNBACK  D3_set_poly_type(POLY *poly,ULONG type);
EXPORT  void    FNBACK  D3_set_poly_point_underside(POLY *poly,SLONG index,SLONG x,SLONG y,SLONG z,SLONG set_flag);
EXPORT  void    FNBACK  D3_move_poly_by_barycenter(POLY *pl,SLONG new_bx,SLONG new_by);
EXPORT  void    FNBACK  D3_translate_poly_world_coordinates(POLY *pl,SLONG px,SLONG py,SLONG pz);

EXPORT  SLONG   FNBACK  compute_greatest_common_divisor(SLONG mm,SLONG nn);

#endif//_XPOLY_H_INCLUDE_
//=======================================================================
