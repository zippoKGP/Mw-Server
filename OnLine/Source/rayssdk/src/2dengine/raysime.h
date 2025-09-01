/*
**  raysime.h
**  rays ime functions header.
**  
**  Jack, 2002.5.16.
*/
#ifndef _RAYSIME_H_
#define _RAYSIME_H_
#include <imm.h>
#pragma comment(lib, "imm32.lib")


// DEFINES /////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WM_IME_STRING
#define WM_IME_STRING       (WM_USER+999)   //�û�����ĺ����ִ���Ϣ
#endif//WM_IME_STRING

// STRUCTURES //////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct  tagIME_DATA
{
    bool    g_bIme;                     //IME�����־
	char    g_szCompStr[MAX_PATH];      //�洢ת����Ĵ�
	char    g_szCompReadStr[MAX_PATH];  //�洢����Ĵ�
	char    g_szCandList[MAX_PATH];     //�洢������ִ�ѡ�ֱ�
	int     g_nImeCursor;               //�洢ת����Ĵ��е��α�λ��
	char    g_szImeName[64];            //�洢���뷨������
	bool    g_bImeSharp;                //ȫ�α�־
	bool    g_bImeSymbol;               //���ı���־
    char    g_szChineStr[MAX_PATH];     //�洢����ĺ��ִ�
	CANDIDATELIST *g_lpCandList;        //�洢��׼��ѡ�ֱ�
} IME_DATA, *LPIME_DATA;


// GLOBALS /////////////////////////////////////////////////////////////////////////////////////////////////////////
extern  IME_DATA    *app_ime;


// FUNCTIONS ///////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  int     FNBACK  init_ime(void);
EXPORT  void    FNBACK  active_ime(int active);
EXPORT  void    FNBACK  free_ime(void);
EXPORT  void    FNBACK  disable_ime(void);
//
EXPORT  void    FNBACK  disable_ime(void);
EXPORT  void    FNBACK  enable_ime(void);
EXPORT  void    FNBACK  next_ime(void);
EXPORT  void    FNBACK  prev_ime(void);
EXPORT  void    FNBACK  sharp_ime( HWND hWnd );
EXPORT  void    FNBACK  symbol_ime( HWND hWnd );
EXPORT  char*   FNBACK  get_ime_name(void);
EXPORT  bool    FNBACK  is_ime_sharp(void);
EXPORT  bool    FNBACK  is_ime_symbol(void);
EXPORT  bool    FNBACK  get_ime_input( char **pszCompStr, char **pszCompReadStr, int *pnImeCursor, char **pszCandList );
EXPORT  bool    FNBACK  get_ime_result_chine_str(char *buffer);
//
EXPORT  bool    FNBACK  ime_on_WM_IME_SETCONTEXT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_INPUTLANGCHANGEREQUEST(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_INPUTLANGCHANGE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_IME_SETCONTEXT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_IME_STARTCOMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_IME_ENDCOMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_IME_NOTIFY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
EXPORT  bool    FNBACK  ime_on_WM_IME_COMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam );
//
EXPORT  void    FNBACK  redraw_ime(void *vbitmap);
EXPORT  void    FNBACK  main_game_redraw_ime(void *vbitmap);
EXPORT  void    FNBACK  redraw_ime(SLONG font_size, SLONG gap_xl, SLONG gap_yl, SLONG gap_cand, void *vbitmap);
//

#endif//_RAYSIME_H_

