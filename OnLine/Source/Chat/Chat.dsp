# Microsoft Developer Studio Project File - Name="Chat" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=Chat - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "Chat.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "Chat.mak" CFG="Chat - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "Chat - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "Chat - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "Chat - Win32 Release"

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
# ADD CPP /nologo /W3 /GX /O2 /I "..\engine" /I "..\dx9sdk\include" /I "..\public" /I "..\rayssdk\include" /I "..\public\valve" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetDir=.\Release
InputPath=.\Release\Chat.lib
SOURCE="$(InputPath)"

"..\lib\chat.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist $(TargetDir)\chat.lib copy $(TargetDir)\chat.lib ..\lib\chat.lib

# End Custom Build

!ELSEIF  "$(CFG)" == "Chat - Win32 Debug"

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
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "..\engine" /I "..\dx9sdk\include" /I "..\public" /I "..\rayssdk\include" /I "..\public\valve" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"..\lib\Chat.lib"
# Begin Custom Build
TargetDir=\余澄丹的工作\神界源码\src_main\lib
InputPath=..\lib\chat.lib
SOURCE="$(InputPath)"

"..\lib\chat.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist $(TargetDir)\chat.lib copy $(TargetDir)\chat.lib ..\lib\chat.lib

# End Custom Build

!ENDIF 

# Begin Target

# Name "Chat - Win32 Release"
# Name "Chat - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\Chat.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatData.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatFace.cpp
# End Source File
# Begin Source File

SOURCE=.\ChatPanel.cpp
# End Source File
# Begin Source File

SOURCE=.\UGDraw.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ChatData.h
# End Source File
# Begin Source File

SOURCE=.\ChatDef.h
# End Source File
# Begin Source File

SOURCE=.\ChatFace.h
# End Source File
# Begin Source File

SOURCE=.\ChatPanel.h
# End Source File
# Begin Source File

SOURCE=..\public\ichat.h
# End Source File
# Begin Source File

SOURCE=.\UGDraw.h
# End Source File
# End Group
# End Target
# End Project
