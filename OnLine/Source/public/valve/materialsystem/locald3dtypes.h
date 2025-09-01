//========= Copyright © 1996-2002, Valve LLC, All rights reserved. ============
//
// Purpose: 
//
// $NoKeywords: $
//=============================================================================

#ifndef LOCALD3DTYPES_H
#define LOCALD3DTYPES_H
#ifdef _WIN32
#pragma once
#endif

#define UG_USE_DIRECT3D_VER_9
#ifdef UG_USE_DIRECT3D_VER_9

#include <d3d9.h>
#include <d3dx9.h>
#include <d3dx9core.h>
#include <D3DX9Math.h>
#include <dxerr9.h>

struct IDirect3DTexture9;
struct IDirect3DBaseTexture9;
struct IDirect3DCubeTexture9;
struct IDirect3D9;
struct IDirect3DDevice9;
typedef IDirect3DTexture9 IDirect3DTexture;
typedef IDirect3DBaseTexture9 IDirect3DBaseTexture;
typedef IDirect3DCubeTexture9 IDirect3DCubeTexture;
typedef IDirect3DDevice9 IDirect3DDevice;
typedef IDirect3D9 IDirect3D;
typedef struct _D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER9;
typedef D3DADAPTER_IDENTIFIER9 D3DADAPTER_IDENTIFIER;
typedef struct _D3DMATERIAL9 D3DMATERIAL9;
typedef D3DMATERIAL9 D3DMATERIAL;
typedef struct _D3DLIGHT9 D3DLIGHT9;
typedef D3DLIGHT9 D3DLIGHT;
struct IDirect3DSurface9;
typedef IDirect3DSurface9 IDirect3DSurface;
typedef struct _D3DCAPS9 D3DCAPS9;
typedef D3DCAPS9 D3DCAPS;
typedef struct _D3DVIEWPORT9 D3DVIEWPORT9;
typedef D3DVIEWPORT9 D3DVIEWPORT;
struct IDirect3DIndexBuffer9;
typedef IDirect3DIndexBuffer9 IDirect3DIndexBuffer;
struct IDirect3DVertexBuffer9;
typedef IDirect3DVertexBuffer9 IDirect3DVertexBuffer;
struct IDirect3DVertexShader9;
struct IDirect3DPixelShader9;

typedef IDirect3DVertexShader9 *HardwareVertexShader_t;
typedef IDirect3DPixelShader9 *HardwarePixelShader_t;

typedef IDirect3DSwapChain9 IDirect3DSwapChain;

#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")

//////////////////////////////////////////////////////////////////////
#else // d3d8

#include <d3d8.h>
#include <d3dx8.h>
#include <d3dx8core.h>
#include <D3DX8Math.h>
#include <dxerr8.h>

struct IDirect3DTexture8;
struct IDirect3DBaseTexture8;
struct IDirect3DCubeTexture8;
struct IDirect3D8;
struct IDirect3DDevice8;
typedef IDirect3DTexture8 IDirect3DTexture;
typedef IDirect3DBaseTexture8 IDirect3DBaseTexture;
typedef IDirect3DCubeTexture8 IDirect3DCubeTexture;
typedef IDirect3DDevice8 IDirect3DDevice;
typedef IDirect3D8 IDirect3D;
typedef struct _D3DADAPTER_IDENTIFIER8 D3DADAPTER_IDENTIFIER8;
typedef D3DADAPTER_IDENTIFIER8 D3DADAPTER_IDENTIFIER;
typedef struct _D3DMATERIAL8 D3DMATERIAL8;
typedef D3DMATERIAL8 D3DMATERIAL;
typedef struct _D3DLIGHT8 D3DLIGHT8;
typedef D3DLIGHT8 D3DLIGHT;
struct IDirect3DSurface8;
typedef IDirect3DSurface8 IDirect3DSurface;
typedef struct _D3DCAPS8 D3DCAPS8;
typedef D3DCAPS8 D3DCAPS;
typedef struct _D3DVIEWPORT8 D3DVIEWPORT8;
typedef D3DVIEWPORT8 D3DVIEWPORT;
struct IDirect3DIndexBuffer8;
typedef IDirect3DIndexBuffer8 IDirect3DIndexBuffer;
struct IDirect3DVertexBuffer8;
typedef IDirect3DVertexBuffer8 IDirect3DVertexBuffer;

typedef DWORD IDirect3DVertexShader8;
typedef DWORD IDirect3DPixelShader8;

typedef IDirect3DVertexShader8 HardwareVertexShader_t;
typedef IDirect3DPixelShader8 HardwarePixelShader_t;

typedef IDirect3DSwapChain8 IDirect3DSwapChain;

#pragma comment(lib, "d3d8.lib")
#pragma comment(lib, "d3dx8.lib")

#endif

//-----------------------------------------------------------------------------
// The vertex and pixel shader type
//-----------------------------------------------------------------------------
typedef unsigned int VertexShader_t;
typedef unsigned int PixelShader_t;	

#define INVALID_PIXEL_SHADER	( ( PixelShader_t )0xFFFFFFFF )
#define INVALID_VERTEX_SHADER	( ( VertexShader_t )0xFFFFFFFF )


//-----------------------------------------------------------------------------
// Bitpattern for an invalid shader
//-----------------------------------------------------------------------------
#define INVALID_HARDWARE_VERTEX_SHADER ( ( HardwareVertexShader_t )NULL )
#define	INVALID_HARDWARE_PIXEL_SHADER ( ( HardwarePixelShader_t )NULL )

typedef IDirect3D* LPDIRECT3D;
typedef IDirect3DDevice *LPDIRECT3DDEVICE;
typedef IDirect3DIndexBuffer *LPDIRECT3DINDEXBUFFER;
typedef IDirect3DVertexBuffer *LPDIRECT3DVERTEXBUFFER;
typedef IDirect3DTexture *LPDIRECT3DTEXTURE;
typedef IDirect3DSurface *LPDIRECT3DSURFACE;
typedef IDirect3DSwapChain *LPDIRECT3DSWAPCHAIN;

typedef unsigned int PixelShader_t;
typedef unsigned int VertexShader_t;

#endif // LOCALD3DTYPES_H
