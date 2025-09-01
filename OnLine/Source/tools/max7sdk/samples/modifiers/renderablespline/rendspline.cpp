/**********************************************************************
 *<
	FILE: RendSpline.cpp

	DESCRIPTION:	Renderable Spline Modifier

	CREATED BY:		Alexander Esppeschit Bicalho (discreet)

	HISTORY:		2/1/2004 (Created)

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#include "RendSpline.h"
#include "object.h"
#include "shape.h"
#include "spline3d.h"
#include "splshape.h"
#include "linshape.h"

#define RendSpline_CLASS_ID	Class_ID(0x817f4d17, 0x8bc7074a)
#define EDITSPL_CHANNELS (PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|PART_DISPLAY|PART_TOPO)


#define PBLOCK_REF	0
//#define NUM_OLDVERSIONS	0

class RendSplineParamsMapDlgProc;

class RendSpline : public Modifier {
	public:
		static IParamMap2 *pmapParam;
		static float nlength;
		static float nangle;
		static int nsides;
		static float vlength;
		static float vangle;
		static int vsides;
		static RendSplineParamsMapDlgProc* paramDlgProc;

		class ParamAccessor;

		// Parameter block
		IParamBlock2	*pblock;	//ref 0

		static IObjParam *ip;			//Access to the interface
		
		// From Animatable
		TCHAR *GetObjectName() { return GetString(IDS_CLASS_NAME); }

				ChannelMask ChannelsUsed()  { return EDITSPL_CHANNELS; }
		//TODO: Add the channels that the modifier actually modifies
		ChannelMask ChannelsChanged() { return EDITSPL_CHANNELS; }
				
		//TODO: Return the ClassID of the object that the modifier can modify
		Class_ID InputType() { return Class_ID(SPLINESHAPE_CLASS_ID,0); }

		void ModifyObject(TimeValue t, ModContext &mc, ObjectState *os, INode *node);
		void NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc);

		void NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index);
		void NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index);


		Interval LocalValidity(TimeValue t);

		// From BaseObject
		//TODO: Return true if the modifier changes topology
		BOOL ChangeTopology() {return TRUE;}		
		
		CreateMouseCallBack* GetCreateMouseCallBack() {return NULL;}

		BOOL HasUVW();
		void SetGenUVW(BOOL sw);

		void BeginEditParams(IObjParam *ip, ULONG flags,Animatable *prev);
		void EndEditParams(IObjParam *ip, ULONG flags,Animatable *next);


		Interval GetValidity(TimeValue t);

		// Automatic texture support
		
		// Loading/Saving
		IOResult Load(ILoad *iload);
		//IOResult Save(ISave *isave);

		//From Animatable
		Class_ID ClassID() {return RendSpline_CLASS_ID;}		
		SClass_ID SuperClassID() { return OSM_CLASS_ID; }
		void GetClassName(TSTR& s) {s = GetString(IDS_CLASS_NAME);}

		RefTargetHandle Clone( RemapDir &remap );
		RefResult NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, 
			PartID& partID,  RefMessage message);


		int NumSubs() { return 1; }
		TSTR SubAnimName(int i) { return GetString(IDS_PARAMS); }				
		Animatable* SubAnim(int i) { return pblock; }

		// TODO: Maintain the number or references here
		int NumRefs() { return 1; }
		RefTargetHandle GetReference(int i) { return pblock; }
		void SetReference(int i, RefTargetHandle rtarg) { pblock=(IParamBlock2*)rtarg; }

		int	NumParamBlocks() { return 1; }					// return number of ParamBlocks in this instance
		IParamBlock2* GetParamBlock(int i) { return pblock; } // return i'th ParamBlock
		IParamBlock2* GetParamBlockByID(BlockID id) { return (pblock->ID() == id) ? pblock : NULL; } // return id'd ParamBlock

		void DeleteThis() { delete this; }		
		//Constructor/Destructor

		RendSpline();
		~RendSpline();		

};



class RendSplineClassDesc : public ClassDesc2 {
	public:
	int 			IsPublic() { return TRUE; }
	void *			Create(BOOL loading = FALSE) { return new RendSpline(); }
	const TCHAR *	ClassName() { return GetString(IDS_CLASS_NAME); }
	SClass_ID		SuperClassID() { return OSM_CLASS_ID; }
	Class_ID		ClassID() { return RendSpline_CLASS_ID; }
	const TCHAR* 	Category() { return GetString(IDS_CATEGORY); }

	const TCHAR*	InternalName() { return _T("RendSpline"); }	// returns fixed parsable name (scripter-visible name)
	HINSTANCE		HInstance() { return hInstance; }					// returns owning module handle
	
};

// Initializations

float			RendSpline::nlength =  1.0f;
float			RendSpline::vlength = 1.0f;
float			RendSpline::nangle =  0.0f;
float			RendSpline::vangle = 0.0f;
int				RendSpline::nsides = 12;
int				RendSpline::vsides = 12;
RendSplineParamsMapDlgProc* RendSpline::paramDlgProc = NULL;


static RendSplineClassDesc RendSplineDesc;
ClassDesc2* GetRendSplineDesc() { return &RendSplineDesc; }




enum { rendspline_params };


//TODO: Add enums for various parameters
enum { 
	rnd_thickness, 
	rnd_viewThickness,
	rnd_sides,
	rnd_viewSides,
	rnd_angle,
	rnd_viewAngle,
	rnd_render, 
	rnd_genuvw,
	rnd_display,
	rnd_useView,
	rnd_ViewportOrRender
};

enum {rbViewport, rbRender};

class RendSplineParamsMapDlgProc : public ParamMap2UserDlgProc {
	public:
		RendSpline *mod;		
		RendSplineParamsMapDlgProc(RendSpline *m) {mod = m;}		
		BOOL DlgProc(TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam);	
		void Initialize (HWND hWnd, TimeValue t);
		void Update (HWND hWnd, TimeValue t, ParamID id);
		void Update (HWND hWnd, TimeValue t);
		void Update (TimeValue t, Interval& valid, IParamMap2* pmap)
		{
			Update(pmap->GetHWnd(), t);
		}
		void EnableButtons(HWND hWnd, TimeValue t, BOOL display, BOOL useView);
		void DeleteThis() {
				delete this;
				}
	};

class RendSpline::ParamAccessor : public PBAccessor {
public:
	virtual void Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t);
};

void RendSpline::ParamAccessor::Set(PB2Value& v, ReferenceMaker* owner, ParamID id, int tabIndex, TimeValue t)
{
	if (owner != NULL && RendSpline::paramDlgProc != NULL)
	{
		RendSpline* mod = static_cast<RendSpline*>(owner);
		IParamBlock2* pb = owner->GetParamBlock(0);
		if (pb != NULL)
		{
			IParamMap2* pm = pb->GetMap(0);
			
			if (pm != NULL)
				RendSpline::paramDlgProc->Update(pm->GetHWnd(), t, id);
		}
	}
}

static RendSpline::ParamAccessor updateUI;

static ParamBlockDesc2 rendspline_param_blk ( rendspline_params, _T("params"),  0, &RendSplineDesc, 
	P_AUTO_CONSTRUCT + P_AUTO_UI, PBLOCK_REF, 
	//rollout
	IDD_PANEL, IDS_PARAMS, 0, 0, NULL,
	// params
	rnd_thickness, 			_T("Thickness"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_THICK, 
		p_default, 		1.0f, 
		p_range, 		0.0f,1000000.0f, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_THICK_EDIT,	IDC_THICK_SPIN, 0.1f, 
		p_accessor,		&updateUI,
		end,

	rnd_viewThickness, 			_T("Viewport_Thickness"), 		TYPE_FLOAT, 	0, 	IDS_VIEW_THICK, 
		p_default, 		1.0f, 
		p_range, 		0.0f,1000000.0f, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_UNIVERSE, IDC_THICK_EDIT,	IDC_THICK_SPIN, 0.1f, 
		p_accessor,		&updateUI,
		end,

	rnd_sides, 			_T("Sides"), 		TYPE_INT, 	P_ANIMATABLE, 	IDS_SIDES, 
		p_default, 		12, 
		p_range, 		3,100, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_INT, IDC_SIDE_EDIT,	IDC_SIDE_SPIN, SPIN_AUTOSCALE,
		p_accessor,		&updateUI,
		end,

	rnd_viewSides, 			_T("Viewport_Sides"), 		TYPE_INT, 	0, 	IDS_VIEW_SIDES, 
		p_default, 		12, 
		p_range, 		3,100, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_INT, IDC_SIDE_EDIT,	IDC_SIDE_SPIN, SPIN_AUTOSCALE,
		p_accessor,		&updateUI,
		end,

	rnd_angle, 			_T("Angle"), 		TYPE_FLOAT, 	P_ANIMATABLE, 	IDS_ANGLE, 
		p_default, 		0.0f, 
		p_range, 		-360.0f,360.0f, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_ANGLE_EDIT,	IDC_ANGLE_SPIN, 0.1f, 
		p_accessor,		&updateUI,
		end,
		
	rnd_viewAngle, 			_T("Viewport_Angle"), 		TYPE_FLOAT, 	0, 	IDS_VIEW_ANGLE, 
		p_default, 		0.0f, 
		p_range, 		-360.0f,360.0f, 
//		p_ui, 			TYPE_SPINNER,		EDITTYPE_FLOAT, IDC_ANGLE_EDIT,	IDC_ANGLE_SPIN, 0.1f, 
		p_accessor,		&updateUI,
		end,
		
	rnd_render, 		_T("Renderable"), TYPE_BOOL, P_RESET_DEFAULT,	IDS_RENDERABLE, 	
		p_default, 		TRUE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_RENDERABLE, 
		p_accessor,		&updateUI,
		end, 

	rnd_genuvw, 		_T("mapcoords"), TYPE_BOOL, P_RESET_DEFAULT,	IDS_GENUVW, 	
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_GENUVW, 
		p_accessor,		&updateUI,
		end, 

	rnd_display, 		_T("displayRenderSettings"), TYPE_BOOL, P_RESET_DEFAULT,	IDS_DISPLAY, 	
		p_default, 		TRUE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_DISPLAY, 
		p_accessor,		&updateUI,
		end, 

	rnd_useView, 		_T("useViewportSettings"), TYPE_BOOL, P_RESET_DEFAULT,	IDS_VIEW, 	
		p_default, 		FALSE, 
		p_ui, 			TYPE_SINGLECHEKBOX, IDC_USE_VIEWPORT, 
		p_accessor,		&updateUI,
		end, 
	
	rnd_ViewportOrRender, _T("ViewportOrRender"), TYPE_RADIOBTN_INDEX, 0, IDS_VIEWPORTORRENDERER,
		p_default, rbRender,
		p_ui, 			TYPE_RADIO, 2, IDC_RAD_VIEW, IDC_RAD_RENDER,
		p_vals,			rbViewport, rbRender,
		p_range, rbViewport, rbRender,
		p_accessor,		&updateUI,
		end,

	end
	);




IObjParam *RendSpline::ip			= NULL;


BOOL RendSplineParamsMapDlgProc::DlgProc(
		TimeValue t,IParamMap2 *map,HWND hWnd,UINT msg,WPARAM wParam,LPARAM lParam)
			
{

switch (msg) {
	case WM_INITDIALOG:
		Initialize(hWnd,t);
		Update(hWnd,t);
		break;

	case CC_SPINNER_CHANGE:			
		{
		int id = LOWORD(wParam);
		ISpinnerControl *iSpin = (ISpinnerControl*)lParam;
		int whichval;
		mod->pblock->GetValue(rnd_ViewportOrRender,t,whichval, FOREVER);
		BOOL isViewport = (whichval == rbViewport);
		if (!theHold.Holding()) theHold.Begin();
		BOOL redraw = TRUE;
		switch ( id ) {
		case IDC_THICK_SPIN:
			if (isViewport)
				mod->pblock->SetValue(rnd_viewThickness,t,iSpin->GetFVal(),FALSE);
			else
				mod->pblock->SetValue(rnd_thickness,t,iSpin->GetFVal(),FALSE);
			break;
		case IDC_ANGLE_SPIN:
			if (isViewport)
				mod->pblock->SetValue(rnd_viewAngle,t,iSpin->GetFVal(),FALSE);
			else
				mod->pblock->SetValue(rnd_angle,t,iSpin->GetFVal(),FALSE);
			break;
		case IDC_SIDE_SPIN:
			if (isViewport)
				mod->pblock->SetValue(rnd_viewSides,t,iSpin->GetIVal(),FALSE);
			else
				mod->pblock->SetValue(rnd_sides,t,iSpin->GetIVal(),FALSE);
			break;
			}
		}
		break;

	case CC_SPINNER_BUTTONDOWN:
		theHold.Begin();
		return TRUE;
	case WM_CUSTEDIT_ENTER:
	case CC_SPINNER_BUTTONUP:
		if (HIWORD(wParam) || msg==WM_CUSTEDIT_ENTER) 
			theHold.Accept(GetString(IDS_SHAPE_PARAMETER_CHANGE));
		else 
			theHold.Cancel();
		return TRUE;

}
return FALSE;
}

void RendSplineParamsMapDlgProc::Initialize(HWND hWnd, TimeValue t)
{
	if (hWnd == NULL) return;
	else
	{
		Interval valid;
		ISpinnerControl* iThickSpinner = GetISpinner(GetDlgItem(hWnd, IDC_THICK_SPIN));
		iThickSpinner->SetLimits(0.0f, 99999999.0f, FALSE);
		iThickSpinner->SetAutoScale(TRUE);
		iThickSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_THICK_EDIT), EDITTYPE_POS_UNIVERSE);
		ReleaseISpinner(iThickSpinner);

		ISpinnerControl* iAngleSpinner = GetISpinner(GetDlgItem(hWnd, IDC_ANGLE_SPIN));
		iAngleSpinner->SetLimits(-99999999.0f, 99999999.0f, FALSE);
		iAngleSpinner->SetAutoScale(TRUE);
		iAngleSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_ANGLE_EDIT), EDITTYPE_FLOAT);
		ReleaseISpinner(iAngleSpinner);

		ISpinnerControl* iSidesSpinner = GetISpinner(GetDlgItem(hWnd, IDC_SIDE_SPIN));
		iSidesSpinner->SetLimits(3.0f, 100.0f, FALSE);
		iSidesSpinner->SetAutoScale(TRUE);
		iSidesSpinner->LinkToEdit(GetDlgItem(hWnd, IDC_SIDE_EDIT), EDITTYPE_POS_INT);
		ReleaseISpinner(iSidesSpinner);		

		CheckRadioButton(hWnd, IDC_RAD_VIEW, IDC_RAD_RENDER, IDC_RAD_RENDER);
	}

}
void RendSplineParamsMapDlgProc::EnableButtons(HWND hWnd, TimeValue t, BOOL display, BOOL useView)
{
	// Enable/Disable buttons acording to the UI

	if (display)
	{
		EnableWindow(GetDlgItem(hWnd, IDC_USE_VIEWPORT), TRUE);
		if (useView)
		{
			EnableWindow(GetDlgItem(hWnd, IDC_RAD_VIEW), TRUE);
			// but don't change the state of the radio buttons
		}
		else
		{
			// disable viewport radio; set radio to renderer
			mod->pblock->SetValue(rnd_ViewportOrRender,t,rbRender, FALSE);
			EnableWindow(GetDlgItem(hWnd, IDC_RAD_VIEW), FALSE);
		}
	}
	else
	{
		// disable use viewport settings and viewport radio; set radio to renderer
		EnableWindow(GetDlgItem(hWnd, IDC_USE_VIEWPORT), FALSE);
		mod->pblock->SetValue(rnd_ViewportOrRender,t,rbRender, FALSE);
		EnableWindow(GetDlgItem(hWnd, IDC_RAD_VIEW), FALSE);
	}
}
void RendSplineParamsMapDlgProc::Update(HWND hWnd, TimeValue t, ParamID id)
{
	if (hWnd != NULL && mod != NULL)
	{
		Interval valid;
		switch (id)
		{
			case rnd_ViewportOrRender:
			{
				int whichval;
				mod->pblock->GetValue(rnd_ViewportOrRender,t,whichval, valid);
				int whichRadio = (whichval == rbViewport) ? IDC_RAD_VIEW : IDC_RAD_RENDER;
				CheckRadioButton(hWnd, IDC_RAD_VIEW, IDC_RAD_RENDER, whichRadio);
				Update(hWnd, t, rnd_sides);
				Update(hWnd, t, rnd_angle);
				Update(hWnd, t, rnd_thickness);
			} break;

			case rnd_display:
			case rnd_useView:		
			{
				BOOL display, useView;
				mod->pblock->GetValue(rnd_display,t,display, valid); // sets value
				mod->pblock->GetValue(rnd_useView,t,useView, valid); // sets value
				EnableButtons(hWnd, t, display, useView);
			} break;

			case rnd_thickness:
			case rnd_viewThickness:
			{
				// Update Spinners to the correct values

				ISpinnerControl* iThickSpinner = GetISpinner(GetDlgItem(hWnd, IDC_THICK_SPIN));

				float thick;
				int whichval;
				mod->pblock->GetValue(rnd_ViewportOrRender,t,whichval, valid);
				int whichRadio = (whichval == rbViewport) ? IDC_RAD_VIEW : IDC_RAD_RENDER;
				if (whichRadio == IDC_RAD_VIEW)
					mod->pblock->GetValue(rnd_viewThickness,t,thick,valid); // sets value
				else
					mod->pblock->GetValue(rnd_thickness,t,thick,valid); // sets value

				iThickSpinner->SetValue(thick,FALSE);

				iThickSpinner->SetKeyBrackets(mod->pblock->KeyFrameAtTime(rnd_thickness,t));

				ReleaseISpinner(iThickSpinner);
			} break;

			case rnd_angle:
			case rnd_viewAngle:
			{
				// Update Spinners to the correct values

				ISpinnerControl* iAngleSpinner = GetISpinner(GetDlgItem(hWnd, IDC_ANGLE_SPIN));

				float angle;
				int whichval;
				mod->pblock->GetValue(rnd_ViewportOrRender,t,whichval, valid);
				int whichRadio = (whichval == rbViewport) ? IDC_RAD_VIEW : IDC_RAD_RENDER;
				if (whichRadio == IDC_RAD_VIEW)
					mod->pblock->GetValue(rnd_viewAngle,t,angle,valid); // sets value
				else
					mod->pblock->GetValue(rnd_angle,t,angle,valid); // sets value

				iAngleSpinner->SetValue(angle,FALSE);

				iAngleSpinner->SetKeyBrackets(mod->pblock->KeyFrameAtTime(rnd_angle,t));

				ReleaseISpinner(iAngleSpinner);
			} break;

			case rnd_sides:
			case rnd_viewSides:
			{
				// Update Spinners to the correct values

				ISpinnerControl* iSideSpinner = GetISpinner(GetDlgItem(hWnd, IDC_SIDE_SPIN));

				int side;
				int whichval;
				mod->pblock->GetValue(rnd_ViewportOrRender,t,whichval, valid);
				int whichRadio = (whichval == rbViewport) ? IDC_RAD_VIEW : IDC_RAD_RENDER;
				if (whichRadio == IDC_RAD_VIEW)
					mod->pblock->GetValue(rnd_viewSides,t,side,valid); // sets value
				else
					mod->pblock->GetValue(rnd_sides,t,side,valid); // sets value

				iSideSpinner->SetValue(side,FALSE);

				iSideSpinner->SetKeyBrackets(mod->pblock->KeyFrameAtTime(rnd_sides,t));

				ReleaseISpinner(iSideSpinner);
			} break;
		}
	}
}

void RendSplineParamsMapDlgProc::Update(HWND hWnd, TimeValue t)
{
	Update(hWnd, t, rnd_viewThickness);
	Update(hWnd, t, rnd_sides);
	Update(hWnd, t, rnd_viewSides);
	Update(hWnd, t, rnd_angle);
	Update(hWnd, t, rnd_viewAngle);
	Update(hWnd, t, rnd_render);
	Update(hWnd, t, rnd_genuvw);
	Update(hWnd, t, rnd_display);
	Update(hWnd, t, rnd_useView);
	Update(hWnd, t, rnd_ViewportOrRender);
}

	
//--- RendSpline -------------------------------------------------------
RendSpline::RendSpline()
{
	RendSplineDesc.MakeAutoParamBlocks(this);
}

RendSpline::~RendSpline()
{

}


/*===========================================================================*\
 |	The validity of the parameters.  First a test for editing is performed
 |  then Start at FOREVER, and intersect with the validity of each item
\*===========================================================================*/
Interval RendSpline::LocalValidity(TimeValue t)
{
	// if being edited, return NEVER forces a cache to be built 
	// after previous modifier.
	if (TestAFlag(A_MOD_BEING_EDITED))
		return NEVER;  
	Interval valid = GetValidity(t);	
	return valid;
}


/*************************************************************************************************
*
	Between NotifyPreCollapse and NotifyPostCollapse, Modify is
	called by the system.  NotifyPreCollapse can be used to save any plugin dependant data e.g.
	LocalModData
*
\*************************************************************************************************/

void RendSpline::NotifyPreCollapse(INode *node, IDerivedObject *derObj, int index)
{
	//TODO:  Perform any Pre Stack Collapse methods here
}



/*************************************************************************************************
*
	NotifyPostCollapse can be used to apply the modifier back onto to the stack, copying over the
	stored data from the temporary storage.  To reapply the modifier the following code can be 
	used

	Object *bo = node->GetObjectRef();
	IDerivedObject *derob = NULL;
	if(bo->SuperClassID() != GEN_DERIVOB_CLASS_ID)
	{
		derob = CreateDerivedObject(obj);
		node->SetObjectRef(derob);
	}
	else
		derob = (IDerivedObject*) bo;

	// Add ourselves to the top of the stack
	derob->AddModifier(this,NULL,derob->NumModifiers());

*
\*************************************************************************************************/

void RendSpline::NotifyPostCollapse(INode *node,Object *obj, IDerivedObject *derObj, int index)
{
	//TODO: Perform any Post Stack collapse methods here.

}


/*************************************************************************************************
*
	ModifyObject will do all the work in a full modifier
    This includes casting objects to their correct form, doing modifications
	changing their parameters, etc
*
\************************************************************************************************/

void RendSpline::ModifyObject(TimeValue t, ModContext &mc, ObjectState * os, INode *node) 
{
	Interval valid = GetValidity(t);
	// and intersect it with the channels we use as input (see ChannelsUsed)
	valid &= os->obj->ChannelValidity(t,TOPO_CHAN_NUM);
	valid &= os->obj->ChannelValidity(t,GEOM_CHAN_NUM);

//	float nlength;
	
	BOOL doRender = false, doDisplay = false, doUVW = false, useView = false;

	pblock->GetValue(rnd_thickness,t,nlength,valid);
	pblock->GetValue(rnd_sides,t,nsides,valid);
	pblock->GetValue(rnd_angle,t,nangle,valid);
	pblock->GetValue(rnd_viewThickness,t,vlength,valid);
	pblock->GetValue(rnd_viewSides,t,vsides,valid);
	pblock->GetValue(rnd_viewAngle,t,vangle,valid);
	pblock->GetValue(rnd_render,t,doRender,valid);
	pblock->GetValue(rnd_display,t,doDisplay,valid);
	pblock->GetValue(rnd_genuvw,t,doUVW,valid);
	pblock->GetValue(rnd_useView,t,useView,valid);

	theHold.Suspend(); // need to suspend Undo
	SplineShape *shape = (SplineShape *)os->obj; //->ConvertToType(t,splineShapeClassID);
	shape->SetRenderable(doRender);
	if (doRender)
	{
		shape->SetDispRenderMesh(doDisplay);
		shape->SetGenUVW(doUVW);
	}
	else
	{
		shape->SetDispRenderMesh(false);
		shape->SetGenUVW(false);
	}

	nlength = nlength < 0.0f ? 0.0f : (nlength > 1000000.0f ? 1000000.0f : nlength);
	vlength = vlength < 0.0f ? 0.0f : (vlength > 1000000.0f ? 1000000.0f : vlength);

	nsides = nsides < 0 ? 0 : (nsides > 100? 100 : nsides);
	vsides = vsides < 0 ? 0 : (vsides > 100? 100 : vsides);

	shape->SetThickness(t,nlength);
	shape->SetSides(t,nsides);
	shape->SetAngle(t,nangle);

	shape->SetViewportAngle(vangle);
	shape->SetViewportSides(vsides);
	shape->SetViewportThickness(vlength);

	shape->SetUseViewport(useView);

//	shape->SetViewportOrRenderer; // radiobutton


	shape->shape.UpdateSels();	
	shape->shape.InvalidateGeomCache();
	theHold.Resume(); // it's now safe to resume Undo


	os->obj->SetChannelValidity(TOPO_CHAN_NUM, valid);
	os->obj->SetChannelValidity(GEOM_CHAN_NUM, valid);
	os->obj->SetChannelValidity(TEXMAP_CHAN_NUM, valid);
	os->obj->SetChannelValidity(MTL_CHAN_NUM, valid);
	os->obj->SetChannelValidity(SELECT_CHAN_NUM, valid);
	os->obj->SetChannelValidity(SUBSEL_TYPE_CHAN_NUM, valid);
	os->obj->SetChannelValidity(DISP_ATTRIB_CHAN_NUM, valid);

	os->obj = shape;

	//os->obj->UnlockObject();


}


void RendSpline::BeginEditParams( IObjParam *ip, ULONG flags,Animatable *prev )
{
	this->ip = ip;

	ip->EnableShowEndResult(FALSE);

	TimeValue t = ip->GetTime();
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_BEGIN_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_ON);
	SetAFlag(A_MOD_BEING_EDITED);	

	RendSplineDesc.BeginEditParams(ip, this, flags, prev);

	// pointcache_param_blk.SetUserDlgProc(new PointCacheParamsMapDlgProc(this));

	paramDlgProc = new RendSplineParamsMapDlgProc(this);
	rendspline_param_blk.SetUserDlgProc(paramDlgProc);

}

void RendSpline::EndEditParams( IObjParam *ip, ULONG flags,Animatable *next)
{
	RendSplineDesc.EndEditParams(ip, this, flags, next);

	TimeValue t = ip->GetTime();

	paramDlgProc = NULL;
	ip->EnableShowEndResult(TRUE);


	ClearAFlag(A_MOD_BEING_EDITED);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_END_EDIT);
	NotifyDependents(Interval(t,t), PART_ALL, REFMSG_MOD_DISPLAY_OFF);
	this->ip = NULL;
}



Interval RendSpline::GetValidity(TimeValue t)
{
	float f;
	int i;
	Interval valid = FOREVER;

	// Start our interval at forever...
	// Intersect each parameters interval to narrow it down.
	pblock->GetValue(rnd_thickness,t,f,valid);
	pblock->GetValue(rnd_sides,t,i,valid);
	pblock->GetValue(rnd_angle,t,f,valid);

	return valid;
}



RefTargetHandle RendSpline::Clone(RemapDir& remap)
{
	RendSpline* newmod = new RendSpline();	
	//TODO: Add the cloning code here
	newmod->ReplaceReference(PBLOCK_REF,pblock->Clone(remap));
	BaseClone(this, newmod, remap);
	return(newmod);
}


//From ReferenceMaker 
RefResult RendSpline::NotifyRefChanged(
		Interval changeInt, RefTargetHandle hTarget,
		PartID& partID,  RefMessage message) 
{
	//TODO: Add code to handle the various reference changed messages
	return REF_SUCCEED;
}

/****************************************************************************************
*
 	NotifyInputChanged is called each time the input object is changed in some way
 	We can find out how it was changed by checking partID and message
*
\****************************************************************************************/

void RendSpline::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc)
{

}



//From Object
BOOL RendSpline::HasUVW() 
{ 
	//TODO: Return whether the object has UVW coordinates or not
	BOOL doUVW, doRender;
	pblock->GetValue(rnd_render,0.0f,doRender,FOREVER); // check if it's renderable
	pblock->GetValue(rnd_genuvw,0.0f,doUVW,FOREVER); // check if UVW is on
	return (doUVW && doRender); // if it's renderable and UVW is on, returns true
}

void RendSpline::SetGenUVW(BOOL sw) 
{  

	if (sw==HasUVW()) return;
	else
	{
		BOOL doRender;
		pblock->GetValue(rnd_render,0.0f,doRender,FOREVER); // check if it's renderable
		if (doRender)
			pblock->SetValue(rnd_genuvw,0.0f,sw,0); // sets UVW ON if needed
	}
	//TODO: Set the plugin internal value to sw				
}

IOResult RendSpline::Load(ILoad *iload)
{
	Modifier::Load(iload);
	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(0, 0, &rendspline_param_blk, this, PBLOCK_REF);
	iload->RegisterPostLoadCallback(plcb);
	
	return IO_OK;
}

//IOResult RendSpline::Save(ISave *isave)
//{
	//TODO: Add code to allow plugin to save its data
//	Modifier::Save(isave);
//	ParamBlock2PLCB* plcb = new ParamBlock2PLCB(0, 0, &rendspline_param_blk, this, PBLOCK_REF);

//	return IO_OK;
//}

