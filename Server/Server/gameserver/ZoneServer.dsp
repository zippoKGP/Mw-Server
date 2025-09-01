# Microsoft Developer Studio Project File - Name="ZoneServer" - Package Owner=<4>
# Microsoft Developer Studio Generated Build File, Format Version 6.00
# ** DO NOT EDIT **

# TARGTYPE "Win32 (x86) Application" 0x0101

CFG=ZoneServer - Win32 Service Release
!MESSAGE This is not a valid makefile. To build this project using NMAKE,
!MESSAGE use the Export Makefile command and run
!MESSAGE 
!MESSAGE NMAKE /f "ZoneServer.mak".
!MESSAGE 
!MESSAGE You can specify a configuration when running NMAKE
!MESSAGE by defining the macro CFG on the command line. For example:
!MESSAGE 
!MESSAGE NMAKE /f "ZoneServer.mak" CFG="ZoneServer - Win32 Service Release"
!MESSAGE 
!MESSAGE Possible choices for configuration are:
!MESSAGE 
!MESSAGE "ZoneServer - Win32 Release" (based on "Win32 (x86) Application")
!MESSAGE "ZoneServer - Win32 Debug" (based on "Win32 (x86) Application")
!MESSAGE "ZoneServer - Win32 Service Debug" (based on "Win32 (x86) Application")
!MESSAGE "ZoneServer - Win32 Service Release" (based on "Win32 (x86) Application")
!MESSAGE 

# Begin Project
# PROP AllowPerConfigDependencies 0
# PROP Scc_ProjName ""$/RF_Server/ZoneServer", HKAAAAAA"
# PROP Scc_LocalPath "."
CPP=cl.exe
MTL=midl.exe
RSC=rc.exe

!IF  "$(CFG)" == "ZoneServer - Win32 Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 0
# PROP BASE Output_Dir "Release"
# PROP BASE Intermediate_Dir "Release"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 0
# PROP Output_Dir "..\..\RF_Build\release\ZoneServer"
# PROP Intermediate_Dir "..\..\RF_Build\release\ZoneServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /GX /O2 /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /c
# ADD CPP /nologo /MD /W3 /GX /O2 /I "./GameMain" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /D "WIN32" /D "NDEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /FR /Yu"stdafx.h" /FD /c
# ADD BASE MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "NDEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "NDEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /machine:I386
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\..\RF_Bin\DX3D.lib R3EngineMTDLL_NoSound.lib JpegLibMT.lib /nologo /subsystem:windows /map /machine:I386 /out:"..\..\RF_Bin\ZoneServerR.exe"

!ELSEIF  "$(CFG)" == "ZoneServer - Win32 Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "Debug"
# PROP BASE Intermediate_Dir "Debug"
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\ZoneServer"
# PROP Intermediate_Dir "..\..\RF_Build\debug\ZoneServer"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_AFXDLL" /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MDd /W3 /Gm /GX /ZI /Od /I "./GameMain" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /I "../../R3Engine/1stClass" /I "../../R3Engine/2ndClass" /I "../../R3Engine/Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /YX /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 /nologo /subsystem:windows /debug /machine:I386 /pdbtype:sept
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\..\RF_Bin\DX3D.lib ..\..\R3Engine\R3EngineMTDLL_NoSound.lib ..\..\R3Engine\JpegLibMT.lib odbc32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"..\..\RF_Bin\ZoneServerD.exe" /MAPINFO:EXPORTS /MAPINFO:LINES

!ELSEIF  "$(CFG)" == "ZoneServer - Win32 Service Debug"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ZoneServer___Win32_Service_Debug"
# PROP BASE Intermediate_Dir "ZoneServer___Win32_Service_Debug"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\ZoneServerSD"
# PROP Intermediate_Dir "..\..\RF_Build\debug\ZoneServerSD"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "./GameMain" /I "../R3Particle" /I "../R3Engine/1stclass" /I "../R3Engine/2ndclass" /I "../R3Engine/common" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "./GameMain" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /I "../../R3Engine/1stClass" /I "../../R3Engine/2ndClass" /I "../../R3Engine/Common" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "__SERVICE" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\bin\DX3D.lib ..\R3Engine\R3EngineMTDLL_NoSound.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"..\bin\ZoneServerD.exe" /MAPINFO:EXPORTS /MAPINFO:LINES
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\..\RF_Bin\DX3D.lib ..\..\R3Engine\R3EngineMTDLL_NoSound.lib ..\..\R3Engine\JpegLibMT.lib odbc32.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"..\..\RF_Bin\ZoneServerSD.exe" /MAPINFO:EXPORTS /MAPINFO:LINES

!ELSEIF  "$(CFG)" == "ZoneServer - Win32 Service Release"

# PROP BASE Use_MFC 6
# PROP BASE Use_Debug_Libraries 1
# PROP BASE Output_Dir "ZoneServer___Win32_Service_Release"
# PROP BASE Intermediate_Dir "ZoneServer___Win32_Service_Release"
# PROP BASE Ignore_Export_Lib 0
# PROP BASE Target_Dir ""
# PROP Use_MFC 6
# PROP Use_Debug_Libraries 1
# PROP Output_Dir "..\..\RF_Build\debug\ZoneServerSR"
# PROP Intermediate_Dir "..\..\RF_Build\debug\ZoneServerSR"
# PROP Ignore_Export_Lib 0
# PROP Target_Dir ""
# ADD BASE CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "./GameMain" /I "../R3Particle" /I "../R3Engine/1stclass" /I "../R3Engine/2ndclass" /I "../R3Engine/common" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "__SERVICE" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD CPP /nologo /MD /W3 /Gm /GX /ZI /Od /I "./GameMain" /I "../Network" /I "../Protocol" /I "../Table" /I "../Table/Common" /I "../Common" /I "./" /I "../Database" /D "WIN32" /D "_DEBUG" /D "_WINDOWS" /D "_MBCS" /D "_AFXDLL" /D "__SERVICE" /Fr /Yu"stdafx.h" /FD /GZ /c
# ADD BASE MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD MTL /nologo /D "_DEBUG" /mktyplib203 /win32
# ADD BASE RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
# ADD RSC /l 0x412 /d "_DEBUG" /d "_AFXDLL"
BSC32=bscmake.exe
# ADD BASE BSC32 /nologo
# ADD BSC32 /nologo
LINK32=link.exe
# ADD BASE LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\bin\DX3D.lib ..\R3Engine\R3EngineMTDLL_NoSound.lib ..\R3Engine\JpegLibMT.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"..\bin\ZoneServerSD.exe" /MAPINFO:EXPORTS /MAPINFO:LINES
# ADD LINK32 winmm.lib ddraw.lib dxguid.lib Ws2_32.lib d3d8.lib d3dx8dt.lib ..\..\RF_Bin\DX3D.lib R3EngineMTDLL_NoSound.lib JpegLibMT.lib /nologo /subsystem:windows /profile /debug /machine:I386 /out:"..\..\RF_Bin\ZoneServerSR.exe" /MAPINFO:EXPORTS /MAPINFO:LINES

!ENDIF 

# Begin Target

# Name "ZoneServer - Win32 Release"
# Name "ZoneServer - Win32 Debug"
# Name "ZoneServer - Win32 Service Debug"
# Name "ZoneServer - Win32 Service Release"
# Begin Group "Wnd"

# PROP Default_Filter ""
# Begin Group "Resource Files"

# PROP Default_Filter "ico;cur;bmp;dlg;rc2;rct;bin;rgs;gif;jpg;jpeg;jpe"
# Begin Source File

SOURCE=.\Resource.h
# End Source File
# Begin Source File

SOURCE=.\StdAfx.cpp
# ADD CPP /Yc"stdafx.h"
# End Source File
# Begin Source File

SOURCE=.\StdAfx.h
# End Source File
# Begin Source File

SOURCE=.\ZoneServer.ico
# End Source File
# Begin Source File

SOURCE=.\ZoneServer.rc
# End Source File
# Begin Source File

SOURCE=.\ZoneServer.rc2
# End Source File
# Begin Source File

SOURCE=.\ZoneServerDoc.ico
# End Source File
# End Group
# Begin Group "ViewWnd"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\DisplayView.cpp
# End Source File
# Begin Source File

SOURCE=.\DisplayView.h
# End Source File
# Begin Source File

SOURCE=.\InfoSheet.cpp
# End Source File
# Begin Source File

SOURCE=.\InfoSheet.h
# End Source File
# Begin Source File

SOURCE=.\IPXTab.cpp
# End Source File
# Begin Source File

SOURCE=.\IPXTab.h
# End Source File
# Begin Source File

SOURCE=.\MapTab.cpp
# End Source File
# Begin Source File

SOURCE=.\MapTab.h
# End Source File
# Begin Source File

SOURCE=.\ObjectSearchDlg.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectSearchDlg.h
# End Source File
# Begin Source File

SOURCE=.\ObjectTab.cpp
# End Source File
# Begin Source File

SOURCE=.\ObjectTab.h
# End Source File
# Begin Source File

SOURCE=.\ServerTab.cpp
# End Source File
# Begin Source File

SOURCE=.\ServerTab.h
# End Source File
# Begin Source File

SOURCE=.\TCPTab.cpp
# End Source File
# Begin Source File

SOURCE=.\TCPTab.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\MainFrm.cpp
# End Source File
# Begin Source File

SOURCE=.\MainFrm.h
# End Source File
# Begin Source File

SOURCE=.\opendlg.cpp
# End Source File
# Begin Source File

SOURCE=.\opendlg.h
# End Source File
# Begin Source File

SOURCE=.\ZoneServer.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneServer.h
# End Source File
# Begin Source File

SOURCE=.\ZoneServerDoc.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneServerDoc.h
# End Source File
# Begin Source File

SOURCE=.\ZoneServerView.cpp
# End Source File
# Begin Source File

SOURCE=.\ZoneServerView.h
# End Source File
# End Group
# Begin Group "GameMain"

# PROP Default_Filter ""
# Begin Group "Object"

# PROP Default_Filter ""
# Begin Group "Player"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\database\characterdb.cpp
# End Source File
# Begin Source File

SOURCE=..\database\characterdb.h
# End Source File
# Begin Source File

SOURCE=..\database\ErrorEventFromServer.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\PartyPlayer.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\PartyPlayer.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Attack.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Chat.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Class.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Dungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Effect.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Item.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Mgr.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Party.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Position.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Quest.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Recall.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_SendMsg.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_System.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_Trade.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Player_UI.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Player_Unit.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\PlayerDB.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\PlayerDB.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\PlayerStruct.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\PlayerStruct.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\QuestMgr.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\QuestMgr.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\StorageList.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\StorageList.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\UserDB.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\UserDB.h
# End Source File
# End Group
# Begin Group "NPCs"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameMain\AIMessage.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\Animus.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Animus.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\GuardTower.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\GuardTower.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\Monster.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Monster.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MonsterAI.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MonsterAI.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\MonsterLootingMgr.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\NPC.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\NPC.h
# End Source File
# End Group
# Begin Group "Item"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gamemain\BattleDungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\BattleDungeon.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\ItemBox.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\ItemBox.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\ItemStore.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\ItemStore.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\ParkingUnit.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\ParkingUnit.h
# End Source File
# End Group
# Begin Group "Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gamemain\Attack.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Attack.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\Character.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\Character.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\EconomySystem.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\EconomySystem.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\GameObject.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\GameObject.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\GameObjectList.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MethodParameter.h
# End Source File
# Begin Source File

SOURCE=..\database\RFErrMsgFormCode.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\SFDataEffect.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\SFDataEffect.h
# End Source File
# End Group
# End Group
# Begin Group "Map"

# PROP Default_Filter ""
# Begin Group "Display"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameMain\CollLineDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\CollLineDraw.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\DummyDraw.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\DummyDraw.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapDisplay.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapDisplay.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapExtend.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapExtend.h
# End Source File
# End Group
# Begin Source File

SOURCE=.\GameMain\MapData.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapData.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapDummy.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapDummy.h
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapOperate.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MapOperate.h
# End Source File
# End Group
# Begin Group "Main"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\GameMain\MainThread.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MainThread.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\MainThread_db.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MsgProcess.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MsgProcess.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\MyNetWork_Account.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Attack.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Chat.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\MyNetWork_Dungeon.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Init.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Item.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Map.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\Mynetwork_Message.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Party.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Position.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\MyNetwork_Recall.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Resource.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_System.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_Trade.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWork_UI.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\MyNetwork_Unit.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWorking.cpp
# End Source File
# Begin Source File

SOURCE=.\GameMain\MyNetWorking.h
# End Source File
# End Group
# Begin Group "Game Common"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\common\ConnNumPHMgr.h
# End Source File
# Begin Source File

SOURCE=..\Common\ddutil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\ddutil.h
# End Source File
# Begin Source File

SOURCE=..\Common\dxutil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\dxutil.h
# End Source File
# Begin Source File

SOURCE=..\Common\MsgData.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MsgData.h
# End Source File
# Begin Source File

SOURCE=..\common\MyClassUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MyClassUtil.h
# End Source File
# Begin Source File

SOURCE=..\Common\MyUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Common\MyUtil.h
# End Source File
# End Group
# Begin Group "Data"

# PROP Default_Filter ""
# Begin Source File

SOURCE=.\gamemain\DBQrySyn.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\MgrAvatorItemHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\MgrAvatorItemHistory.h
# End Source File
# Begin Source File

SOURCE=.\gamemain\MgrAvatorLvHistory.cpp
# End Source File
# Begin Source File

SOURCE=.\gamemain\MgrAvatorLvHistory.h
# End Source File
# End Group
# End Group
# Begin Group "Protocol"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Protocol\Protocol.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_account_world.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_msg_client.h
# End Source File
# Begin Source File

SOURCE=..\protocol\pt_world_msg.h
# End Source File
# Begin Source File

SOURCE=..\Protocol\pt_zone_client.h
# End Source File
# End Group
# Begin Group "Table"

# PROP Default_Filter ""
# Begin Group "ReProcess"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Table\ItemLootTable.h
# End Source File
# Begin Source File

SOURCE=..\Table\ItemMakeDataTable.h
# End Source File
# Begin Source File

SOURCE=..\Table\ItemUpgradeTable.h
# End Source File
# Begin Source File

SOURCE=..\Table\MapDataTable.h
# End Source File
# Begin Source File

SOURCE=..\Table\OreCuttingTable.h
# End Source File
# Begin Source File

SOURCE=..\Table\WeaponBulletLinkTable.h
# End Source File
# End Group
# Begin Source File

SOURCE=..\Table\Common\AmuletItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\AnimusFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\AnimusItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\BaseFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\BatteryItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\BattleDungeonItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\BootyItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\BulletItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\ClassFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\DfnEquipItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\EtcItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ExpFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\FaceItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ForceFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ForceItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\GuardTowerItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ItemLootFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ItemMakeDataFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ItemUpgradeFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\MakeToolItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\MapFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\MapItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\MonBlockFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\MonsterCharacterFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\NPCharacterFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\OreCuttingFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\OreItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\PlayerCharacterFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\PortalFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\PotionItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\QuestEventFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\QuestFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\ResourceItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\RingItemFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\SkillFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\StoreListFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\TOWNItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitBulletFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitFrameFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitKeyItemFld.h
# End Source File
# Begin Source File

SOURCE=..\table\common\UnitPartFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\WeaponBulletLinkFld.h
# End Source File
# Begin Source File

SOURCE=..\Table\Common\WeaponItemFld.h
# End Source File
# End Group
# Begin Group "Network"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\Network\NetProcess.cpp
# End Source File
# Begin Source File

SOURCE=..\Network\NetProcess.h
# End Source File
# Begin Source File

SOURCE=..\Network\NetSocket.cpp
# End Source File
# Begin Source File

SOURCE=..\Network\NetSocket.h
# End Source File
# Begin Source File

SOURCE=..\Network\NetUtil.cpp
# End Source File
# Begin Source File

SOURCE=..\Network\NetUtil.h
# End Source File
# Begin Source File

SOURCE=..\Network\NetWorking.cpp
# End Source File
# Begin Source File

SOURCE=..\Network\NetWorking.h
# End Source File
# End Group
# Begin Group "Database"

# PROP Default_Filter ""
# Begin Source File

SOURCE=..\database\rfnewdatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\rfnewdatabase.h
# End Source File
# Begin Source File

SOURCE=..\database\rfworlddatabase.cpp
# End Source File
# Begin Source File

SOURCE=..\database\rfworlddatabase.h
# End Source File
# End Group
# End Target
# End Project
