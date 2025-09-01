/*
**  raysime.cpp
**  rays ime functions.
**  
**  Jack, 2002.5.16.
**
**  Referenced:   http://www.gameres.com/Control/ime.htm
**  
*/
#include "rays.h"
#include "xgrafx.h"
#include "winfont.h"
#include "raysime.h"
#include "xinput.h"

IME_DATA    *app_ime = NULL;

static  void    convert_candidate_list( CANDIDATELIST *pCandList, char *pszCandList );

BOOL		g_bImeActive = false;
BOOL        g_bImeCandidate = false;    //modified by coolly

//////////////////////////////////////////////////////////////////////////////////////////////////////////
// FUNCTIONS                                                                                            //
//////////////////////////////////////////////////////////////////////////////////////////////////////////
EXPORT  int     FNBACK  init_ime(void)
{
    if(NULL == (app_ime = (IME_DATA*)malloc(sizeof(IME_DATA))))
        return  -1;
    app_ime->g_lpCandList = NULL;
    disable_ime();

    return  0;
}


EXPORT  void    FNBACK  active_ime(int active)
{
}


EXPORT  void    FNBACK  free_ime(void)
{
    if(app_ime)
    {
        disable_ime();
        if( app_ime->g_lpCandList )
        {
            free( app_ime->g_lpCandList );
            app_ime->g_lpCandList = NULL;
        }
        free(app_ime);
    }
    app_ime = NULL;
}


//�رղ���ֹ���뷨���� IME �Ѿ�����رգ��˺���Ҳ������ȼ����� IME.
EXPORT  void    FNBACK  disable_ime(void)
{
    if(! app_ime)
        return;

	while( ImmIsIME( GetKeyboardLayout( 0 )))
    {
        //��� IME ��ͨ����Ȧ�л�����һ���ر�
		ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );    
    }
	app_ime->g_bIme = false;
	app_ime->g_szImeName[0] = 0;
	app_ime->g_szCompStr[0] = 0;
	app_ime->g_szCompReadStr[0] = 0;
	app_ime->g_szCandList[0] = 0;
    app_ime->g_szChineStr[0] = 0;
	app_ime->g_nImeCursor = 0;
}


//�������뷨���˺���ҿ������ȼ����� IME.
EXPORT  void    FNBACK  enable_ime(void)
{
    if(! app_ime)
        return;
    app_ime->g_bIme = true;
}


//�л�����һ�����뷨������ enable_ime �����Ч.
EXPORT  void    FNBACK  next_ime(void)
{
    if(! app_ime)
        return;

	if( app_ime->g_bIme )
    {
        ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );
    }
}


//�л�����һ�����뷨������ enable_ime �����Ч.
EXPORT  void    FNBACK  prev_ime(void)
{
    if(! app_ime)
        return;

	if( app_ime->g_bIme )
    {
        ActivateKeyboardLayout(( HKL )HKL_PREV, 0 );
    }
}


//�л�ȫ��/����
EXPORT  void    FNBACK  sharp_ime( HWND hWnd )  
{
    if(! app_ime)
        return;

	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE );
}


//�л���/Ӣ�ı��.
EXPORT  void    FNBACK  symbol_ime( HWND hWnd )
{
    if(! app_ime)
        return;

	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SYMBOL_TOGGLE );
}


EXPORT  char*   FNBACK  get_ime_name(void)
{
    if(! app_ime)
        return  NULL;

	return app_ime->g_szImeName[0] ? (char *)app_ime->g_szImeName : NULL;
}


//�Ƿ�ȫ��
EXPORT  bool    FNBACK  is_ime_sharp(void)
{
    if(! app_ime)
        return  false;

	return app_ime->g_bImeSharp;
}


//�Ƿ����ı��
EXPORT  bool    FNBACK  is_ime_symbol(void)
{
    if(! app_ime)
        return  false;

	return app_ime->g_bImeSymbol;
}


EXPORT  bool    FNBACK  get_ime_input( char **pszCompStr, char **pszCompReadStr, int *pnImeCursor, char **pszCandList )
{
    if(! app_ime)
        return  false;

	if( pszCompStr ) *pszCompStr = app_ime->g_szCompStr;
	if( pszCompReadStr ) *pszCompReadStr = app_ime->g_szCompReadStr;
	if( pnImeCursor ) *pnImeCursor = app_ime->g_nImeCursor;
	if( pszCandList ) *pszCandList = app_ime->g_szCandList;

    return  true;
}



EXPORT  bool    FNBACK  get_ime_result_chine_str(char *buffer)
{
    if(! app_ime)
        return  false;

    if(app_ime->g_szChineStr[0])
    {
        strcpy((char *)buffer, (const char *)app_ime->g_szChineStr);
        app_ime->g_szChineStr[0] = 0;
        return  true;
    }

    return  false;
}



EXPORT  bool    FNBACK  ime_on_WM_IME_SETCONTEXT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	if (lParam == ISC_SHOWUIALLCANDIDATEWINDOW && !wParam)
	{
		g_bImeActive = wParam;
	}

	if(! app_ime)
        return  false;

    return  true;
}


EXPORT  bool    FNBACK  ime_on_WM_IME_STARTCOMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	g_bImeActive = true;

    if(! app_ime)
        return  false;

    return  true;
}


EXPORT  bool    FNBACK  ime_on_WM_IME_ENDCOMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{ 
	g_bImeActive = false;
	
	reset_data_key();

    if(! app_ime)
        return  false;

    return  true;
}


EXPORT  bool    FNBACK  ime_on_WM_INPUTLANGCHANGEREQUEST(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    if(! app_ime)
        return  false;

	return !app_ime->g_bIme; //�����ֹime�򷵻�false����ʱ�Ӵ�����Ӧ����0������DefWindowProc������뷨
}


EXPORT  bool    FNBACK  ime_on_WM_INPUTLANGCHANGE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HKL hKL;

    if(! app_ime)
	{
		//ime�ı�
		hKL = GetKeyboardLayout(0);
		
		if(!ImmIsIME( hKL ))
		{
			g_bImeActive = false;
		}

        return  false;
	}

	//ime�ı�
	hKL = GetKeyboardLayout(0);

	if( ImmIsIME( hKL ))
    {
		HIMC hIMC = ImmGetContext( hWnd );

		ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, app_ime->g_szImeName );//ȡ�������뷨����

		DWORD dwConversion, dwSentence;
		ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
		app_ime->g_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false; //ȡ��ȫ�α�־
		app_ime->g_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;   //ȡ�����ı���־
		ImmReleaseContext( hWnd, hIMC );
	}
	else//Ӣ������
    {
		app_ime->g_szImeName[ 0 ] = 0;
    }
	return false;   //���Ƿ���false����Ϊ��Ҫ�Ӵ���������DefWindowProc��������
}


EXPORT  bool    FNBACK  ime_on_WM_IME_NOTIFY(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HIMC    hIMC;
    DWORD   dwSize;
    DWORD   dwConversion, dwSentence;
    if(wParam == IMN_CLOSECANDIDATE)
	{
		g_bImeCandidate = false;            //modified by coolly		
	}
	if(wParam == IMN_OPENCANDIDATE)
	{
       g_bImeCandidate = true;             //modified by coolly
	}
    if(! app_ime)
	{
		if (wParam == IMN_CLOSESTATUSWINDOW)
		{
			g_bImeActive = false;
			
			reset_data_key();			
		}
		
		return  false;
	}


    switch( wParam )
    {
    case IMN_SETCONVERSIONMODE://ȫ��/���Σ���/Ӣ�ı��ı�
        hIMC = ImmGetContext( hWnd );
        ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
        app_ime->g_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
        app_ime->g_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
        ImmReleaseContext( hWnd, hIMC );
        break;
    case IMN_OPENCANDIDATE://����ѡ��״̬
		              
    case IMN_CHANGECANDIDATE://ѡ�ֱ�ҳ
        hIMC = ImmGetContext( hWnd );
        if( app_ime->g_lpCandList )
        {
            free( app_ime->g_lpCandList );
            app_ime->g_lpCandList = NULL;
        }//�ͷ���ǰ��ѡ�ֱ�
        
        if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
        {
            app_ime->g_lpCandList = (LPCANDIDATELIST)malloc( dwSize );
            if( app_ime->g_lpCandList )
                ImmGetCandidateList( hIMC, 0, app_ime->g_lpCandList, dwSize );
        }//�õ��µ�ѡ�ֱ�
        ImmReleaseContext( hWnd, hIMC );
        if( app_ime->g_lpCandList )
        {
            //ѡ�ֱ�����ɴ�
            convert_candidate_list( app_ime->g_lpCandList, app_ime->g_szCandList );
        }
        break;
    case IMN_CLOSECANDIDATE://�ر�ѡ�ֱ�
        if( app_ime->g_lpCandList )
        {
            free( app_ime->g_lpCandList );
            app_ime->g_lpCandList = NULL;
        }//�ͷ�
        app_ime->g_szCandList[0] = 0;

        break;			
    }
    return true;//���Ƿ���true����ֹime���ڴ�
}


//����ı�
EXPORT  bool    FNBACK  ime_on_WM_IME_COMPOSITION(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    static  unsigned char str[MAX_PATH];
    HIMC hIMC;
    DWORD dwSize;
    
    if(! app_ime)
        return  false;

    hIMC = ImmGetContext( hWnd );
    if( lParam & GCS_COMPSTR )
    {
        dwSize = ImmGetCompositionString( hIMC, GCS_COMPSTR, 
            (void*)app_ime->g_szCompStr, sizeof( app_ime->g_szCompStr ));
        app_ime->g_szCompStr[dwSize] = 0;
    }//ȡ��szCompStr
    
    if( lParam & GCS_COMPREADSTR )
    {
        dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, 
            (void*)app_ime->g_szCompReadStr, sizeof( app_ime->g_szCompReadStr ));
        app_ime->g_szCompReadStr[dwSize] = 0;
    }//ȡ��szCompReadStr
    
    if( lParam & GCS_CURSORPOS )
    {
        app_ime->g_nImeCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
    }//ȡ��nImeCursor
    
    if( lParam & GCS_RESULTSTR )
    {
        dwSize = ImmGetCompositionString( hIMC, GCS_RESULTSTR, (void*)str, sizeof( str ));//ȡ�ú������봮
        str[dwSize] = 0;
        strcpy((char *)app_ime->g_szChineStr, (const char *)str);   //������ĺ��ִ��洢����
        SendMessage( hWnd, WM_IME_STRING, (WPARAM)str, dwSize);
    }
    ImmReleaseContext( hWnd, hIMC );
    
    return true;//���Ƿ���true����ֹime���ڴ�
}



//ת�� CandidateList ��һ������\t�ָ�ÿһ��.
static  void    convert_candidate_list( CANDIDATELIST *pCandList, char *pszCandList )
{
	unsigned int i;

    if(! app_ime)
        return;

	if( pCandList->dwCount < pCandList->dwSelection )
    {
		pszCandList[0] = 0;
		return;
	}
    //��ѡ����ų���������΢��ƴ���ڶ��ε�ѡ�ֱ����һҳ���ٰ�PageDown�����������������һ��˳�ѡ��״̬����ʼһ���µ�����
    //��΢��ƴ���Լ���ime���ڿ��Խ��������⣬����΢��ƴ��ʵ���˸���Ľ��棬����ʹ�������ֲ�̫��׼������
    //�������޷����������⣬����ʵ��ʹ����Ҳ�������������£�����������׼���뷨������������bug
    //�Ǳ�׼���뷨����ʵ�ֵĽ���Ƚ��٣�����Ӧ��Ҳ������������bug
    //Jack, 2002.5.17.
    /*
	for( i = 0; ( i < pCandList->dwCount - pCandList->dwSelection ) && ( i < pCandList->dwPageSize ); i++ )
    {
		*pszCandList++ = ( i % 10 != 9 ) ? i % 10 + '1' : '0';//ÿ���Ӧ����λ��
		*pszCandList++ = '.';   //��'.'�ָ�
		strcpy( pszCandList, (char*)pCandList + pCandList->dwOffset[ pCandList->dwSelection + i ] );//ÿ��ʵ�ʵ�����
		pszCandList += strlen( pszCandList );
		*pszCandList++ = '\t';//��֮����'\t'�ָ�
	}
	*( pszCandList - 1 )= 0;//��β�����������һ��'\t'
    */
    for(i = 0; i < min(pCandList->dwCount - pCandList->dwPageStart, pCandList->dwPageSize); i++)
    {
		*pszCandList++ = ( i % 10 != 9 ) ? i % 10 + '1' : '0';//ÿ���Ӧ����λ��
		*pszCandList++ = '.';   //��'.'�ָ�
		strcpy( pszCandList, (char*)pCandList + pCandList->dwOffset[ pCandList->dwPageStart + i ] );//ÿ��ʵ�ʵ�����
		pszCandList += strlen( pszCandList );
		*pszCandList++ = '\t';//��֮����'\t'�ָ�
    }
	*( pszCandList - 1 )= 0;//��β�����������һ��'\t'
}



/******************************************************************************************************************/
/* REDRAW IME FUNCTIONS                                                                                           */
/******************************************************************************************************************/
EXPORT  void    FNBACK  redraw_ime(void *vbitmap)
{
    enum    {   IME_FONT_SIZE       =   24   };
    BMP     *bitmap = (BMP*)vbitmap;
    CANDIDATELIST *pCandList = NULL;
    char    *szCompStr = NULL, *szCompReadStr = NULL, *szCandList = NULL;
    SLONG   bar_sx, bar_sy, bar_xl, bar_yl;
    SLONG   disp_x, disp_y, disp_xl, disp_yl;
    char    info[256], tmp[80];
    SLONG   i, len, index;
    bool    result;

    if(! app_ime)
        return;

    if( get_ime_name() )
    {
        result = get_ime_input( &szCompStr, &szCompReadStr, NULL, &szCandList );
        if(result)
        {
            //(1) background ----------------------------------------------------
            bar_sx = 0;
            bar_sy = bitmap->h - IME_FONT_SIZE - 16;
            bar_xl = bitmap->w;
            bar_yl = IME_FONT_SIZE + 16;
            alpha_put_bar(bar_sx, bar_sy, bar_xl, bar_yl, SYSTEM_BLUE, bitmap, 128);
            
            //set font color
            print16(0, 0, "~C0", PEST_PUT, bitmap);
            
            //(2) ime name ------------------------------------------------------
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "ȫ��" : "���");
            len = strlen((const char *)info);
            disp_x = bar_sx + bar_xl - len * IME_FONT_SIZE / 2 - 8;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            print24( disp_x, disp_y,  (USTR*)info, PEST_PUT, bitmap);
            //sprintf((char *)info, "%s", is_ime_symbol() ? "���ı��" : "Ӣ�ı��");
            //print16( IME_SYMBOL_SX, IME_SYMBOL_SY, (USTR*)info, PEST_PUT, bitmap);
            
            //(3) cand list -----------------------------------------------------
            /*
            if(szCandList)
            {
            disp_x = bar_sx + 8;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            print24( disp_x, disp_y, (USTR*)szCandList, PEST_PUT, bitmap);
            }
            */
            disp_x = bar_sx + 8;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            pCandList = app_ime->g_lpCandList;
            if(pCandList)
            {
                for(i = 0; i < min((SLONG)pCandList->dwCount - (SLONG)pCandList->dwPageStart, (SLONG)pCandList->dwPageSize); i++)
                {
                    index = pCandList->dwPageStart+i;
                    sprintf((char *)info, "%d.%s", 1 + i, (char*)pCandList + pCandList->dwOffset[index]);
                    len = strlen((const char *)info);
                    
                    if(index == (SLONG)pCandList->dwSelection)
                        sprintf((char *)tmp, "~C4~O3%s~C0~O0", info);
                    else
                        sprintf((char *)tmp, "%s", info);
                    print24(disp_x, disp_y, (USTR*)tmp, PEST_PUT, bitmap);
                    disp_x += 8 + len * IME_FONT_SIZE / 2;
                }
            }
            
            
            //(4) comp str ------------------------------------------------------
            if(szCompStr)
            {
                len = strlen((const char *)szCompStr);
                disp_x = bar_sx + 8;
                disp_y = bar_sy - IME_FONT_SIZE - 8;
                disp_xl = len * IME_FONT_SIZE / 2;
                disp_yl = IME_FONT_SIZE;
                alpha_put_bar(disp_x - 8, disp_y - 8, disp_xl + 16, disp_yl + 16, SYSTEM_BLUE, bitmap, 128);
                print24( disp_x, disp_y, (USTR*)szCompStr, PEST_PUT, bitmap);
            }
            
            //(4) comp read str -------------------------------------------------
            //print16( IME_INPUT_COMP_READ_SX, IME_INPUT_COMP_READ_SY, (USTR*)szCompReadStr, PEST_PUT, bitmap);
            
            //restore font color.
            print16(0, 0, "~C0", PEST_PUT, bitmap);
        }
    }
}


EXPORT  void    FNBACK  main_game_redraw_ime(void *vbitmap)
{
    enum    {   IME_FONT_SIZE       =   12   };
    BMP     *bitmap = (BMP*)vbitmap;
    CANDIDATELIST *pCandList = NULL;
    char    *szCompStr = NULL, *szCompReadStr = NULL, *szCandList = NULL;
    SLONG   bar_sx, bar_sy, bar_xl, bar_yl;
    SLONG   disp_x, disp_y, disp_xl, disp_yl;
    char    info[256], tmp[80];
    SLONG   i, len, index;
    bool    result;

    if(! app_ime)
        return;

    if( get_ime_name() )
    {
        result = get_ime_input( &szCompStr, &szCompReadStr, NULL, &szCandList );
        if(result)
        {
            //(1) background ----------------------------------------------------
            bar_sx = 232;
            bar_sy = bitmap->h - IME_FONT_SIZE - 8;
            bar_xl = bitmap->w - bar_sx;
            bar_yl = bitmap->h - bar_sy; 
            alpha_put_bar(bar_sx, bar_sy, bar_xl, bar_yl, SYSTEM_BLUE, bitmap, 128);
            
            //set font color
            print12(0, 0, "~C0", PEST_PUT, bitmap);
            
            //(2) ime name ------------------------------------------------------
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "ȫ��" : "���");
            len = strlen((const char *)info);
            disp_x = bar_sx + bar_xl - len * IME_FONT_SIZE / 2 - 4;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            print12( disp_x, disp_y,  (USTR*)info, PEST_PUT, bitmap);
            
            //(3) cand list -----------------------------------------------------
            disp_x = bar_sx + 8;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            pCandList = app_ime->g_lpCandList;
            if(pCandList)
            {
                for(i = 0; i < min((SLONG)pCandList->dwCount - (SLONG)pCandList->dwPageStart, (SLONG)pCandList->dwPageSize); i++)
                {
                    index = pCandList->dwPageStart+i;
                    sprintf((char *)info, "%d%s", 1 + i, (char*)pCandList + pCandList->dwOffset[index]);
                    len = strlen((const char *)info);
                    
                    if(index == (SLONG)pCandList->dwSelection)
                        sprintf((char *)tmp, "~C4~O3%s~C0~O0", info);
                    else
                        sprintf((char *)tmp, "%s", info);
                    print12(disp_x, disp_y, (USTR*)tmp, PEST_PUT, bitmap);
                    disp_x += 4 + len * IME_FONT_SIZE / 2;
                }
            }
            
            
            //(4) comp str ------------------------------------------------------
            if(szCompStr)
            {
                len = strlen((const char *)szCompStr);
                disp_x = bar_sx + 8;
                disp_y = bar_sy - IME_FONT_SIZE - 8;
                disp_xl = len * IME_FONT_SIZE / 2;
                disp_yl = IME_FONT_SIZE;
                alpha_put_bar(disp_x - 8, disp_y - 8, disp_xl + 16, disp_yl + 16, SYSTEM_BLUE, bitmap, 128);
                print12( disp_x, disp_y, (USTR*)szCompStr, PEST_PUT, bitmap);
            }
            
            //(4) comp read str -------------------------------------------------
            //print16( IME_INPUT_COMP_READ_SX, IME_INPUT_COMP_READ_SY, (USTR*)szCompReadStr, PEST_PUT, bitmap);
            
            //restore font color.
            print12(0, 0, "~C0", PEST_PUT, bitmap);
        }
    }
}



//���� : ��IME�����뼰����ѡ�����ʾ��ָ��������.
//���� : SLONG  font_size       ->  ��ʾ���ֵĴ�С, ����ȡ12,16,20,24,28,32.
//       SLONG  gap_xl          ->  ����ѡ���������ֺ����ֱ��������ұ߽�ļ�϶
//       SLONG  gap_yl          ->  ����ѡ���������ֺ����ֱ��������±߽�ļ�϶
//       SLONG  gap_cand        ->  ����ѡ������ÿ��ѡ����Ŀ֮��ļ�϶
//       void * vbitmap         ->  Ŀ�껺����ָ��.
//���� : ��.
EXPORT  void    FNBACK  redraw_ime(SLONG font_size, SLONG gap_xl, SLONG gap_yl, SLONG gap_cand, void *vbitmap)
{
    BMP     *bitmap = (BMP*)vbitmap;
    CANDIDATELIST *pCandList = NULL;
    char    *szCompStr = NULL, *szCompReadStr = NULL, *szCandList = NULL;
    SLONG   bar_sx, bar_sy, bar_xl, bar_yl;
    SLONG   disp_x, disp_y, disp_xl, disp_yl;
    char    info[256], tmp[80];
    SLONG   i, len, index;
    bool    result;

    if(! app_ime)
        return;

    if( get_ime_name() )
    {
        result = get_ime_input( &szCompStr, &szCompReadStr, NULL, &szCandList );
        if(result)
        {
            //(1) background ----------------------------------------------------
            bar_sx = 0;
            bar_sy = bitmap->h - font_size - gap_yl * 2;
            bar_xl = bitmap->w;
            bar_yl = font_size + gap_yl * 2;
            alpha_put_bar(bar_sx, bar_sy, bar_xl, bar_yl, SYSTEM_BLUE, bitmap, 128);
            
            //set font color
            if(font_size <= 12)
                print_range_640x480x16M(0, 0, (USTR *)"~Z0~C0", PEST_PUT, bitmap);
            else if(font_size <= 16)
                print_range_640x480x16M(0, 0, (USTR *)"~Z1~C0", PEST_PUT, bitmap);
            else if(font_size <= 20)
                print_range_640x480x16M(0, 0, (USTR *)"~Z2~C0", PEST_PUT, bitmap);
            else if(font_size <= 24)
                print_range_640x480x16M(0, 0, (USTR *)"~Z3~C0", PEST_PUT, bitmap);
            else if(font_size <= 28)
                print_range_640x480x16M(0, 0, (USTR *)"~Z4~C0", PEST_PUT, bitmap);
            else
                print_range_640x480x16M(0, 0, (USTR *)"~Z5~C0", PEST_PUT, bitmap);
            
            //(2) ime name ------------------------------------------------------
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "ȫ��" : "���");
            len = strlen((const char *)info);
            disp_x = bar_sx + bar_xl - len * font_size / 2 - gap_xl;
            disp_y = bar_sy + bar_yl / 2 - font_size / 2;
            print_range_640x480x16M( disp_x, disp_y,  (USTR*)info, PEST_PUT, bitmap);
            //sprintf((char *)info, "%s", is_ime_symbol() ? "���ı��" : "Ӣ�ı��");
            //print16( IME_SYMBOL_SX, IME_SYMBOL_SY, (USTR*)info, PEST_PUT, bitmap);
            
            //(3) cand list -----------------------------------------------------
            disp_x = bar_sx + gap_xl;
            disp_y = bar_sy + bar_yl / 2 - font_size / 2;
            pCandList = app_ime->g_lpCandList;
            if(pCandList)
            {
                for(i = 0; i < min((SLONG)pCandList->dwCount - (SLONG)pCandList->dwPageStart, (SLONG)pCandList->dwPageSize); i++)
                {
                    index = pCandList->dwPageStart+i;
                    sprintf((char *)info, "%d.%s", 1 + i, (char*)pCandList + pCandList->dwOffset[index]);
                    len = strlen((const char *)info);
                    
                    if(index == (SLONG)pCandList->dwSelection)
                    {
                        alpha_put_bar(disp_x-(gap_cand+1)/2, disp_y-(gap_yl+1)/2, len * font_size/2 + (gap_cand+1)/2*2, 
                            font_size+(gap_yl+1)/2*2, SYSTEM_WHITE, bitmap, 128);
                        sprintf((char *)tmp, "~C4~O8%s~C0~O0", info);
                        print_range_640x480x16M( disp_x, disp_y,  (USTR*)tmp, PEST_PUT, bitmap);
                    }
                    else
                    {
                        sprintf((char *)tmp, "%s", info);
                        print_range_640x480x16M( disp_x, disp_y,  (USTR*)tmp, PEST_PUT, bitmap);
                    }
                    disp_x += gap_cand + len * font_size / 2;
                }
            }
            
            
            //(4) comp str ------------------------------------------------------
            if(szCompStr)
            {
                len = strlen((const char *)szCompStr);
                disp_x = bar_sx + gap_xl;
                disp_y = bar_sy - font_size - gap_yl;
                disp_xl = len * font_size / 2;
                disp_yl = font_size;
                alpha_put_bar(disp_x - gap_xl, disp_y - gap_yl, disp_xl + gap_xl * 2, disp_yl + gap_yl * 2, 
                    SYSTEM_BLUE, bitmap, 128);
                print_range_640x480x16M( disp_x, disp_y,  (USTR*)szCompStr, PEST_PUT, bitmap);
            }
            
            //(4) comp read str -------------------------------------------------
            //print16( IME_INPUT_COMP_READ_SX, IME_INPUT_COMP_READ_SY, (USTR*)szCompReadStr, PEST_PUT, bitmap);
            
            //restore font color.
            print_range_640x480x16M(0, 0, (USTR *)"~Z0~C0", PEST_PUT, bitmap);
        }
    }
}


