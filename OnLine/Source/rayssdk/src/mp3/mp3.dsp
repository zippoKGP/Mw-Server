# Microsoft Developer Studio Project File - Name="mp3" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=mp3 - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "mp3.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "mp3.mak" CFG="mp3 - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "mp3 - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "mp3 - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/rayssdk-mp3", DSAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "mp3 - Win32 Release"

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
# ADD CPP /nologo /MT /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /c
# ADD BASE RSC /l 0x404 /d "NDEBUG"
# ADD RSC /l 0x404 /d "NDEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"\rayssdk\lib\mp3.lib"

!ELSEIF  "$(CFG)" == "mp3 - Win32 Debug"

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
# ADD CPP /nologo /MTd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_MBCS" /D "_LIB" /FR /YX /FD /GZ /c
# ADD BASE RSC /l 0x404 /d "_DEBUG"
# ADD RSC /l 0x404 /d "_DEBUG"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LIB32=link.exe -lib
# ADD BASE LIB32 /nologo
# ADD LIB32 /nologo /out:"\rayssdk\lib\mp3dbg.lib"

!ENDIF 

# Begin Target

# Name "mp3 - Win32 Release"
# Name "mp3 - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\amp1dec.cpp
# End Source File
# Begin Source File

SOURCE=.\amp2dec.cpp
# End Source File
# Begin Source File

SOURCE=.\amp3dec.cpp
# End Source File
# Begin Source File

SOURCE=.\ampdec.cpp
# End Source File
# Begin Source File

SOURCE=.\ampsynth.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfarc.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfcon.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfhttp.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfile.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfilef.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfmem.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfplnt.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfplwv.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binfstd.cpp
# End Source File
# Begin Source File

SOURCE=.\binfile\binftcp.cpp
# End Source File
# Begin Source File

SOURCE=.\dummy.cpp
# End Source File
# Begin Source File

SOURCE=.\mp3.cpp
# End Source File
# Begin Source File

SOURCE=.\mpgsplit.cpp
# End Source File
# Begin Source File

SOURCE=.\timer.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\ampdec.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfarc.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfcon.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfhttp.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfile.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfmem.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfplnt.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfplwv.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binfstd.h
# End Source File
# Begin Source File

SOURCE=.\binfile\binftcp.h
# End Source File
# Begin Source File

SOURCE=.\mp3.h
# End Source File
# Begin Source File

SOURCE=.\mpgsplit.h
# End Source File
# Begin Source File

SOURCE=.\binfile\ptypes.h
# End Source File
# Begin Source File

SOURCE=.\timer.h
# End Source File
# End Group
# Begin Group "Text Files"

# PROP Default_Filter "txt"
# Begin Source File

SOURCE=.\COPYING.txt
# End Source File
# Begin Source File

SOURCE=.\README.txt
# End Source File
# End Group
# End Target
# End Project
