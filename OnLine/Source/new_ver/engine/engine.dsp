# Microsoft Developer Studio Project File - Name="engine" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=engine - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "engine.mak" CFG="engine - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "engine - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "engine - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName "engine"
# PROP Scc_LocalPath ".."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "engine - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "Release"
# PROP Intermediate_Dir "Release"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "..\dx9sdk\lib" /I "..\public" /I "..\rayssdk\include" /I "..\dx9sdk\include" /I "..\public\valve" /D "NDEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "NO_EXPORT" /FR /YX /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /incremental:yes /machine:I386 /nodefaultlib:"LIBC.lib" /out:"Release/main.exe" /libpath:"..\lib" /libpath:"..\rayssdk\lib" /libpath:"..\dx9sdk\lib"
# Begin Custom Build
TargetPath=.\Release\main.exe
InputPath=.\Release\main.exe
SOURCE="$(InputPath)"

"..\..\bin\main.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\bin\main.exe attrib -r ..\..\bin\main.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\bin\main.exe 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "engine - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "Debug"
# PROP Intermediate_Dir "Debug"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "..\dx9sdk\include" /I "..\public" /I "..\rayssdk\include" /I "..\public\valve" /D "_DEBUG" /D "WIN32" /D "_WINDOWS" /D "_MBCS" /D "NO_EXPORT" /FR /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x409 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:windows /debug /machine:I386 /nodefaultlib:"LIBCD.lib" /nodefaultlib:"libcmtd.lib" /nodefaultlib:"libc.lib" /out:"../../bin/main.exe" /pdbtype:sept /libpath:"..\lib" /libpath:"..\rayssdk\lib" /libpath:"..\dx9sdk\lib"
# Begin Custom Build
TargetPath=\war\bin\main.exe
InputPath=\war\bin\main.exe
SOURCE="$(InputPath)"

"..\..\bin\main.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist  ..\..\bin\main.exe attrib -r ..\..\bin\main.exe 
	if exist $(TargetPath) copy $(TargetPath) ..\..\bin\main.exe 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "engine - Win32 Release"
# Name "engine - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\account.cpp
# End Source File
# Begin Source File

SOURCE=.\chat_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\data_proc.cpp
# End Source File
# Begin Source File

SOURCE=.\data_proc.h
# End Source File
# Begin Source File

SOURCE=.\dm_music.cpp
# End Source File
# Begin Source File

SOURCE=.\edit.cpp
# End Source File
# Begin Source File

SOURCE=.\engine_global.cpp
# End Source File
# Begin Source File

SOURCE=.\entry.cpp
# End Source File
# Begin Source File

SOURCE=.\fight.CPP
# End Source File
# Begin Source File

SOURCE=.\fight.h
# End Source File
# Begin Source File

SOURCE=.\findpath.cpp
# End Source File
# Begin Source File

SOURCE=.\findpath.h
# End Source File
# Begin Source File

SOURCE=.\game.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain.cpp
# End Source File
# Begin Source File

SOURCE=.\graph.cpp
# End Source File
# Begin Source File

SOURCE=..\public\valve\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\macro.cpp
# End Source File
# Begin Source File

SOURCE=.\net_msg.cpp
# End Source File
# Begin Source File

SOURCE=.\resource.rc
# End Source File
# Begin Source File

SOURCE=.\system.cpp
# End Source File
# Begin Source File

SOURCE=.\taskman.cpp
# End Source File
# Begin Source File

SOURCE=.\utility.cpp
# End Source File
# Begin Source File

SOURCE=.\Weather.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\account.h
# End Source File
# Begin Source File

SOURCE=..\public\chat_info.h
# End Source File
# Begin Source File

SOURCE=.\chat_msg.h
# End Source File
# Begin Source File

SOURCE=..\ChatWindow.h
# End Source File
# Begin Source File

SOURCE=..\public\ChatWindow.h
# End Source File
# Begin Source File

SOURCE=..\data.h
# End Source File
# Begin Source File

SOURCE=..\public\data.h
# End Source File
# Begin Source File

SOURCE=.\dm_music.h
# End Source File
# Begin Source File

SOURCE=..\door_info.h
# End Source File
# Begin Source File

SOURCE=..\public\door_info.h
# End Source File
# Begin Source File

SOURCE=..\door_srv.h
# End Source File
# Begin Source File

SOURCE=..\public\door_srv.h
# End Source File
# Begin Source File

SOURCE=.\edit.h
# End Source File
# Begin Source File

SOURCE=.\engine_global.h
# End Source File
# Begin Source File

SOURCE=.\game.h
# End Source File
# Begin Source File

SOURCE=.\gamemain.h
# End Source File
# Begin Source File

SOURCE=.\graph.h
# End Source File
# Begin Source File

SOURCE=..\ichat.h
# End Source File
# Begin Source File

SOURCE=..\public\ichat.h
# End Source File
# Begin Source File

SOURCE=..\idoorserver.h
# End Source File
# Begin Source File

SOURCE=..\public\idoorserver.h
# End Source File
# Begin Source File

SOURCE=..\ijl.h
# End Source File
# Begin Source File

SOURCE=..\public\ijl.h
# End Source File
# Begin Source File

SOURCE=..\public\valve\interface.h
# End Source File
# Begin Source File

SOURCE=.\Jpg.cpp
# End Source File
# Begin Source File

SOURCE=.\Jpg.h
# End Source File
# Begin Source File

SOURCE=..\login.h
# End Source File
# Begin Source File

SOURCE=..\public\login.h
# End Source File
# Begin Source File

SOURCE=.\macro.h
# End Source File
# Begin Source File

SOURCE=..\macrodef.h
# End Source File
# Begin Source File

SOURCE=..\public\macrodef.h
# End Source File
# Begin Source File

SOURCE=..\map.h
# End Source File
# Begin Source File

SOURCE=..\public\map.h
# End Source File
# Begin Source File

SOURCE=..\map_info.h
# End Source File
# Begin Source File

SOURCE=..\public\map_info.h
# End Source File
# Begin Source File

SOURCE=.\net.cpp
# End Source File
# Begin Source File

SOURCE=.\net.h
# End Source File
# Begin Source File

SOURCE=.\net_msg.h
# End Source File
# Begin Source File

SOURCE=..\player_info.h
# End Source File
# Begin Source File

SOURCE=..\public\player_info.h
# End Source File
# Begin Source File

SOURCE=..\public\recording.h
# End Source File
# Begin Source File

SOURCE=.\resource.h
# End Source File
# Begin Source File

SOURCE=.\system.h
# End Source File
# Begin Source File

SOURCE=.\taskman.h
# End Source File
# Begin Source File

SOURCE=.\TextMsg.h
# End Source File
# Begin Source File

SOURCE=..\public\ugdef.h
# End Source File
# Begin Source File

SOURCE=..\ugdef.h
# End Source File
# Begin Source File

SOURCE=..\public\UGSQL.h
# End Source File
# Begin Source File

SOURCE=..\UGSQL.h
# End Source File
# Begin Source File

SOURCE=.\utility.h
# End Source File
# Begin Source File

SOURCE=".\vari-ext.h"
# End Source File
# Begin Source File

SOURCE=.\variable.h
# End Source File
# Begin Source File

SOURCE=..\public\version.h
# End Source File
# Begin Source File

SOURCE=..\version.h
# End Source File
# Begin Source File

SOURCE=.\Weather.h
# End Source File
# Begin Source File

SOURCE=..\public\zconf.h
# End Source File
# Begin Source File

SOURCE=..\zconf.h
# End Source File
# Begin Source File

SOURCE=..\public\zlib.h
# End Source File
# Begin Source File

SOURCE=..\zlib.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\res\main.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\ijl15.lib
# End Source File
# Begin Source File

SOURCE=..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\lib\control_src.lib
# End Source File
# Begin Source File

SOURCE=..\lib\chat.lib
# End Source File
# Begin Source File

SOURCE=..\lib\utillib.lib
# End Source File
# Begin Source File

SOURCE=..\lib\ChatWindow.lib
# End Source File
# End Target
# End Project
