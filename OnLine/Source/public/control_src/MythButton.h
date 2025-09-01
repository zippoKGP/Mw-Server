// MythButton.h: interface for the CMythButton class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MYTHBUTTON_H__5A9756AC_77EC_4D6F_A9B6_A6CB2669187B__INCLUDED_)
#define AFX_MYTHBUTTON_H__5A9756AC_77EC_4D6F_A9B6_A6CB2669187B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ugbutton.h"

class CMythButton : public CUGButton
{
public:
	CMythButton();

	virtual ~CMythButton();

public:
	int Init(int handle, const char *cmd);

	HRESULT Paint(void *bmp, int handle, char *cmd);

private:
	char m_strCmd[32];
};

#endif // !defined(AFX_MYTHBUTTON_H__5A9756AC_77EC_4D6F_A9B6_A6CB2669187B__INCLUDED_)
