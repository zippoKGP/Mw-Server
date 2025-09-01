# Microsoft Developer Studio Project File - Name="jpeg" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Static Library" 0x0104

CFG=jpeg - Win32 Debug
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "jpeg.mak" CFG="jpeg - Win32 Debug"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "jpeg - Win32 Release" (based on "Win32 (x86) Static Library")
!MESSAGE "jpeg - Win32 Debug" (based on "Win32 (x86) Static Library")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/rayssdk-jpeg", GQAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
RSC=rc.exe

!IF  "$(CFG)" == "jpeg - Win32 Release"

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
# ADD LIB32 /nologo /out:"\rayssdk\lib\jpeg.lib"

!ELSEIF  "$(CFG)" == "jpeg - Win32 Debug"

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
# ADD LIB32 /nologo /out:"\rayssdk\lib\jpegdbg.lib"

!ENDIF 

# Begin Target

# Name "jpeg - Win32 Release"
# Name "jpeg - Win32 Debug"
# Begin Group "Source Files"

# PROP Default_Filter "cpp;c;cxx;rc;def;r;odl;idl;hpj;bat"
# Begin Source File

SOURCE=.\other\Bmp.cpp
# End Source File
# Begin Source File

SOURCE=.\other\cmdlib.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jcomapi.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdapimin.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdapistd.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdatasrc.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdcoefct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdcolor.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jddctmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdhuff.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdinput.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdmainct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdmarker.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdmaster.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdpostct.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdsample.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdtrans.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jerror.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jfdctflt.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jidctflt.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jmemmgr.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jmemnobs.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jpgload.cpp
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jutils.cpp
# End Source File
# Begin Source File

SOURCE=.\other\LBMLIB.CPP
# End Source File
# Begin Source File

SOURCE=.\pakstuff.cpp
# End Source File
# Begin Source File

SOURCE=.\unzip.cpp
# End Source File
# End Group
# Begin Group "Header Files"

# PROP Default_Filter "h;hpp;hxx;hm;inl"
# Begin Source File

SOURCE=.\other\BMP.H
# End Source File
# Begin Source File

SOURCE=.\other\cmdlib.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jchuff.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jconfig.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdct.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jdhuff.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jerror.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jinclude.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jmemsys.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jmorecfg.h
# End Source File
# Begin Source File

SOURCE=.\jpeg.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jpegint.h
# End Source File
# Begin Source File

SOURCE=.\jpeglib.h
# End Source File
# Begin Source File

SOURCE=.\jpeg6\jversion.h
# End Source File
# Begin Source File

SOURCE=.\other\LBMLIB.H
# End Source File
# Begin Source File

SOURCE=.\pakstuff.h
# End Source File
# Begin Source File

SOURCE=.\str.h
# End Source File
# Begin Source File

SOURCE=.\unzip.h
# End Source File
# End Group
# End Target
# End Project
