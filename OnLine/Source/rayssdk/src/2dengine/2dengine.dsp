# Microsoft Developer Studio Project File - Name="2dengine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=2dengine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "2dengine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "2dengine.mak" CFG="2dengine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "2dengine - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "2dengine - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/rayssdk-2dengine", ANAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "2dengine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /MT /W3 /GX /Od /I "..\\" /I "..\..\..\public\valve" /I "..\..\..\dx9sdk\common\include" /I "..\..\include" /I "..\..\..\platformsdk\include" /I "..\..\..\dx9sdk\include" /I ".\mp3file" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FAs /FR /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\2dengine.lib"

!ELSEIF  "$(CFG)" == "2dengine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /I "..\..\..\dx9sdk\common\include" /I "..\..\include" /I "..\..\..\platformsdk\include" /I "..\..\..\dx9sdk\include" /I ".\mp3file" /I "..\..\..\public\valve" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FA /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\..\lib\2denginedbg.lib"

!ENDIF 

# Begin Target

# Name "2dengine - Win32 Release"
# Name "2dengine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\..\..\dx9sdk\Common\Src\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy.cpp
# End Source File
# Begin Source File

SOURCE=.\entry.cpp
# End Source File
# Begin Source File

SOURCE=.\function.cpp
# End Source File
# Begin Source File

SOURCE=.\lzo.cpp
# End Source File
# Begin Source File

SOURCE=.\menutree.cpp
# End Source File
# Begin Source File

SOURCE=.\menuwin.cpp
# End Source File
# Begin Source File

SOURCE=.\mmx.cpp
# End Source File
# Begin Source File

SOURCE=.\packfile.cpp
# End Source File
# Begin Source File

SOURCE=.\polling.cpp
# End Source File
# Begin Source File

SOURCE=.\raysime.cpp
# End Source File
# Begin Source File

SOURCE=.\readinfo.cpp
# End Source File
# Begin Source File

SOURCE=.\ripple.cpp
# End Source File
# Begin Source File

SOURCE=.\str2num.cpp
# End Source File
# Begin Source File

SOURCE=.\topo.cpp
# End Source File
# Begin Source File

SOURCE=.\undo.cpp
# End Source File
# Begin Source File

SOURCE=.\vbmp.cpp
# End Source File
# Begin Source File

SOURCE=.\vflic.cpp
# End Source File
# Begin Source File

SOURCE=.\vpcx.cpp
# End Source File
# Begin Source File

SOURCE=.\vpsd.cpp
# End Source File
# Begin Source File

SOURCE=.\vtga.cpp
# End Source File
# Begin Source File

SOURCE=.\winfont.cpp
# End Source File
# Begin Source File

SOURCE=.\winmain.cpp
# End Source File
# Begin Source File

SOURCE=.\xcak.cpp
# End Source File
# Begin Source File

SOURCE=.\xcdrom.cpp
# End Source File
# Begin Source File

SOURCE=.\xdraw.cpp
# End Source File
# Begin Source File

SOURCE=.\xfont.cpp
# End Source File
# Begin Source File

SOURCE=.\xgrafx.cpp
# End Source File
# Begin Source File

SOURCE=.\xinput.cpp
# End Source File
# Begin Source File

SOURCE=.\xkiss.cpp
# End Source File
# Begin Source File

SOURCE=.\xmedia.cpp
# End Source File
# Begin Source File

SOURCE=.\xmem.cpp
# End Source File
# Begin Source File

SOURCE=.\xmodule.cpp
# End Source File
# Begin Source File

SOURCE=.\xplay.cpp
# End Source File
# Begin Source File

SOURCE=.\xpoly.cpp
# End Source File
# Begin Source File

SOURCE=.\xrla.cpp
# End Source File
# Begin Source File

SOURCE=.\xrle.cpp
# End Source File
# Begin Source File

SOURCE=.\xsound.cpp
# End Source File
# Begin Source File

SOURCE=.\xsystem.cpp
# End Source File
# Begin Source File

SOURCE=.\xtimer.cpp
# End Source File
# Begin Source File

SOURCE=.\xvga.cpp
# End Source File
# Begin Source File

SOURCE=.\ybitio.cpp
# End Source File
# Begin Source File

SOURCE=.\ylzss.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\2dlocal.h
# End Source File
# Begin Source File

SOURCE=.\function.h
# End Source File
# Begin Source File

SOURCE=.\lzo.h
# End Source File
# Begin Source File

SOURCE=.\menutree.h
# End Source File
# Begin Source File

SOURCE=.\menuwin.h
# End Source File
# Begin Source File

SOURCE=.\mmx.h
# End Source File
# Begin Source File

SOURCE=.\packfile.h
# End Source File
# Begin Source File

SOURCE=.\polling.h
# End Source File
# Begin Source File

SOURCE=.\rays.h
# End Source File
# Begin Source File

SOURCE=.\raysime.h
# End Source File
# Begin Source File

SOURCE=.\readinfo.h
# End Source File
# Begin Source File

SOURCE=.\ripple.h
# End Source File
# Begin Source File

SOURCE=.\str2num.h
# End Source File
# Begin Source File

SOURCE=.\topo.h
# End Source File
# Begin Source File

SOURCE=.\undo.h
# End Source File
# Begin Source File

SOURCE=.\vbmp.h
# End Source File
# Begin Source File

SOURCE=.\vflic.h
# End Source File
# Begin Source File

SOURCE=.\vpcx.h
# End Source File
# Begin Source File

SOURCE=.\vpsd.h
# End Source File
# Begin Source File

SOURCE=.\vtga.h
# End Source File
# Begin Source File

SOURCE=.\winfont.h
# End Source File
# Begin Source File

SOURCE=..\..\include\winmain.h
# End Source File
# Begin Source File

SOURCE=.\xbig2gb.h
# End Source File
# Begin Source File

SOURCE=.\xcak.h
# End Source File
# Begin Source File

SOURCE=.\xcdrom.h
# End Source File
# Begin Source File

SOURCE=.\xdraw.h
# End Source File
# Begin Source File

SOURCE=.\xfont.h
# End Source File
# Begin Source File

SOURCE=.\xgb2big.h
# End Source File
# Begin Source File

SOURCE=.\xgrafx.h
# End Source File
# Begin Source File

SOURCE=.\xinput.h
# End Source File
# Begin Source File

SOURCE=.\xkeyb.h
# End Source File
# Begin Source File

SOURCE=.\xkiss.h
# End Source File
# Begin Source File

SOURCE=.\xmedia.h
# End Source File
# Begin Source File

SOURCE=.\xmem.h
# End Source File
# Begin Source File

SOURCE=.\xmodule.h
# End Source File
# Begin Source File

SOURCE=.\xplay.h
# End Source File
# Begin Source File

SOURCE=.\xpoly.h
# End Source File
# Begin Source File

SOURCE=.\xrla.h
# End Source File
# Begin Source File

SOURCE=.\xrle.h
# End Source File
# Begin Source File

SOURCE=.\xsound.h
# End Source File
# Begin Source File

SOURCE=.\xsystem.h
# End Source File
# Begin Source File

SOURCE=.\xtimer.h
# End Source File
# Begin Source File

SOURCE=.\xvga.h
# End Source File
# Begin Source File

SOURCE=.\ybitio.h
# End Source File
# Begin Source File

SOURCE=.\ylzss.h
# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\intro.txt
# End Source File
# Begin Source File

SOURCE=.\update.txt
# End Source File
# End Group
# Begin Group "MP3 Files"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\mp3file\amp1dec.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\amp2dec.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\amp3dec.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\ampdec.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\ampdec.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\ampsynth.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfarc.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfarc.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfile.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfile.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfstd.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\binfstd.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\dummymp3.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\mp3file.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\mp3file.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\mpgsplit.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\mpgsplit.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\ptypes.h
# End Source File
# Begin Source File

SOURCE=.\mp3file\timer.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3file\timer.h
# End Source File
# End Group
# End Target
# End Project
