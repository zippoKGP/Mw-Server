/**********************************************************************
 *<
	FILE: ViewportLoader.cpp

	DESCRIPTION:	Viewport Manager for loading up Effects

	CREATED BY:		Neil Hazzard

	HISTORY:		Created:  02/15/02
					

 *>	Copyright (c) 2002, All Rights Reserved.
 **********************************************************************/
#include "d3dx9.h"

#include "maxscrpt\MAXScrpt.h"
#include "maxscrpt\MAXObj.h"
#include "maxscrpt\3DMath.h"
#include "maxscrpt\msplugin.h"

#include "ViewportManager.h"
#include "ViewportLoader.h"

#ifndef NO_ASHLI
#include "rtmax.h"
#include "ID3D9GraphicsWindow.h"
#pragma comment( lib, "rtmax.lib" )
#endif


class ViewportLoaderClassDesc:public ClassDesc2 {
	public:
	int 			IsPublic() {return 1;}
	void *			Create(BOOL loading) {return new ViewportLoader;}
	const TCHAR *	ClassName() { return GetString(IDS_VPMCLASSNAME); }

	SClass_ID		SuperClassID() {return CUST_ATTRIB_CLASS_ID;}
	Class_ID 		ClassID() {return VIEWPORTLOADER_CLASS_ID;}
	const TCHAR* 	Category() {return _T("");}
	const TCHAR*	InternalName() { return _T("ViewportManager"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }			// returns owning module handle
	};

static ViewportLoaderClassDesc theViewportLoaderClassDesc;
ClassDesc2* GetViewportLoaderDesc(){ return &theViewportLoaderClassDesc;}

static void GetSceneLights(Tab<INode*> & lights)
{
	Interface *ip	  = GetCOREInterface();
	TimeValue t  = ip->GetTime();
	INode * Root  = ip->GetRootNode();
	int Count = Root->NumberOfChildren();
	int i=0;

	for( i=0; i < Count; i++) 
	{
		INode * node = Root->GetChildNode(i);
		ObjectState Os   = node->EvalWorldState(t);

		if(Os.obj && Os.obj->SuperClassID() == LIGHT_CLASS_ID) 
		{
			lights.Append(1, &node);
		}
	}
}

static LPDIRECT3DDEVICE9 GetDevice()
{
#ifndef NO_ASHLI
	GraphicsWindow		*GW;
	ViewExp				*View;
	LPDIRECT3DDEVICE9	Device;

	View = GetCOREInterface()->GetActiveViewport();

	if(View)
	{
		GW = View->getGW();

		if(GW)
		{
			ID3D9GraphicsWindow *D3DGW = (ID3D9GraphicsWindow *)GW->GetInterface(D3D9_GRAPHICS_WINDOW_INTERFACE_ID);

			if(D3DGW)
			{
				Device = D3DGW->GetDevice();

				return(Device);
			}
		}
	}
	return NULL;
#endif
}

static void SaveEffectFile(StdMat2 * mtl, TCHAR * fileName)
{
#ifndef NO_ASHLI

	if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0))
		return;

	LPDIRECT3DDEVICE9	pDevice = GetDevice();

	if(!pDevice)
		return;

	D3DCAPS9		Caps;

	pDevice->GetDeviceCaps(&Caps);
	int instSize = Caps.PS20Caps.NumInstructionSlots;

	bool useLPRT;
	TCHAR filename[MAX_PATH];
	TCHAR value[10] = {_T("true;")};
	_tcscpy(filename,GetCOREInterface()->GetDir(APP_PLUGCFG_DIR));
	_tcscat(filename,"\\DXDisplay.ini");

	useLPRT = GetPrivateProfileInt(_T("Settings"),_T("LPRTEnabled"),0,filename) ? true : false;

	if(!useLPRT)
		_tcscpy(value,_T("false;"));

	IHLSLCodeGenerator * codeGen = IHLSLCodeGenerator::GetHLSLCodeGenerator();
	bool bTransparency;
	TSTR path,name;
	
	Tab<INode*> lights;
	GetSceneLights(lights);
	TCHAR * effectString = codeGen->GenerateEffectFile((StdMat2*)mtl,lights,IHLSLCodeGenerator::PS_2_0,bTransparency,instSize,useLPRT);

	if(!effectString)
	{
		delete codeGen;
		return;
	}

	TSTR buf = TSTR(effectString);

	buf = buf + _T("\nbool UseLPRT = ");
	buf = buf + value;

	FILE *fp;
	SplitFilename(TSTR(fileName),&path,&name,NULL);
	fp = _tfopen(fileName,"w");
	_ftprintf(fp,"%s",buf.data());
	fclose(fp);

	TSTR hlslName = path + _T("\\") + name + _T(".hlsl");
	fp = _tfopen(hlslName.data(),"w");
	effectString = codeGen->GenerateFragmentCode((StdMat2*)mtl,lights,bTransparency);
	_ftprintf(fp,"%s",effectString);
	fclose(fp);
	delete codeGen;
#endif
}

static ICustAttribContainer * GetOwnerContainer(ReferenceTarget * owner)
{
	ICustAttribContainer * cc = NULL;
	//Lets find our Parent - which should be a custom Attribute Container
	RefList &list = owner->GetRefList();
	RefListItem *ptr = list.first;
	while (ptr) 
	{
		if (ptr->maker) 
		{
			if (ptr->maker->ClassID()==Class_ID(0x5ddb3626, 0x23b708db) ) 
			{
				cc = (ICustAttribContainer*)ptr->maker;
			}
		}
		ptr = ptr->next;
	}
	
	return cc;
}

class EffectPBAccessor : public PBAccessor
{
public:
	void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)    // set from v
	{
		ViewportLoader* m = (ViewportLoader*)owner;
		IParamMap2 * map = m->pblock->GetMap();
		ICustAttribContainer * cc = NULL;
		//Lets find our Parent - which should be a custom Attribute Container

		cc = GetOwnerContainer(m);

		bool hasParent = true;
		MtlBase * mb = NULL;
		if(cc)
			mb = (MtlBase *) cc->GetOwner();
		else
			hasParent = false;

		switch (id)
		{
			case pb_effect: {
				if(v.i == m->effectNum)
					break;

				ClassDesc* pCD = m->GetEffectCD((v.i)-1);
				BOOL state;
				m->pblock->GetValue(pb_enabled,t,state,FOREVER);

				if(pCD==NULL)
				{
					if(map)
						map->Enable(pb_enabled,FALSE);
					if(hasParent)
						mb->ClearMtlFlag(MTL_HW_MAT_ENABLED);

					if(mb->ClassID() != Class_ID(DMTL_CLASS_ID,0)){
						map->Enable(pb_dxStdMat,FALSE);
						map->Enable(pb_saveFX2,FALSE);
					}

				}
				else
				{
					if(map)
						map->Enable(pb_enabled,TRUE);
					if(state && hasParent)
						mb->SetMtlFlag(MTL_HW_MAT_ENABLED);
				}
				m->effectNum = v.i;
				m->LoadEffect(pCD); 
				GetCOREInterface()->ForceCompleteRedraw();
				break;
			}
			case pb_enabled:
				{
					if((m->effect == NULL && v.i == 1)||!hasParent)
					{
						v.i = 0;
					}
//					IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
//					MtlBase *mtl = mtlEdit->GetCurMtl();
					MtlBase *mtl = mb;
					// The following forces the GFX to copy our Viewportdata and not from the standard material data
					if(hasParent)
					{
						if(v.i)
							mtl->SetMtlFlag(MTL_HW_MAT_ENABLED);
						else
							mtl->ClearMtlFlag(MTL_HW_MAT_ENABLED);
						
						GetCOREInterface()->RedrawViews(t);
					}

					if(map)
					{
						EnableWindow(GetDlgItem(map->GetHWnd(),IDC_SAVE_FX),!v.i);
						EnableWindow(GetDlgItem(map->GetHWnd(),IDC_DX_STDMAT),!v.i);
						
						if(mb->ClassID() != Class_ID(DMTL_CLASS_ID,0)){
							map->Enable(pb_dxStdMat,FALSE);
							map->Enable(pb_saveFX2,FALSE);
						}

					}
				}
				break;
			case pb_dxStdMat:
				{
					MtlBase *mtl = mb;
					if(map)
					{
						if(!m->effect && v.i ==0)
							map->Enable(pb_enabled, FALSE);
						else
							map->Enable(pb_enabled, ! v.i);

						map->Enable(pb_effect, !v.i);
						EnableWindow(GetDlgItem(map->GetHWnd(),IDC_SAVE_FX),v.i);
					}
					if(hasParent)
					{
						BOOL pluginActive;
						m->pblock->GetValue(pb_enabled,0,pluginActive,FOREVER);
						if(v.i)
							mtl->SetMtlFlag(MTL_HW_MAT_ENABLED);
						else{
							if(!pluginActive)
								mtl->ClearMtlFlag(MTL_HW_MAT_ENABLED);
						}

						GetCOREInterface()->RedrawViews(t);
						mtl->NotifyDependents(FOREVER,0,REFMSG_SEL_NODES_DELETED,NOTIFY_ALL,FALSE); // button update  

					}
				}
				break;
				case pb_saveFX2:
					SaveEffectFile((StdMat2*)mb,v.s);
					break;

		}
	}

};

static EffectPBAccessor effectPBAccessor;


BOOL EffectsDlgProc::DlgProc(TimeValue t, IParamMap2 *map, HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) 
	{
		case WM_INITDIALOG:
		{

			HWND hwndeffect = GetDlgItem(hWnd, IDC_EFFECTLIST);
			SendMessage(hwndeffect, CB_RESETCONTENT, 0L, 0L);
			for (int i = 0; i < vl->NumShaders(); i++) {
				ClassDesc* pClassD = vl->GetEffectCD(i);
				int n = SendMessage(hwndeffect, CB_ADDSTRING, 0L, (LPARAM)(pClassD->ClassName()) );
			}
			SendMessage(hwndeffect, CB_INSERTSTRING, 0L, (LPARAM)GetString(IDS_STR_NONE));
			SendMessage(hwndeffect, CB_SETCURSEL, vl->effectNum,0L);
			if(vl->effectNum == 0)
				map->Enable(pb_enabled,FALSE);
			else
				map->Enable(pb_enabled,TRUE);

			BOOL dxStdMat;
			vl->pblock->GetValue(pb_dxStdMat,0,dxStdMat,FOREVER);
			if(dxStdMat)
			{
				map->Enable(pb_enabled, FALSE);
				map->Enable(pb_effect, FALSE);
			}
			EnableWindow(GetDlgItem(map->GetHWnd(),IDC_SAVE_FX),dxStdMat);
			ICustAttribContainer * cc = GetOwnerContainer(vl);
			if(cc)
			{
				MtlBase * mtl = (MtlBase*)cc->GetOwner();
				if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0)){
					map->Enable(pb_dxStdMat,FALSE);
					map->Enable(pb_saveFX2,FALSE);
				}
			}
			return TRUE;
		}
	}
	return FALSE;
}




static ParamBlockDesc2 param_blk ( viewport_manager_params, _T("DirectX Manager"),  0, &theViewportLoaderClassDesc, 
								  P_AUTO_CONSTRUCT + P_AUTO_UI + P_HASCATEGORY, PBLOCK_REF, 
	//rollout
	IDD_CUSTATTRIB, IDS_VIEWPORT_MANAGER, 0, 0, NULL, ROLLUP_CAT_CUSTATTRIB, 
	// params

	pb_enabled, 	_T("enabled"),		TYPE_BOOL, 	0, 	IDS_ENABLED, 
		p_default, 		FALSE, 
		p_ui,			TYPE_SINGLECHEKBOX, IDC_ENABLED,
		p_accessor,		&effectPBAccessor,
		end,

	pb_effect,		_T("effect"),	TYPE_INT,		0, 	IDS_EFFECT, 	
		p_default, 		0, 
		p_ui, 			TYPE_INTLISTBOX, IDC_EFFECTLIST, 0,
		p_accessor,		&effectPBAccessor,
		end,

	pb_dxStdMat,	_T("dxStdMat"), TYPE_BOOL,	0,	IDS_DX_STDMAT,
		p_default,		FALSE,
		p_ui,			TYPE_SINGLECHEKBOX, IDC_DX_STDMAT,
		p_accessor,		&effectPBAccessor,
		end,

	pb_saveFX2,		_T("SaveFXFile"), TYPE_FILENAME, 0, IDS_SAVE_FX,
		p_ui,		TYPE_FILESAVEBUTTON,IDC_SAVE_FX,
		p_file_types,	IDS_FILE_TYPES,
		p_caption,		IDS_TITLE,
		p_accessor,		&effectPBAccessor,
		end,

	end
);


static FPInterfaceDesc viewport_manager_interface(
    VIEWPORT_SHADER_MANAGER_INTERFACE, _T("viewportmanager"), 0, &theViewportLoaderClassDesc, 0,

		ViewportLoader::get_num_effects2,		_T("getNumViewportEffects"),		0, TYPE_INT, 0, 0,
		ViewportLoader::get_active_effect2,		_T("getActiveViewportEffect"),		0, TYPE_REFTARG, 0, 0,
		ViewportLoader::set_effect2,				_T("setViewportEffect"),			0, TYPE_REFTARG,0,1,
			_T("effectindex"),	0,	TYPE_INT,
		ViewportLoader::get_effect_name2,		_T("getViewportEffectName"),		0,TYPE_STRING,0,1,
			_T("effectindex"), 0, TYPE_INT,
		ViewportLoader::activate_effect2,		_T("activateEffect"),				0,0,0,2,
			_T("material"), 0, TYPE_MTL,
			_T("state"),0,TYPE_BOOL,
		ViewportLoader::is_dxStdMat_enabled2, _T("isDxStdMatEnabled"),			0, TYPE_bool,0,0,
		ViewportLoader::set_dxStdMat2,		  _T("activateDxStdMat"),			0,TYPE_VOID,0,1,
			_T("active"), 0, TYPE_bool,	 
		ViewportLoader::saveFXfile2, _T("saveFXFile"),		0, TYPE_bool,0,1,
			_T("fileName"), 0 , TYPE_STRING,
		ViewportLoader::get_EffectName2,		_T("getActiveEffectName"),		0, TYPE_STRING, 0, 0,
		
	end
);


//  Get Descriptor method for Mixin Interface
//  *****************************************
FPInterfaceDesc* IViewportShaderManager::GetDesc()
{
     return &viewport_manager_interface;
}


ViewportLoader::ViewportLoader()
{
	theViewportLoaderClassDesc.MakeAutoParamBlocks(this); 
	effectIndex = NOT_FOUND;
	LoadEffectsList();
	masterDlg = NULL;
	clientDlg = NULL;
	effect = NULL;
	effectNum = 0;
	mEdit =  NULL;
	mparam = NULL;
	oldEffect = NULL;
	undo = false;
}

ViewportLoader::~ViewportLoader()
{
}

ReferenceTarget *ViewportLoader::Clone(RemapDir &remap)
{
	ViewportLoader *pnew = new ViewportLoader;
	pnew->ReplaceReference(0,pblock->Clone(remap));
	if(effect)
		pnew->ReplaceReference(1,effect->Clone(remap));
	pnew->effectNum = effectNum;
	BaseClone(this, pnew, remap);
	return pnew;
}


ParamDlg *ViewportLoader::CreateParamDlg(HWND hwMtlEdit, IMtlParams *imp)
{

	mEdit = hwMtlEdit;
	mparam = imp;;
	masterDlg =  theViewportLoaderClassDesc.CreateParamDlgs(hwMtlEdit, imp, this);
	param_blk.SetUserDlgProc(new EffectsDlgProc(this));


	if(effect){
		IDXDataBridge * vp = (IDXDataBridge*)effect->GetInterface(VIEWPORT_SHADER_CLIENT_INTERFACE);
		if(vp)
		{
			clientDlg = vp->CreateEffectDlg(hwMtlEdit,imp);
			masterDlg->AddDlg(clientDlg);
		}
	}

	theViewportLoaderClassDesc.RestoreRolloutState();

	return masterDlg;
}

void ViewportLoader::SetReference(int i, RefTargetHandle rtarg) 
{
	switch(i)
	{
		case PBLOCK_REF: 
			pblock = (IParamBlock2 *)rtarg;
			break;
		case 1:
			effect = (ReferenceTarget *)rtarg;
			break;
	}
}

RefTargetHandle ViewportLoader::GetReference(int i)
{
	switch(i)
	{
		case PBLOCK_REF: 
			return pblock;
		case 1:
			return effect;

		default: return NULL;
	}
}	

RefResult ViewportLoader::NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget,PartID &partID, RefMessage message)
{
	return REF_SUCCEED;
}


int ViewportLoader::NumSubs()  
{
	return 1;
}

Animatable* ViewportLoader::SubAnim(int i)
{
	if(effect)
	{
		IParamBlock2 * pblock2 =  effect->GetParamBlock(0);
		return pblock2; 
	}

	return NULL;
}

TSTR ViewportLoader::SubAnimName(int i)
{
	// we use the data from the Actual shader
	if(effect)
	{
		IDXDataBridge * vp = (IDXDataBridge*)effect->GetInterface(VIEWPORT_SHADER_CLIENT_INTERFACE);
		TCHAR * name = vp->GetName();
		return TSTR(name);

	}
	else
		return TSTR(_T(""));
}

SvGraphNodeReference ViewportLoader::SvTraverseAnimGraph(IGraphObjectManager *gom, Animatable *owner, int id, DWORD flags) 
{ 
	// Only continue traversal if an effect is present and active
	if( effect && GetDXShaderManager()->IsVisible())
		return SvStdTraverseAnimGraph(gom, owner, id, flags); 
	else
		return SvGraphNodeReference();
}

#define MANAGER_ACTIVENUM_CHUNK	0x1000
#define MANAGER_ENABLED_CHUNK	0x1001

IOResult ViewportLoader::Save(ISave *isave)
{
	ULONG nb;
	isave->BeginChunk(MANAGER_ACTIVENUM_CHUNK);
	isave->Write(&effectNum, sizeof(effectNum), &nb);			
	isave->EndChunk();
	return IO_OK;
}


// this is used in the case that the file is being opened on a system with more effects
// it will try to find the effect from the new list, otherwise it will force a "None" and no UI
class PatchEffect : public PostLoadCallback 
{
public:
	ViewportLoader*	v;
	PatchEffect(ViewportLoader* pv){ v = pv;}
	void proc(ILoad *iload)
	{
		v->effectNum = v->FindEffectIndex(v->effect);
		v->pblock->SetValue(pb_effect,0,v->effectNum);
		delete this;

	}
};


IOResult ViewportLoader::Load(ILoad *iload)
{
	ULONG nb;
	IOResult res;
	int id;
	PatchEffect* pe = new PatchEffect(this);
	iload->RegisterPostLoadCallback(pe);
	while (IO_OK==(res=iload->OpenChunk())) {
		switch(id = iload->CurChunkID())  {

			case MANAGER_ACTIVENUM_CHUNK:
				res = iload->Read(&effectNum, sizeof(effectNum), &nb);
				break;

		}
		iload->CloseChunk();
		if (res!=IO_OK) 
			return res;
	}
	return IO_OK;
}




// compare function for sorting Shader Tab<>
static int classDescListCompare(const void *elem1, const void *elem2) 
{
	ClassDesc* s1 = *(ClassDesc**)elem1;
	ClassDesc* s2 = *(ClassDesc**)elem2;
	TSTR sn1 = s1->ClassName();  // need to snap name string, since both use GetString()
	TSTR sn2 = s2->ClassName();
	return _tcscmp(sn1.data(), sn2.data());
}



ClassDesc * ViewportLoader::FindandLoadDeferedEffect(ClassDesc * defered)
{
	SubClassList* scList = GetCOREInterface()->GetDllDir().ClassDir().GetClassList(REF_TARGET_CLASS_ID);
	for (long i = 0, j = 0; i < scList->Count(ACC_ALL); ++i) {
		if ( (*scList)[ i ].IsPublic() && ((*scList)[ i ].ClassID() == defered->ClassID()) ) {

			ClassDesc* pClassD = (*scList)[ i ].FullCD();
			LoadEffectsList();
			return pClassD;
		}
	}
	return NULL;

}

#define D3D9_GRAPHICS_WINDOW_INTERFACE_ID Interface_ID(0x56424386, 0x2151b83)

void ViewportLoader::LoadEffectsList()
{
	// loads static shader list with name-sorted Shader ClassDesc*'s
	bool bdx9 = false;

	ViewExp *vpt;
	vpt = GetCOREInterface()->GetActiveViewport();	
	GraphicsWindow *gw = vpt->getGW();

	if(gw->GetInterface(D3D9_GRAPHICS_WINDOW_INTERFACE_ID))
	{
		bdx9 = true;
	}

	effectsList.ZeroCount();
	SubClassList* scList = GetCOREInterface()->GetDllDir().ClassDir().GetClassList(REF_TARGET_CLASS_ID);
	theHold.Suspend(); // LAM - 3/24/03 - defect 446356 - doing a DeleteThis on created effects, need to make sure hold is off
	for (long i = 0, j = 0; i < scList->Count(ACC_ALL); ++i) {
		if ( (*scList)[ i ].IsPublic() ) {
			ClassDesc* pClassD = (*scList)[ i ].FullCD();
			const TCHAR *cat = pClassD->Category();
			TCHAR *defcat = GetString(IDS_DX_VIEWPORT_EFFECT);
			if ((cat) && (_tcscmp(cat,defcat) == 0)) 
			{
			
				ReferenceTarget * effect = (ReferenceTarget *)pClassD->Create(TRUE);
				if(effect)
				{

					IDX9DataBridge * vp = (IDX9DataBridge*)effect->GetInterface(VIEWPORT_SHADER9_CLIENT_INTERFACE);
					if( vp)
					{
						if(bdx9)
						{
							
							if(vp->GetDXVersion() >=9.0f || vp->GetDXVersion() == 1.0f)
							{
								effectsList.Append(1, &pClassD);
							}
						}
						else
						{
							if(vp->GetDXVersion() < 9.0f)
							{
								effectsList.Append(1, &pClassD);
							}

						}
					}
					else
					{
						IDXDataBridge * vp = (IDXDataBridge*)effect->GetInterface(VIEWPORT_SHADER_CLIENT_INTERFACE);
						if(vp && !bdx9)
						{
							effectsList.Append(1, &pClassD);
						}
					}

					effect->DeleteAllRefsFromMe();
					effect->DeleteThis();
				}
			}

		}
	}
	theHold.Resume();
	effectsList.Sort(&classDescListCompare);
}

int ViewportLoader::NumShaders()
{
	if (effectsList.Count() == 0)
		LoadEffectsList();
	return effectsList.Count();
}

ClassDesc* ViewportLoader::GetEffectCD(int i)
{
	if (effectsList.Count() == 0)
		LoadEffectsList();
	return (i >= 0 && i < effectsList.Count()) ? effectsList[i] : NULL;
}

int ViewportLoader::FindEffectIndex(ReferenceTarget * e)
{
	if(e==NULL)
		return 0;
	for(int i=0;i<effectsList.Count();i++)
	{
		if(e->ClassID()==effectsList[i]->ClassID())
			return i+1;		//take into account "None" at 0
	}
	return 0;	//none found put up the "None"
}


void ViewportLoader::LoadEffect(ClassDesc * pd)
{

	ReferenceTarget * newEffect;
	ReferenceTarget * oldEffect;

	if (theHold.Holding())
	{
		oldEffect = effect;

//		theHold.Suspend(); 


		if(pd == NULL){
			newEffect = NULL;
		}
		else{
			newEffect = (ReferenceTarget *)pd->Create(FALSE);
			if(!newEffect)
			{
				// maybe defered
				ClassDesc * def = FindandLoadDeferedEffect(pd);
				if(def)
				{
					newEffect = (ReferenceTarget *)def->Create(FALSE);
				}

			}

		}

//		theHold.Resume(); 

		if (theHold.Holding())
			theHold.Put(new AddEffectRestore(this,oldEffect,newEffect));

		SwapEffect(newEffect);
	}
}


bool IsLoaderActiveInMedit(CustAttrib * loader)
{
	IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
	MtlBase * mtl = mtlEdit->GetCurMtl();

	ICustAttribContainer * cc = mtl->GetCustAttribContainer();
	if(cc)
	{

		for(int i = 0; i< cc->GetNumCustAttribs();i++)
		{
			if(loader == cc->GetCustAttrib(i))
				return true;
		}
	}
	return false;
}


void ViewportLoader::SwapEffect(ReferenceTarget *e)
{
	ReplaceReference(1,e);

//	theHold.Suspend(); 


	if(IsLoaderActiveInMedit(this))
	{
	
	if(clientDlg)
		clientDlg->DeleteThis();

	if(masterDlg)
	{
		masterDlg->DeleteDlg(clientDlg);
		masterDlg->ReloadDialog();
		clientDlg = NULL;
	}


	if(mEdit && mparam && effect)
	{
		IDXDataBridge * vp = (IDXDataBridge*)effect->GetInterface(VIEWPORT_SHADER_CLIENT_INTERFACE);
		clientDlg = vp->CreateEffectDlg(mEdit,mparam);
		masterDlg->AddDlg(clientDlg);
		
	}
//	theHold.Resume(); 
	}
	else
		clientDlg = NULL;		//need this as something changed us without being active.
	
	NotifyDependents(FOREVER,PART_ALL,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	undo = false;

}

/////////////////////////// IViewportShaderManager Function Publishing Methods/////////////////////////////////////

int ViewportLoader::GetNumEffects()
{
	return NumShaders();
}

//We only return the effect if we are active..
ReferenceTarget* ViewportLoader::GetActiveEffect()
{
	return effect;	//return our active effect.
}


TCHAR * ViewportLoader::GetEffectName(int i)
{

	ClassDesc* pClassD = GetEffectCD(i-1);
	return (TCHAR*)pClassD->ClassName();
}

ReferenceTarget * ViewportLoader::SetViewportEffect(int i)
{
	theHold.Begin();
	pblock->SetValue(pb_effect,0,i);	//"None" is at position 0;
	theHold.Accept(GetString(IDS_STR_UNDO));

	if(effect)
		return effect;
	else
		return NULL;
}

void ViewportLoader::ActivateEffect(MtlBase * mtl, BOOL state)
{

	ICustAttribContainer* cc = mtl->GetCustAttribContainer();
	if(!cc)
		return;
	MtlBase * mb = (MtlBase *) cc->GetOwner();
	if(!mb)
		return;

	if(state && effect)
		mb->SetMtlFlag(MTL_HW_MAT_ENABLED);
	else
		mb->ClearMtlFlag(MTL_HW_MAT_ENABLED);
	
	GetCOREInterface()->ForceCompleteRedraw();

}

bool ViewportLoader::IsDxStdMtlEnabled()
{
	bool active = false;
	if(pblock)
	{
		BOOL enabled;
		pblock->GetValue(pb_dxStdMat,0,enabled,FOREVER);
		if(enabled)
			active = true;
	}
	return active;
}

void ViewportLoader::SetDxStdMtlEnabled(bool state)
{
	if(pblock)
		pblock->SetValue(pb_dxStdMat,0,(BOOL)state);
}

TCHAR * ViewportLoader::GetActiveEffectName()
{
	ClassDesc* pCD = GetEffectCD(effectNum-1);
	if(pCD)
		return (TCHAR*)pCD->ClassName();
	else
		return NULL;

}

bool ViewportLoader::SaveFXFile(TCHAR * fileName)
{
	if(!pblock)
		return false;

	pblock->SetValue(pb_saveFX2,0,fileName);
	return true;
}

BaseInterface* ViewportLoader::GetInterface(Interface_ID id) 
{ 
	if (id == VIEWPORT_SHADER_MANAGER_INTERFACE) 
		return (IViewportShaderManager*)this; 
	else if(id == VIEWPORT_SHADER_MANAGER_INTERFACE2)
		return (IViewportShaderManager2*)this;
	else
		return FPMixinInterface::GetInterface(id);
}

#ifndef NO_ASHLI

class DXEffectActions : public FPStaticInterface {
protected:
	DECLARE_DESCRIPTOR(DXEffectActions)

	enum {
		kSaveEffectFile = 1,
		kDxDisplay=2,
	};

	void SaveEffectFile();
	void ActivateDXDisplay();
	BOOL IsChecked(FunctionID actionID);
	BOOL IsEnabled(FunctionID actionID);

	BEGIN_FUNCTION_MAP
		VFN_0(kSaveEffectFile, SaveEffectFile)
		VFN_0(kDxDisplay, ActivateDXDisplay)
	END_FUNCTION_MAP

	static DXEffectActions mDXEffectActions;
};

#define SAVEEFFECTFILE_ACTIONS		Interface_ID(0x26615cff, 0x26273586)

DXEffectActions DXEffectActions::mDXEffectActions(
	SAVEEFFECTFILE_ACTIONS, _T("SaveEffectFile"), IDS_DLL_CATEGORY, &theViewportLoaderClassDesc, FP_ACTIONS,
	kActionMaterialEditorContext,

	kSaveEffectFile, _T("SaveAsFXFile"), IDS_SAVEASEFFECTFILE, 0,
	f_menuText,	IDS_SAVEASEFFECTFILE,
	end,

	kDxDisplay, _T("DXDisplay"), IDS_DXDISPLAY, 0,
	f_menuText,	IDS_DXDISPLAY,
	end,


	end
	);


static bool IsDxMaterialEnabled(MtlBase * map)
{
	bool enabled = false;
	ICustAttribContainer * cont = map->GetCustAttribContainer();

	if(cont)
	{
		for(int i=0; i< cont->GetNumCustAttribs(); i++)
		{
			CustAttrib * ca = cont->GetCustAttrib(i);
			if(ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2)){

				IViewportShaderManager2 * vsm = (IViewportShaderManager2*)ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2);

				return vsm->IsDxStdMtlEnabled();
			}
		}
	}
	return enabled;
}

static void SetDxMaterialEnabled(MtlBase * map, bool set)
{
	bool enabled = false;
	ICustAttribContainer * cont = map->GetCustAttribContainer();

	if(cont)
	{
		for(int i=0; i< cont->GetNumCustAttribs(); i++)
		{
			CustAttrib * ca = cont->GetCustAttrib(i);
			if(ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2)){

				IViewportShaderManager2 * vsm = (IViewportShaderManager2*)ca->GetInterface(VIEWPORT_SHADER_MANAGER_INTERFACE2);
				return vsm->SetDxStdMtlEnabled(set);
			}
		}
	}

}



static BOOL file_exists(TCHAR *filename)
{
	HANDLE findhandle;
	WIN32_FIND_DATA file;
	findhandle = FindFirstFile(filename, &file);
	FindClose(findhandle);
	if (findhandle == INVALID_HANDLE_VALUE)
		return FALSE;
	else
		return TRUE;
}


static bool BrowseOutFile(TSTR &file) {
	BOOL silent = TheManager->SetSilentMode(TRUE);
	BitmapInfo biOutFile;
	FilterList fl;

	OPENFILENAME ofn;
	// get OPENFILENAME items
	HWND   hWnd = GetCOREInterface()->GetMAXHWnd();
	TCHAR  file_name[260] = _T("");
	TSTR cap_str = GetString(IDS_TITLE);

	fl.Append(GetString(IDS_FX_FILTER_NAME));
	fl.Append(GetString(IDS_FX_FILTER_DESC));

	// setup OPENFILENAME
	memset(&ofn, 0, sizeof(OPENFILENAME));
	ofn.lStructSize = sizeof(OPENFILENAME);
	ofn.hwndOwner = hWnd;
	ofn.hInstance = (HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE);
	// WIN64 Cleanup: Shuler
	ofn.lpstrFile = file_name;
	ofn.nMaxFile = sizeof(file_name) / sizeof(TCHAR);
	ofn.lpstrFilter = fl;
	ofn.nFilterIndex = 1;
	ofn.lpstrTitle   = cap_str;
	ofn.lpstrFileTitle = _T("");
	ofn.nMaxFileTitle = 0;
	ofn.lpstrInitialDir = _T("");
	ofn.Flags = 0;

	// throw up the dialog
	while (GetSaveFileName(&ofn))
	{
		TCHAR* fn = (TCHAR*)_alloca(strlen(ofn.lpstrFile) + 4 + 1), *p;
		DWORD i;
		strcpy(fn, ofn.lpstrFile);
		TCHAR type[MAX_PATH];
		BMMSplitFilename(ofn.lpstrFile, NULL, NULL, type);
		if (type[0] == 0 || _tcschr(type, _T('\\')) != NULL)	// no suffix, add from filter types if not *.*
		{
			for (i = 1, p = (TCHAR*)ofn.lpstrFilter; i < ofn.nFilterIndex; i++)
			{
				while (*p) p++; p++; while (*p) p++; p++; // skip to indexed type
			}
			while (*p) p++; p++; p++; // jump descripter, '*'
			if (*p == _T('.') && p[1] != _T('*')) // add if .xyz
				strcat(fn, p);
		}

		if (file_exists(fn))
		{
			// file already exists, query owverwrite
			TCHAR buf[256];
			MessageBeep(MB_ICONEXCLAMATION);
			sprintf(buf,"%s %s",fn,GetString(IDS_FILE_REPLACE));
			if (MessageBox(hWnd, buf, GetString(IDS_SAVE_AS), MB_ICONQUESTION | MB_YESNO) == IDNO)
			{
				// don't overwrite, remove the path from the name, try again
				_tcscpy(buf, file_name + ofn.nFileOffset);
				_tcscpy(file_name, buf);
				continue;
			}
		}
		file = fn;
		return true;
		break;
	}
	return false;

}

#define UI_MAKEBUSY			SetCursor(LoadCursor(NULL, IDC_WAIT));
#define UI_MAKEFREE			SetCursor(LoadCursor(NULL, IDC_ARROW));

void DXEffectActions::SaveEffectFile()
{
	IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
	MtlBase *mtl = mtlEdit->GetCurMtl();
	if(!mtl)
		return;		

	TSTR File;
	if(!BrowseOutFile(File))
		return;
	UI_MAKEBUSY
	::SaveEffectFile((StdMat2*)mtl,File.data());
	UI_MAKEFREE
}

void DXEffectActions::ActivateDXDisplay()
{
	IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
	MtlBase *mtl = mtlEdit->GetCurMtl();
	if(!mtl)
		return;	
	MSPlugin* plugin = (MSPlugin*)((ReferenceTarget*)mtl)->GetInterface(I_MAXSCRIPTPLUGIN);
	ReferenceTarget * targ = NULL;
	if(plugin){
		targ = plugin->get_delegate();
		mtl = (MtlBase*)targ;
	}
	bool setvalue;
	if(IsDxMaterialEnabled(mtl))
	{
		setvalue = false;
	}
	else
		setvalue = true;
	
	SetDxMaterialEnabled(mtl,setvalue);

}

BOOL DXEffectActions::IsChecked(FunctionID actionID)
{
	if(actionID == kDxDisplay)
	{
		IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
		MtlBase *mtl = mtlEdit->GetCurMtl();
		if(!mtl)
			return FALSE;		
		MSPlugin* plugin = (MSPlugin*)((ReferenceTarget*)mtl)->GetInterface(I_MAXSCRIPTPLUGIN);
		ReferenceTarget * targ = NULL;
		if(plugin){
			targ = plugin->get_delegate();
			mtl = (MtlBase*)targ;
		}
		BOOL setvalue = FALSE;
		if(IsDxMaterialEnabled(mtl))
		{
			setvalue = TRUE;
		}
		return setvalue;
	}
	return FALSE;
}

BOOL DXEffectActions::IsEnabled(FunctionID actionID)
{
	BOOL active = TRUE;

	IMtlEditInterface *mtlEdit = (IMtlEditInterface *)GetCOREInterface(MTLEDIT_INTERFACE);
	MtlBase *mtl = mtlEdit->GetCurMtl();
	if(!mtl)
		return FALSE;

	MSPlugin* plugin = (MSPlugin*)((ReferenceTarget*)mtl)->GetInterface(I_MAXSCRIPTPLUGIN);
	ReferenceTarget * targ = NULL;
	if(plugin){
		targ = plugin->get_delegate();
		mtl = (MtlBase*)targ;
	}

	if(actionID == kDxDisplay)
	{
		IDXShaderManagerInterface * sm = (IDXShaderManagerInterface*) GetCOREInterface(IDX_SHADER_MANAGER);
		if(!sm)
			return FALSE;
		if(!sm->IsVisible() || mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0))
			return FALSE; 
	}
	if(actionID == kSaveEffectFile)
	{
		if(mtl->ClassID() != Class_ID(DMTL_CLASS_ID,0))
			return FALSE;
	}

	return active;
}
#endif