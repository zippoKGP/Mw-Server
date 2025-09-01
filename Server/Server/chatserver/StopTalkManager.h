// StopTalkManager.h: interface for the CStopTalkManager class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_STOPTALKMANAGER_H__FF34BDDA_0FE1_41E9_9E92_F3CAA723A2E5__INCLUDED_)
#define AFX_STOPTALKMANAGER_H__FF34BDDA_0FE1_41E9_9E92_F3CAA723A2E5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CStopTalkManager  
{
public:
	CStopTalkManager();
	virtual ~CStopTalkManager();

public:
	UGINT		init();
	UGINT		render(UGDWORD dwTimer);
	UGINT		cleanup();
	//返回0表示禁言成功，1表示解禁言，否则错误
	UGINT		addPlayer(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes);
	//返回0表示没有被禁言，否则被禁言
	UGINT		findPlayer(UGINT nPlayerID,UGDWORD dwTimer);
	UGINT		setPlayer(UGINT nPlayerID,BOOL bRemove);
	UGINT		loadDB();
	
protected:
	UGINT		insertDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes);
	UGINT		updateDB(UGINT nPlayerID,BOOL bRemove);
	UGINT		updateDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes);
	UGINT		updateDB(UGINT nPlayerID,UGDWORD dwBeginer,UGDWORD dwTimes,BOOL bRemove);
	UGINT		deleteDB(UGINT nPlayerID);
	UGINT		queryDB(UGPCHAR pchQuery);
	UGINT		addPlayer(UGINT nPlayerID,UGINT nBeginer,UGINT nTimes,UGINT nRemoved);
	
private:
	UGDWORD		m_dwLastRenderTimer;

private:
	map<UGINT,CStopTalk*>	m_map;
	list<CStopTalk*>		m_list;
	CStopTalk*				m_pStopTalk;

};

#endif // !defined(AFX_STOPTALKMANAGER_H__FF34BDDA_0FE1_41E9_9E92_F3CAA723A2E5__INCLUDED_)
