#ifndef __UG_GLOBAL_FUNCTION__
#define __UG_GLOBAL_FUNCTION__
//-------------------------------------------------------------------------------------
template<class T>
void ReleasePC(T& pC)
{
	if(pC)
	{
		pC->Release();
		pC = NULL;
	}
	return;
}

template<class T>
void ReleaseP(T& p)
{
	if(p)
	{
		delete p;
		p = NULL;
	}
	return;
}

template<class T>
void ReleasePA(T& pA)
{
	if(pA)
	{
		delete[] pA;
		pA = NULL;
	}
	return;
}

//-------------------------------------------------------------------------------------
#endif//( __UG_GLOBAL_FUNCTION__)
