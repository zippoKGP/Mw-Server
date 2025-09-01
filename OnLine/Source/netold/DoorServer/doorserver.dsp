# Microsoft Developer Studio Project File - Name="doorserver" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=doorserver - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "doorserver.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "doorserver.mak" CFG="doorserver - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "doorserver - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "doorserver - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "doorserver - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\output\Release"
# PROP Intermediate_Dir "..\intermediate\Release\doorserver"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W4 /GX /O2 /I "../../public/valve" /I "../../public" /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386 /out:"..\output\Release/doorsvr.exe"
# Begin Custom Build
TargetPath=\Project\myth_world\src_main\net\output\Release\doorsvr.exe
InputPath=\Project\myth_world\src_main\net\output\Release\doorsvr.exe
SOURCE="$(InputPath)"

"..\..\..\server_bin\doorsvr.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\server_bin\doorsvr.exe attrib -r ..\..\..\server_bin\doorsvr.exe 
	copy $(TargetPath) ..\..\..\server_bin\doorsvr.exe 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "doorserver - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\output\Debug"
# PROP Intermediate_Dir "..\intermediate\Debug\doorserver"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W4 /Gm /GX /ZI /Od /I "../../public/valve" /I "../../public" /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /out:"..\output\Debug/doorsvr.exe" /pdbtype:sept
# Begin Custom Build
TargetPath=\Project\myth_world\src_main\net\output\Debug\doorsvr.exe
InputPath=\Project\myth_world\src_main\net\output\Debug\doorsvr.exe
SOURCE="$(InputPath)"

"..\..\..\server_bin\doorsvr.exe" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\..\server_bin\doorsvr.exe attrib -r ..\..\..\server_bin\doorsvr.exe 
	copy $(TargetPath) ..\..\..\server_bin\doorsvr.exe 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "doorserver - Win32 Release"
# Name "doorserver - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\DoorApp.cpp
# End Source File
# Begin Source File

SOURCE=.\DoorConfig.cpp
# End Source File
# Begin Source File

SOURCE=.\doorserver.cpp
# End Source File
# Begin Source File

SOURCE=.\doorserver.rc
# End Source File
# Begin Source File

SOURCE=.\GM.cpp
# End Source File
# Begin Source File

SOURCE=.\GMManager.cpp
# End Source File
# Begin Source File

SOURCE=..\..\public\valve\interface.cpp
# End Source File
# Begin Source File

SOURCE=.\Worldbase.cpp
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManager.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\DoorApp.h
# End Source File
# Begin Source File

SOURCE=.\DoorConfig.h
# End Source File
# Begin Source File

SOURCE=.\GM.h
# End Source File
# Begin Source File

SOURCE=.\GMManager.h
# End Source File
# Begin Source File

SOURCE=.\incall.h
# End Source File
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\Worldbase.h
# End Source File
# Begin Source File

SOURCE=.\WorldbaseManager.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\icon1.ico
# End Source File
# End Group
# Begin Source File

SOURCE=..\..\lib\UGClass.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\UGPlat_lib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\utillib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\tier0.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\public\vstdlib.lib
# End Source File
# Begin Source File

SOURCE=..\..\lib\mysql\libmySQL.lib
# End Source File
# End Target
# End Project
