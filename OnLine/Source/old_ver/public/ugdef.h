/******************************************************************************
*@Copyright UniGium Information Technology (ShangHai) Co.,Ltd. 
 2003 ALL RIGHTS RESERVED.

*file name    : ugdef.h
*owner        : Andy
*description  : 
*modified     : 
******************************************************************************/ 

#ifndef __UG_DEF_H__
#define __UG_DEF_H__

//////////////////////////////////////////////////////////////////////////
//
#if !defined (SAFE_RELEASE)
#define SAFE_RELEASE(p) { if (p) { (p)->Release(); (p) = NULL; }}
#endif

#if !defined (SAFE_RELEASE_EX)
#define SAFE_RELEASE_EX(p) { if (p) { while((p)->Release()); (p) = NULL; }}
#endif

//////////////////////////////////////////////////////////////////////////
//
#if !defined (SAFE_DELETE)
#define SAFE_DELETE(p) { if (p) { delete (p);  (p) = NULL; } }
#endif

//////////////////////////////////////////////////////////////////////////
//
#if !defined (SAFE_DELETE_ARRAY)
#define SAFE_DELETE_ARRAY(p) { if (p) { delete [] (p); (p) = NULL; } }
#endif

#if !defined (SAFE_DELETEEX)
#define SAFE_DELETEEX(p, n)	{ if (p) { for (int _i = 0; _i < n; _i ++) \
							{ SAFE_DELETE(p[_i]); }} }
#endif

#if !defined (SAFE_RELEASE_VECTOR)
#define SAFE_RELEASE_VECTOR(p)	{ { for (int _i = 0; _i < (int)p.Count(); _i ++) \
							{ SAFE_RELEASE(p[_i]); } p.RemoveAll();} }
#endif

#if !defined (SAFE_DELETE_VECTOR)
#define SAFE_DELETE_VECTOR(p)	{ { for (int _i = 0; _i < (int)p.Count(); _i ++) \
							{ SAFE_DELETE(p[_i]); } p.RemoveAll();} }
#endif

#if !defined (SAFE_CLEAR_VECTOR)
#define SAFE_CLEAR_VECTOR(p)    { { p.RemoveAll(); } }
#endif

#if !defined (SAFE_RELEASEEX)
#define SAFE_RELEASEEX(p, n) { if (p) { for (int _i = 0; _i < n; _i ++)\
								{SAFE_RELEASE(p[_i]);}}}
#endif

//////////////////////////////////////////////////////////////////////////
//
#define OBJECT_MESH_CLASS_ID		0x100
#define OBJECT_ANIM_MESH_CLASS_ID   0x110
#define OBJECT_BONE_MESH_CLASS_ID   0x120

#define OBJECT_LIGHT_CLASS_ID		0x200
#define OBJECT_CAMERA_CLASS_ID		0x300
#define OBJECT_PATH_CLASS_ID		0x400
#define OBJECT_POINT_CLASS_ID		0x500
#define OBJECT_BOXGIZMO_CLASS_ID	0x600
#define OBJECT_MATERIAL_CLASS_ID	0x700

//////////////////////////////////////////////////////////////////////////
//
#define MAKE_MAX_TO_MIN(m) {(m[0]) = FLT_MIN; (m[1]) = FLT_MIN; (m[2]) = FLT_MIN;}

#define MAKE_MIN_TO_MAX(m) {(m[0]) = FLT_MAX; (m[1]) = FLT_MAX; (m[2]) = FLT_MAX;}

//////////////////////////////////////////////////////////////////////////
// COLOR define
typedef unsigned long UGCOLOR;

#define UGCOLOR_ARGB(a,r,g,b) \
((UGCOLOR)((((a)&0xff)<<24)|(((r)&0xff)<<16)|(((g)&0xff)<<8)|((b)&0xff)))

#define UGCOLOR_RGBA(r,g,b,a) UGCOLOR_ARGB(a,r,g,b)
#define UGCOLOR_XRGB(r,g,b)   UGCOLOR_ARGB(0xff,r,g,b)
#define UGCOLOR_XYUV(y,u,v)   UGCOLOR_ARGB(0xff,y,u,v)
#define UGCOLOR_AYUV(a,y,u,v) UGCOLOR_ARGB(a,y,u,v)

#define UGCOLOR_COLORVALUE(r,g,b,a) \
UGCOLOR_RGBA((DWORD)((r)*255.f),(DWORD)((g)*255.f),(DWORD)((b)*255.f),(DWORD)((a)*255.f))

#define UGIP_ABCD(a,b,c,d) UGCOLOR_ARGB(d,c,b,a)

//////////////////////////////////////////////////////////////////////////
//
#define ADD_STATE_BIT(p, s)	   ((p) |= (s))
#define REMOVE_STATE_BIT(p, s) ((p) &= ~(s))
#define MODIFY_STATE_BIT(p, s, d) { REMOVE_STATE_BIT(p,s); ADD_STATE_BIT(p,d); }
#define IS_STATE_BIT(p, s) (p & s)

#endif // #ifndef __UG_DEF_H__