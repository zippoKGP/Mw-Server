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


//关闭并禁止输入法，如 IME 已经打开则关闭，此后玩家不能用热键呼出 IME.
EXPORT  void    FNBACK  disable_ime(void)
{
    if(! app_ime)
        return;

	while( ImmIsIME( GetKeyboardLayout( 0 )))
    {
        //如果 IME 打开通过回圈切换到下一个关闭
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


//允许输入法，此后玩家可以用热键呼出 IME.
EXPORT  void    FNBACK  enable_ime(void)
{
    if(! app_ime)
        return;
    app_ime->g_bIme = true;
}


//切换到下一种输入法，必须 enable_ime 后才有效.
EXPORT  void    FNBACK  next_ime(void)
{
    if(! app_ime)
        return;

	if( app_ime->g_bIme )
    {
        ActivateKeyboardLayout(( HKL )HKL_NEXT, 0 );
    }
}


//切换到上一种输入法，必须 enable_ime 后才有效.
EXPORT  void    FNBACK  prev_ime(void)
{
    if(! app_ime)
        return;

	if( app_ime->g_bIme )
    {
        ActivateKeyboardLayout(( HKL )HKL_PREV, 0 );
    }
}


//切换全形/半形
EXPORT  void    FNBACK  sharp_ime( HWND hWnd )  
{
    if(! app_ime)
        return;

	ImmSimulateHotKey( hWnd, IME_CHOTKEY_SHAPE_TOGGLE );
}


//切换中/英文标点.
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


//是否全形
EXPORT  bool    FNBACK  is_ime_sharp(void)
{
    if(! app_ime)
        return  false;

	return app_ime->g_bImeSharp;
}


//是否中文标点
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

	return !app_ime->g_bIme; //如果禁止ime则返回false，此时视窗函数应返回0，否则DefWindowProc会打开输入法
}


EXPORT  bool    FNBACK  ime_on_WM_INPUTLANGCHANGE(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
    HKL hKL;

    if(! app_ime)
	{
		//ime改变
		hKL = GetKeyboardLayout(0);
		
		if(!ImmIsIME( hKL ))
		{
			g_bImeActive = false;
		}

        return  false;
	}

	//ime改变
	hKL = GetKeyboardLayout(0);

	if( ImmIsIME( hKL ))
    {
		HIMC hIMC = ImmGetContext( hWnd );

		ImmEscape( hKL, hIMC, IME_ESC_IME_NAME, app_ime->g_szImeName );//取得新输入法名字

		DWORD dwConversion, dwSentence;
		ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
		app_ime->g_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false; //取得全形标志
		app_ime->g_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;   //取得中文标点标志
		ImmReleaseContext( hWnd, hIMC );
	}
	else//英文输入
    {
		app_ime->g_szImeName[ 0 ] = 0;
    }
	return false;   //总是返回false，因为需要视窗函数调用DefWindowProc继续处理
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
    case IMN_SETCONVERSIONMODE://全形/半形，中/英文标点改变
        hIMC = ImmGetContext( hWnd );
        ImmGetConversionStatus( hIMC, &dwConversion, &dwSentence );
        app_ime->g_bImeSharp = ( dwConversion & IME_CMODE_FULLSHAPE )? true : false;
        app_ime->g_bImeSymbol = ( dwConversion & IME_CMODE_SYMBOL )? true : false;
        ImmReleaseContext( hWnd, hIMC );
        break;
    case IMN_OPENCANDIDATE://进入选字状态
		              
    case IMN_CHANGECANDIDATE://选字表翻页
        hIMC = ImmGetContext( hWnd );
        if( app_ime->g_lpCandList )
        {
            free( app_ime->g_lpCandList );
            app_ime->g_lpCandList = NULL;
        }//释放以前的选字表
        
        if( dwSize = ImmGetCandidateList( hIMC, 0, NULL, 0 ))
        {
            app_ime->g_lpCandList = (LPCANDIDATELIST)malloc( dwSize );
            if( app_ime->g_lpCandList )
                ImmGetCandidateList( hIMC, 0, app_ime->g_lpCandList, dwSize );
        }//得到新的选字表
        ImmReleaseContext( hWnd, hIMC );
        if( app_ime->g_lpCandList )
        {
            //选字表整理成串
            convert_candidate_list( app_ime->g_lpCandList, app_ime->g_szCandList );
        }
        break;
    case IMN_CLOSECANDIDATE://关闭选字表
        if( app_ime->g_lpCandList )
        {
            free( app_ime->g_lpCandList );
            app_ime->g_lpCandList = NULL;
        }//释放
        app_ime->g_szCandList[0] = 0;

        break;			
    }
    return true;//总是返回true，防止ime窗口打开
}


//输入改变
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
    }//取得szCompStr
    
    if( lParam & GCS_COMPREADSTR )
    {
        dwSize = ImmGetCompositionString( hIMC, GCS_COMPREADSTR, 
            (void*)app_ime->g_szCompReadStr, sizeof( app_ime->g_szCompReadStr ));
        app_ime->g_szCompReadStr[dwSize] = 0;
    }//取得szCompReadStr
    
    if( lParam & GCS_CURSORPOS )
    {
        app_ime->g_nImeCursor = 0xffff & ImmGetCompositionString( hIMC, GCS_CURSORPOS, NULL, 0 );
    }//取得nImeCursor
    
    if( lParam & GCS_RESULTSTR )
    {
        dwSize = ImmGetCompositionString( hIMC, GCS_RESULTSTR, (void*)str, sizeof( str ));//取得汉字输入串
        str[dwSize] = 0;
        strcpy((char *)app_ime->g_szChineStr, (const char *)str);   //将输入的汉字串存储起来
        SendMessage( hWnd, WM_IME_STRING, (WPARAM)str, dwSize);
    }
    ImmReleaseContext( hWnd, hIMC );
    
    return true;//总是返回true，防止ime窗口打开
}



//转换 CandidateList 到一个串，\t分隔每一项.
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
    //待选字序号超出总数，微软拼音第二次到选字表最后一页后再按PageDown会出现这种情况，并且会退出选字状态，开始一个新的输入
    //但微软拼音自己的ime窗口可以解决这个问题，估计微软拼音实现了更多的介面，所以使用了这种不太标准的资料
    //我现在无法解决这个问题，而且实际使用中也很少遇到这种事，而且其他标准输入法不会引起这种bug
    //非标准输入法估计实现的介面比较少，所以应该也不会引起这种bug
    //Jack, 2002.5.17.
    /*
	for( i = 0; ( i < pCandList->dwCount - pCandList->dwSelection ) && ( i < pCandList->dwPageSize ); i++ )
    {
		*pszCandList++ = ( i % 10 != 9 ) ? i % 10 + '1' : '0';//每项对应的数位键
		*pszCandList++ = '.';   //用'.'分隔
		strcpy( pszCandList, (char*)pCandList + pCandList->dwOffset[ pCandList->dwSelection + i ] );//每项实际的内容
		pszCandList += strlen( pszCandList );
		*pszCandList++ = '\t';//项之间以'\t'分隔
	}
	*( pszCandList - 1 )= 0;//串尾，并覆盖最后一个'\t'
    */
    for(i = 0; i < min(pCandList->dwCount - pCandList->dwPageStart, pCandList->dwPageSize); i++)
    {
		*pszCandList++ = ( i % 10 != 9 ) ? i % 10 + '1' : '0';//每项对应的数位键
		*pszCandList++ = '.';   //用'.'分隔
		strcpy( pszCandList, (char*)pCandList + pCandList->dwOffset[ pCandList->dwPageStart + i ] );//每项实际的内容
		pszCandList += strlen( pszCandList );
		*pszCandList++ = '\t';//项之间以'\t'分隔
    }
	*( pszCandList - 1 )= 0;//串尾，并覆盖最后一个'\t'
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
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "全角" : "半角");
            len = strlen((const char *)info);
            disp_x = bar_sx + bar_xl - len * IME_FONT_SIZE / 2 - 8;
            disp_y = bar_sy + bar_yl / 2 - IME_FONT_SIZE / 2;
            print24( disp_x, disp_y,  (USTR*)info, PEST_PUT, bitmap);
            //sprintf((char *)info, "%s", is_ime_symbol() ? "中文标点" : "英文标点");
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
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "全角" : "半角");
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



//功能 : 将IME的输入及文字选择等显示在指定缓冲中.
//参数 : SLONG  font_size       ->  显示文字的大小, 可以取12,16,20,24,28,32.
//       SLONG  gap_xl          ->  文字选择列中文字和文字背景区左右边界的间隙
//       SLONG  gap_yl          ->  文字选择列中文字和文字背景区上下边界的间隙
//       SLONG  gap_cand        ->  文字选择列中每个选择项目之间的间隙
//       void * vbitmap         ->  目标缓冲区指标.
//返回 : 无.
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
            sprintf((char *)info, "%s/%s", get_ime_name(), is_ime_sharp() ? "全角" : "半角");
            len = strlen((const char *)info);
            disp_x = bar_sx + bar_xl - len * font_size / 2 - gap_xl;
            disp_y = bar_sy + bar_yl / 2 - font_size / 2;
            print_range_640x480x16M( disp_x, disp_y,  (USTR*)info, PEST_PUT, bitmap);
            //sprintf((char *)info, "%s", is_ime_symbol() ? "中文标点" : "英文标点");
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


