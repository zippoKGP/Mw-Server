/*
**      WINFONT.CPP
**      windows font show functions.
**          
**      Dixon, 2000.3.13.
**          Created for 16Bit engine.
**      ZJian, 2001.1.12.
**          Added in.
**      ZJian, 2001.4.24.
**          Add COPY_PUT type for m_now_back_color control.
**      Dixon, 2001.9.18.
**          Modify the CreateFont weight para from FW_NORMAL to FW_BOLD.
**      Jack, 2002.4.10.
**          Add support check, GDI_FONT::m_support_system, we can combine
**              SUPPORT_FONT_SYSTEM_GB,
**              SUPPORT_FONT_SYSTEM_BIG5
**          to control the support systems.
**
*/
#include "rays.h"
#include "xsystem.h"
#include "xgrafx.h"
#include "winfont.h"
#include "xbig2gb.h"
#include "xgb2big.h"

//----------------------------------------------------------------------------


//------------------------------------------------------------------------------
GDI_FONT g_cFont;
USTR    font_name[128] = {0};


GDI_FONT    ::  GDI_FONT_CLASS  (void)
{
    for(SLONG i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        this->m_pabtBitMap[i]      = NULL;
        this->m_ahDc[i]            = NULL;
        this->m_ahFont[i]          = NULL;
        
        this->m_ahBmpSave[i]       = NULL;
        this->m_ahFontSave[i]      = NULL;
        this->m_ahBmp[i]           = NULL;
    } 
    this->m_lPitchBM            = FONT_BUFFER_WIDTH ;
    
    ////////////////////////////////////////////////
    m_now_front_color = 0;
    m_now_back_color = 0;
    //We will setup m_iaColor & m_iaBackColor later ...
    m_lSizeLevel    = 0;
    m_special       = 0;
    m_alpha_level   = 0;
    //
    //Jack, 2002.4.10.
    //默认支持繁体系统。
    m_support_system = SUPPORT_FONT_SYSTEM_GB;
    m_convert_big5togb = TRUE;
    m_weight = FW_BOLD;
	
    return;
}



GDI_FONT    ::  ~GDI_FONT_CLASS (void)
{
    /*
    for(SLONG i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        SelectPalette( m_ahDc[i], m_ahPalSave[i], FALSE ) ;
        SelectObject( m_ahDc[i], m_ahFontSave[i]) ;
        SelectObject( m_ahDc[i], m_ahBmpSave[i] ) ;
        
        DeleteObject( m_ahDc[i] ) ;
        DeleteObject( m_ahBmp[i]) ;
    }
    */
    for(SLONG i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        if( m_ahDc[i] )
        {
            SelectPalette( m_ahDc[i], m_ahPalSave[i], FALSE ) ;
            SelectObject( m_ahDc[i], m_ahFontSave[i]) ;
            SelectObject( m_ahDc[i], m_ahBmpSave[i] ) ;
            
            DeleteObject( m_ahDc[i] ) ;
            DeleteObject( m_ahBmp[i]) ;
        }
    }

    if(m_hPal)
    {
        DeleteObject( m_hPal );
    }

    for(i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        if(m_ahFont[i])
        {
            DeleteObject( m_ahFont[i]);
        }
    }
    
    /////////////////////////////
    if(this->font_bufinfo)
    {
        GlobalFree(this->font_bufinfo);
        font_bufinfo = NULL;
    }

    for(i=0;i<MAX_FONT_SIZE_TYPE;i++)
    { 
        this->m_pabtBitMap[i] = NULL;
    }
    if(this->font_cache)
    {
        GlobalFree(this->font_cache);
        this->font_cache = NULL;
    }
    return;
}

SLONG GDI_FONT      ::  MyCreateFont(USTR * pbFile)
{
    //USTR    fbFileName[40];
    for(SLONG i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        if(pbFile == NULL)
        {
            if(m_gb_flag)
            {
                m_ahFont[i] = CreateFont(
                    s_taFaceFontWH[i].lHeight,
                    s_taFaceFontWH[i].lWidth/2, 
                    0, 
                    0,
                    m_weight,   //FW_BOLD, //THIN, //FW_NORMAL,  //FW_BOLD,//FW_NORMAL,
                    FALSE, 
                    FALSE, 
                    FALSE,
                    GB2312_CHARSET, 
                    OUT_DEFAULT_PRECIS, 
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN , 
                    (const char *)pbFile ) ;
            }
            else  
            {
                m_ahFont[i] = CreateFont(
                    s_taFaceFontWH[i].lHeight,
                    s_taFaceFontWH[i].lWidth/2, 
                    0, 
                    0,
                    m_weight,   //FW_BOLD,//FW_NORMAL, 
                    FALSE, 
                    FALSE, 
                    FALSE,
                    CHINESEBIG5_CHARSET, 
                    OUT_DEFAULT_PRECIS, 
                    CLIP_DEFAULT_PRECIS,
                    DEFAULT_QUALITY,
                    FIXED_PITCH|FF_MODERN , 
                    (const char *)pbFile ) ;
            }
        }
        else
        {
            //strcpy((char *)fbFileName,FONT_NAME);
            m_ahFont[i] = CreateFont(
                s_taFaceFontWH[i].lHeight,
                s_taFaceFontWH[i].lWidth/2, 
                0, 
                0, 
                m_weight, //FW_BOLD, //FW_NORMAL, 
                FALSE, 
                FALSE,
                FALSE,
                DEFAULT_CHARSET , 
                OUT_DEFAULT_PRECIS, 
                CLIP_DEFAULT_PRECIS,
                DRAFT_QUALITY, 
                FIXED_PITCH|FF_MODERN , 
                (const char *)pbFile ) ;
        }
    }           
    return(TTN_OK);
}



SLONG GDI_FONT      ::  Initialize(void)
{
    LPBITMAPINFO pstBitMapInfo;
    LPLOGPALETTE pstLogPalette;
    BITMAP   stBitMap;
    SLONG lRet=TTN_OK;
    SLONG i;
    
    //check windows country
    if(is_gb_windows())
        m_gb_flag=0x01;

    font_bufinfo = (FONT_BUFFER_INFO *)GlobalAlloc(GPTR, sizeof(FONT_BUFFER_INFO)*MAX_FONT_NUM);
    if(font_bufinfo == NULL)
    {
        lRet = TTN_NOT_OK;
        goto EXIT;
    }
    memset(font_bufinfo,0,sizeof(FONT_BUFFER_INFO)*MAX_FONT_NUM);
    
    font_cache = (ULONG*)GlobalAlloc(GPTR, FONT_BUFFER_SIZE);
    if(font_cache == NULL)
    {
        lRet = TTN_OK;
        goto EXIT;
    }
    
    /////////////////////////////////////////////////////////////////////////////////////
    //Windows handle create
    pstLogPalette = (LPLOGPALETTE) GlobalAlloc(GPTR,    sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) );
    pstLogPalette->palVersion = 0x300;
    pstLogPalette->palNumEntries = 2;
    
    for(i = 0 ; i < 2 ; i++)
    {
        (DWORD&) pstLogPalette->palPalEntry[i] = (DWORD) i;
        pstLogPalette->palPalEntry[i].peFlags = PC_EXPLICIT;
    }
    
    m_hPal = CreatePalette( pstLogPalette ) ;
    
    for(i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        m_ahDc[i] = CreateCompatibleDC( NULL );
        if(m_ahDc[i]==NULL)
        {
            //ZJian,marked
            //error_message((USTR *)"Graph compatible bitmap ");
            goto EXIT;
        }
    }
    pstBitMapInfo = (LPBITMAPINFO) GlobalAlloc(GPTR,    sizeof(BITMAPINFO) + sizeof(RGBQUAD) );
    
    pstBitMapInfo->bmiHeader.biSize          = sizeof( BITMAPINFOHEADER ) ;
    pstBitMapInfo->bmiHeader.biWidth         = 32 ; //FONT_WIDTH_ZENKAKU ;
    pstBitMapInfo->bmiHeader.biHeight        = 0-32 ;//-s_taFontWH[m_lSizeLevel].lHeight ;
    pstBitMapInfo->bmiHeader.biPlanes        = 1 ;
    pstBitMapInfo->bmiHeader.biBitCount      = 1 ;
    pstBitMapInfo->bmiHeader.biCompression   = BI_RGB ;
    pstBitMapInfo->bmiHeader.biSizeImage     = 0 ;
    pstBitMapInfo->bmiHeader.biXPelsPerMeter = 0 ;
    pstBitMapInfo->bmiHeader.biYPelsPerMeter = 0 ;
    pstBitMapInfo->bmiHeader.biClrUsed       = 2 ;
    pstBitMapInfo->bmiHeader.biClrImportant  = 0 ;
    
    for(i = 0 ; i < 2 ; i++)
    {
        (DWORD&) pstBitMapInfo->bmiColors[i] = (DWORD) i;
    }

    //PS:
    //Font face name is not the filename of the font file.
    //And it isn't the fontname in the system explorer.
    //But we can use EnumFonts to search it.
    //Or use the system's some paint tools to see it.

//ZJian, 2001.5.7.  
    if(font_name[0] != 0x00)
        this->MyCreateFont((USTR*)font_name);
    else
        this->MyCreateFont(NULL);

    //this->MyCreateFont(NULL);   //OK.
    //this->MyCreateFont((USTR*)"标楷体");    //OK.
    //this->MyCreateFont((USTR*)"华康瘦金体");    //OK.
    
    for(i=0;i<MAX_FONT_SIZE_TYPE;i++)
    {
        m_ahBmp[i] = CreateDIBSection( m_ahDc[1], pstBitMapInfo,
            DIB_PAL_COLORS, (LPVOID*) &m_pabtBitMap[i], NULL, NULL );
        if(m_ahBmp[i]==NULL)
        {
            //ZJian, marked
            //        error_message((USTR *)"Font bitmap create error");
            goto EXIT;
        }
        
        m_ahBmpSave[i]  = ( HBITMAP )SelectObject( m_ahDc[i], m_ahBmp[i] ) ;
        // default set to 0 level font size
        // test  DIXON

        m_ahFontSave[i]   = ( HFONT )SelectObject( m_ahDc[i], m_ahFont[i] ) ;
        m_ahPalSave[i] = SelectPalette( m_ahDc[i], m_hPal, FALSE ) ;
        
        SetTextColor( m_ahDc[i], PALETTEINDEX(1)) ;
        SetBkMode( m_ahDc[i],TRANSPARENT ) ;
    }
    
    GetObject( m_ahBmp[0], sizeof(BITMAP), &stBitMap );
    m_lPitchBM = stBitMap.bmWidthBytes;

    // setup font colors -----------------------------------------------
    for(i=0;i<MAX_FONT_COLOR;i++)
        m_iaColor[i] = SYSTEM_WHITE;
    for(i=0;i<MAX_FONT_COLOR;i++)
        m_iaBackColor[i] = SYSTEM_BLACK;

    // Jack, changed the color tone, thanks AK. [16:55,9/13/2002]
    //红字 + 黄底 : 热情,莽撞,强壮
    m_iaColor[1] = SYSTEM_RED;
    m_iaBackColor[1] = SYSTEM_YELLOW;

    //绿字 + 蓝底 : 自然,青春
    m_iaColor[2] = SYSTEM_GREEN;
    m_iaBackColor[2] = SYSTEM_BLUE;

    //蓝字 + 白底 : 王子,朴实
    m_iaColor[3] = SYSTEM_BLUE;
    m_iaBackColor[3] = SYSTEM_WHITE;

    //黄字 + 蓝底 : 健康,阳光
    m_iaColor[4] = SYSTEM_YELLOW;
    m_iaBackColor[4] = SYSTEM_BLUE;

    //青字 + 粉红底 : 邪恶,阴险
    m_iaColor[5] = SYSTEM_CYAN;
    m_iaBackColor[5] = SYSTEM_PINK;

    //粉红字 + 蓝底 : 妖艳,冒险
    m_iaColor[6] = SYSTEM_PINK;
    m_iaBackColor[6] = SYSTEM_BLUE;

    //白字 + ???
    m_iaColor[7] = SYSTEM_WHITE;
    m_iaBackColor[7] = SYSTEM_BLACK;

    //黑字 + 白底 : 地位,权力
    m_iaColor[8] = SYSTEM_BLACK;
    m_iaBackColor[8] = SYSTEM_WHITE;

    m_iaColor[9] = SYSTEM_DARK0;
    m_iaBackColor[9] = SYSTEM_DARK6;

    m_iaColor[10] = SYSTEM_DARK1;
    m_iaBackColor[10] = SYSTEM_DARK6;

    m_iaColor[11] = SYSTEM_DARK2;
    m_iaBackColor[11] = SYSTEM_DARK6;

    m_iaColor[12] = SYSTEM_DARK3;
    m_iaBackColor[12] = SYSTEM_DARK6;

    m_iaColor[13] = SYSTEM_DARK4;
    m_iaBackColor[13] = SYSTEM_DARK0;

    //灰5字 + 黑底 : 淡泊,平凡
    m_iaColor[14] = SYSTEM_DARK5;
    m_iaBackColor[14] = SYSTEM_BLACK;

    m_iaColor[15] = SYSTEM_DARK6;
    m_iaBackColor[15] = SYSTEM_DARK0;


EXIT:
    //Jack, 2002.3.28.
    //found memory leak of pstBitMapInfo, pstLogPalette. need to free it ?
    if(pstBitMapInfo)
    {
        GlobalFree(pstBitMapInfo);
        pstBitMapInfo = NULL;
    }
    if(pstLogPalette)
    {
        GlobalFree(pstLogPalette);
        pstLogPalette = NULL;
    }


    return(lRet);
}



SLONG GDI_FONT      ::  SearchFont(UHINT iChar)
{
    SLONG lNext=0,lPre1=0,lPre2=0;
    
    lNext= font_bufinfo[0].iNext;
    for(;lNext>0;)
    {
        if( (iChar == font_bufinfo[lNext].iChar) && 
            (s_taFontWH[m_lSizeLevel].lWidth == font_bufinfo[lNext].xl ) &&
            (s_taFontWH[m_lSizeLevel].lHeight == font_bufinfo[lNext].yl ) )
        {
            return(lNext);  
        }
        lPre2=lPre1;
        lPre1=lNext;
        lNext=font_bufinfo[lPre1].iNext;
    }

    for(SLONG i=1;i<MAX_FONT_NUM;i++)
    {
        if(font_bufinfo[i].iChar ==  0)
        {
            GdiWriteFont(font_cache + (i)*FONT_DATA_SIZE ,iChar);
            font_bufinfo[i].iChar = iChar;
            font_bufinfo[i].xl    = s_taFontWH[m_lSizeLevel].lWidth;
            font_bufinfo[i].yl    = s_taFontWH[m_lSizeLevel].lHeight;
            font_bufinfo[i].iNext = font_bufinfo[0].iNext;
            font_bufinfo[0].iNext = (UHINT)i;
            return(i);
        }
    }
    
    if(lPre1<=0)
    {
        GdiWriteFont(font_cache + (1)*FONT_DATA_SIZE ,iChar);   
        //Add to first link
        font_bufinfo[1].iChar = iChar;
        font_bufinfo[1].xl    = s_taFontWH[m_lSizeLevel].lWidth;
        font_bufinfo[1].yl    =  s_taFontWH[m_lSizeLevel].lHeight;
        
        font_bufinfo[1].iNext = font_bufinfo[0].iNext;
        font_bufinfo[0].iNext     = 1;
        lPre1=1;
    }
    else
    {
        GdiWriteFont(font_cache + (lPre1)*FONT_DATA_SIZE ,iChar);

        //Add to first link
        font_bufinfo[lPre2].iNext = 0;
        font_bufinfo[lPre1].iChar = iChar;
        font_bufinfo[lPre1].xl    = s_taFontWH[m_lSizeLevel].lWidth;
        font_bufinfo[lPre1].yl    =  s_taFontWH[m_lSizeLevel].lHeight;
        font_bufinfo[lPre1].iNext = font_bufinfo[0].iNext;
        font_bufinfo[0].iNext     = (UHINT)lPre1;
        font_bufinfo[lPre2].iNext = 0;
    }
    return(lPre1);
}

SLONG GDI_FONT      ::  GdiWriteFont(ULONG *plDes,UHINT iChar)
{
    USTR    abText[3]={(USTR )(iChar >> 8),(USTR )(iChar & 0xFF) ,0x00};
    USTR    abDesText[3];
    LPBYTE  pbtSrc = m_pabtBitMap[m_lSizeLevel] ;
    SLONG   y;
    
    memset( pbtSrc,0x00,m_lPitchBM<<5);//<<5 = *32
    
    //如果系统为简体系统, 并且应用程式支持简体系统.
    if((m_gb_flag) && (m_support_system & SUPPORT_FONT_SYSTEM_GB) )
    {
        // Is English 
        if((abText[0]<0xA1)||(abText[0+1]<0x40)) 
        {
            TextOut(m_ahDc[m_lSizeLevel],0,0,(const char *)abText,2);
        }
        //Convert BIG5 to GB
        else if(m_convert_big5togb)
        {
            abDesText[1]=HIBYTE(big2gb_convert_table[(abText[0]-0xA1)*0xBF+abText[0+1]-0x40]);
            abDesText[0]=LOBYTE(big2gb_convert_table[(abText[0]-0xA1)*0xBF+abText[0+1]-0x40]);
            TextOut(m_ahDc[m_lSizeLevel],0,0,(const char *)abDesText,2);
        }
        else
        {
            TextOut(m_ahDc[m_lSizeLevel],0,0,(const char *)abText,2);
        }
    }
    //如果系统为繁体系统, 并且应用程式支持繁体系统.
    else if(m_support_system & SUPPORT_FONT_SYSTEM_BIG5)
    {
        TextOut(m_ahDc[m_lSizeLevel],0,0,(const char *)abText,2);
    }
    //不支持, 使用符号'.'来替代文字.
    else
    {
        abDesText[0] = '.';
        abDesText[1] = '.';
        TextOut(m_ahDc[m_lSizeLevel],0,0,(const char *)abDesText,2);
    }
    
    ////////////////////////////////////////////////////////////////////////
    // Record data to buffer
    for(  y = 0 ; y < s_taFontWH[m_lSizeLevel].lHeight ; y++ )
    {
        // pbtSrc[0]=0;
        *( plDes ) = MAKE_ULONG(pbtSrc[0], pbtSrc[1], pbtSrc[2], pbtSrc[3]) ;
        plDes++ ;
        pbtSrc += m_lPitchBM;
    }
    return(TTN_OK);
}


ULONG * GDI_FONT    ::  GetFont( UHINT iChar )
{
    SLONG lPos=SearchFont(iChar);
    return(font_cache + lPos*FONT_DATA_SIZE);
}

void    GDI_FONT    ::  RsetFontSizeLevel(void)
{
    if(m_lSizeLevel < 0)
        m_lSizeLevel = 0;
    if(m_lSizeLevel > MAX_FONT_SIZE_TYPE-1)
        m_lSizeLevel = MAX_FONT_SIZE_TYPE-1;
}


void   GDI_FONT     ::  Buf_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap)
{
    SLONG lPos=0;
    SLONG lLen=strlen((char *)bStr);
    UHINT iChar;
    SLONG x,y,i,j;
    ULONG lMask;
    SLONG lControlFlag;
    ULONG *plPoint; 

    for(lPos=0;lPos<lLen;lPos++)
    {
        lControlFlag=0;
        if( (bStr[lPos]>=0x80) && (lPos+1<lLen ) )
        {
            // Chinese word 
            iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],bStr[lPos+1]); /// NEED CONTINUE HERE
            plPoint = & font_cache[ SearchFont(iChar)* FONT_DATA_SIZE];
            ///////////////////////////////////////////////////////////////////////////
            //Begine write to buffer
            lMask=0x80000000;
            for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
            {
                for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth;x++,i++ )
                {
                    if((lMask>>i) & plPoint[j])
                    {
                        switch(m_special)
                        {
                        case 0x00:
                            bitmap->line[y][sx+i] = m_iaColor[m_now_front_color]; 
                            break;
                        case 0x01:
                            oper_alpha_color(&m_iaColor[m_now_front_color],
                                &bitmap->line[y][sx+i],m_alpha_level<<3);
                            break;
                        }
                    }
                    else
                    {
                        switch(m_special)
                        {
                        case 0x00:
                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_back_color];
                            break;
                        case 0x01:
                            oper_alpha_color(&m_iaBackColor[m_now_back_color],
                                &bitmap->line[y][sx+i],m_alpha_level<<3);
                            break;
                        }
                    }
                    
                }
            }
            sx+=s_taFontWH[m_lSizeLevel].lWidth;
            lPos++; 
        }
        else    
        {
            
            // Control flag setting
            if( (bStr[lPos]=='~') && (lPos+2<lLen) )
            {
                switch( bStr[lPos+1] )
                {
                case 'S':case 's':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_special=0x00;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '1':
                            m_special=0x01;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '2':
                            m_special=0x02;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        }
                    }
                    break;
                case 'A':case 'a':
                    {
                        if(bStr[lPos+2] >= '0' && bStr[lPos+2] <= '9')
                        {
                            m_alpha_level = 32 * (bStr[lPos+2] - '0') / 10;
                            lPos += 2;
                            lControlFlag = 1;
                        }
                    }
                    break;
                case 'Z':case 'z':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '+':
                            m_lSizeLevel++;
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE - 1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE - 1;
                            lControlFlag=1;
                            lPos+=2;
                            break; 
                        case '-':
                            m_lSizeLevel--;
                            if(m_lSizeLevel < 0)
                                m_lSizeLevel = 0;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            m_lSizeLevel = bStr[lPos+2] - '0';
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE-1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE-1;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'C':case 'c':
                    {
                        if( (bStr[lPos+2] >= '0') && (bStr[lPos+2] <= '9') )
                        {
                            m_now_front_color=bStr[lPos+2] - '0';
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                case 'O':case 'o':
                    {
                        if(bStr[lPos+2] >= '1' && bStr[lPos+2] <= '9')
                        {
                            m_frame_type=FONT_FRAME_OUTLINE ;
                            m_iaBackColor[m_now_front_color] = m_iaColor[bStr[lPos+2] - '0'];
                            lControlFlag=1;
                            lPos+=2;
                        }
                        else if(bStr[lPos+2] == '0')
                        {
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                }
            } 
            if(lControlFlag)
            {
                RsetFontSizeLevel();
            }
            else 
            { 
                // single byte word 
                iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],0); /// NEED CONTINUE HERE
                plPoint = & font_cache[SearchFont(iChar)* FONT_DATA_SIZE];
                ///////////////////////////////////////////////////////////////////////////
                lMask=0x80000000;
                for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
                {
                    for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth/2;x++,i++ )
                    {
                        if((lMask>>i) & plPoint[j])
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaColor[m_now_front_color],
                                    &bitmap->line[y][sx+i],m_alpha_level<<3);
                                break;
                            }
                        }
                        else
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaBackColor[m_now_back_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaBackColor[m_now_back_color],
                                    &m_iaBackColor[m_now_back_color],m_alpha_level<<3);
                                break;
                            }
                        }
                    }
                }
                sx+=s_taFontWH[m_lSizeLevel].lWidth/2;
            }
        } 
    }
    return;
}



void   GDI_FONT     ::  Buf_range_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap)
{
    SLONG lPos=0;
    SLONG lLen=strlen((char *)bStr);
    UHINT iChar;
    SLONG x,y,i,j;
    ULONG lMask;
    SLONG lControlFlag;
    ULONG *plPoint; 
    
    for(lPos=0;lPos<lLen;lPos++)
    {
        lControlFlag=0;
        
        if( (bStr[lPos]>=0x80) && (lPos+1<lLen  ) )
        {
            // Chinese word 
            iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],bStr[lPos+1]); /// NEED CONTINUE HERE
            plPoint = & font_cache[ SearchFont(iChar)* FONT_DATA_SIZE];
            ///////////////////////////////////////////////////////////////////////////
            //Begine write to buffer
            lMask=0x80000000;
            for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
            {
                for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth;x++,i++ )
                {
                    if( x<0 || x>=bitmap->w || y<0 || y>=bitmap->h )
                    {
                        continue;
                    } 
                    else
                    {
                        if((lMask>>i) & plPoint[j])
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaColor[m_now_front_color],
                                    &bitmap->line[y][sx+i],m_alpha_level<<3);
                                break;
                            }
                        }
                        else
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaBackColor[m_now_back_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaBackColor[m_now_back_color],
                                    &bitmap->line[y][sx+i],m_alpha_level<<3);
                                break;
                            }
                        }
                        
                    }
                }
            }
            sx+=s_taFontWH[m_lSizeLevel].lWidth;
            lPos++; 
        }
        else    
        {
            // Control flag setting
            if( (bStr[lPos]=='~') && (lPos+2<lLen) )
            {
                switch( bStr[lPos+1] )
                {
                case 'S':case 's':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_special=0x00;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '1':
                            m_special=0x01;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '2':
                            m_special=0x02;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        }
                    }
                    break;
                case 'A':case 'a':
                    {
                        if(bStr[lPos+2] >= '0' && bStr[lPos+2] <= '9')
                        {
                            m_alpha_level = 32 * (bStr[lPos+2] - '0') / 10;
                            lPos += 2;
                            lControlFlag = 1;
                        }
                    }
                    break;
                case 'Z':case 'z':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '+':
                            m_lSizeLevel++;
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE - 1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE - 1;
                            lControlFlag=1;
                            lPos+=2;
                            break; 
                        case '-':
                            m_lSizeLevel--;
                            if(m_lSizeLevel < 0)
                                m_lSizeLevel = 0;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            m_lSizeLevel = bStr[lPos+2] - '0';
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE-1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE-1;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'C':case 'c':
                    {
                        if( (bStr[lPos+2] >= '0') && (bStr[lPos+2] <= '9') )
                        {
                            m_now_front_color=bStr[lPos+2] - '0';
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                case 'O':case 'o':
                    {
                        if(bStr[lPos+2] >= '1' && bStr[lPos+2] <= '9')
                        {
                            m_frame_type=FONT_FRAME_OUTLINE ;
                            m_iaBackColor[m_now_front_color] = m_iaColor[bStr[lPos+2] - '0'];
                            lControlFlag=1;
                            lPos+=2;
                        }
                        else if(bStr[lPos+2] == '0')
                        {
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                }
            } 

            if(lControlFlag)
            {
                RsetFontSizeLevel();
            }
            else 
            { 
                // single byte word 
                iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],0); /// NEED CONTINUE HERE
                plPoint = & font_cache[SearchFont(iChar)* FONT_DATA_SIZE];
                ///////////////////////////////////////////////////////////////////////////
                lMask=0x80000000;
                for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
                {
                    for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth/2;x++,i++ )
                    {
                        if( x<0 || x>=bitmap->w || y<0 || y>=bitmap->h )
                        {
                            break;
                        } 
                        else
                        { 
                            if((lMask>>i) & plPoint[j])
                            {
                                switch(m_special)
                                {
                                case 0x00:
                                    bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                    break;
                                case 0x01:
                                    oper_alpha_color(&m_iaColor[m_now_front_color],
                                        &bitmap->line[y][sx+i],m_alpha_level<<3);
                                    break;
                                }
                            }
                            else
                            {
                                bitmap->line[y][sx+i] = m_iaBackColor[m_now_back_color];
                            }
                            
                        } 
                    }
                }
                sx+=s_taFontWH[m_lSizeLevel].lWidth/2;
            }
        } 
    }
    return;
}



void   GDI_FONT     ::  Buf_pest_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap)
{
    SLONG lPos=0;
    SLONG lLen=strlen((char *)bStr);
    UHINT iChar;
    SLONG x,y,i,j;
    ULONG lMask;
    SLONG lControlFlag;
    ULONG *plPoint; 

    for(lPos=0;lPos<lLen;lPos++)
    {
        lControlFlag=0;
        if( (bStr[lPos]>=0x80)  &&  (lPos+1<lLen  )     )
        {
            // Chinese word 
            iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],bStr[lPos+1]); /// NEED CONTINUE HERE
            plPoint = & font_cache[ SearchFont(iChar)* FONT_DATA_SIZE];
            ///////////////////////////////////////////////////////////////////////////
            //Begine write to buffer
            lMask=0x80000000;
            for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
            {
                for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth;x++,i++ )
                {
                    if((lMask>>i) & plPoint[j])
                    {
                        switch(m_special)
                        {
                        case 0x00:
                            bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                            break;
                        case 0x01:
                            oper_alpha_color(&m_iaColor[m_now_front_color],
                                &bitmap->line[y][sx+i],m_alpha_level<<3);
                            break;
                        }
                        switch(m_frame_type)
                        {
                        case FONT_FRAME_DUMP:
                            break;
                        case FONT_FRAME_OUTLINE:
                            if(i==0)
                            {
                                if( x>0  )
                                    bitmap->line[y][sx+i-1] = m_iaBackColor[m_now_front_color];
                            }
                            if(i==s_taFontWH[m_lSizeLevel].lWidth-1)
                            {
                                if( x<bitmap->w-1  )
                                    bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                            }
                            if(j==0)
                            {
                                if( y>0  )
                                    bitmap->line[y-1][sx+i] = m_iaBackColor[m_now_front_color];
                            }
                            if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                            {
                                if( y<bitmap->h-1  )
                                    bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                            }
                            break;
                        case FONT_FRAME_SHADOW:
                            if(i==s_taFontWH[m_lSizeLevel].lWidth-1)
                            {
                                if( x<bitmap->w-1  )
                                    bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                            }
                            if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                            {
                                if( y<bitmap->h-1  )
                                    bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                            }
                            break;
                        }
                    }
                    else
                    {
                        switch(m_frame_type)
                        {
                        case FONT_FRAME_DUMP:
                            break;
                        case FONT_FRAME_OUTLINE:
                            //up
                            if(j>=1)
                            {
                                if((lMask>>i) & plPoint[j-1])
                                {
                                    bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                    break;
                                }
                            }
                            //right
                            if(i<s_taFontWH[m_lSizeLevel].lWidth-1)
                            {
                                if((lMask>>(i+1)) & plPoint[j])
                                {
                                    bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                    break;
                                }
                            }
                            //down
                            if(j<s_taFontWH[m_lSizeLevel].lHeight-1)
                            {
                                if((lMask>>i) & plPoint[j+1])
                                {
                                    bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                    break;
                                }
                            }
                            break;
                        case FONT_FRAME_SHADOW:
                            //up
                            if(j>=1)
                            {
                                if((lMask>>i) & plPoint[j-1])
                                {
                                    bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                    break;
                                }
                            }
                            //left
                            if(i>=1)
                            {
                                if((lMask>>(i-1)) & plPoint[j])
                                {
                                    bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                    break;
                                }
                            }
                            break;
                        }
                    } 
                }
            }
            sx+=s_taFontWH[m_lSizeLevel].lWidth;
            lPos++; 
        }
        else    
        {   
            // Control flag setting
            if( (bStr[lPos]=='~') && (lPos+2<lLen) )
            {
                switch( bStr[lPos+1] )
                {
                case 'S':case 's':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_special=0x00;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '1':
                            m_special=0x01;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '2':
                            m_special=0x02;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        }
                    }
                    break;
                case 'A':case 'a':
                    {
                        if(bStr[lPos+2] >= '0' && bStr[lPos+2] <= '9')
                        {
                            m_alpha_level = 32 * (bStr[lPos+2] - '0') / 10;
                            lPos += 2;
                            lControlFlag = 1;
                        }
                    }
                    break;
                case 'Z':case 'z':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '+':
                            m_lSizeLevel++;
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE - 1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE - 1;
                            lControlFlag=1;
                            lPos+=2;
                            break; 
                        case '-':
                            m_lSizeLevel--;
                            if(m_lSizeLevel < 0)
                                m_lSizeLevel = 0;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            m_lSizeLevel = bStr[lPos+2] - '0';
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE-1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE-1;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'C':case 'c':
                    {
                        if( (bStr[lPos+2] >= '0') && (bStr[lPos+2]<='9') )
                        {
                            m_now_front_color = bStr[lPos+2] - '0';
                            lControlFlag = 1;
                            lPos += 2;
                        }
                    }
                    break;
                case 'F':case 'f':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '1':
                            m_frame_type=FONT_FRAME_OUTLINE;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '2':
                            m_frame_type=FONT_FRAME_SHADOW;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'O':case 'o':
                    {
                        if(bStr[lPos+2] >= '1' && bStr[lPos+2] <= '9')
                        {
                            m_frame_type=FONT_FRAME_OUTLINE ;
                            m_iaBackColor[m_now_front_color] = m_iaColor[bStr[lPos+2] - '0'];
                            lControlFlag=1;
                            lPos+=2;
                        }
                        else if(bStr[lPos+2] == '0')
                        {
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                 }       
            } 
            if(lControlFlag)
            {
                RsetFontSizeLevel();
            }
            else 
            { 
                // single byte word 
                iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],0); /// NEED CONTINUE HERE
                plPoint = & font_cache[SearchFont(iChar)* FONT_DATA_SIZE];
                ///////////////////////////////////////////////////////////////////////////
                lMask=0x80000000;
                for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
                {
                    for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth/2;x++,i++ )
                    {
                        if((lMask>>i) & plPoint[j])
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaColor[m_now_front_color],
                                    &bitmap->line[y][sx+i],m_alpha_level<<3);
                                break;
                            }
                            switch(m_frame_type)
                            {
                            case FONT_FRAME_DUMP:
                                break;
                            case FONT_FRAME_OUTLINE:
                                {
                                    if(i==0)
                                    {
                                        if( x>0  )
                                            bitmap->line[y][sx+i-1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==0)
                                    {
                                        if( y>0  )
                                            bitmap->line[y-1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                }
                                break;
                            case FONT_FRAME_SHADOW:
                                {
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                }
                                break;
                            }
                        }
                        else
                        {
                            switch(m_frame_type)
                            {
                            case FONT_FRAME_DUMP:
                                break;
                            case FONT_FRAME_OUTLINE:
                                {
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //right
                                    if(i<s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if((lMask>>(i+1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //down
                                    if(j<s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if((lMask>>i) & plPoint[j+1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                }
                                break;
                            case FONT_FRAME_SHADOW:
                                {
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        } 
                    }
                }
                sx+=s_taFontWH[m_lSizeLevel].lWidth/2;
            }
        } 
    }
    return;
}



void   GDI_FONT     ::  Buf_range_pest_print(SLONG sx,SLONG sy,USTR * bStr,BMP *bitmap)
{
    SLONG lPos=0;
    SLONG lLen=strlen((char *)bStr);
    UHINT iChar;
    SLONG x,y,i,j;
    ULONG lMask;
    SLONG lControlFlag;
    ULONG *plPoint;

    for(lPos=0;lPos<lLen;lPos++)
    {
        lControlFlag=0;
        if( (bStr[lPos]>=0x80)  &&  (lPos+1<lLen  )  )
        {
            // Chinese word
            iChar   = (UHINT)  MAKE_UHINT(bStr[lPos],bStr[lPos+1]); /// NEED CONTINUE HERE
            plPoint = & font_cache[ SearchFont(iChar)* FONT_DATA_SIZE];
            ///////////////////////////////////////////////////////////////////////////
            //Begine write to buffer
            lMask=0x80000000;
            for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
            {
                for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth;x++,i++ )
                {
                    if( (x<0) || (x>=bitmap->w) || (y<0) || (y>=bitmap->h) )
                    {
                        //ZJian modified, 2001.4.19.
                        //break;
                        continue;
                    } 
                    else
                    {
                        if((lMask>>i) & plPoint[j])
                        {
                            switch(m_special)
                            {
                            case 0x00:
                                bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                break;
                            case 0x01:
                                oper_alpha_color(&m_iaColor[m_now_front_color],
                                    &bitmap->line[y][sx+i],m_alpha_level<<3);
                                break;
                            }
                            switch(m_frame_type)
                            {
                            case FONT_FRAME_DUMP:
                                break;
                            case FONT_FRAME_OUTLINE:
                                {
                                    if(i==0)
                                    {
                                        if( x>0  )
                                            bitmap->line[y][sx+i-1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==0)
                                    {
                                        if( y>0  )
                                            bitmap->line[y-1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                }
                                break;
                            case FONT_FRAME_SHADOW:
                                {
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i] = m_iaBackColor[m_now_front_color];
                                    }
                                }
                                break;
                            }
                        }
                        else
                        {
                            switch(m_frame_type)
                            {
                            case FONT_FRAME_DUMP:
                                break;
                            case FONT_FRAME_OUTLINE:
                                {
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //right
                                    if(i<s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if((lMask>>(i+1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //down
                                    if(j<s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if((lMask>>i) & plPoint[j+1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                }
                                break;
                            case FONT_FRAME_SHADOW:
                                {
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                }
                                break;
                            }
                        } 
                    }
                }
            }
        sx+=s_taFontWH[m_lSizeLevel].lWidth;
        lPos++; 
        }
        else    
        {
            // Control flag setting
            if( (bStr[lPos]=='~') && (lPos+2<lLen) )
            {
                switch( bStr[lPos+1] )
                {
                case 'S':case 's':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_special=0x00;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '1':
                            m_special=0x01;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        case '2':
                            m_special=0x02;
                            lPos+=2;
                            lControlFlag=1;
                            break;
                        }
                    }
                    break;
                case 'A':case 'a':
                    {
                        if(bStr[lPos+2] >= '0' && bStr[lPos+2] <= '9')
                        {
                            m_alpha_level = 32 * (bStr[lPos+2] - '0') / 10;
                            lPos += 2;
                            lControlFlag = 1;
                        }
                    }
                    break;
                case 'Z':case 'z':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '+':
                            m_lSizeLevel++;
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE - 1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE - 1;
                            lControlFlag=1;
                            lPos+=2;
                            break; 
                        case '-':
                            m_lSizeLevel--;
                            if(m_lSizeLevel < 0)
                                m_lSizeLevel = 0;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '0': case '1': case '2': case '3': case '4':
                        case '5': case '6': case '7': case '8': case '9':
                            m_lSizeLevel = bStr[lPos+2] - '0';
                            if(m_lSizeLevel > MAX_FONT_SIZE_TYPE-1)
                                m_lSizeLevel = MAX_FONT_SIZE_TYPE-1;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'C':case 'c':
                    {
                        if( (bStr[lPos+2] >= '0') && (bStr[lPos+2] <= '9') )
                        {
                            m_now_front_color=bStr[lPos+2] - '0';
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                case 'F':case 'f':
                    {
                        switch(bStr[lPos+2])
                        {
                        case '0':
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '1':
                            m_frame_type=FONT_FRAME_OUTLINE;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        case '2':
                            m_frame_type=FONT_FRAME_SHADOW;
                            lControlFlag=1;
                            lPos+=2;
                            break;
                        }
                    }
                    break;
                case 'O':case 'o':
                    {
                        if(bStr[lPos+2] >= '1' && bStr[lPos+2] <= '9')
                        {
                            m_frame_type=FONT_FRAME_OUTLINE ;
                            m_iaBackColor[m_now_front_color] = m_iaColor[bStr[lPos+2] - '0'];
                            lControlFlag=1;
                            lPos+=2;
                        }
                        else if(bStr[lPos+2] == '0')
                        {
                            m_frame_type=FONT_FRAME_DUMP;
                            lControlFlag=1;
                            lPos+=2;
                        }
                    }
                    break;
                }       
            } 

            if(lControlFlag)
            {
                RsetFontSizeLevel();
            }
            else 
            { 
                // single byte word 
                iChar   =(UHINT)   MAKE_UHINT(bStr[lPos],0); /// NEED CONTINUE HERE
                plPoint = & font_cache[SearchFont(iChar)* FONT_DATA_SIZE];
                ///////////////////////////////////////////////////////////////////////////
                lMask=0x80000000;
                for(y=sy,j=0;y<sy+s_taFontWH[m_lSizeLevel].lHeight;j++,y++)
                {
                    for(x=sx,i=0;x<sx+s_taFontWH[m_lSizeLevel].lWidth/2;x++,i++ )
                    {
                        if( (x<0) || (x>=bitmap->w) || (y<0) || (y>=bitmap->h) )
                        {
                            break;
                        } 
                        else
                        { 
                            if((lMask>>i) & plPoint[j])
                            {
                                switch(m_special)
                                {
                                case 0x00:
                                    bitmap->line[y][sx+i] = m_iaColor[m_now_front_color];
                                    break;
                                case 0x01:
                                    oper_alpha_color(&m_iaColor[m_now_front_color],
                                        &bitmap->line[y][sx+i],m_alpha_level<<3);
                                    break;
                                }
                                switch(m_frame_type)
                                {
                                case FONT_FRAME_DUMP:
                                    break;
                                case FONT_FRAME_OUTLINE:
                                    if(i==0)
                                    {
                                        if( x>0  )
                                            bitmap->line[y][sx+i-1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==0)
                                    {
                                        if( y>0  )
                                            bitmap->line[y-1][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    break;
                                case FONT_FRAME_SHADOW:
                                    if(i==s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if( x<bitmap->w-1  )
                                            bitmap->line[y][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    if(j==s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if( y<bitmap->h-1  )
                                            bitmap->line[y+1][sx+i+1] = m_iaBackColor[m_now_front_color];
                                    }
                                    break;
                                }
                            }
                            else
                            {
                                switch(m_frame_type)
                                {
                                case FONT_FRAME_DUMP:
                                    break;
                                case FONT_FRAME_OUTLINE:
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //right
                                    if(i<s_taFontWH[m_lSizeLevel].lWidth/2-1)
                                    {
                                        if((lMask>>(i+1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //down
                                    if(j<s_taFontWH[m_lSizeLevel].lHeight-1)
                                    {
                                        if((lMask>>i) & plPoint[j+1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    break;
                                case FONT_FRAME_SHADOW:
                                    //up
                                    if(j>=1)
                                    {
                                        if((lMask>>i) & plPoint[j-1])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    //left
                                    if(i>=1)
                                    {
                                        if((lMask>>(i-1)) & plPoint[j])
                                        {
                                            bitmap->line[y][sx+i] = m_iaBackColor[m_now_front_color];
                                            break;
                                        }
                                    }
                                    break;
                                }
                            } 
                        } 
                    }
                }
                sx+=s_taFontWH[m_lSizeLevel].lWidth/2;
            }
        } 
    }
    return;
}


SLONG    GDI_FONT   ::  GetWordWidth(void)
{
    return(s_taFontWH[m_lSizeLevel].lWidth);
}

/////////////////////////////////////////////////////////////////////////////////////////////
//c function
int     init_winfont(void)
{
    g_cFont.Initialize();
    return 0;
}



void    active_winfont(int active)
{
    return;
}


void    free_winfont(void)
{
    return;
}


void    print_640x480x16M(SLONG x,SLONG y,USTR *data,SLONG put_type,BMP *bit_map)
{
    if(put_type == PEST_PUT)
        g_cFont.Buf_pest_print(x,y,data,bit_map);
    else
        g_cFont.Buf_print(x,y,data,bit_map);
}

void    print_range_640x480x16M(SLONG x,SLONG y,USTR *data,SLONG put_type,BMP *bit_map)
{
    if(put_type == PEST_PUT)
    {
        g_cFont.Buf_range_pest_print(x,y,data,bit_map);
    }
    else
    {
        //ZJian, for COPY_PUT control, 2001.4.24.
        g_cFont.SetBackColor( (UHINT)( (put_type & 0xffff0000) >> 16) );
        //
        g_cFont.Buf_range_print(x,y,data,bit_map);
    }
}

void    set_word_color(UHINT color)
{
    g_cFont.SetWordColor(color);
}

void    set_back_color(UHINT color)
{
    g_cFont.SetBackColor(color);
}

void    set_word_color(SLONG lab,UHINT color)
{
    g_cFont.SetWordColor(lab,color);
}

void    set_back_color(SLONG lab,UHINT color)
{
    g_cFont.SetBackColor(lab,color);
}

SLONG  get_word_width(void)
{
    return(g_cFont.GetWordWidth());
}

void    set_winfont_support_system(SLONG flags)
{
    g_cFont.SetSupportFontSystem(flags);
}


void    set_winfont_name(USTR *name)
{
    strcpy((char *)font_name, (const char *)name);
}


void    set_winfont_convert_big5_to_gb(SLONG flag)
{
    g_cFont.SetConvertBig5ToGB(flag);
}


void    set_winfont_weight(SLONG weight)
{
    g_cFont.SetFontWeight(weight);
}


UHINT   convert_word_gb2big(UHINT gb_code)
{
    UCHR    gb_hi, gb_lo, big_hi, big_lo;
    UHINT   big_code, code;
    SLONG   table_index;

    gb_hi = (UCHR)((gb_code >> 8) & 0xff);
    gb_lo = (UCHR)(gb_code & 0xff);
    table_index = (gb_hi - 0xa1) * 94 + (gb_lo - 0xa1);
    code = gb2big_convert_table[table_index];
    big_hi = (UCHR)(code & 0xff);
    big_lo = (UCHR)((code >> 8) & 0xff);
    big_code = ((big_hi << 8) & 0xff00) | big_lo;

    return  big_code;
}


UHINT   convert_word_big2gb(UHINT big_code)
{
    UCHR    gb_hi, gb_lo, big_hi, big_lo;
    UHINT   gb_code, code;
    SLONG   table_index;

    big_hi = (UCHR)((big_code >> 8) & 0xff);
    big_lo = (UCHR)(big_code & 0xff);
    table_index = (big_hi - 0xa1) * 191 + (big_lo - 0x40);
    code = big2gb_convert_table[table_index];
    gb_hi = (UCHR)(code & 0xff);
    gb_lo = (UCHR)((code >> 8) & 0xff);
    gb_code = ((gb_hi << 8) & 0xff00) | gb_lo;

    return  gb_code;
}

