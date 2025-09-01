# Microsoft Developer Studio Project File - Name="chatserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=chatserver - Win32 Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "chatserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "chatserver.mak" CFG="chatserver - Win32 Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "chatserver - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "chatserver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "chatserver - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../public/valve" /I "../public" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"Release/chatsvr.exe"
# Begin Custom Build
TargetPath=.\Release\chatsvr.exe
InputPath=.\Release\chatsvr.exe
SOURCE="$(InputPath)"

"..\..\chatsvr\chatsvr.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\chatsvr\chatsvr.exe  attrib -r ..\..\chatsvr\chatsvr.exe 
	copy $(TargetPath) ..\..\chatsvr\chatsvr.exe 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "chatserver - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../public/valve" /I "../public" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"Debug/chatsvr.exe" /pdbtype:sept
# Begin Custom Build
TargetPath=.\Debug\chatsvr.exe
InputPath=.\Debug\chatsvr.exe
SOURCE="$(InputPath)"

"..\..\chatsvr\chatsvr.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\chatsvr\chatsvr.exe  attrib -r ..\..\chatsvr\chatsvr.exe 
	copy $(TargetPath) ..\..\chatsvr\chatsvr.exe 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "chatserver - Win32 Release"
# Name "chatserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\chatserver.cpp
# End Source File
# Begin Source File

SOURCE=.\chatserver.rc
# End Source File
# Begin Source File

SOURCE=.\Cout.cpp
# End Source File
# Begin Source File

SOURCE=.\Friends.cpp
# End Source File
# Begin Source File

SOURCE=.\gmtools.cpp
# End Source File
# Begin Source File

SOURCE=..\public\valve\interface.cpp
# SUBTRACT CPP /YX
# End Source File
# Begin Source File

SOURCE=.\MailMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\MailMsgManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\PlayerManager.cpp
# End Source File
# Begin Source File

SOURCE=.\Scene.cpp
# End Source File
# Begin Source File

SOURCE=.\SceneManager.cpp
# End Source File
# Begin Source File

SOURCE=.\StopTalk.cpp
# End Source File
# Begin Source File

SOURCE=.\StopTalkManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WBConnect.cpp
# End Source File
# Begin Source File

SOURCE=.\Worldbase.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldbaseConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManager.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManagerConfig.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\Cout.h
# End Source File
# Begin Source File

SOURCE=.\Friends.h
# End Source File
# Begin Source File

SOURCE=.\gmtools.h
# End Source File
# Begin Source File

SOURCE=.\incall.h
# End Source File
# Begin Source File

SOURCE=.\MailMsg.h
# End Source File
# Begin Source File

SOURCE=.\MailMsgManager.h
# End Source File
# Begin Source File

SOURCE=.\Player.h
# End Source File
# Begin Source File

SOURCE=.\PlayerManager.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Scene.h
# End Source File
# Begin Source File

SOURCE=.\SceneManager.h
# End Source File
# Begin Source File

SOURCE=.\StopTalk.h
# End Source File
# Begin Source File

SOURCE=.\StopTalkManager.h
# End Source File
# Begin Source File

SOURCE=.\WBConnect.h
# End Source File
# Begin Source File

SOURCE=.\Worldbase.h
# End Source File
# Begin Source File

SOURCE=.\WorldbaseConfig.h
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManager.h
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManagerConfig.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\chatsvr.ico
# End Source File
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# Begin Source File

SOURCE=.\main.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\lib\libmySQL.lib
# End Source File
# Begin Source File

SOURCE=..\lib\UGClass.lib
# End Source File
# Begin Source File

SOURCE=..\lib\UGPlat_lib.lib
# End Source File
# Begin Source File

SOURCE=..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\lib\public\tier0.lib
# End Source File
# End Target
# End Project
