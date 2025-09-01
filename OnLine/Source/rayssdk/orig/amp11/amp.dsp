# Microsoft Developer Studio Project File - Name="amp" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Console Application" 0x0103

CFG=amp - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "amp.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "amp.mak" CFG="amp - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "amp - Win32 Release" (based on "Win32 (x86) Console Application")
!MESSAGE "amp - Win32 Debug" (based on "Win32 (x86) Console Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "amp - Win32 Release"

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
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /machine:I386

!ELSEIF  "$(CFG)" == "amp - Win32 Debug"

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
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_CONSOLE" /D "_MBCS" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept
# ADD LINK32 kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib kernel32.lib user32.lib gdi32.lib winspool.lib comdlg32.lib advapi32.lib shell32.lib ole32.lib oleaut32.lib uuid.lib odbc32.lib odbccp32.lib /nologo /subsystem:console /debug /machine:I386 /pdbtype:sept

!ENDIF 

# Begin Target

# Name "amp - Win32 Release"
# Name "amp - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=..\amp11\amp11.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\amp1dec.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\amp2dec.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\amp3dec.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\ampdec.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\ampsynth.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfarc.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfcon.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfhttp.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfile.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfilef.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfmem.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfplnt.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfplwv.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfstd.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binftcp.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\mpgsplit.cpp
# End Source File
# Begin Source File

SOURCE=..\amp11\timer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=..\amp11\ampdec.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfarc.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfcon.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfhttp.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfile.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfmem.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfplnt.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfplwv.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binfstd.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\binftcp.h
# End Source File
# Begin Source File

SOURCE=..\amp11\mpgsplit.h
# End Source File
# Begin Source File

SOURCE=..\amp11\binfile\ptypes.h
# End Source File
# Begin Source File

SOURCE=..\amp11\timer.h
# End Source File
# End Group
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# End Group
# End Target
# End Project
