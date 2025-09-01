// MaxMaterialCollection.cpp : Implementation of CMaxMaterialCollection
#include "stdafx.h"
#include "Comsrv.h"
#include "MaxMaterialCollection.h"
#include "imtledit.h"

#ifdef EXTENDED_OBJECT_MODEL

#include "MaxMaterial.h"
#include <comutil.h>

//we create an XMLMAterial importer, need its interface and guids
#include "xmlmtl.h"
#include "xmlmtl_i.c"

//Refmaker array access
#define NUM_MMC_REFS 2
#define SCENE_MTLLIB_REFNUM 0
#define SCRATCH_MTLLIB_REFNUM 1
#define RECYCLE_MTLLIB_REFNUM 2
// add new refs here

#define RECYCLE_MATS
#define PERSIST_SCRATCH

#ifdef PERSIST_SCRATCH
static ScratchLib theScratchMtlLib;
#endif

extern TCHAR *GetString(int id);
bool CMaxMaterialCollection::s_InMakeWrapper = false;
HMODULE CMaxMaterialCollection::hXmlMtlDllModule = NULL;



/////////////////////////////////////////////////////////////////////////////
// CMaxMaterialCollection
STDMETHODIMP CMaxMaterialCollection::FinalConstruct()
{
	HRESULT hr = E_FAIL;
	//simply make a reference to the scene material collection
	MtlBaseLib* pMtlLib = GetCOREInterface()->GetSceneMtls();
	mLastSceneMtlLibSize = 0;

	assert(pMtlLib);
#ifdef RECYCLE_MATS
#endif

	RegisterNotification(NotifyProc, this, NOTIFY_FILE_PRE_OPEN);
	RegisterNotification(NotifyProc, this, NOTIFY_FILE_POST_OPEN);
	RegisterNotification(NotifyProc, this, NOTIFY_FILE_PRE_MERGE);
	RegisterNotification(NotifyProc, this, NOTIFY_FILE_POST_MERGE);
	RegisterNotification(NotifyProc, this, NOTIFY_FILE_PRE_SAVE);
	RegisterNotification(NotifyProc, this, NOTIFY_FILE_POST_SAVE);
	RegisterNotification(NotifyProc, this, NOTIFY_PRE_IMPORT);
	RegisterNotification(NotifyProc, this, NOTIFY_POST_IMPORT);
	RegisterNotification(NotifyProc, this, NOTIFY_SYSTEM_PRE_NEW);
	RegisterNotification(NotifyProc, this, NOTIFY_SYSTEM_POST_NEW);
	RegisterNotification(NotifyProc, this, NOTIFY_SYSTEM_PRE_RESET);
	RegisterNotification(NotifyProc, this, NOTIFY_SYSTEM_POST_RESET);
	RegisterNotification(NotifyProc, this, NOTIFY_SCENE_UNDO);
	RegisterNotification(NotifyProc, this, NOTIFY_SCENE_REDO);

	RegisterNotification(NotifyProc, (void *)this, NOTIFY_MEDIT_SHOW);



	RefResult res = MakeRefByID(FOREVER, SCENE_MTLLIB_REFNUM, (RefTargetHandle) pMtlLib);
#ifdef PERSIST_SCRATCH
	mpScratchMtlLib = NULL;
	res = MakeRefByID(FOREVER, SCRATCH_MTLLIB_REFNUM, (RefTargetHandle) &theScratchMtlLib);
#endif

	Resume();
	if(res == REF_SUCCEED)
		hr = S_OK;
	return hr;
}

void CMaxMaterialCollection::FinalRelease()
{
	DeleteAllRefsFromMe();
}


STDMETHODIMP CMaxMaterialCollection::createNew(IMaxMaterial **ppMtl)
{
	if(!ppMtl)
		return E_INVALIDARG;
	CComPtr<IMaxMaterial> pMtlWrapper;

	//Create the wrapper
	MtlWrapper *pWrapperObject;
	HRESULT hr = MtlWrapper::CreateInstance(&pWrapperObject);
	assert(SUCCEEDED(hr));
	if(!SUCCEEDED(hr))
		return hr;

	//Initalize the new wrapper
	pWrapperObject->setCollection(this);

	hr = pWrapperObject->QueryInterface(IID_IMaxMaterial, (void **)ppMtl);
	if(!SUCCEEDED(hr))
		return hr;

	return hr;
}

static bool getUL(const VARIANT& v, unsigned long& val)
{
	switch (v.vt) {
		case VT_UI1: val = v.bVal; break;
		case VT_I2: val = v.iVal; break;
		case VT_I4: val = v.lVal; break;
		case VT_BYREF|VT_UI1: val = *v.pbVal; break;
		case VT_BYREF|VT_I2: val = *v.piVal; break;
		case VT_BYREF|VT_I4: val = *v.plVal; break;
		case VT_I1: val = v.cVal; break;
		case VT_UI2: val = v.uiVal; break;
		case VT_UI4: val = v.ulVal; break;
		case VT_INT: val = v.intVal; break;
		case VT_UINT: val = v.uintVal; break;
		case VT_BYREF|VT_I1: val = *v.pcVal; break;
		case VT_BYREF|VT_UI2: val = *v.puiVal; break;
		case VT_BYREF|VT_UI4: val = *v.pulVal; break;
		case VT_BYREF|VT_INT: val = *v.pintVal; break;
		case VT_BYREF|VT_UINT: val = *v.puintVal; break;
		case VT_BSTR: {
			unsigned long ul;
			char c;
			if (swscanf(_bstr_t(v.bstrVal), L" %lu %c", &ul, &c) != 1)
				return false;
			val = ul;
		} break;
		default:
			return false;
	}
	return true;
}

STDMETHODIMP CMaxMaterialCollection::createFromMaxID(VARIANT partA, VARIANT partB, IMaxMaterial ** ppMtl)
{
	unsigned long a, b;
	if(!ppMtl || !getUL(partA, a) || !getUL(partB, b))
		return E_INVALIDARG;

	//Create the wrapper
	MtlWrapper *pWrapperObject;
	HRESULT hr = MtlWrapper::CreateInstance(&pWrapperObject);
	assert(SUCCEEDED(hr));
	if(!SUCCEEDED(hr))
		return hr;

	hr = pWrapperObject->QueryInterface(IID_IMaxMaterial, (void **)ppMtl);
	if(!SUCCEEDED(hr))
		return hr;

	Mtl* mtl = static_cast<Mtl*>(::CreateInstance(MATERIAL_CLASS_ID, Class_ID(a, b)));
	if (mtl == NULL)
		return E_INVALIDARG;

	//Initalize the new wrapper
	pWrapperObject->setCollection(this);
	pWrapperObject->setMtl(mtl);

	return hr;
}

//Implement reference maker methods

RefResult CMaxMaterialCollection::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,RefMessage message)
{
	if(m_suspendCount > 0)
		return REF_SUCCEED;
	if(hTarget == mpSceneMtlLib)
	{
		switch(message)
		{
		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
			OnSceneLibChanged();
			break;
		default:
			break;
		}
	}
	else if(hTarget == mpScratchMtlLib)
	{
		switch(message)
		{
		case REFMSG_SUBANIM_STRUCTURE_CHANGED:
			OnScratchLibChanged();
			break;
		default:
			break;
		}
	}
	return REF_SUCCEED;
}

int CMaxMaterialCollection::NumRefs()
{
	return NUM_MMC_REFS;
}

RefTargetHandle CMaxMaterialCollection::GetReference(int i)
{
	switch(i)
	{
	case SCENE_MTLLIB_REFNUM:
		return (RefTargetHandle) mpSceneMtlLib;
		break;
#ifdef PERSIST_SCRATCH
	case SCRATCH_MTLLIB_REFNUM:
		return (RefTargetHandle) mpScratchMtlLib;
		break;
#endif
	default:
		assert(0);
	}
	return NULL;
}

void CMaxMaterialCollection::SetReference(int i, RefTargetHandle rtarg)
{
	switch(i)
	{
	case SCENE_MTLLIB_REFNUM:
//		assert(rtarg->ClassID() == mtlBaseLibClassID); //not visible in the sdk
		mpSceneMtlLib = (MtlBaseLib *) rtarg;
		break;
#ifdef PERSIST_SCRATCH
	case SCRATCH_MTLLIB_REFNUM:
		mpScratchMtlLib = (ScratchLib *) rtarg;
		break;
#endif
	default:
		assert(0);
	}

}

static const TSTR sXmlMaterialModuleName("stdplugs\\XmlMtl.dll");

// DllGetClassObject function pointer
typedef HRESULT (STDAPICALLTYPE *DLLGETCLASSOBJECT_PROC)(REFCLSID, REFIID, LPVOID*);

HRESULT CMaxMaterialCollection::CreatePluglet(REFCLSID rclsid, REFIID riid, void** ppPluglet, LPCTSTR sModule)
{
	HRESULT hr = E_FAIL;

	if (sModule && _tcslen(sModule) > 0)
	{
		//TODO should we be managing the module handles somehow?
		hXmlMtlDllModule = LoadLibrary(sModule);
		if (hXmlMtlDllModule == NULL)
		{
			// Failed to load DLL, the last error will be set by the system
			return E_FAIL;
		}

		// Get the proc addres for function DllGetClassObject
		DLLGETCLASSOBJECT_PROC pFunc = 
				(DLLGETCLASSOBJECT_PROC)GetProcAddress(hXmlMtlDllModule, "DllGetClassObject");
		assert(pFunc != NULL);
		if (pFunc == NULL)
		{
			// DllGetClassObject function is not exported
			// The specified procedure could not be found.
			FreeLibrary(hXmlMtlDllModule);
			hXmlMtlDllModule = NULL;
			SetLastError(ERROR_PROC_NOT_FOUND);
			return E_FAIL;
		}

		// Get the class factory interface using DllGetClassObject
		CComPtr<IClassFactory> pCF;
		hr = pFunc(rclsid, IID_IClassFactory, (void**) &pCF);
		assert(!FAILED(hr) && pCF != NULL);
		if (FAILED(hr) || pCF == NULL)
		{
			FreeLibrary(hXmlMtlDllModule);
			hXmlMtlDllModule = NULL;
			SetLastError(hr); // Failed to get interface
			return E_FAIL;
		}

		// Create the COM object using the class factory
		hr = pCF->CreateInstance(NULL, riid, (void**)ppPluglet);
		assert(SUCCEEDED(hr) && ppPluglet != NULL);
		if (FAILED(hr) || ppPluglet == NULL)
		{
			pCF = NULL; // will release the COM object
			FreeLibrary(hXmlMtlDllModule);
			hXmlMtlDllModule = NULL;
			SetLastError(hr); // Failed to create COM object
			return E_FAIL;
		}
	}
	return hr;
}

HRESULT CMaxMaterialCollection::GetXMLImpExp(IVIZPointerClient** ppClient)
{
#ifdef XXX
	//Begin temporary
	// temporary impl: Create an XMLMaterial COM object and ask it to import
	//TODO (JH 7/25/02 Done By CC) implement this differently, CoCreate is slow
	HRESULT hr = CoCreateInstance(CLSID_XmlMaterial, NULL, CLSCTX_INPROC_SERVER,
		__uuidof(IVIZPointerClient), (void**)ppClient);
	if(!SUCCEEDED(hr) || *ppClient == NULL)
	{
		//TODO (JH 7/25/02 Done By CC)Remove the message box 
		MessageBox(NULL, "Failed to create XML Material importer, is it registered?", NULL, MB_OK | MB_TASKMODAL);
		return E_FAIL;
	}
	VARIANT vIp, vImpIp;
	vIp.vt = vImpIp.vt = VT_BYREF;
	vIp.byref = GetCOREInterface();
	vImpIp.byref = GetCOREInterface();//this is a HACK
	return (*ppClient)->InitPointers(vIp, vImpIp);
#else
	*ppClient = NULL;
	Interface *ip = GetCOREInterface();
	TSTR plugletModuleName(ip->GetDir(APP_MAXROOT_DIR));
	plugletModuleName.Append(sXmlMaterialModuleName);
	HRESULT hr = CreatePluglet(CLSID_XmlMaterial, __uuidof(IVIZPointerClient), (void**)ppClient, plugletModuleName.data());
	if(SUCCEEDED(hr) && *ppClient)
	{
		VARIANT vIp, vImpIp;
		VariantInit(&vIp);
		VariantInit(&vImpIp);
		V_VT(&vIp) = VT_BYREF;
		V_BYREF(&vIp) = ip;
		// We don't have access to an import interface ptr here.
		HRESULT hr = (*ppClient)->InitPointers(vIp, vImpIp);
	}
	return hr;
#endif
}

static const USHORT kScratchLibChunk = 0xe430;
static _bstr_t sMaterialsTag(_T("Materials"));

// These are ReferenceMaker methods but they are called when
// the GUP is saved, which is not in the reference hierarchy.
// These methods cannot use the reference hierarchy for
// saving and loading data.
IOResult CMaxMaterialCollection::Save(ISave *isave)
{
	// Since the library can't be saved using the reference
	// hierarchy, we will use XML. This is imperfect, but
	// for the tool palette it should be enough, since the basis of
	// materials in the tool palette is also XML.

	// Don't save anything we don't need to.
	if (mpScratchMtlLib == NULL || mpScratchMtlLib->Count() <= 0)
		return IO_OK;

	// Get the VIZ Importer/Exporter
	CComPtr<IVIZPointerClient> pPointerClient;
	HRESULT hr = GetXMLImpExp(&pPointerClient);
	if(hr != S_OK)
		return IO_ERROR;

	// Get the export interface
	CComQIPtr<IMaterialXmlExport> pILib = pPointerClient;
	ATLASSERT(pILib);
	if(!pILib)
		return IO_ERROR;

	// Create an XML document
	CComPtr<IXMLDOMDocument> doc;
	hr = doc.CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER);
	if (hr != S_OK || doc == NULL)
		return IO_ERROR;

	CComPtr<IXMLDOMElement> materials;
	hr = doc->createElement(sMaterialsTag.GetBSTR(), &materials);
	if (FAILED(hr) || materials == NULL)
		return IO_ERROR;
	hr = doc->putref_documentElement(materials);
	if (FAILED(hr))
		return IO_ERROR;

	hr = Fire_OnSaveScratchMaterials(this, materials);
	if (FAILED(hr))
		return IO_ERROR;
	if (hr != S_OK)
		return IO_OK;		// Nothing to save

	// Now read the data from the XML document and write it
	// to the MAX data stream
	_bstr_t xml;
	hr = doc->get_xml(xml.GetAddress());
	if (hr != S_OK)
		return IO_ERROR;

	isave->BeginChunk(kScratchLibChunk);
	IOResult res = isave->WriteCString(LPCTSTR(xml));
	isave->EndChunk();
	return res;
}

static IOResult ImportLibrary(ILoad* iload, MtlBaseLib& lib)
{
	// Get the VIZ Importer/Exporter
	CComPtr<IVIZPointerClient> pPointerClient;
	HRESULT hr = CMaxMaterialCollection::GetXMLImpExp(&pPointerClient);
	if(hr != S_OK)
		return IO_ERROR;

	// Get the export interface
	CComQIPtr<IXmlMaterial> pIMtl = pPointerClient;
	ATLASSERT(pIMtl);
	if(!pIMtl)
		return IO_ERROR;

	// Create an XML document
	CComPtr<IXMLDOMDocument> doc;
	hr = doc.CoCreateInstance(CLSID_DOMDocument, NULL, CLSCTX_INPROC_SERVER);
	if (hr != S_OK || doc == NULL)
		return IO_ERROR;

	char* xmlStr = NULL;
	IOResult res = iload->ReadCStringChunk(&xmlStr);
	if (res != IO_OK)
		return res;
	if (xmlStr == NULL)
		return IO_OK;
	_bstr_t xml(xmlStr);

	VARIANT_BOOL result;
	hr = doc->loadXML(xml.GetBSTR(), &result);
	if (hr != S_OK || result == 0)
		return IO_ERROR;

	CComBSTR query = "./Materials/Material";
	CComPtr<IXMLDOMNodeList> list;
	hr = doc->selectNodes(query, &list);
	if (hr != S_OK || list == NULL)
		return IO_ERROR;

	long i, len = 0;
	hr = list->get_length(&len);
	if (hr != S_OK)
		return IO_ERROR;

	long failed = 0;
	for (i = 0; i < len ; ++i) {
		CComPtr<IXMLDOMNode> node;
		hr = list->get_item(i, &node);
		if (hr == S_OK && node != NULL) {
			VARIANT vResult;
			vResult.vt = VT_BYREF;
			vResult.byref = NULL;
			hr = pIMtl->ImportMaterial(node, &vResult);
			if(SUCCEEDED(hr) && vResult.vt == VT_BYREF && vResult.byref != NULL)
			{
				lib.Add(static_cast<Mtl*>(vResult.byref));
			}
			else
				++failed;
		}
	}
	
	return failed == 0 ? IO_OK : IO_ERROR;
}

// These are ReferenceMaker methods but they are called when
// the GUP is saved, which is not in the reference hierarchy.
// These methods cannot use the reference hierarchy for
// saving and loading data.
IOResult CMaxMaterialCollection::Load(ILoad *iload)
{
	// Since the library can't be loaded using the reference
	// hierarchy, we will use XML. This is imperfect, but
	// for the tool palette it should be enough, since the basis of
	// materials in the tool palette is also XML.
	IOResult res;
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
			case kScratchLibChunk: {
				// Ignore errors
				deleteMaterials(false, false, true);
				ImportLibrary(iload, theScratchMtlLib);
			} break;
		}
		iload->CloseChunk();
		if (res != IO_OK)
			return res;
	}

	return IO_OK;
}

HRESULT CMaxMaterialCollection::MakeWrapperObject(Mtl* pMtl)
{
	if (pMtl == NULL || s_InMakeWrapper)
		return E_FAIL;

	s_InMakeWrapper = true;

	HRESULT hr = RecursiveMakeWrapperObject(pMtl);

	s_InMakeWrapper = false;
	return hr;
}

HRESULT CMaxMaterialCollection::RecursiveMakeWrapperObject(Mtl* pMtl){

	HRESULT hr = S_OK;
	if(pMtl)
	{
		if (pMtl->IsMultiMtl()) {
			int count = pMtl->NumSubMtls();
			for (int i = 0;
				i < count && SUCCEEDED(hr = RecursiveMakeWrapperObject(pMtl->GetSubMtl(i)));
				++i) {
			}
		}
		else {
			DWORD key = (DWORD) pMtl;

			if(mMtlMap.find(key) == mMtlMap.end()) {
				//Create the wrapper
				CComObject<CMaxMaterial> *pWrapperObject;
				hr = CComObject<CMaxMaterial>::CreateInstance(&pWrapperObject);
				if(SUCCEEDED(hr)) {
					//Initalize the new wrapper
					pWrapperObject->setCollection(this);
					pWrapperObject->setMtl(pMtl);
				}
			}
			else 
				hr = S_FALSE;

		}
	}
	return hr;
}

void CMaxMaterialCollection::OnSceneLibChanged()
{
	//figure out which materials were added or deleted and fire appropriate events.
	//this is currently pretty inefficient since we hav to walk the entire list of materials to 
	//figure out which ones to add or remove. Could improve this by adding better messagin from 
	//mpSceneLib

	int numLibMtls = mpSceneMtlLib->Count();
	//int numMtlsWrapped = mMtlMap.size();

	//adding material
	if(numLibMtls > mLastSceneMtlLibSize )
	{

		for(int i=0; i< numLibMtls; i++)
		{
			//get the max material to be wrapped
			MtlBase *pMtl = mpSceneMtlLib->operator[](i);
			assert(pMtl); //can this be empty?
			if(pMtl == NULL || pMtl->SuperClassID() != MATERIAL_CLASS_ID)
				continue;

			//TODO validate the downcast
			HRESULT hr = MakeWrapperObject(static_cast<Mtl*>(pMtl));
			assert(SUCCEEDED(hr));
			if (!SUCCEEDED(hr))
				return;

		}
	}
	//removing material
	/*
	We no longer track removals other than to update our record of the new size (mLastSceneMtlLibSize)
	The idea is that we keep all of our wrappers alive unless explicitly destroyed. 
	We assume that all new materials will pass through the scenematerials lib via OkMtlForScene or some equivalent method.

	else if(numLibMtls < mLastSceneMtlLibSize )
	{
		ifxClxnType::iterator it=mMtlMap.begin();
		while(it!=mMtlMap.end())
		{
			Mtl *pMtlToCheck = (Mtl *) (*it).first;
			if(mpSceneMtlLib->FindMtl(pMtlToCheck) == -1)//no longer in scene
			{
				pMtlWrapper.Attach((*it).second);
#ifdef XXXRECYCLE_MATS
			//add the wrapper to our data structure
				mRecycledMtlMap.insert(*it);
				pMtlWrapper.p->AddRef();
#endif
				Fire_OnMaterialRemoved(this, pMtlWrapper);
				it = mMtlMap.erase(it);
				pMtlWrapper.Release();
			}
			else
				it++;
		}
	}
	*/
	//update our record of the size of sceneMtlLib
	mLastSceneMtlLibSize = numLibMtls;
/* TO DO implement an integrity checker
#ifndef NDEBUG
	else
		CheckIntegrity();
#endif
		*/
}

void CMaxMaterialCollection::OnScratchLibChanged()
{
#ifdef PERSIST_SCRATCH

	//figure out which materials were added or deleted and fire appropriate events.
	//this is currently pretty inefficient since we hav to walk the entire list of materials to 
	//figure out which ones to add or remove. Could improve this by adding better messagin from 
	//mpSceneLib

	int numLibMtls = mpScratchMtlLib->Count();
	//int numMtlsWrapped = mMtlMap.size();

	//adding material
	if(numLibMtls > mLastScratchMtlLibSize )
	{

		for(int i=0; i< numLibMtls; i++)
		{
			//get the max material to be wrapped
			MtlBase *pMtl = mpScratchMtlLib->operator[](i);
			assert(pMtl); //can this be empty?
			if(pMtl == NULL || pMtl->SuperClassID() != MATERIAL_CLASS_ID)
				continue;

			DWORD key = (DWORD) pMtl;

			if(mMtlMap.find(key)!= mMtlMap.end())
				continue;//found

			//Create the wrapper
			CComObject<CMaxMaterial> *pWrapperObject;
			HRESULT hr = CComObject<CMaxMaterial>::CreateInstance(&pWrapperObject);
			assert(SUCCEEDED(hr));
			if(!SUCCEEDED(hr))
				return;

			//Initalize the new wrapper
			pWrapperObject->setCollection(this);
			//TODO validate the downcast
			pWrapperObject->setMtl((Mtl *)pMtl);

		}
	}
	mLastScratchMtlLibSize = numLibMtls;
#endif
}

//stop gap method
//if the material editor becomes visible and teh material in it is not
//already wrapped we better add it. 
void CMaxMaterialCollection::OnMeditVisible()
{
	if(!m_suspendCount)
	{
		MtlBase *m = GetMtlEditInterface()->GetCurMtl();
		if(m->SuperClassID()!=MATERIAL_CLASS_ID)
			return;
		CComPtr<IMaxMaterial> pWrapper;
		FindItemKey(DWORD(m), &pWrapper.p);
		if(!pWrapper)
		{
			MakeWrapperObject((Mtl*)m);
			FindItemKey(DWORD(m), &pWrapper.p);
			if(pWrapper)
				AddPersistence(pWrapper);
		}
	}
}

STDMETHODIMP CMaxMaterialCollection::get_count(short *pVal)
{

	*pVal = mMtlMap.size();
	return S_OK;
}

STDMETHODIMP CMaxMaterialCollection::getMaterial(int which, IMaxMaterial **ppMtl)
{
	// TODO: This is O(n), make it constant time by addin a different collection type or
	//fiure out hoe to get random access from a map
	if(*ppMtl != NULL)
		return E_INVALIDARG;

	ifxClxnType::iterator it=mMtlMap.begin();
	int i= 0;
	while(it!=mMtlMap.end() && i<which)
	{
		if(i == which)
		{
			//11/13/02*ppMtl = (*it).second;
			(*it).second->QueryInterface(ppMtl);
			assert(*ppMtl);
			//11/13/02(*ppMtl)->AddRef();//copying an interface pointer
			return S_OK;
		}
		it++;//just advance the iterator to the requested entry
		i++;//and our counter
	}

	return E_INVALIDARG;
}

STDMETHODIMP CMaxMaterialCollection::FindItem(/*[in]*/ GUID id, /*[out, retval]*/ IMaxMaterial **ppMtl)
{
	if(*ppMtl != NULL)
		return E_INVALIDARG;

	CMaxMaterial* pMtl = findItem(id);
	if (pMtl == NULL)
		return S_FALSE;
	return pMtl->QueryInterface(__uuidof(IMaxMaterial), reinterpret_cast<void**>(ppMtl));
}

CMaxMaterial* CMaxMaterialCollection::findItem(const GUID& id)
{
	// TODO: This is O(n)
	// Can we gain anything by maintanig maps indexed by guid
	ifxClxnType::iterator it = mMtlMap.begin();
	GUID MtlId;
	for ( ; it != mMtlMap.end(); ++it)
	{
		it->second->get_ID(&MtlId);
		if(MtlId == id)
			return it->second;
	}

	return NULL;
}

//static which forwards to instance
void CMaxMaterialCollection::NotifyProc(void *param, NotifyInfo *info)
{
		CMaxMaterialCollection *pinst = (CMaxMaterialCollection *) param;
		pinst->OnMaxNotify(info);
}

void CMaxMaterialCollection::OnMaxNotify(NotifyInfo *info)
{
	switch(info->intcode)
	{
	case NOTIFY_SCENE_UNDO:
	case NOTIFY_SCENE_REDO:
		{
			ifxClxnType::iterator p = mMtlMap.begin();
			while (p != mMtlMap.end()) {
				ifxClxnType::iterator del = p;		// Just in case p is removed from the map
				++p;
				del->second->CheckReference(true, true);
			}
		} break;

	case NOTIFY_FILE_PRE_OPEN:
	case NOTIFY_SYSTEM_PRE_NEW:
	case NOTIFY_SYSTEM_PRE_RESET:
		// The scratch materials are not preserved between
		// different files.

		Suspend();
		deleteMaterials(true, true, true);// get rid of all materials
		break;
	case NOTIFY_FILE_PRE_MERGE:
	case NOTIFY_PRE_IMPORT:
	case NOTIFY_FILE_PRE_SAVE:


		Suspend();
		break;
	case NOTIFY_FILE_POST_OPEN:
	case NOTIFY_SYSTEM_POST_NEW:
	case NOTIFY_SYSTEM_POST_RESET:
	case NOTIFY_FILE_POST_MERGE:
	case NOTIFY_FILE_POST_SAVE:
	case NOTIFY_POST_IMPORT:
		{
		Resume();
		OnSceneLibChanged();
		OnScratchLibChanged();
		}
		break;
	case NOTIFY_MEDIT_SHOW:
		{
			OnMeditVisible();
		}
		break;
	default:
		break;
	}
}

//called by a wrapper when node count leaves 0 
void CMaxMaterialCollection::OnMtlAddedToScene(IMaxMaterial * pMtl)
{
	Fire_OnMaterialAssigned(this, pMtl);
}

//called by a wrapper when node count reaches 0 
void CMaxMaterialCollection::OnMtlRemovedFromScene(IMaxMaterial * pMtl)
{
	Fire_OnMaterialUnAssigned(this, pMtl);
}

//called by wrapper when release by editor
void CMaxMaterialCollection::OnMtlEndEdit(IMaxMaterial *pMtl)
{
	Fire_OnMaterialChanged(this, pMtl);
}

//called by wrapper when release by editor
void CMaxMaterialCollection::OnMtlChanged(IMaxMaterial *pMtl)
{
	Fire_OnMaterialChanged(this, pMtl);
}

//called by a wrapper when its material self-destructs
void CMaxMaterialCollection::OnMtlDeleted(IMaxMaterial * pMtl, DWORD key)
{
	ifxClxnType::iterator it = mMtlMap.find(key);
	if(it != mMtlMap.end())
	{
		Fire_OnMaterialRemoved(this, pMtl);
		it = mMtlMap.erase(it);
		pMtl->Release();//should destroy both the material and the wrapper
	}
}

bool CMaxMaterialCollection::AddMtl(MtlWrapper* pMtl, DWORD key)
{
	CComPtr<IMaxMaterial> pMtlWrapper = pMtl;
	assert(pMtlWrapper);

	//add the wrapper to our data structure
	mMtlMap.insert(ifxClxnType::value_type(key, pMtl));

	//if this material already has clients dependents
	//go ahead and notify any clients
	//for instance this happens after a load
	//it only doesn't happen when a new material is being added to the scene from scratch
	VARIANT_BOOL vScene, vRefs;
	HRESULT hr = pMtlWrapper->IsReferenced(&vRefs);
	hr = pMtlWrapper->InScene(&vScene);
	if(vScene == VARIANT_TRUE)
		Fire_OnMaterialAssigned(this, pMtlWrapper);
	else if(vRefs == VARIANT_TRUE)//TODO invent another message?
		Fire_OnMaterialAssigned(this, pMtlWrapper);

	pMtlWrapper.Detach(); //don't release the pointer
	return true;
}

STDMETHODIMP CMaxMaterialCollection::emptyTrash(void)
{
	return deleteMaterials(true, false, false);
}

HRESULT CMaxMaterialCollection::deleteMaterials(bool recycle, bool scene, bool scratch)
{
	HRESULT hr = S_OK;
	//walk our map and remove any wrappers on unreferenced materials
	CComPtr<IMaxMaterial> pMtlWrapper;

	ifxClxnType::iterator it=mMtlMap.begin();
	VARIANT_BOOL vHasDependents, inScene;
	while(it!=mMtlMap.end())
	{
		pMtlWrapper.Attach((*it).second);
		hr = pMtlWrapper->IsReferenced(&vHasDependents);
		ATLASSERT(SUCCEEDED(hr));
		if (scene || scratch) {
			hr = pMtlWrapper->InScene(&inScene);
			ATLASSERT(SUCCEEDED(hr));
		}
		if ((scene && inScene == VARIANT_TRUE)
				|| (scratch && inScene == VARIANT_FALSE && vHasDependents == VARIANT_TRUE)
				|| (recycle && vHasDependents == VARIANT_FALSE))
		{
			Fire_OnMaterialRemoved(this, pMtlWrapper);
			it = mMtlMap.erase(it);
			pMtlWrapper.Release();//should destroy both the material and the wrapper
		}
		else
		{
			pMtlWrapper.Detach();
			it++;
		}
	}

	if (scratch) {
		theScratchMtlLib.DeleteAll();
		mLastScratchMtlLibSize = 0;
	}
	if (scene) {
		mLastSceneMtlLibSize = 0;
	}

	return hr;
}

STDMETHODIMP CMaxMaterialCollection::FindItemKey(DWORD key, IMaxMaterial ** ppMtl)
{
	if(*ppMtl != NULL)
		return E_INVALIDARG;

	CMaxMaterial* pMtl = findItemKey(key);
	if (pMtl == NULL)
		return S_FALSE;
	return pMtl->QueryInterface(__uuidof(IMaxMaterial), reinterpret_cast<void**>(ppMtl));
}

CMaxMaterial* CMaxMaterialCollection::findItemKey(DWORD key)
{
	ifxClxnType::iterator it = mMtlMap.find(key);
	if(it != mMtlMap.end())
		return it->second;

	return NULL;
}

void CMaxMaterialCollection::switchMaterial(CMaxMaterial* wrapper, Mtl* mtl)
{
	mMtlMap.erase(DWORD(wrapper->GetReference(REFIDX_MTL)));
	wrapper->ReplaceReference(REFIDX_MTL, mtl);
	mMtlMap.insert(ifxClxnType::value_type(DWORD(wrapper->GetReference(REFIDX_MTL)),
		static_cast<MtlWrapper*>(wrapper)));
	OnMtlChanged(wrapper);
}



STDMETHODIMP CMaxMaterialCollection::AddPersistence(IMaxMaterial * pMtl)
{
#ifdef PERSIST_SCRATCH

	if(mpScratchMtlLib)
	{
		ifxClxnType::iterator it=mMtlMap.begin();
		GUID MtlId, id;
		pMtl->get_ID(&id);
		while(it!=mMtlMap.end())
		{
			MtlWrapper *pm = (*it).second;
			pm->get_ID(&MtlId);
			if(MtlId == id)
			{
				USHORT ct = pm->AddPersistence();
				if(ct ==1)
				{
#ifndef DID486021 //removing this, we don't want this inside this method (which now gets called often)
				  //rather we create the undo object at some specific instances where this method is called
					theHold.Begin();
					theHold.Put(new PersistMtlRestore(mpScratchMtlLib, pm->getMtl()));
					theHold.Accept(GetString(IDS_TOGGLE_PERSIST));
#endif
					mpScratchMtlLib->Add(pm->getMtl());
				}
				return S_OK;
			}
			it++;
		}
	}
#endif
	return S_FALSE;
}

STDMETHODIMP CMaxMaterialCollection::RemovePersistence(IMaxMaterial * pMtl)
{
#ifdef PERSIST_SCRATCH

	if(mpScratchMtlLib)
	{
		ifxClxnType::iterator it=mMtlMap.begin();
		GUID MtlId, id;
		pMtl->get_ID(&id);
		while(it!=mMtlMap.end())
		{
			MtlWrapper *pm = (*it).second;
			pm->get_ID(&MtlId);
			if(MtlId == id)
			{
				USHORT ct = pm->RemovePersistence();
				if(ct ==0)
				{
#ifndef DID486021 //removing this, we don't want this inside this method (which now gets called often)
				  //rather we create the undo object at some specific instances where this method is called
					theHold.Begin();
					theHold.Put(new PersistMtlRestore(mpScratchMtlLib, pm->getMtl()));
					theHold.Accept(GetString(IDS_TOGGLE_PERSIST));
#endif
					mpScratchMtlLib->Remove(pm->getMtl());
				}
				return S_OK;
			}
			it++;
		}
	}
#endif
	return S_FALSE;
}



STDMETHODIMP CMaxMaterialCollection::IsSelectionValid(VARIANT_BOOL* vResult)
{
	Interface* ip = GetCOREInterface();

	*vResult = VARIANT_FALSE;
	if(ip->GetSelNodeCount())
		*vResult = VARIANT_TRUE;

	return S_OK;
}

#endif	// EXTENDED_OBJECT_MODEL
