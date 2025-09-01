/*
**  xkeyb.cpp
**  keyboard functions.
**  
**  Jack, 2002.4.9.
*/

/*
void ShowKey (HWND hwnd, int iType, char *szMessage,
              WPARAM wParam, LPARAM lParam)
     {
     static char *szFormat[2] = { "%-14s %3d    %c %6u %4d %3s %3s %4s %4s",
                                  "%-14s    %3d %c %6u %4d %3s %3s %4s %4s" } ;
     char        szBuffer[80] ;
     HDC         hdc ;

     ScrollWindow (hwnd, 0, -cyChar, &rect, &rect) ;
     hdc = GetDC (hwnd) ;

     SelectObject (hdc, GetStockObject (SYSTEM_FIXED_FONT)) ;

     TextOut (hdc, cxChar, rect.bottom - cyChar, szBuffer,
              wsprintf (szBuffer, szFormat [iType],
                        szMessage, 
                        wParam,                         //Key
                        (BYTE) (iType ? wParam : ' '),  //Char
                        LOWORD (lParam),                //Repeat
                        HIWORD (lParam) & 0xFF,         //Scan
                        (PSTR) (0x01000000 & lParam ? "Yes"  : "No"),   //Ext
                        (PSTR) (0x20000000 & lParam ? "Yes"  : "No"),   //ALT
                        (PSTR) (0x40000000 & lParam ? "Down" : "Up"),   //Prev
                        (PSTR) (0x80000000 & lParam ? "Up"   : "Down")  //Tran
                        )) ;

     ReleaseDC (hwnd, hdc) ;
     ValidateRect (hwnd, NULL) ;
     }
*/