# Microsoft Developer Studio Project File - Name="UGClass_lib" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=UGClass_lib - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "UGClass_lib.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "UGClass_lib.mak" CFG="UGClass_lib - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "UGClass_lib - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "UGClass_lib - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""
# PROP Scc_LocalPath ""
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "UGClass_lib - Win32 Release"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\output\Release"
# PROP Intermediate_Dir "..\intermediate\Release\UGClass_lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /YX /FD /c
# ADD CPP /nologo /W3 /GX /O2 /I "../../public/valve" /I "../../public" /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x804 /d "NDEBUG"
# ADD RSC /l 0x804 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=\Project\myth_world\src_main\net\output\Release\UGClass_lib.lib
InputPath=\Project\myth_world\src_main\net\output\Release\UGClass_lib.lib
SOURCE="$(InputPath)"

"..\..\lib\UGClass.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\lib\UGClass.lib attrib -r ..\..\lib\UGClass.lib 
	copy $(TargetPath) ..\..\lib\UGClass.lib 
	
# End Custom Build

!ELSEIF  "$(CFG)" == "UGClass_lib - Win32 Debug"

# PROP BASE Use_MFC 0
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 0
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\output\Debug"
# PROP Intermediate_Dir "..\intermediate\Debug\UGClass_lib"
# PROP Target_Dir ""
# ADD BASE CPP /nologo /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /YX /FD /GZ /c
# ADD CPP /nologo /W3 /Gm /GX /ZI /Od /I "../../public/valve" /I "../../public" /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x804 /d "_DEBUG"
# ADD RSC /l 0x804 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo
# Begin Custom Build
TargetPath=\Project\myth_world\src_main\net\output\Debug\UGClass_lib.lib
InputPath=\Project\myth_world\src_main\net\output\Debug\UGClass_lib.lib
SOURCE="$(InputPath)"

"..\..\lib\UGClass.lib" : $(SOURCE) "$(INTDIR)" "$(OUTDIR)"
	if exist ..\..\lib\UGClass.lib attrib -r ..\..\lib\UGClass.lib 
	copy $(TargetPath) ..\..\lib\UGClass.lib 
	
# End Custom Build

!ENDIF 

# Begin Target

# Name "UGClass_lib - Win32 Release"
# Name "UGClass_lib - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\FileLine.cpp
# End Source File
# Begin Source File

SOURCE=.\IndexKeyValue.cpp
# End Source File
# Begin Source File

SOURCE=.\Ini.cpp
# End Source File
# Begin Source File

SOURCE=.\IniApp.cpp
# End Source File
# Begin Source File

SOURCE=.\IniKeyValue.cpp
# End Source File
# Begin Source File

SOURCE=.\MD5Encrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\NumberIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\StringIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\TEAEncrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\UGChar.cpp
# End Source File
# Begin Source File

SOURCE=.\UGEncrypt.cpp
# End Source File
# Begin Source File

SOURCE=.\UGIndex.cpp
# End Source File
# Begin Source File

SOURCE=.\UGIni.cpp
# End Source File
# Begin Source File

SOURCE=.\UGLog.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\FileLine.h
# End Source File
# Begin Source File

SOURCE=.\IncAll.h
# End Source File
# Begin Source File

SOURCE=.\IndexKeyValue.h
# End Source File
# Begin Source File

SOURCE=.\Ini.h
# End Source File
# Begin Source File

SOURCE=.\IniApp.h
# End Source File
# Begin Source File

SOURCE=.\IniKeyValue.h
# End Source File
# Begin Source File

SOURCE=.\MD5Encrypt.h
# End Source File
# Begin Source File

SOURCE=.\NumberIndex.h
# End Source File
# Begin Source File

SOURCE=.\StringIndex.h
# End Source File
# Begin Source File

SOURCE=.\TEAEncrypt.h
# End Source File
# Begin Source File

SOURCE=..\..\public\net\UGBenEncrypt.h
# End Source File
# Begin Source File

SOURCE=..\..\public\net\UGBenIni.h
# End Source File
# Begin Source File

SOURCE=..\..\public\net\UGBenLog.h
# End Source File
# Begin Source File

SOURCE=.\UGChar.h
# End Source File
# Begin Source File

SOURCE=..\..\include\UGEncrypt.h
# End Source File
# Begin Source File

SOURCE=..\..\include\UGGlobalFun.h
# End Source File
# Begin Source File

SOURCE=..\..\include\UGIndex.h
# End Source File
# Begin Source File

SOURCE=..\..\include\UGIni.h
# End Source File
# Begin Source File

SOURCE=..\..\include\UGLog.h
# End Source File
# End Group
# End Target
# End Project
