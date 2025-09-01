; CLW file contains information for the MFC ClassWizard

[General Info]
Version=1
LastClass=CTCPTab
LastTemplate=CDialog
NewFileInclude1=#include "stdafx.h"
NewFileInclude2=#include "zoneserver.h"
LastPage=0

ClassCount=14
Class1=CDisplayView
Class2=CInfoSheet
Class3=CIPXTab
Class4=CMainFrame
Class5=CMapTab
Class6=CObjectSearchDlg
Class7=CObjectTab
Class8=COpenDlg
Class9=CServerTab
Class10=CTCPTab
Class11=CGameServerApp
Class12=CAboutDlg
Class13=CGameServerDoc
Class14=CGameServerView

ResourceCount=22
Resource1=IDD_DIALOG_SEARCH
Resource2=IDD_DIALOG_TCPTAB
Resource3=IDR_MAINFRAME
Resource4=IDD_ABOUTBOX
Resource5=IDD_GAMESERVER_FORM
Resource6=IDD_DIALOG_DISPLAY
Resource7=IDD_DIALOG_MAPTAB
Resource8=IDD_DIALOG_SERVERTAB
Resource9=IDD_DIALOG_OBJECTTAB
Resource10=IDD_DIALOG_IPXTAB
Resource11=IDD_DIALOG_Loading
Resource12=IDR_MAINFRAME (Korean)
Resource13=IDD_DIALOG_DISPLAY (Korean)
Resource14=IDD_DIALOG_SERVERTAB (Korean)
Resource15=IDD_GAMESERVER_FORM (Korean)
Resource16=IDD_DIALOG_SEARCH (Korean)
Resource17=IDD_DIALOG_TCPTAB (Korean)
Resource18=IDD_DIALOG_OBJECTTAB (Korean)
Resource19=IDD_ABOUTBOX (Korean)
Resource20=IDD_DIALOG_IPXTAB (Korean)
Resource21=IDD_DIALOG_MAPTAB (Korean)
Resource22=IDD_DIALOG_Loading (Korean)

[CLS:CDisplayView]
Type=0
BaseClass=CDialog
HeaderFile=displayview.h
ImplementationFile=displayview.cpp

[CLS:CInfoSheet]
Type=0
BaseClass=CPropertySheet
HeaderFile=infosheet.h
ImplementationFile=infosheet.cpp

[CLS:CIPXTab]
Type=0
BaseClass=CPropertyPage
HeaderFile=ipxtab.h
ImplementationFile=ipxtab.cpp

[CLS:CMainFrame]
Type=0
BaseClass=CFrameWnd
HeaderFile=mainfrm.h
ImplementationFile=mainfrm.cpp

[CLS:CMapTab]
Type=0
BaseClass=CPropertyPage
HeaderFile=maptab.h
ImplementationFile=maptab.cpp

[CLS:CObjectSearchDlg]
Type=0
BaseClass=CDialog
HeaderFile=objectsearchdlg.h
ImplementationFile=objectsearchdlg.cpp

[CLS:CObjectTab]
Type=0
BaseClass=CPropertyPage
HeaderFile=objecttab.h
ImplementationFile=objecttab.cpp

[CLS:COpenDlg]
Type=0
BaseClass=CDialog
HeaderFile=opendlg.h
ImplementationFile=opendlg.cpp

[CLS:CServerTab]
Type=0
BaseClass=CPropertyPage
HeaderFile=servertab.h
ImplementationFile=servertab.cpp

[CLS:CTCPTab]
Type=0
BaseClass=CPropertyPage
HeaderFile=tcptab.h
ImplementationFile=tcptab.cpp
Filter=D
VirtualFilter=idWC

[CLS:CGameServerApp]
Type=0
BaseClass=CWinApp
HeaderFile=zoneserver.h
ImplementationFile=zoneserver.cpp

[CLS:CAboutDlg]
Type=0
BaseClass=CDialog
HeaderFile=zoneserver.cpp
ImplementationFile=zoneserver.cpp
LastObject=CAboutDlg

[CLS:CGameServerDoc]
Type=0
BaseClass=CDocument
HeaderFile=zoneserverdoc.h
ImplementationFile=zoneserverdoc.cpp

[CLS:CGameServerView]
Type=0
BaseClass=CFormView
HeaderFile=zoneserverview.h
ImplementationFile=zoneserverview.cpp

[DLG:IDD_DIALOG_DISPLAY]
Type=1
Class=CDisplayView
ControlCount=0

[DLG:IDD_DIALOG_IPXTAB]
Type=1
Class=CIPXTab
ControlCount=21
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_MAXSOCKETNUM,edit,1350641794
Control11=IDC_EDIT_CONNSOCKETNUM,edit,1350641794
Control12=IDC_EDIT_RECVBYTE,edit,1350641794
Control13=IDC_EDIT_SENDBYTE,edit,1350641794
Control14=IDC_EDIT_RecvErr,edit,1350641794
Control15=IDC_EDIT_SendErr,edit,1350641794
Control16=IDC_EDIT_SendBlock,edit,1350641794
Control17=IDC_EDIT_BUFLIST,edit,1350641794
Control18=IDC_STATIC,static,1342308352
Control19=IDC_EDIT_ODDMSG,edit,1350641794
Control20=IDC_BUTTON_UPDATELISTSIZE,button,1342242816
Control21=IDC_EDIT_SENDSOCKETNUM,edit,1350641794

[DLG:IDD_DIALOG_MAPTAB]
Type=1
Class=CMapTab
ControlCount=2
Control1=IDC_TREE_MAP,SysTreeView32,1350631431
Control2=IDC_BUTTON_MAPCHANGE,button,1342242816

[DLG:IDD_DIALOG_SEARCH]
Type=1
Class=CObjectSearchDlg
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_OBJECT_INDEX,edit,1350639744
Control7=IDC_COMBO_ObjectKind,combobox,1344340227
Control8=IDC_COMBO_ObjectID,combobox,1344340227
Control9=IDC_EDIT_CharName,edit,1350631552
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_OBJECTTAB]
Type=1
Class=CObjectTab
ControlCount=5
Control1=IDC_TREE_OBJECT,SysTreeView32,1350631431
Control2=IDC_BUTTON_UPDATE,button,1342242816
Control3=IDC_BUTTON_AUTO,button,1342242816
Control4=IDC_BUTTON_SEARCH,button,1342242816
Control5=IDC_BUTTON_DESTORY,button,1342242816

[DLG:IDD_DIALOG_Loading]
Type=1
Class=COpenDlg
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC_Load,static,1342308352

[DLG:IDD_DIALOG_SERVERTAB]
Type=1
Class=CServerTab
ControlCount=2
Control1=IDC_TREE_SERVER,SysTreeView32,1350631431
Control2=IDC_BUTTON_UPDATESERVER,button,1342242816

[DLG:IDD_DIALOG_TCPTAB]
Type=1
Class=CTCPTab
ControlCount=16
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_CONNSOCKETNUM,edit,1350641794
Control7=IDC_EDIT_RECVBYTE,edit,1350641794
Control8=IDC_EDIT_SENDBYTE,edit,1350641794
Control9=IDC_EDIT_RecvErr,edit,1350641794
Control10=IDC_EDIT_SendErr,edit,1350641794
Control11=IDC_EDIT_SendBlock,edit,1350641794
Control12=IDC_STATIC,static,1342308352
Control13=IDC_EDIT_ODDMSG,edit,1350641794
Control14=IDC_EDIT_RecvBlk,edit,1350641794
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_CONNNUM,edit,1350641794

[DLG:IDD_ABOUTBOX]
Type=1
Class=CAboutDlg
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_GAMESERVER_FORM]
Type=1
Class=CGameServerView
ControlCount=5
Control1=IDC_BUTTON_LOGFILE,button,1342242816
Control2=IDC_BUTTON_MONSTER,button,1342242816
Control3=IDC_BUTTON_DISPLAYALL,button,1342242816
Control4=IDC_BUTTON_PreClose,button,1342242816
Control5=IDC_BUTTON_ServerClose,button,1342242816

[TB:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_MENU_DISPLAY
Command10=ID_BUTTON_WRITELOG
Command11=ID_BUTTON_MONSTER
Command12=ID_BUTTON_DUMMY
CommandCount=12

[MNU:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_APP_ABOUT
CommandCount=2

[ACL:IDR_MAINFRAME]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_DIALOG_TCPTAB (Korean)]
Type=1
Class=?
ControlCount=16
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_CONNSOCKETNUM,edit,1350641794
Control7=IDC_EDIT_RECVBYTE,edit,1350641794
Control8=IDC_EDIT_SENDBYTE,edit,1350641794
Control9=IDC_EDIT_RecvErr,edit,1350641794
Control10=IDC_EDIT_SendErr,edit,1350641794
Control11=IDC_EDIT_SendBlock,edit,1350641794
Control12=IDC_STATIC,static,1342308352
Control13=IDC_EDIT_ODDMSG,edit,1350641794
Control14=IDC_EDIT_RecvBlk,edit,1350641794
Control15=IDC_STATIC,static,1342308352
Control16=IDC_EDIT_CONNNUM,edit,1350641794

[TB:IDR_MAINFRAME (Korean)]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_EDIT_CUT
Command5=ID_EDIT_COPY
Command6=ID_EDIT_PASTE
Command7=ID_FILE_PRINT
Command8=ID_APP_ABOUT
Command9=ID_MENU_DISPLAY
Command10=ID_BUTTON_WRITELOG
Command11=ID_BUTTON_MONSTER
Command12=ID_BUTTON_DUMMY
CommandCount=12

[MNU:IDR_MAINFRAME (Korean)]
Type=1
Class=?
Command1=ID_APP_EXIT
Command2=ID_APP_ABOUT
CommandCount=2

[ACL:IDR_MAINFRAME (Korean)]
Type=1
Class=?
Command1=ID_FILE_NEW
Command2=ID_FILE_OPEN
Command3=ID_FILE_SAVE
Command4=ID_FILE_PRINT
Command5=ID_EDIT_UNDO
Command6=ID_EDIT_CUT
Command7=ID_EDIT_COPY
Command8=ID_EDIT_PASTE
Command9=ID_EDIT_UNDO
Command10=ID_EDIT_CUT
Command11=ID_EDIT_COPY
Command12=ID_EDIT_PASTE
Command13=ID_NEXT_PANE
Command14=ID_PREV_PANE
CommandCount=14

[DLG:IDD_ABOUTBOX (Korean)]
Type=1
Class=?
ControlCount=4
Control1=IDC_STATIC,static,1342177283
Control2=IDC_STATIC,static,1342308480
Control3=IDC_STATIC,static,1342308352
Control4=IDOK,button,1342373889

[DLG:IDD_GAMESERVER_FORM (Korean)]
Type=1
Class=?
ControlCount=5
Control1=IDC_BUTTON_LOGFILE,button,1342242816
Control2=IDC_BUTTON_MONSTER,button,1342242816
Control3=IDC_BUTTON_DISPLAYALL,button,1342242816
Control4=IDC_BUTTON_PreClose,button,1342242816
Control5=IDC_BUTTON_ServerClose,button,1342242816

[DLG:IDD_DIALOG_DISPLAY (Korean)]
Type=1
Class=?
ControlCount=0

[DLG:IDD_DIALOG_MAPTAB (Korean)]
Type=1
Class=?
ControlCount=2
Control1=IDC_TREE_MAP,SysTreeView32,1350631431
Control2=IDC_BUTTON_MAPCHANGE,button,1342242816

[DLG:IDD_DIALOG_SERVERTAB (Korean)]
Type=1
Class=?
ControlCount=2
Control1=IDC_TREE_SERVER,SysTreeView32,1350631431
Control2=IDC_BUTTON_UPDATESERVER,button,1342242816

[DLG:IDD_DIALOG_SEARCH (Korean)]
Type=1
Class=?
ControlCount=10
Control1=IDOK,button,1342242817
Control2=IDCANCEL,button,1342242816
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_EDIT_OBJECT_INDEX,edit,1350639744
Control7=IDC_COMBO_ObjectKind,combobox,1344340227
Control8=IDC_COMBO_ObjectID,combobox,1344340227
Control9=IDC_EDIT_CharName,edit,1350631552
Control10=IDC_STATIC,static,1342308352

[DLG:IDD_DIALOG_OBJECTTAB (Korean)]
Type=1
Class=?
ControlCount=5
Control1=IDC_TREE_OBJECT,SysTreeView32,1350631431
Control2=IDC_BUTTON_UPDATE,button,1342242816
Control3=IDC_BUTTON_AUTO,button,1342242816
Control4=IDC_BUTTON_SEARCH,button,1342242816
Control5=IDC_BUTTON_DESTORY,button,1342242816

[DLG:IDD_DIALOG_IPXTAB (Korean)]
Type=1
Class=?
ControlCount=21
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC,static,1342308352
Control3=IDC_STATIC,static,1342308352
Control4=IDC_STATIC,static,1342308352
Control5=IDC_STATIC,static,1342308352
Control6=IDC_STATIC,static,1342308352
Control7=IDC_STATIC,static,1342308352
Control8=IDC_STATIC,static,1342308352
Control9=IDC_STATIC,static,1342308352
Control10=IDC_EDIT_MAXSOCKETNUM,edit,1350641794
Control11=IDC_EDIT_CONNSOCKETNUM,edit,1350641794
Control12=IDC_EDIT_RECVBYTE,edit,1350641794
Control13=IDC_EDIT_SENDBYTE,edit,1350641794
Control14=IDC_EDIT_RecvErr,edit,1350641794
Control15=IDC_EDIT_SendErr,edit,1350641794
Control16=IDC_EDIT_SendBlock,edit,1350641794
Control17=IDC_EDIT_BUFLIST,edit,1350641794
Control18=IDC_STATIC,static,1342308352
Control19=IDC_EDIT_ODDMSG,edit,1350641794
Control20=IDC_BUTTON_UPDATELISTSIZE,button,1342242816
Control21=IDC_EDIT_SENDSOCKETNUM,edit,1350641794

[DLG:IDD_DIALOG_Loading (Korean)]
Type=1
Class=?
ControlCount=2
Control1=IDC_STATIC,static,1342308352
Control2=IDC_STATIC_Load,static,1342308352

