/**********************************************************************
 *<
	FILE: EditPoly.cpp

	DESCRIPTION: Edit Poly Modifier

	CREATED BY: Steve Anderson, based on Face Extrude modifier by Berteig, and my own Poly Select modifier.

	HISTORY: created March 2002

 *>	Copyright (c) 2002 Discreet, All Rights Reserved.
 **********************************************************************/

#include "epoly.h"
#include "EditPoly.h"
#include "MeshDLib.h"
#include "setkeymode.h"

// NOTE to third parties trying to build this sample code:
// You will probably have trouble because cfgmgr.h is not present.
// Comment out this #define to be able to build Edit Poly;
// and also remove cfgmgr.lib from the list of libraries to link to for the EditPolyMod project.
#define EDITPOLY_USE_CONFIGURATION_MANAGER

#ifdef EDITPOLY_USE_CONFIGURATION_MANAGER
#include "..\..\..\..\cfgmgr\cfgmgr.h"
#endif

static GenSubObjType SOT_Vertex(1);
static GenSubObjType SOT_Edge(2);
static GenSubObjType SOT_Border(9);
static GenSubObjType SOT_Face(4);
static GenSubObjType SOT_Element(5);

//--- ClassDescriptor and class vars ---------------------------------

Tab<PolyOperation *> EditPolyMod::mOpList;
IObjParam *EditPolyMod::ip = NULL;
EditPolyMod *EditPolyMod::mpCurrentEditMod = NULL;
bool EditPolyMod::mUpdateCachePosted = FALSE;
DWORD EditPolyMod::mHitLevelOverride = 0;
DWORD EditPolyMod::mDispLevelOverride = 0;
bool EditPolyMod::mHitTestResult = false;
bool EditPolyMod::mDisplayResult = false;
bool EditPolyMod::mIgnoreNewInResult = false;
bool EditPolyMod::mForceIgnoreBackfacing = false;
bool EditPolyMod::mSuspendConstraints = false;
bool EditPolyMod::mSliceMode(false);
int EditPolyMod::mLastCutEnd(-1);
int EditPolyMod::mAttachType(0);
bool EditPolyMod::mAttachCondense(true);
int EditPolyMod::mCreateShapeType(IDC_SHAPE_SMOOTH);
int EditPolyMod::mCloneTo(IDC_CLONE_ELEM);
bool EditPolyMod::mDetachToObject(true);
bool EditPolyMod::mDetachAsClone(false);
TSTR EditPolyMod::mCreateShapeName(_T(""));
TSTR EditPolyMod::mDetachName(_T(""));

IParamMap2 *EditPolyMod::mpDialogAnimate = NULL;
IParamMap2 *EditPolyMod::mpDialogSelect = NULL;
IParamMap2 *EditPolyMod::mpDialogSoftsel = NULL;
IParamMap2 *EditPolyMod::mpDialogGeom = NULL;
IParamMap2 *EditPolyMod::mpDialogSubobj = NULL;
IParamMap2 *EditPolyMod::mpDialogSurface = NULL;
IParamMap2 *EditPolyMod::mpDialogOperation = NULL;
IParamMap2 *EditPolyMod::mpDialogPaintDeform = NULL;
bool EditPolyMod::mRollupAnimate = false;
bool EditPolyMod::mRollupSelect = false;
bool EditPolyMod::mRollupSoftsel = true;
bool EditPolyMod::mRollupGeom = false;
bool EditPolyMod::mRollupSubobj = false;
bool EditPolyMod::mRollupSurface = false;
bool EditPolyMod::mRollupPaintDeform = true;
EditPolyBackspaceUser EditPolyMod::backspacer;
EditPolyActionCB *EditPolyMod::mpEPolyActions = NULL;

SelectModBoxCMode *			EditPolyMod::selectMode		 = NULL;
MoveModBoxCMode*			EditPolyMod::moveMode        = NULL;
RotateModBoxCMode*			EditPolyMod::rotMode		 = NULL;
UScaleModBoxCMode*			EditPolyMod::uscaleMode      = NULL;
NUScaleModBoxCMode*			EditPolyMod::nuscaleMode     = NULL;
SquashModBoxCMode*			EditPolyMod::squashMode      = NULL;
EditPolyCreateVertCMode*	EditPolyMod::createVertMode  = NULL;
EditPolyCreateEdgeCMode*	EditPolyMod::createEdgeMode	= NULL;
EditPolyCreateFaceCMode*	EditPolyMod::createFaceMode	= NULL;
EditPolyAttachPickMode*		EditPolyMod::attachPickMode	= NULL;
EditPolyShapePickMode *		EditPolyMod::mpShapePicker = NULL;
EditPolyDivideEdgeCMode*	EditPolyMod::divideEdgeMode	= NULL;
EditPolyDivideFaceCMode *	EditPolyMod::divideFaceMode	= NULL;
EditPolyBridgeBorderCMode *	EditPolyMod::bridgeBorderMode = NULL;
EditPolyBridgePolygonCMode *	EditPolyMod::bridgePolyMode = NULL;
EditPolyExtrudeCMode *		EditPolyMod::extrudeMode		= NULL;
EditPolyExtrudeVECMode *	EditPolyMod::extrudeVEMode = NULL;
EditPolyChamferCMode *		EditPolyMod::chamferMode		= NULL;
EditPolyBevelCMode*			EditPolyMod::bevelMode		= NULL;
EditPolyInsetCMode*			EditPolyMod::insetMode = NULL;
EditPolyOutlineCMode*		EditPolyMod::outlineMode = NULL;
EditPolyCutCMode *			EditPolyMod::cutMode		= NULL;
EditPolyQuickSliceCMode *	EditPolyMod::quickSliceMode = NULL;
EditPolyWeldCMode *			EditPolyMod::weldMode		= NULL;
EditPolyHingeFromEdgeCMode *EditPolyMod::hingeFromEdgeMode = NULL;
EditPolyPickHingeCMode *	EditPolyMod::pickHingeMode = NULL;
EditPolyPickBridge1CMode *	EditPolyMod::pickBridgeTarget1 = NULL;
EditPolyPickBridge2CMode *	EditPolyMod::pickBridgeTarget2 = NULL;
EditPolyEditTriCMode *		EditPolyMod::editTriMode		= NULL;
EditPolyTurnEdgeCMode *		EditPolyMod::turnEdgeMode = NULL;

//--- EditPolyMod methods -------------------------------

EditPolyMod::EditPolyMod() : selLevel(EPM_SL_OBJECT), mpParams(NULL), 
			mpSliceControl(NULL), mpCurrentOperation(NULL),
			mPreserveMapSettings (true,false), mpPointMaster(NULL) {
	GetEditPolyDesc()->MakeAutoParamBlocks(this);

	MakeRefByID (FOREVER, EDIT_SLICEPLANE_REF, NewDefaultMatrix3Controller());
	Matrix3 tm(1);
	mpSliceControl->GetValue (0, &tm, FOREVER, CTRL_RELATIVE);

	if (mOpList.Count () == 0) InitializeOperationList ();
	mCurrentDisplayFlags = 0;

	mLastOperation = ep_op_null;

	LoadPreferences ();

	if (mCreateShapeName.Length() == 0) mCreateShapeName = GetString (IDS_SHAPE);
	if (mDetachName.Length() == 0) mDetachName = GetString(IDS_OBJECT);

	mSliceMode = false;
	mLastCutEnd = -1;

	theHold.Suspend();
	MakeRefByID (FOREVER, EDIT_MASTER_POINT_REF, NewDefaultMasterPointController());
	theHold.Resume();
}

void EditPolyMod::ResetClassParams (BOOL fileReset) {
	EditPolyMod::mRollupAnimate = false;
	EditPolyMod::mRollupSelect = false;
	EditPolyMod::mRollupSoftsel = true;
	EditPolyMod::mRollupGeom = false;
	EditPolyMod::mRollupSubobj = false;
	EditPolyMod::mRollupSurface = false;
	EditPolyMod::mRollupPaintDeform = true;
	EditPolyMod::mUpdateCachePosted = false;
	EditPolyMod::mHitLevelOverride = 0;
	EditPolyMod::mDispLevelOverride = 0;
	EditPolyMod::mHitTestResult = false;
	EditPolyMod::mDisplayResult = false;
	EditPolyMod::mIgnoreNewInResult = false;
	EditPolyMod::mForceIgnoreBackfacing = false;
	EditPolyMod::mSuspendConstraints = false;
	EditPolyMod::mSliceMode = false;
	EditPolyMod::mLastCutEnd = -1;
	EditPolyMod::mAttachType = 0;
	EditPolyMod::mAttachCondense = false;
	EditPolyMod::mCreateShapeType = IDC_SHAPE_SMOOTH;
	EditPolyMod::mCloneTo = IDC_CLONE_ELEM;
	EditPolyMod::mDetachToObject = true;
	EditPolyMod::mDetachAsClone = false;
	EditPolyMod::mCreateShapeName = GetString(IDS_SHAPE);
	EditPolyMod::mDetachName = GetString(IDS_OBJECT);

	LoadPreferences ();
}

void EditPolyMod::SavePreferences ()
{
#ifdef EDITPOLY_USE_CONFIGURATION_MANAGER
	getCfgMgr().setSection (_T("EPolySettings"));

	int createShapeSmooth = (mCreateShapeType == IDC_SHAPE_SMOOTH) ? 1 : 0;
	getCfgMgr().putInt (_T("CreateShapeSmooth"), createShapeSmooth);

	getCfgMgr().putInt (_T("DetachToObject"), mDetachToObject);
	getCfgMgr().putInt (_T("DetachAsClone"), mDetachAsClone);
#endif
}

void EditPolyMod::LoadPreferences ()
{
#ifdef EDITPOLY_USE_CONFIGURATION_MANAGER
	if (!getCfgMgr().sectionExists (_T("EPolySettings"))) return;
	getCfgMgr().setSection (_T("EPolySettings"));

	if (getCfgMgr().keyExists (_T("CreateShapeSmooth"))) {
		int createShapeSmooth;
		getCfgMgr().getInt (_T("CreateShapeSmooth"), &createShapeSmooth);
		mCreateShapeType = createShapeSmooth ? IDC_SHAPE_SMOOTH : IDC_SHAPE_LINEAR;
	}

	if (getCfgMgr().keyExists (_T("DetachToObject"))) {
		int detachToObject;
		getCfgMgr().getInt (_T("DetachToObject"), &detachToObject);
		mDetachToObject = (detachToObject != 0);
	}

	if (getCfgMgr().keyExists (_T("DetachAsClone"))) {
		int detachAsClone;
		getCfgMgr().getInt (_T("DetachAsClone"), &detachAsClone);
		mDetachAsClone = (detachAsClone != 0);
	}
#endif
}

RefTargetHandle EditPolyMod::Clone(RemapDir& remap) {
	EditPolyMod *mod = new EditPolyMod();
	mod->selLevel = selLevel;
	mod->mPreserveMapSettings = mPreserveMapSettings;
	mod->mPointRange = mPointRange;
	mod->mPointNode = mPointNode;
	mod->paintDeformActive = paintDeformActive;
	mod->paintSelActive = paintSelActive;

	mod->ReplaceReference (EDIT_PBLOCK_REF, mpParams->Clone(remap));
	mod->ReplaceReference (EDIT_SLICEPLANE_REF, mpSliceControl->Clone (remap));
	if (mPointControl.Count()) {
		mod->AllocPointControllers (mPointControl.Count());
		for (int i=0; i<mPointControl.Count(); i++) {
			if (mPointControl[i] == NULL) continue;
			mod->MakeRefByID (FOREVER, EDIT_POINT_BASE_REF + i, mPointControl[i]->Clone (remap));
		}
	}

	BaseClone(this, mod, remap);
	return mod;
}

BOOL EditPolyMod::DependOnTopology(ModContext &mc) {
	if (mpCurrentOperation) return true;
	if (!mc.localData) return false;
	EditPolyData *pData = (EditPolyData *) mc.localData;
	return pData->HasCommittedOperations();
}

void EditPolyMod::ModifyObject (TimeValue t, ModContext &mc, ObjectState *os, INode *node) {
	if (!os->obj->IsSubClassOf (polyObjectClassID)) return;
	PolyObject *pObj = (PolyObject *) os->obj;
	MNMesh &mesh = pObj->GetMesh();

	EditPolyData *pData  = (EditPolyData *) mc.localData;
	if (!pData) mc.localData = pData = new EditPolyData (pObj, t);
	if (pData->CacheValidity().InInterval(t) && pData->GetMesh()) {
		mesh = *pData->GetMesh();
	} else {
		if (!mesh.GetFlag (MN_MESH_FILLED_IN)) mesh.FillInMesh();
		mesh.ClearSpecifiedNormals();	// We always lose specified normals in this mod.

		// In order to "Get Stack Selection", we need to preserve it somehow, even though
		// we often set the MN_SEL flag according to temporary needs.
		// Here, we copy it over, in all selection levels, to the MN_EDITPOLY_STACK_SELECTION flag.
		mesh.ClearVFlags (MN_EDITPOLY_STACK_SELECT);
		mesh.ClearEFlags (MN_EDITPOLY_STACK_SELECT);
		mesh.ClearFFlags (MN_EDITPOLY_STACK_SELECT);
		mesh.PropegateComponentFlags (MNM_SL_VERTEX, MN_EDITPOLY_STACK_SELECT, MNM_SL_VERTEX, MN_SEL);
		mesh.PropegateComponentFlags (MNM_SL_EDGE, MN_EDITPOLY_STACK_SELECT, MNM_SL_EDGE, MN_SEL);
		mesh.PropegateComponentFlags (MNM_SL_FACE, MN_EDITPOLY_STACK_SELECT, MNM_SL_FACE, MN_SEL);

		pData->ApplyAllOperations (mesh);
		pData->SetCache (pObj, t);

		//May not be necessary, already handled by SetCache() / SynchBitArrays()
		UpdatePaintObject(pData);
	}

	pData->ApplyHide (mesh);

	// Set the selection:
	mesh.selLevel = meshSelLevel[selLevel];
	mesh.dispFlags = mCurrentDisplayFlags;
	pData->GetMesh()->selLevel = meshSelLevel[selLevel];
	pData->GetMesh()->dispFlags = mCurrentDisplayFlags;

	Interval ourValidity(FOREVER);
	Interval selectValidity(FOREVER);
	int stackSel;
	mpParams->GetValue (epm_stack_selection, t, stackSel, selectValidity);
	if (stackSel)
	{
		mesh.ClearVFlags (MN_SEL);
		mesh.PropegateComponentFlags (MNM_SL_VERTEX, MN_SEL, MNM_SL_VERTEX, MN_EDITPOLY_STACK_SELECT);
		mesh.ClearEFlags (MN_SEL);
		mesh.PropegateComponentFlags (MNM_SL_EDGE, MN_SEL, MNM_SL_EDGE, MN_EDITPOLY_STACK_SELECT);
		mesh.ClearFFlags (MN_SEL);
		mesh.PropegateComponentFlags (MNM_SL_FACE, MN_SEL, MNM_SL_FACE, MN_EDITPOLY_STACK_SELECT);

		pData->GetMesh()->ClearVFlags (MN_SEL);
		pData->GetMesh()->PropegateComponentFlags (MNM_SL_VERTEX, MN_SEL, MNM_SL_VERTEX, MN_EDITPOLY_STACK_SELECT);
		pData->GetMesh()->ClearEFlags (MN_SEL);
		pData->GetMesh()->PropegateComponentFlags (MNM_SL_EDGE, MN_SEL, MNM_SL_EDGE, MN_EDITPOLY_STACK_SELECT);
		pData->GetMesh()->ClearFFlags (MN_SEL);
		pData->GetMesh()->PropegateComponentFlags (MNM_SL_FACE, MN_SEL, MNM_SL_FACE, MN_EDITPOLY_STACK_SELECT);

		selectValidity &= pObj->ChannelValidity (t, SELECT_CHAN_NUM);
	}
	else
	{
		mesh.VertexSelect (pData->mVertSel);
		mesh.EdgeSelect (pData->mEdgeSel);
		mesh.FaceSelect (pData->mFaceSel);
		pData->GetMesh()->VertexSelect (pData->mVertSel);
		pData->GetMesh()->EdgeSelect (pData->mEdgeSel);
		pData->GetMesh()->FaceSelect (pData->mFaceSel);
	}

	float *softsel = SoftSelection (t, pData, selectValidity);
	if (softsel)
	{
		mesh.SupportVSelectionWeights ();
		float *vs = mesh.getVSelectionWeights ();
		int nv = mesh.VNum();
		memcpy (vs, softsel, nv*sizeof(float));

		pData->GetMesh()->SupportVSelectionWeights ();
		vs = pData->GetMesh()->getVSelectionWeights ();
		memcpy (vs, softsel, nv*sizeof(float));
	}
	else
	{
		mesh.setVDataSupport (VDATA_SELECT, false);
		pData->GetMesh()->setVDataSupport (VDATA_SELECT, false);
	}

	PolyOperation *chosenOp = GetPolyOperation();
	int msl = meshSelLevel[selLevel];
	Interval geomValidity = FOREVER;

	if (chosenOp != NULL) {
		// Selection could change any other channel, so match validities appropriately:
		ourValidity &= selectValidity;

		int id = chosenOp->OpID();
		int numBefore=0;
		if (((id==ep_op_weld_vertex)||(id==ep_op_weld_edge)) && mpDialogOperation)
		{
			// Weld dialog has readouts of before, after vertex counts.
			// This seems like a logical place to update them.

			// Get the before figure.
			numBefore = mesh.numv;
			for (int i=0; i<mesh.numv; i++) {
				if (mesh.v[i].GetFlag (MN_DEAD)) numBefore--;
			}
		}

		chosenOp->RecordSelection (mesh, this, pData);
		chosenOp->SetUserFlags (mesh);
		chosenOp->GetValues (this, t, ourValidity);
		if (chosenOp->ModContextTM())
		{
			if (mc.tm) *(chosenOp->ModContextTM()) = Inverse(*mc.tm);
			else chosenOp->ModContextTM()->IdentityMatrix();
		}
		chosenOp->GetNode (this, t, ourValidity);

		// If we're doing a transform and have point controllers,
		// update the local transform data.
		if (chosenOp->OpID() == ep_op_transform) UpdateTransformData (t, geomValidity, pData);

		bool ret = chosenOp->Apply (mesh, pData->GetPolyOpData());
		if (ret && chosenOp->CanDelete ()) mesh.CollapseDeadStructs ();

		if (ret && (chosenOp->OpID()==ep_op_cut) && pData->GetFlag (kEPDataLastCut))
		{
			LocalCutData *pCut = (LocalCutData *) pData->GetPolyOpData();
			mLastCutEnd = pCut->GetEndVertex ();
			pData->ClearFlag (kEPDataLastCut);
		}

		if (((id==ep_op_weld_vertex)||(id==ep_op_weld_edge)) && mpDialogOperation) {
			// Weld dialog has readouts of before, after vertex counts.
			// This seems like a logical place to update them.
			HWND hWnd = GetDlgHandle (ep_settings);
			if (hWnd) {
				TSTR buf;
				HWND hBefore = GetDlgItem (hWnd, IDC_WELD_BEFORE);
				if (hBefore) {
					buf.printf ("%d", numBefore);
					LPCTSTR lbuf = static_cast<LPCTSTR>(buf);
					SendMessage (hBefore, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(lbuf));
				}

				HWND hAfter = GetDlgItem (hWnd, IDC_WELD_AFTER);
				if (hAfter) {
					int numAfter = mesh.numv;
					for (int i=0; i<mesh.numv; i++) {
						if (mesh.v[i].GetFlag (MN_DEAD)) numAfter--;
					}
					buf.printf ("%d", numAfter);
					LPCTSTR lbuf = static_cast<LPCTSTR>(buf);
					SendMessage (hAfter, WM_SETTEXT, 0, reinterpret_cast<LPARAM>(lbuf));
				}
			}
		}
	}

	// TODO: Potential Optimizations:
	// When is the output cache actually needed?  Can we avoid setting it the rest of the time?
	// How about only saving the PARTs of the output cache we actually need?
	// Maybe we make an "OutputCacheNeeded()" method?
	// We could save the memory used by one "basic" copy of the output mesh - significant savings.
	// Also, setting this cache clears the "TempDataOutput" cache, which means we have to rebuild any TempData we need.
	// Perhaps we can avoid this in cases where the output cache hasn't changed.
	pData->SetOutputCache (mesh);

	pObj->UpdateValidity (GEOM_CHAN_NUM, ourValidity & geomValidity);
	pObj->UpdateValidity (TOPO_CHAN_NUM, ourValidity);
	pObj->UpdateValidity (VERT_COLOR_CHAN_NUM, ourValidity);
	pObj->UpdateValidity (TEXMAP_CHAN_NUM, ourValidity);
	pObj->UpdateValidity (SELECT_CHAN_NUM, ourValidity & selectValidity);
}

float *EditPolyMod::SoftSelection (TimeValue t, EditPolyData *pData, Interval & validity)
{
	int useSoftsel;
	mpParams->GetValue (epm_ss_use, t, useSoftsel, validity);
	if (!useSoftsel) return NULL;

	bool softSelLock = mpParams->GetInt( epm_ss_lock ) != 0;

	//If the soft selection is locked, don't change it
	if (softSelLock && pData->GetPaintSelValues()) {
		if (pData->GetMesh() && pData->GetPaintSelCount() != pData->GetMesh()->numv) {
			// Some sort of topological change has occurred?
			// Get the soft selection values from the mesh.
			// NOTE: This shouldn't happen - this should be covered by EditPolyData::SynchBitArrays().
			//DbgAssert(false);
			pData->SetPaintSelCount (pData->GetMesh()->numv);
			if (pData->GetMesh()->getVSelectionWeights())
				memcpy (pData->GetPaintSelValues(), pData->GetMesh()->getVSelectionWeights(), pData->GetMesh()->numv*sizeof(float));
		}

		return pData->GetPaintSelValues();
	}

	float bubble, pinch, falloff;
	mpParams->GetValue (epm_ss_falloff, t, falloff, validity);
	mpParams->GetValue (epm_ss_pinch, t, pinch, validity);
	mpParams->GetValue (epm_ss_bubble, t, bubble, validity);

	int useEDist, edist=0, affectBackfacing;
	Interval edistValid=FOREVER;
	mpParams->GetValue (epm_ss_edist_use, t, useEDist, validity);
	if (useEDist) {
		mpParams->GetValue (epm_ss_edist, t, edist, edistValid);
		validity &= edistValid;
	}

	mpParams->GetValue (epm_ss_affect_back, t, affectBackfacing, validity);

	return pData->GetSoftSelection (t, falloff, pinch, bubble,
		edist, !affectBackfacing, edistValid);
}

Interval EditPolyMod::LocalValidity(TimeValue t) {
	if (TestAFlag(A_MOD_BEING_EDITED)) return NEVER;
	return GetValidity (t);
}

Interval EditPolyMod::GetValidity (TimeValue t) {
	// We can't just use mpParams->GetValidity,
	// because the parameters we're not currently using
	// shouldn't influence the validity interval.
	Interval ret = FOREVER;
	PolyOperation *pOp = GetPolyOperation ();
	if (pOp != NULL) {
		pOp->GetValues (this, t, ret);
		pOp->GetNode (this, t, ret);
	}
	return ret;
}

// --- Slice: not really a command mode, just looks like it.--------- //

bool EditPolyMod::EpInSlice () {
	int currentOp = GetPolyOperationID();
	if ((currentOp != ep_op_slice) && (currentOp != ep_op_slice_face)) return false;
	if (ip->GetCommandMode() == quickSliceMode) return false;
	return true;
}

static bool slicePreviewMode = false;
static bool sPopSliceCommandMode = false;

void EditPolyMod::EnterSliceMode () {
	int currentOp = GetPolyOperationID();
	if ((currentOp != ep_op_slice) && (currentOp != ep_op_slice_face))
	{
		// Need to record that we started slice operation here, so we know to auto-cancel it later.
		slicePreviewMode = true;
		if (meshSelLevel[selLevel] == MNM_SL_FACE) EpModSetOperation (ep_op_slice_face);
		else EpModSetOperation (ep_op_slice);
	} else slicePreviewMode = false;
	mSliceMode = true;

	UpdateSliceUI();

	// If we're already in a SO move or rotate mode, stay in it;
	// Otherwise, enter SO move.
	if ((ip->GetCommandMode() != moveMode) && (ip->GetCommandMode() != rotMode)) {
		ip->PushCommandMode (moveMode);
		sPopSliceCommandMode = true;
	} else sPopSliceCommandMode = false;

	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	EpModRefreshScreen ();
}

void EditPolyMod::ExitSliceMode () {
	if (slicePreviewMode) {
		EpModCancel ();
		slicePreviewMode = false;
	}
	mSliceMode = false;

	UpdateSliceUI();

	if (sPopSliceCommandMode && (ip->GetCommandStackSize()>1)) ip->PopCommandMode ();

	NotifyDependents(FOREVER, PART_DISPLAY, REFMSG_CHANGE);
	EpModRefreshScreen ();
}

bool EditPolyMod::InSlicePreview () {
	if (!EpInSlice()) return false;
	return slicePreviewMode;
}

Matrix3 EditPolyMod::CompSliceMatrix (TimeValue t,INode *inode,ModContext *mc) {
	if (mc->localData == NULL) mc->localData = (LocalModData *) new EditPolyData();
	EditPolyData *pData = (EditPolyData *) mc->localData;
	if (pData->GetSlicePlaneSize() < 0) {
		float size1 = (mc->box->pmax.x - mc->box->pmin.x)*.52f;
		float size2 = (mc->box->pmax.y - mc->box->pmin.y)*.52f;
		if (size2>size1) size1=size2;
		if (size1 < 1.0f) size1 = 1.0f;
		pData->SetSlicePlaneSize (size1);
	}

	Interval iv;
	Matrix3 tm(1);
	if (mc && mc->tm) tm = Inverse(*(mc->tm));
	if (inode) tm = tm * inode->GetObjTMBeforeWSM(t,&iv);

	Matrix3 planeTM(1);
	mpSliceControl->GetValue (t, &planeTM, FOREVER, CTRL_RELATIVE);

	return planeTM*tm;
}

void EditPolyMod::GetSlicePlaneBoundingBox (TimeValue t, INode *inode, ViewExp *vpt, Box3 & box, ModContext *mc) {
	Matrix3 tm = CompSliceMatrix(t,inode,mc);
	EditPolyData *smd=(EditPolyData *)mc->localData;
	float size = smd->GetSlicePlaneSize ();
	box.Init();
	box += Point3(-size,-size, 0.0f)*tm;
	box += Point3(-size,size,0.0f)*tm;
	box += Point3(size,size,0.0f)*tm;
	box += Point3(size,-size,0.0f)*tm;
}

void EditPolyMod::DisplaySlicePlane (TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {
	GraphicsWindow *gw = vpt->getGW();
	Matrix3 tm = CompSliceMatrix(t,inode,mc);
	EditPolyData *smd=(EditPolyData *)mc->localData;
	float size = smd->GetSlicePlaneSize();
	int savedLimits;

	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);

	// Draw rectangle representing slice plane.
	if (mSliceMode) gw->setColor(LINE_COLOR,GetUIColor(COLOR_SEL_GIZMOS));
	else gw->setColor (LINE_COLOR, GetUIColor (COLOR_GIZMOS));

	Point3 rp[5];
	rp[0] = Point3(-size,-size,0.0f);
	rp[1] = Point3(-size,size,0.0f);
	rp[2] = Point3(size,size,0.0f);
	rp[3] = Point3(size,-size,0.0f);
	gw->polyline (4, rp, NULL, NULL, TRUE, NULL);
	
	gw->setRndLimits(savedLimits);
}

int EditPolyMod::HitTestSlice (TimeValue t, INode* inode, int type, int crossing, int flags,
						  IPoint2 *p, ViewExp *vpt, ModContext* mc) {
	GraphicsWindow *gw = vpt->getGW();
	Point3 pt;
	HitRegion hr;
	int savedLimits, res = 0;
	Matrix3 tm = CompSliceMatrix(t,inode,mc);
	EditPolyData *smd=(EditPolyData *)mc->localData;
	float size = smd->GetSlicePlaneSize();

	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);	
	gw->setTransform(tm);

	gw->clearHitCode();
	Point3 rp[5];
	rp[0] = Point3 (-size, -size, 0.0f);
	rp[1] = Point3 (-size,  size, 0.0f);
	rp[2] = Point3 ( size,  size, 0.0f);
	rp[3] = Point3 ( size, -size, 0.0f);
	gw->polyline (4, rp, NULL, NULL, TRUE, NULL);
	if (gw->checkHitCode()) {
		vpt->LogHit(inode, mc, gw->getHitDistance(), 0, NULL);
		res = 1;
	}

	gw->setRndLimits(savedLimits);
	return res;
}

// What EditPolyMod level should we be hit-testing on?
DWORD EditPolyMod::CurrentHitLevel (int *selByVert) {
	// Under normal circumstances, pretty much the level we're at.
	DWORD hitLev = hitLevel[selLevel];

	// But we might be selecting by vertex...
	int sbv;
	if (selByVert == NULL) selByVert = &sbv;
	mpParams->GetValue (epm_by_vertex, TimeValue(0), *selByVert, FOREVER);
	if (*selByVert) {
		hitLev = SUBHIT_MNVERTS;
		if (selLevel != EPM_SL_VERTEX) hitLev |= SUBHIT_MNUSECURRENTSEL;
		if (selLevel == EPM_SL_BORDER) hitLev |= SUBHIT_OPENONLY;
	}

	// And if we're in a command mode, it may override the current hit level:
	if (mHitLevelOverride != 0x0) hitLev = mHitLevelOverride;
	return hitLev;
}

MNMesh *EditPolyMod::GetMeshToHitTest (EditPolyData *pData) {
	if (mHitTestResult) return pData->GetMeshOutput();
	if (mpCurrentOperation) {
		if (mpCurrentOperation->OpID() == ep_op_transform) return pData->GetMeshOutput();
	}
	return pData->GetMesh();
}

int EditPolyMod::HitTest (TimeValue t, INode* inode, int type, int crossing, int flags,
						  IPoint2 *p, ViewExp *vpt, ModContext* mc) {

	if (mSliceMode && CheckNodeSelection (ip, inode))
		return HitTestSlice (t, inode, type, crossing, flags, p, vpt, mc);

	if (!selLevel && !mHitLevelOverride) return 0;

	Interval valid;
	int savedLimits;
	GraphicsWindow *gw = vpt->getGW();
	HitRegion hr;
	
	int ignoreBackfacing;
	mpParams->GetValue (epm_ignore_backfacing, t, ignoreBackfacing, FOREVER);
	if (mForceIgnoreBackfacing) ignoreBackfacing = true;

	// Setup GW
	MakeHitRegion(hr,type, crossing,4,p);
	gw->setHitRegion(&hr);
	Matrix3 mat = inode->GetObjectTM(t);
	gw->setTransform(mat);	
	gw->setRndLimits(((savedLimits = gw->getRndLimits()) | GW_PICK) & ~GW_ILLUM);
	if (ignoreBackfacing) gw->setRndLimits (gw->getRndLimits() | GW_BACKCULL);
	else gw->setRndLimits (gw->getRndLimits() & ~GW_BACKCULL);
	gw->clearHitCode();

	if (!mc->localData) return 0;
	EditPolyData *pData = (EditPolyData *) mc->localData;
	MNMesh *pMesh = GetMeshToHitTest (pData);
	if (pMesh == NULL) return 0;

	pData->SetConverterFlag (MNM_SELCONV_REQUIRE_ALL, (crossing||(type==HITTYPE_POINT))?false:true);

	int selByVert;
	DWORD hitLev = CurrentHitLevel (&selByVert);

	SubObjHitList hitList;
	int res = pMesh->SubObjectHitTest(gw, gw->getMaterial(), &hr,
		flags|hitLev, hitList);

	MeshSubHitRec *rec = hitList.First();

	if (rec) {
		int max=0;
		if (mHitTestResult && mIgnoreNewInResult && pData->GetMesh()) {
			if (hitLev & SUBHIT_MNVERTS) max = pData->GetMesh()->numv;
			else if (hitLev & SUBHIT_MNEDGES) max = pData->GetMesh()->nume;
			else if (hitLev & SUBHIT_MNFACES) max = pData->GetMesh()->numf;
		}

		Tab<int> diagStart;
		Tab<int> diagToFace;
		if (hitLev & SUBHIT_MNDIAGONALS) {
			diagStart.SetCount (pMesh->numf+1);
			diagStart[0] = 0;
			for (int i=0; i<pMesh->numf; i++) {
				int numDiags = (pMesh->f[i].GetFlag (MN_DEAD)||pData->HiddenFace(i)) ? 0 : pMesh->f[i].deg-3;
				diagStart[i+1] = diagStart[i] + numDiags;
			}

			diagToFace.SetCount (diagStart[pMesh->numf]);
			for (i=0; i<pMesh->numf; i++) {
				for (int j=diagStart[i]; j<diagStart[i+1]; j++) diagToFace[j] = i;
			}
		}

		for (; rec; rec = rec->Next()) {
			if ((max>0) && (rec->index >= max)) {
				res--;
				continue;
			}
			if (hitLev & SUBHIT_MNDIAGONALS)
			{
				int fid = diagToFace[rec->index];
				int did = rec->index - diagStart[fid];
				vpt->LogHit (inode, mc, rec->dist, rec->index, new MNDiagonalHitData (fid, did));
			}
			else vpt->LogHit(inode,mc,rec->dist,rec->index,NULL);
		}
	}

	gw->setRndLimits(savedLimits);	
	return res;	
}

void EditPolyMod::UpdateDisplayFlags () {
	mCurrentDisplayFlags = editPolyLevelToDispFlags[selLevel] | mDispLevelOverride;

	// If we're showing the gizmo, always suspend showing the end result vertices.
	if (ShowGizmo()) mCurrentDisplayFlags -= (mCurrentDisplayFlags & (MNDISP_SELVERTS|MNDISP_VERTTICKS));
}

MNMesh *EditPolyMod::GetMeshToDisplay (EditPolyData *pData) {
	if (mDisplayResult) return pData->GetMeshOutput();
	if (mpCurrentOperation) {
		if (mpCurrentOperation->OpID() == ep_op_transform) return pData->GetMeshOutput();
		if (mpCurrentOperation->OpID() == ep_op_create) return pData->GetMeshOutput();
	}
	return pData->GetMesh();
}

int EditPolyMod::Display (TimeValue t, INode* inode, ViewExp *vpt, int flags, ModContext *mc) {
	if (!selLevel) return 0;
	if (!mc->localData) return 0;
	if (EpInSlice() && CheckNodeSelection(ip, inode))
		DisplaySlicePlane (t, inode, vpt, flags, mc);

	// Set up GW
	GraphicsWindow *gw = vpt->getGW();
	Matrix3 tm = inode->GetObjectTM(t);
	int savedLimits;
	gw->setRndLimits((savedLimits = gw->getRndLimits()) & ~GW_ILLUM);
	gw->setTransform(tm);

	if (ShowGizmo())
	{
		EditPolyData *pData = (EditPolyData *) mc->localData;
		MNMesh *pMesh = GetMeshToDisplay(pData);
		if (pMesh) {

			// We need to draw a "gizmo" version of the mesh:
			Point3 colSel=GetSubSelColor();
			Point3 colTicks=GetUIColor (COLOR_VERT_TICKS);
			Point3 colGiz=GetUIColor(COLOR_GIZMOS);
			Point3 colGizSel=GetUIColor(COLOR_SEL_GIZMOS);
			gw->setColor (LINE_COLOR, colGiz);

			Point3 rp[3];
			int i;
			int es[3];
			for (i=0; i<pMesh->nume; i++) {
				if (pMesh->e[i].GetFlag (MN_DEAD)) continue;
				if (!pMesh->e[i].GetFlag (MN_EDGE_INVIS)) {
					es[0] = GW_EDGE_VIS;
				} else {
					if (selLevel < EPM_SL_EDGE) continue;
					if (selLevel > EPM_SL_FACE) continue;
					es[0] = GW_EDGE_INVIS;
				}
				bool displayEdgeSel = false;
				switch (meshSelLevel[selLevel]) {
				case MNM_SL_EDGE:
					displayEdgeSel = IsEdgeSelected (pData, i);
					break;
				case MNM_SL_FACE:
					displayEdgeSel = IsFaceSelected (pData, pMesh->e[i].f1);
					if (pMesh->e[i].f2>-1) displayEdgeSel |= IsFaceSelected (pData, pMesh->e[i].f2);
					break;
				}
				if (displayEdgeSel) gw->setColor (LINE_COLOR, colGizSel);
				else gw->setColor (LINE_COLOR, colGiz);
				rp[0] = pMesh->v[pMesh->e[i].v1].p;
				rp[1] = pMesh->v[pMesh->e[i].v2].p;
				gw->polyline (2, rp, NULL, NULL, FALSE, es);
			}
			DWORD dispFlags = editPolyLevelToDispFlags[selLevel] | mDispLevelOverride;
			if (dispFlags & MNDISP_VERTTICKS) {
				float *ourvw = (dispFlags & MNDISP_SELVERTS) ? SoftSelection(t, pData, FOREVER) : NULL;
				gw->setColor (LINE_COLOR, colTicks);
				for (i=0; i<pMesh->numv; i++) {
					if (pMesh->v[i].GetFlag (MN_DEAD)) continue;
					if (pData->HiddenVertex (i)) continue;

					if (dispFlags & MNDISP_SELVERTS)
					{
						if (IsVertexSelected (pData, i)) gw->setColor (LINE_COLOR, colSel);
						else if (ourvw && ourvw[i]) gw->setColor (LINE_COLOR, SoftSelectionColor(ourvw[i]));
						else gw->setColor (LINE_COLOR, colTicks);
					}

					if(getUseVertexDots()) gw->marker (&(pMesh->v[i].p), VERTEX_DOT_MARKER(getVertexDotType()));
					else gw->marker (&(pMesh->v[i].p), PLUS_SIGN_MRKR);
				}
			}
		}
	}

	gw->setRndLimits(savedLimits);
	return 0;	
}

void EditPolyMod::GetWorldBoundBox(TimeValue t, INode* inode, ViewExp *vpt, Box3& box, ModContext *mc) {
	if (!ip) return;

	if (EpInSlice() && CheckNodeSelection (ip, inode))
		GetSlicePlaneBoundingBox (t, inode, vpt, box, mc);

	if (ShowGizmo ()) {
		Matrix3 tm = inode->GetObjectTM(t);
		EditPolyData *pData = (EditPolyData *) mc->localData;
		MNMesh *pMesh = GetMeshToDisplay(pData);
		if (!pMesh) return;
		box = pMesh->getBoundingBox (&tm);
	}
}

void EditPolyMod::GetSubObjectCenters (SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc) {
	Matrix3 tm = node->GetObjectTM(t);

	if (mSliceMode) {
		Matrix3 tm = CompSliceMatrix (t, node, mc);
		cb->Center (tm.GetTrans(), 0);
		return;
	}

	if (!mc->localData) return;
	if (selLevel == EPM_SL_OBJECT) return;	// shouldn't happen.

	EditPolyData *pData = (EditPolyData *) mc->localData;
	MNMesh *pMesh = GetMeshToDisplay (pData);
	if (!pMesh) return;
	float *vsw = SoftSelection (t, pData, FOREVER);

	if (selLevel == EPM_SL_VERTEX) {
		BitArray vTempSel;
		GetVertexTempSelection (pData, vTempSel);;
		Point3 cent(0,0,0);
		int ct=0;
		for (int i=0; i<pMesh->numv; i++) {
			if (vTempSel[i] || (vsw && vsw[i])) {
				cent += pMesh->v[i].p;
				ct++;
			}
		}
		if (ct) {
			cent /= float(ct);			
			cb->Center(cent*tm,0);
		}
		return;
	}

	MNTempData *pTemp = (pMesh == pData->GetMeshOutput())
		? pData->TempDataOutput() : pData->TempData();

	if (pTemp == NULL) return;
	Tab<Point3> *centers = pTemp->ClusterCenters(meshSelLevel[selLevel], MN_SEL);
	DbgAssert (centers);
	if (!centers) return;
	for (int i=0; i<centers->Count(); i++) cb->Center((*centers)[i]*tm,i);
}

void EditPolyMod::GetSubObjectTMs (SubObjAxisCallback *cb,TimeValue t,INode *node,ModContext *mc) {
	if (mSliceMode) {
		cb->TM (CompSliceMatrix (t, node, mc), 0);
		return;
	}

	if (!mc->localData) return;
	EditPolyData *pData = (EditPolyData *) mc->localData;

	MNMesh *pMesh = GetMeshToDisplay (pData);
	MNTempData *pTemp = (pMesh == pData->GetMeshOutput()) ? pData->TempDataOutput() : pData->TempData ();
	if (!pMesh) return;
	if (!pTemp) return;
	float *vsw = SoftSelection (t, pData, FOREVER);

	Matrix3 otm = node->GetObjectTM(t);
	Matrix3 tm;

	switch (selLevel) {
	case EPM_SL_OBJECT:
		break;

	case EPM_SL_VERTEX:
		if (ip->GetCommandMode()->ID()==CID_SUBOBJMOVE) {
			Matrix3 otm = node->GetObjectTM(t);
			for (int i=0; i<pMesh->numv; i++) {
				if (!pMesh->v[i].FlagMatch (MN_SEL|MN_DEAD, MN_SEL) && (!vsw || !vsw[i])) continue;
				pMesh->GetVertexSpace (i, tm);
				Point3 p = pMesh->v[i].p;
				tm = tm * otm;
				tm.SetTrans(pMesh->v[i].p*otm);
				cb->TM(tm,i);
			}
		} else {
			for (int i=0; i<pMesh->numv; i++) if (pMesh->v[i].FlagMatch (MN_SEL|MN_DEAD, MN_SEL) || (vsw && vsw[i])) break;
			if (i >= pMesh->numv) return;
			Point3 norm(0,0,0), *nptr=pTemp->VertexNormals ()->Addr(0);
			Point3 cent(0,0,0);
			int ct=0;

			// Compute average face normal
			for (; i<pMesh->numv; i++) {
				if (!pMesh->v[i].FlagMatch (MN_SEL|MN_DEAD, MN_SEL) && (!vsw || !vsw[i])) continue;
				cent += pMesh->v[i].p;
				norm += nptr[i];
				ct++;
			}
			cent /= float(ct);
			norm = Normalize (norm/float(ct));

			cent = cent * otm;
			norm = Normalize(VectorTransform(otm,norm));
			Matrix3 mat;
			MatrixFromNormal(norm,mat);
			mat.SetTrans(cent);
			cb->TM(mat,0);
		}
		break;

	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		int i, ct;
		ct = pTemp->ClusterNormals (MNM_SL_EDGE, MN_SEL)->Count();
		for (i=0; i<ct; i++) {
			tm = pTemp->ClusterTM (i) * otm;
			cb->TM(tm,i);
		}
		break;

	default:
		ct = pTemp->ClusterNormals (MNM_SL_FACE, MN_SEL)->Count();
		for (i=0; i<ct; i++) {
			tm = pTemp->ClusterTM (i) * otm;
			cb->TM(tm,i);
		}
		break;
	}
}

// Indicates whether the Show Cage checkbox is relevant:
bool EditPolyMod::ShowGizmoConditions () {
	if (!ip) return false;
	if (mpCurrentEditMod != this) return false;
	if (selLevel == EPM_SL_OBJECT) return false;
	return true;
}

// Indicates whether we're currently showing the cage gizmo.
bool EditPolyMod::ShowGizmo () {
	if (!ip) return false;
	if (mpCurrentEditMod != this) return false;
	if (selLevel == EPM_SL_OBJECT) return false;
	if (mpParams == NULL) return false;
	return mpParams->GetInt (epm_show_cage) != 0;
}

void EditPolyMod::CreatePointControllers () {
	if (mPointControl.Count()) return;

	// Find the number of vertices, and assign the ranges, for all nodes:
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts (list, nodes);
	if (list.Count() == 0) return;
	mPointNode.ZeroCount();
	TSTR empty;
	mPointRange.SetCount (list.Count()+1);
	int numv=0;
	for (int i=0; i<list.Count(); i++) {
		mPointNode.Append (1, &empty);
		mPointNode[i] = nodes[i]->GetName();
		mPointRange[i] = numv;
		if (list[i]->localData == NULL) continue;

		EditPolyData *pData = (EditPolyData *) list[i]->localData;
		if (pData->GetMesh()) numv += pData->GetMesh()->numv;
		else numv += pData->mVertSel.GetSize();
	}
	nodes.DisposeTemporary ();
	mPointRange[list.Count()] = numv;

	AllocPointControllers (numv);
}

void EditPolyMod::AllocPointControllers (int count) {
	int oldCount = mPointControl.Count();
	mPointControl.SetCount (count);
	if (mpPointMaster) mpPointMaster->SetNumSubControllers (count);
	for (int i=oldCount; i<count; i++) {
		mPointControl[i] = NULL;
		if (mpPointMaster) mpPointMaster->SetSubController(i, NULL);
	}
}

void EditPolyMod::DeletePointControllers () {
	if (!mPointControl.Count()) return;

	for (int i=0; i<mPointControl.Count(); i++) {
		if (mPointControl[i] == NULL) continue;
		ReplaceReference (i+EDIT_POINT_BASE_REF, NULL, true);
	}
	AllocPointControllers (0);
}

void EditPolyMod::PlugControllers (TimeValue t, Point3 *pOffsets, int numOffsets, INode *pNode) {
	if (numOffsets<0) return;
	SetKeyModeInterface *ski = GetSetKeyModeInterface(GetCOREInterface());
	if( !ski || !ski->TestSKFlag(SETKEY_SETTING_KEYS) ) {
		if(!AreWeKeying(t)) { return; }
	}

	BitArray set;
	set.SetSize (numOffsets);
	for (int i=0; i<numOffsets; i++) {
		if (pOffsets[i] != Point3(0,0,0)) set.Set(i);
	}

	PlugControllers (t, set, pNode);
}

void EditPolyMod::PlugControllers (TimeValue t, BitArray &set, INode *pNode) {
	SetKeyModeInterface *ski = GetSetKeyModeInterface(GetCOREInterface());
	if( !ski || !ski->TestSKFlag(SETKEY_SETTING_KEYS) ) {
		if(!AreWeKeying(t)) { return; }
	}

	// Find the range for this particular node:
	if (mPointControl.Count() == 0) CreatePointControllers ();
	for (int j=0; j<mPointNode.Count(); j++) {
		if (mPointNode[j] == TSTR(pNode->GetName())) break;
	}
	if (j==mPointNode.Count()) {
		// Node not listed in our set of nodes.
		// This can happen if the Edit Poly is instanced to a new node,
		// which can be done through Maxscript at least.  (A = $.modifiers[#Edit_Poly] / addModifier $Box01 A)
		EditPolyData *pData = GetEditPolyDataForNode (pNode);
		if (pData == NULL) return;

		TSTR empty;
		mPointNode.Append (1, &empty);
		mPointNode[j] = pNode->GetName();
		mPointRange.SetCount (j+2);
		if (pData->GetMesh()) {
			mPointRange[j+1] = mPointRange[j] + pData->GetMesh()->numv;
		} else {
			mPointRange[j+1] = mPointRange[j] + pData->mVertSel.GetSize();
		}
		AllocPointControllers (mPointRange[j+1]);
	}

	BOOL res = FALSE;
	for (int i=0; i<set.NumberSet(); i++) {
		if (!set[i]) continue;
		if (i + mPointRange[j]>=mPointRange[j+1]) break;
		if (PlugController (t, i, mPointRange[j], pNode)) res = TRUE;
	}
	if (res) NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
}

BOOL EditPolyMod::PlugController (TimeValue t, int vertex, int nodeStart, INode *pNode) {
	int i = vertex + nodeStart;
	if (!AreWeKeying(t)) return FALSE;
	if (!mPointControl.Count()) CreatePointControllers ();
	if ( i >= mPointControl.Count()) return FALSE;
	if (mPointControl[i]) return false;	// already got a controller here.

	EditPolyData *pData = GetEditPolyDataForNode (pNode);
	if (!pData) return false;
	if (!pData->GetPolyOpData() || (pData->GetPolyOpData()->OpID() != ep_op_transform)) return false;
	LocalTransformData *pTransData = (LocalTransformData *) pData->GetPolyOpData();

	MakeRefByID (FOREVER, i + EDIT_POINT_BASE_REF, NewDefaultPoint3Controller());
	SuspendAnimate();
	AnimateOff();
	SuspendSetKeyMode(); // AF (5/13/03)
	theHold.Suspend ();
#if defined(NEW_SOA)
	GetPolyObjDescriptor()->Execute(I_EXEC_EVAL_SOA_TIME, reinterpret_cast<ULONG_PTR>(&t));
#endif

	// These guys are all offsets, but I need to initialize to the "current" offset, if any:
	if (pTransData->NumOffsets() > vertex) mPointControl[i]->SetValue (t, pTransData->OffsetPointer(vertex));
	else {
		Point3 zero(0,0,0);
		mPointControl[i]->SetValue (t, &zero);
	}
	ResumeSetKeyMode();
	theHold.Resume ();

	ResumeAnimate ();
	mpPointMaster->SetSubController (i, mPointControl[i]);
	return TRUE;
}

RefTargetHandle EditPolyMod::GetReference(int i) {
	switch (i) {
	case EDIT_PBLOCK_REF: return mpParams;
	case EDIT_SLICEPLANE_REF: return mpSliceControl;
	case EDIT_MASTER_POINT_REF: return mpPointMaster;
	}
	i -= EDIT_POINT_BASE_REF;
	if ((i<0) || (i>=mPointControl.Count())) return NULL;
	return mPointControl[i];
}

void EditPolyMod::SetReference(int i, RefTargetHandle rtarg) {
	switch (i) {
	case EDIT_PBLOCK_REF:
		mpParams = (IParamBlock2*)rtarg;
		return;
	case EDIT_SLICEPLANE_REF:
		mpSliceControl = (Control *)rtarg;
		return;
	case EDIT_MASTER_POINT_REF:
		mpPointMaster = (MasterPointControl*)rtarg;
		if (mpPointMaster) mpPointMaster->SetNumSubControllers (mPointControl.Count());
		return;
	}

	i -= EDIT_POINT_BASE_REF;
	if (i<0) return;	// shouldn't happen.
	if (!mPointControl.Count()) CreatePointControllers ();
	if (i>=mPointControl.Count()) return;
	mPointControl[i] = (Control *) rtarg;
}

RefResult EditPolyMod::NotifyRefChanged( Interval changeInt,RefTargetHandle hTarget, 
		PartID& partID, RefMessage message) {
	if (message == REFMSG_CHANGE) {
		if (hTarget == mpParams) {
			// if this was caused by a NotifyDependents from pblock, LastNotifyParamID()
			// will contain ID to update, else it will be -1 => inval whole rollout
			int pid = mpParams->LastNotifyParamID();
			if (ip && (pid>-1)) InvalidateDialogElement (pid);


			if (((pid==epm_ss_use) || (pid==epm_ss_lock)) && InPaintMode()) 
				EndPaintMode (); //end paint mode before invalidating UI

			switch (pid) {
			case epm_ss_use:
			case epm_ss_edist_use:
			case epm_ss_edist:
				InvalidateDistanceCache ();
				break;
			case epm_ss_affect_back:
			case epm_ss_falloff:
			case epm_ss_pinch:
			case epm_ss_bubble:
			case epm_ss_lock:
				InvalidateSoftSelectionCache ();
				break;
			case epm_current_operation:
				if (mpDialogOperation) EpModCloseOperationDialog ();
				UpdateOpChoice ();
				UpdateEnables (ep_animate);
				UpdateSliceUI ();
				break;
			case epm_extrude_spline_node:
				if (mpCurrentOperation) mpCurrentOperation->ClearNode (pid);
				UpdateOperationDialogParams ();
				break;
			case epm_stack_selection:
				InvalidateDistanceCache ();
				UpdateEnables (ep_select);
				SetNumSelLabel();
				break;
			case epm_show_cage:
				UpdateDisplayFlags ();
				break;
			case epm_animation_mode:
				UpdateEnables (ep_geom);
				UpdateEnables (ep_subobj);
				break;
			}
		}
	}
	return REF_SUCCEED;
}

TSTR EditPolyMod::SubAnimName(int i)
{
	switch (i)
	{
	case EDIT_PBLOCK_REF: return GetString (IDS_PARAMETERS);
	case EDIT_SLICEPLANE_REF: return GetString (IDS_EDITPOLY_SLICE_PLANE);
	case EDIT_MASTER_POINT_REF: return GetString (IDS_EDITPOLY_MASTER_POINT_CONTROLLER);
	}

	i -= EDIT_POINT_BASE_REF;
	if ((i<0) || (i>=mPointControl.Count())) return _T("");

	TSTR buf;
	if (mPointRange.Count()>2) {
		// Find out which node, and which vertex, this refers to:
		for (int j=1; j<mPointRange.Count(); j++) {
			if (i<mPointRange[j]) break;
		}
		if (j==mPointRange.Count()) return _T("");	// shouldn't happen.
		buf.printf ("%s: %s %d", mPointNode[j-1], GetString(IDS_VERTEX), i+1-mPointRange[j-1]);
	} else {
		// Edit Poly only applied to one node, so leave out the node name.
		buf.printf ("%s %d", GetString (IDS_VERTEX), i+1);
	}
	return buf;
}

BOOL EditPolyMod::AssignController (Animatable *control, int subAnim) {
	ReplaceReference (subAnim, (Control*)control);
	if (subAnim==EDIT_MASTER_POINT_REF) {
		int n = mPointControl.Count();
		mpPointMaster->SetNumSubControllers(n);
		for (int i=0; i<n; i++) if (mPointControl[i]) mpPointMaster->SetSubController(i,mPointControl[i]);
	}
	NotifyDependents(FOREVER,PART_ALL,REFMSG_CHANGE);
	NotifyDependents(FOREVER,0,REFMSG_SUBANIM_STRUCTURE_CHANGED);
	return TRUE;
}

BOOL EditPolyMod::SelectSubAnim (int subNum) {
	if (subNum<EDIT_POINT_BASE_REF) return FALSE;	// can only select points, not other controllers.
	subNum -= EDIT_POINT_BASE_REF;
	if (subNum >= mPointControl.Count()) return FALSE;

	for (int j=1; j<mPointRange.Count(); j++) {
		if (subNum < mPointRange[j]) break;
	}
	if (j==mPointRange.Count()) return false;
	subNum -= mPointRange[j-1];

	// Find the node corresponding to the node name:
	TSTR name = mPointNode[j-1];
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts (list, nodes);
	for (int i=0; i<nodes.Count(); i++) {
		if (name == TSTR(nodes[i]->GetName())) break;
	}
	if (i==nodes.Count()) return false;
	INode *pNode = nodes[i]->GetActualINode();
	nodes.DisposeTemporary ();

	EditPolyData *pData = GetEditPolyDataForNode (nodes[i]);
	BitArray nvs;
	nvs.SetSize (pData->mVertSel.GetSize());
	nvs.ClearAll ();
	nvs.Set (subNum);

	BOOL add = GetKeyState(VK_CONTROL)<0;
	BOOL sub = GetKeyState(VK_MENU)<0;

	if (add || sub) {
		EpModSelect (MNM_SL_VERTEX, nvs, false, sub!=0, pNode);
	} else {
		EpModSetSelection (MNM_SL_VERTEX, nvs, pNode);
	}

	EpModRefreshScreen ();
	return TRUE;
}

// Subobject API
int EditPolyMod::NumSubObjTypes() {
	return 5;
}

ISubObjType *EditPolyMod::GetSubObjType(int i) {
	static bool initialized = false;
	if(!initialized) {
		initialized = true;
		SOT_Vertex.SetName(GetString(IDS_EDITPOLY_VERTEX));
		SOT_Edge.SetName(GetString(IDS_EDITPOLY_EDGE));
		SOT_Border.SetName(GetString(IDS_EDITPOLY_BORDER));
		SOT_Face.SetName(GetString(IDS_EDITPOLY_FACE));
		SOT_Element.SetName(GetString(IDS_EDITPOLY_ELEMENT));
	}

	switch(i) {
	case -1:
		if(selLevel > 0) 
			return GetSubObjType(selLevel-1);
		break;
	case 0:
		return &SOT_Vertex;
	case 1:
		return &SOT_Edge;
	case 2:
		return &SOT_Border;
	case 3:
		return &SOT_Face;
	case 4:
		return &SOT_Element;
	}
	return NULL;
}

// Class EPolyBackspaceUser: Used to process backspace key input.
void EditPolyBackspaceUser::Notify() {
	if (!mpEditPoly) return;

	// If we're in create-face mode, send the backspace event there:
	if ((mpEditPoly->createFaceMode)  &&
		(mpEditPoly->ip->GetCommandMode () == mpEditPoly->createFaceMode)) {
		mpEditPoly->createFaceMode->Backspace ();
		return;
	}

	// Otherwise, use it to generate a "remove" event:
	if ((mpEditPoly->selLevel > EPM_SL_OBJECT) && (mpEditPoly->selLevel < EPM_SL_FACE)) {
		theHold.Begin ();
		switch (mpEditPoly->GetMNSelLevel()) {
		case MNM_SL_VERTEX: mpEditPoly->EpModButtonOp (ep_op_remove_vertex); break;
		case MNM_SL_EDGE: mpEditPoly->EpModButtonOp (ep_op_remove_edge); break;
		}
		theHold.Accept (GetString (IDS_REMOVE));
	}
}

void EditPolyMod::ActivateSubobjSel(int level, XFormModes& modes) {
	if (ip) {
		// Register or unregister delete, backspace key notification
		if (selLevel==EPM_SL_OBJECT && level!=EPM_SL_OBJECT) {
			ip->RegisterDeleteUser(this);
			backspacer.SetEPoly (this);
			backspaceRouter.Register (&backspacer);
		}
		if (selLevel!=EPM_SL_OBJECT && level==EPM_SL_OBJECT) {
			ip->UnRegisterDeleteUser(this);
			backspacer.SetEPoly (NULL);
			backspaceRouter.UnRegister (&backspacer);
		}
	}

	if (selLevel != level) {
		ExitAllCommandModes (level == EPM_SL_OBJECT, level == EPM_SL_OBJECT);
	}

	SetSubobjectLevel(level);

	// Fill in modes with our sub-object modes
	if (level!=EPM_SL_OBJECT)
		modes = XFormModes(moveMode,rotMode,nuscaleMode,uscaleMode,squashMode,selectMode);

	// Update UI
	UpdateCageCheckboxEnable ();
	UpdateSelLevelDisplay ();
	UpdateDisplayFlags ();
	SetNumSelLabel ();

	if (ip) {
		ip->PipeSelLevelChanged();
		SetupNamedSelDropDown ();
		UpdateNamedSelDropDown ();
	}
}

void EditPolyMod::SetSubobjectLevel(int level) {
	int oldSelLevel = selLevel;
	selLevel = level;

	EpModAboutToChangeSelection ();

	if (mpCurrentEditMod==this) UpdateUIToSelectionLevel (oldSelLevel);
	EpModLocalDataChanged (PART_SUBSEL_TYPE|PART_DISPLAY);
	if (ip) ip->RedrawViews (ip->GetTime());
}

void EditPolyMod::SelectSubComponent (HitRecord *pFirstHit, BOOL selected, BOOL all, BOOL invert) {
	if (mpParams->GetInt (epm_stack_selection)) return;
	if (selLevel == EPM_SL_OBJECT) return;
	if (!ip) return;
	if (mSliceMode) return;

	EpModAboutToChangeSelection ();

	EditPolyData *pData = NULL;
	HitRecord *pHitRec;

	int selByVert = mpParams->GetInt (epm_by_vertex);

	// Prepare a bitarray representing the particular hits we got:
	HitRecord *hr;
	if (selByVert) {
		for (hr=pFirstHit; hr!=NULL; hr = hr->Next()) {
			pData = (EditPolyData*)hr->modContext->localData;
			if (!pData->GetNewSelection ()) {
				pData->SetupNewSelection (MNM_SL_VERTEX);
				pData->SynchBitArrays();
			}
			pData->GetNewSelection()->Set (hr->hitInfo, true);
			if (!all) break;
		}
	} else {
		if (mpParams->GetInt (epm_select_by_angle) && (selLevel == EPM_SL_FACE)) {
			float selectAngle = mpParams->GetFloat (epm_select_angle);
			for (hr=pFirstHit; hr!=NULL; hr = hr->Next()) {
				pData = (EditPolyData*)hr->modContext->localData;
				if (!pData->GetNewSelection ()) {
					pData->SetupNewSelection (meshSelLevel[selLevel]);
					pData->SynchBitArrays();
				}
				if (!pData->GetMesh()) {
					// Mesh mysteriously missing.  Fall back on selecting hit faces.
					pData->GetNewSelection()->Set (hr->hitInfo, true);
				} else {
					MNMeshUtilities mmu(pData->GetMesh());
					mmu.SelectPolygonsByAngle (hr->hitInfo, selectAngle, *(pData->GetNewSelection()));
				}
				if (!all) break;
			}
		} else {
			for (hr=pFirstHit; hr!=NULL; hr = hr->Next()) {
				pData = (EditPolyData*)hr->modContext->localData;
				if (!pData->GetNewSelection ()) {
					pData->SetupNewSelection (meshSelLevel[selLevel]);
					pData->SynchBitArrays();
				}
				pData->GetNewSelection()->Set (hr->hitInfo, true);
				if (!all) break;
			}
		}
	}

	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);
	int numContexts = list.Count();
	nodes.DisposeTemporary ();

	TCHAR *levelName = LookupMNMeshSelLevel (meshSelLevel[selLevel]);

	// Apply the "new selections" to all the local mod datas:
	// (Must be after assembling all hits so we don't over-invert normals.)
	bool changeOccurred = false;
	for (pHitRec=pFirstHit; pHitRec!=NULL; pHitRec = pHitRec->Next()) {
		pData = (EditPolyData *) pHitRec->modContext->localData;
		if (!pData->GetNewSelection()) continue;

		int numSet = pData->GetNewSelection ()->NumberSet();
		// Translate the hits into the correct selection level:
		if (selByVert && (selLevel>EPM_SL_VERTEX)) {
			pData->TranslateNewSelection (EPM_SL_VERTEX, selLevel);
		} else {
			if (selLevel == EPM_SL_BORDER) pData->TranslateNewSelection (EPM_SL_EDGE, EPM_SL_BORDER);
			if (selLevel == EPM_SL_ELEMENT) pData->TranslateNewSelection (EPM_SL_FACE, EPM_SL_ELEMENT);
		}

		// Spit out the quickest script possible for this action.
		if (numContexts>1) {
			if (invert) {
				// Node and Invert values are non-default
				macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
					2, 2, mr_name, levelName,
					mr_bitarray, pData->GetNewSelection(),
					_T("invert"), mr_bool, true,
					_T("node"), mr_reftarg, pHitRec->nodeRef);
			} else {
				if (selected) {
					// Node value is non-default
					macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
						2, 1, mr_name, levelName,
						mr_bitarray, pData->GetNewSelection(),
						_T("node"), mr_reftarg, pHitRec->nodeRef);
				} else {
					// Node and Selected values are non-default
					macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
						2, 2, mr_name, levelName,
						mr_bitarray, pData->GetNewSelection(),
						_T("select"), mr_bool, false,
						_T("node"), mr_reftarg, pHitRec->nodeRef);
				}
			}
		} else {
			if (invert) {
				// Invert value is non-default
				macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
					2, 1, mr_name, levelName,
					mr_bitarray, pData->GetNewSelection(),
					_T("invert"), mr_bool, true);
			} else {
				if (selected) {
					// all values are default
					macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
						2, 0,  mr_name, levelName,
						mr_bitarray, pData->GetNewSelection());
				} else {
					// Selected value is non-default
					macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].Select"),
						2, 1,  mr_name, levelName,
						mr_bitarray, pData->GetNewSelection(),
						_T("select"), mr_bool, false);
				}
			}
		}

		if (pData->ApplyNewSelection(this, true, invert?true:false, selected?true:false))
			changeOccurred = true;

		if (!all) break;
	}

	if (changeOccurred)
	{
		EpModLocalDataChanged (PART_SELECT);
		macroRecorder->EmitScript ();
	}
}

void EditPolyMod::EpModAboutToChangeSelection () {
	if (!mpCurrentOperation) return;
	if (mpParams->GetInt (epm_animation_mode)) return;
	if ((mpCurrentOperation->OpID() == ep_op_smooth) || (mpCurrentOperation->OpID() == ep_op_set_material))
	{
		TSTR name = mpCurrentOperation->Name();
		bool localHold = (theHold.Holding()==0);
		if (localHold) theHold.Begin ();
		EpModCommit (0, false, true);
		if (localHold) theHold.Accept (name);
	}
}

void EditPolyMod::ClearSelection(int selLevel) {
	if (selLevel == EPM_SL_OBJECT) return;	// shouldn't happen
	if (mSliceMode) return;
	if (!ip) return;
	if (mpParams->GetInt (epm_stack_selection)) return;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);

	int msl = meshSelLevel[selLevel];
	TCHAR *levelName = LookupMNMeshSelLevel (msl);

	bool changeOccurred = false;
	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;

		pData->SetupNewSelection (meshSelLevel[selLevel]);
		BitArray *pSel = pData->GetNewSelection();
		pSel->ClearAll ();

		if (list.Count() == 1)
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 0, mr_name, levelName, mr_bitarray, pSel);
			}
		else
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 1, mr_name, levelName, mr_bitarray, pSel,
				_T("node"), mr_reftarg, nodes[i]);
		}

		if (pData->ApplyNewSelection(this)) changeOccurred = true;
	}
	nodes.DisposeTemporary();
	if (changeOccurred) EpModLocalDataChanged (PART_SELECT);
}

void EditPolyMod::SelectAll(int selLevel) {
	if (!ip) return;
	if (mSliceMode) return;
	if (mpParams->GetInt (epm_stack_selection)) return;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);

	int msl = meshSelLevel[selLevel];
	TCHAR *levelName = LookupMNMeshSelLevel (msl);

	bool changeOccurred = false;
	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;

		pData->SetupNewSelection (msl);
		BitArray *pSel = pData->GetNewSelection();
		pSel->SetAll ();

		if (list.Count() == 1)
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 0, mr_name, levelName, mr_bitarray, pSel);
			}
		else
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 1, mr_name, levelName, mr_bitarray, pSel,
				_T("node"), mr_reftarg, nodes[i]);
		}

		if (pData->ApplyNewSelection(this)) changeOccurred = true;
	}
	nodes.DisposeTemporary();
	if (changeOccurred) EpModLocalDataChanged (PART_SELECT);
}

void EditPolyMod::InvertSelection(int selLevel) {
	if (!ip) return;
	if (mSliceMode) return;
	if (mpParams->GetInt (epm_stack_selection)) return;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);

	int msl = meshSelLevel[selLevel];
	TCHAR *levelName = LookupMNMeshSelLevel (msl);

	bool changeOccurred = false;
	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;

		pData->SetupNewSelection (msl);
		BitArray *pSel = pData->GetNewSelection();
		BitArray *pCurrent = pData->GetCurrentSelection (msl, false);
		for (int i=0; i<pCurrent->GetSize(); i++)
		{
			pSel->Set(i, !(*pCurrent)[i]);
		}

		if (list.Count() == 1)
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 0, mr_name, levelName, mr_bitarray, pSel);
			}
		else
		{
			macroRecorder->FunctionCall(_T("$.modifiers[#Edit_Poly].SetSelection"),
				2, 1, mr_name, levelName, mr_bitarray, pSel,
				_T("node"), mr_reftarg, nodes[i]);
		}

		if (pData->ApplyNewSelection(this, false)) changeOccurred = true;
	}
	nodes.DisposeTemporary();
	if (changeOccurred) EpModLocalDataChanged (PART_SELECT);
}

BitArray *EditPolyMod::EpModGetSelection (int msl, INode *pNode) {

	if (!ip) return NULL;

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);
	nodes.DisposeTemporary ();

	int i;
	if (pNode) {
		for (i=0; i<list.Count(); i++) if (nodes[i] == pNode) break;
	} else i=0;
	if (i==list.Count()) return NULL;

	EditPolyData *pData = (EditPolyData*)list[i]->localData;
	if (!pData) return NULL;
	if (msl == MNM_SL_CURRENT) msl = meshSelLevel[selLevel];
	bool useStackSel = (mpParams->GetInt (epm_stack_selection) != 0);

	return pData->GetCurrentSelection (msl, useStackSel);
}

bool EditPolyMod::EpModSetSelection (int msl, BitArray & selection, INode *pNode) {
	if (!ip) return false;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);

	int i;
	if (pNode) {
		for (i=0; i<list.Count(); i++) if (nodes[i] == pNode) break;
	} else i=0;
	if (i==list.Count()) return false;

	EditPolyData *pData = (EditPolyData*)list[i]->localData;
	if (!pData) return false;

	if (theHold.Holding())
		theHold.Put(new EditPolySelectRestore(this, pData, msl));

	if (msl == MNM_SL_CURRENT) msl = meshSelLevel[selLevel];
	switch (msl)
	{
	case MNM_SL_VERTEX:
		pData->SetVertSel (selection, this, TimeValue(0));
		break;
	case MNM_SL_EDGE:
		pData->SetEdgeSel (selection, this, TimeValue(0));
		break;
	case MNM_SL_FACE:
		pData->SetFaceSel (selection, this, TimeValue(0));
		break;
	}

	EpModLocalDataChanged (PART_SELECT);
	return true;
}

bool EditPolyMod::EpModSelect (int msl, BitArray & selection, bool invert, bool select, INode *pNode) {
	if (!ip) return false;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);

	int i;
	if (pNode) {
		for (i=0; i<list.Count(); i++) if (nodes[i] == pNode) break;
	} else i=0;
	if (i==list.Count()) return false;

	EditPolyData *pData = (EditPolyData*)list[i]->localData;
	if (!pData) return false;

	if (theHold.Holding())
		theHold.Put(new EditPolySelectRestore(this, pData, msl));

	if (msl == MNM_SL_CURRENT) msl = meshSelLevel[selLevel];
	pData->SetupNewSelection (msl);
	BitArray *pSel = pData->GetNewSelection();
	int commonLength = selection.GetSize();
	if (pSel->GetSize()<commonLength) commonLength = pSel->GetSize();
	for (i=0; i<commonLength; i++) pSel->Set(i, selection[i]);
	pData->ApplyNewSelection (this, true, invert, select);

	EpModLocalDataChanged (PART_SELECT);
	return true;
}

// From IMeshSelect
DWORD EditPolyMod::GetSelLevel() {
	switch (selLevel) {
	case EPM_SL_OBJECT: return IMESHSEL_OBJECT;
	case EPM_SL_VERTEX: return IMESHSEL_VERTEX;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		return IMESHSEL_EDGE;
	}
	return IMESHSEL_FACE;
}

void EditPolyMod::SetSelLevel(DWORD level) {
	// This line protects against changing from border to edge, for instance, when told to switch to edge:
	if (GetSelLevel() == level) return;

	switch (level) {
	case IMESHSEL_OBJECT:
		selLevel = EPM_SL_OBJECT;
		break;
	case IMESHSEL_VERTEX:
		selLevel = EPM_SL_VERTEX;
		break;
	case IMESHSEL_EDGE:
		selLevel = EPM_SL_EDGE;
		break;
	case IMESHSEL_FACE:
		selLevel = EPM_SL_FACE;
		break;
	}
	if (ip) ip->SetSubObjectLevel(selLevel);
}

BaseInterface *EditPolyMod::GetInterface (Interface_ID id) {
	if (id == EPOLY_MOD_INTERFACE) return (EPolyMod *)this;
	return FPMixinInterface::GetInterface(id);
}

void EditPolyMod::LocalDataChanged() {
	EpModLocalDataChanged (PART_SELECT);
}

void EditPolyMod::UpdateCache(TimeValue t) {
	NotifyDependents(Interval(t,t), PART_GEOM|SELECT_CHANNEL|PART_SUBSEL_TYPE|
		PART_DISPLAY|PART_TOPO, REFMSG_MOD_EVAL);
	mUpdateCachePosted = FALSE;
}

bool EditPolyMod::IsVertexSelected (EditPolyData *pData, int vertexID) {
	int stackSel;
	mpParams->GetValue (epm_stack_selection, 0, stackSel, FOREVER);
	if (!stackSel) {
		return pData->mVertSel[vertexID] ? true : false;
	} else {
		return pData->GetMesh() ? pData->GetMesh()->v[vertexID].GetFlag (MN_SEL) : false;
	}
}

bool EditPolyMod::IsEdgeSelected (EditPolyData *pData, int edgeID) {
	int stackSel;
	mpParams->GetValue (epm_stack_selection, 0, stackSel, FOREVER);
	if (!stackSel) {
		return pData->mEdgeSel[edgeID] ? true : false;
	} else {
		return pData->GetMesh() ? pData->GetMesh()->e[edgeID].GetFlag (MN_SEL) : false;
	}
}

bool EditPolyMod::IsFaceSelected (EditPolyData *pData, int faceID) {
	int stackSel;
	mpParams->GetValue (epm_stack_selection, 0, stackSel, FOREVER);
	if (!stackSel) {
		return pData->mFaceSel[faceID] ? true : false;
	} else {
		return pData->GetMesh() ? pData->GetMesh()->f[faceID].GetFlag (MN_SEL) : false;
	}
}

void EditPolyMod::GetVertexTempSelection (EditPolyData *pData, BitArray & vertexTempSel) {
	int stackSel;
	mpParams->GetValue (epm_stack_selection, 0, stackSel, FOREVER);
	if (!stackSel) {
		if (pData->GetMesh()) vertexTempSel.SetSize (pData->GetMesh()->numv);
		else vertexTempSel.SetSize (pData->mVertSel.GetSize());
		int i, max;
		MNMesh *pMesh;
		switch (meshSelLevel[selLevel]) {
		case MNM_SL_OBJECT:
			vertexTempSel.SetAll();
			break;
		case MNM_SL_VERTEX:
			vertexTempSel = pData->mVertSel;
			break;
		case MNM_SL_EDGE:
			vertexTempSel.ClearAll();
			pMesh = pData->GetMesh();
			if (!pMesh) break;
			max = pMesh->nume;
			if (pData->mEdgeSel.GetSize()<max) max = pData->mEdgeSel.GetSize();
			for (i=0; i<max; i++) {
				if (pMesh->e[i].GetFlag (MN_DEAD)) continue;
				if (!pData->mEdgeSel[i]) continue;
				vertexTempSel.Set (pMesh->e[i][0]);
				vertexTempSel.Set (pMesh->e[i][1]);
			}
			break;
		case MNM_SL_FACE:
			vertexTempSel.ClearAll();
			pMesh = pData->GetMesh();
			if (!pMesh) break;
			max = pMesh->numf;
			if (pData->mFaceSel.GetSize()<max) max = pData->mFaceSel.GetSize();
			for (i=0; i<max; i++) {
				if (pMesh->f[i].GetFlag (MN_DEAD)) continue;
				if (!pData->mFaceSel[i]) continue;
				for (int j=0; j<pMesh->f[i].deg; j++) {
					vertexTempSel.Set (pMesh->f[i].vtx[j]);
				}
			}
			break;
		}
	} else {
		if (pData->GetMesh()) vertexTempSel = pData->GetMesh()->VertexTempSel ();
		else {
			vertexTempSel.SetSize (pData->mVertSel.GetSize());
			vertexTempSel.ClearAll ();
		}
	}
}

int EditPolyMod::EpModConvertSelection (int epSelLevelFrom, int epSelLevelTo, bool requireAll) {
	if (!ip) return 0;

	if (mpParams->GetInt (epm_stack_selection)) return 0;

	if (epSelLevelFrom == EPM_SL_CURRENT) epSelLevelFrom = selLevel;
	if (epSelLevelTo == EPM_SL_CURRENT) epSelLevelTo = selLevel;
	if (epSelLevelFrom == epSelLevelTo) return 0;

	// We don't select the whole object here.
	if (epSelLevelTo == EPM_SL_OBJECT) return 0;

	EpModAboutToChangeSelection ();

	ModContextList list;
	INodeTab nodes;	
	ip->GetModContexts(list,nodes);

	int numComponentsSelected = 0;
	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;
		MNMesh *pMesh = pData->GetMesh ();
		if (!pMesh) continue;
		DbgAssert (pMesh->GetFlag (MN_MESH_FILLED_IN));
		if (!pMesh->GetFlag (MN_MESH_FILLED_IN)) continue;

		// If 'from' is the whole object, we select all.
		BitArray sel;
		int i;
		if (epSelLevelFrom == EPM_SL_OBJECT) {
			switch (epSelLevelTo) {
			case EPM_SL_VERTEX: 
				sel.SetSize (pMesh->numv);
				sel.SetAll();
				pData->SetVertSel (sel, this, ip->GetTime());
				break;
			case EPM_SL_EDGE:
				sel.SetSize (pMesh->nume);
				sel.SetAll();
				pData->SetEdgeSel (sel, this, ip->GetTime());
				break;
			case EPM_SL_BORDER:
				sel.SetSize (pMesh->nume);
				for (i=0; i<pMesh->nume; i++) sel.Set (i, pMesh->e[i].f2<0);
				pData->SetEdgeSel (sel, this, ip->GetTime());
				break;
			default:
				sel.SetSize (pMesh->numf);
				sel.SetAll(); 
				pData->SetFaceSel (sel, this, ip->GetTime());
				break;
			}
			numComponentsSelected += sel.NumberSet();
		} else {
			int mslFrom = meshSelLevel[epSelLevelFrom];
			int mslTo = meshSelLevel[epSelLevelTo];

			if (mslTo != mslFrom) {
				switch (mslFrom)
				{
				case MNM_SL_VERTEX:
					for (i=0; i<pData->mVertSel.GetSize(); i++)
						pMesh->v[i].SetFlag (MN_EDITPOLY_OP_SELECT, pData->mVertSel[i]!=0);
					break;
				case MNM_SL_EDGE:
					for (i=0; i<pData->mEdgeSel.GetSize(); i++)
						pMesh->e[i].SetFlag (MN_EDITPOLY_OP_SELECT, pData->mEdgeSel[i]!=0);
					break;
				case MNM_SL_FACE:
					for (i=0; i<pData->mFaceSel.GetSize(); i++)
						pMesh->f[i].SetFlag (MN_EDITPOLY_OP_SELECT, pData->mFaceSel[i]!=0);
					break;
				}

				switch (mslTo) {
				case MNM_SL_VERTEX:
					pMesh->ClearVFlags (MN_EDITPOLY_OP_SELECT);
					break;
				case MNM_SL_EDGE:
					pMesh->ClearEFlags (MN_EDITPOLY_OP_SELECT);
					break;
				case MNM_SL_FACE:
					pMesh->ClearFFlags (MN_EDITPOLY_OP_SELECT);
					break;
				}

				// Then propegate flags using the convenient MNMesh method:
				pMesh->PropegateComponentFlags (mslTo, MN_EDITPOLY_OP_SELECT, mslFrom, MN_EDITPOLY_OP_SELECT, requireAll);

				switch (epSelLevelTo) {
				case EPM_SL_VERTEX:
					pMesh->getVerticesByFlag (sel, MN_EDITPOLY_OP_SELECT);
					pData->SetVertSel (sel, this, ip->GetTime());
					break;

				case EPM_SL_EDGE:
					pMesh->getEdgesByFlag (sel, MN_EDITPOLY_OP_SELECT);
					pData->SetEdgeSel (sel, this, ip->GetTime());
					break;

				case EPM_SL_FACE:
					pMesh->getFacesByFlag (sel, MN_EDITPOLY_OP_SELECT);
					pData->SetFaceSel (sel, this, ip->GetTime());
					break;

				case EPM_SL_BORDER:
					sel.SetSize(pMesh->nume);
					sel.ClearAll();

					if (requireAll) {
						// If we require all, then we just need to deselect any borders that have any unselected edges.
						for (i=0; i<sel.GetSize(); i++) {
							if (pMesh->e[i].GetFlag (MN_EDITPOLY_OP_SELECT)) continue;
							if (sel[i]) continue;
							if (pMesh->e[i].f2 > -1) continue;
							pMesh->BorderFromEdge (i, sel);
						}
						// sel now contains all the edges in borders with any unselected edges.
						// Deselect "sel".
						for (i=0; i<pMesh->nume; i++) {
							sel.Set (i, !sel[i] && (pMesh->e[i].f2<0) && pMesh->e[i].GetFlag (MN_EDITPOLY_OP_SELECT));
						}
						pData->SetEdgeSel (sel, this, ip->GetTime());
					} else {
						for (i=0; i<sel.GetSize(); i++) {
							if (!pMesh->e[i].GetFlag (MN_EDITPOLY_OP_SELECT)) continue;
							if (sel[i]) continue;
							if (pMesh->e[i].f2 > -1) continue;
							pMesh->BorderFromEdge (i, sel);
						}
						pData->SetEdgeSel (sel, this, ip->GetTime());
					}
					break;

				case EPM_SL_ELEMENT:
					sel.SetSize(pMesh->numf);
					sel.ClearAll();

					if (requireAll) {
						// If we require all, then we just need to deselect any elements that have any unselected faces.
						for (int i=0; i<pMesh->numf; i++) {
							if (pMesh->f[i].GetFlag (MN_EDITPOLY_OP_SELECT)) continue;
							if (sel[i]) continue;
							pMesh->ElementFromFace (i, sel);
						}
						// sel now contains all the faces in elements with any unselected faces.
						// Deselect "sel".
						for (i=0; i<pMesh->numf; i++) {
							sel.Set (i, !sel[i] && pMesh->f[i].GetFlag (MN_EDITPOLY_OP_SELECT));
						}
						pData->SetFaceSel (sel, this, ip->GetTime());
					} else {
						for (int i=0; i<pMesh->numf; i++) {
							if (!pMesh->f[i].GetFlag (MN_EDITPOLY_OP_SELECT)) continue;
							if (sel[i]) continue;
							pMesh->ElementFromFace (i, sel);
						}
						pData->SetFaceSel (sel, this, ip->GetTime());
					}
					break;
				}
			} else {
				// We do nothing for vertex, edge, or face levels.

				switch (epSelLevelTo) {
				case EPM_SL_BORDER:
					sel.SetSize(pMesh->nume);
					sel.ClearAll();

					if (requireAll) {
						// If we require all, then we just need to deselect any borders that have any unselected edges.
						BitArray & curSel = pData->mEdgeSel;
						for (i=0; i<sel.GetSize(); i++) {
							if (curSel[i]) continue;
							if (sel[i]) continue;
							if (pMesh->e[i].f2 > -1) continue;
							pMesh->BorderFromEdge (i, sel);
						}
						// sel now contains all the edges in borders with any unselected edges.
						// Deselect "sel".
						for (i=0; i<sel.GetSize(); i++)
							sel.Set (i, !sel[i] && curSel[i] && (pMesh->e[i].f2<0));
						pData->SetEdgeSel (sel, this, ip->GetTime());
					} else {
						BitArray & curSel = pData->mEdgeSel;
						for (i=0; i<sel.GetSize(); i++) {
							if (!curSel[i]) continue;
							if (sel[i]) continue;
							if (pMesh->e[i].f2 > -1) continue;
							pMesh->BorderFromEdge (i, sel);
						}
						pData->SetEdgeSel (sel, this, ip->GetTime());
					}
					break;

				case EPM_SL_ELEMENT:
					sel.SetSize(pMesh->numf);
					sel.ClearAll();

					if (requireAll) {
						// If we require all, then we just need to deselect any elements that have any unselected faces.
						BitArray & curSel = pData->mFaceSel;
						for (int i=0; i<pMesh->numf; i++) {
							if (curSel[i]) continue;
							if (sel[i]) continue;
							pMesh->ElementFromFace (i, sel);
						}
						// sel now contains all the faces in elements with any unselected faces.
						// Deselect "sel".
						sel = (~sel) & curSel;
						pData->SetFaceSel (sel, this, ip->GetTime());
					} else {
						BitArray & curSel = pData->mFaceSel;
						for (int i=0; i<pMesh->numf; i++) {
							if (!curSel[i]) continue;
							if (sel[i]) continue;
							pMesh->ElementFromFace (i, sel);
						}
						pData->SetFaceSel (sel, this, ip->GetTime());
					}
					break;
				}
			}
			numComponentsSelected += sel.NumberSet();
		}
	}

	if (requireAll) {
		macroRecorder->FunctionCall (_T("$.modifiers[#Edit_Poly].ConvertSelection"), 2, 1,
			mr_name, LookupEditPolySelLevel (epSelLevelFrom),
			mr_name, LookupEditPolySelLevel (epSelLevelTo),
			_T("requireAll"), mr_bool, true);
	} else {
		macroRecorder->FunctionCall (_T("$.modifiers[#Edit_Poly].ConvertSelection"), 2, 0,
			mr_name, LookupEditPolySelLevel (epSelLevelFrom),
			mr_name, LookupEditPolySelLevel (epSelLevelTo));
	}
	macroRecorder->EmitScript ();

	EpModLocalDataChanged (PART_SELECT);
	return numComponentsSelected;
}

void EditPolyMod::NotifyInputChanged(Interval changeInt, PartID partID, RefMessage message, ModContext *mc) {
	if (!mc->localData) return;
	if (partID == PART_SELECT) return;
	if( InPaintMode() ) EndPaintMode();
	((EditPolyData*)mc->localData)->FreeCache();
	if (!ip || (mpCurrentEditMod != this) || mUpdateCachePosted) return;

	if (!mpDialogSelect) return;
	HWND hWnd = mpDialogSelect->GetHWnd();
	if (!hWnd) return;

	TimeValue t = ip->GetTime();
	PostMessage(hWnd,WM_UPDATE_CACHE,(WPARAM)t,0);
	mUpdateCachePosted = true;
}


// IO
const kChunkModifier = 0x80;
const kChunkSelLevel = 0x100;
const kChunkPreserveMapSettings = 0x110;
const kChunkMeshPaintHandler = 0x120;
const kChunkPointRange = 0x130;
const kChunkPointRangeNum = 0x138;
const kChunkPointNodeName = 0x140;
const kChunkPointNodeNameNum = 0x148;

IOResult EditPolyMod::Save(ISave *isave) {
	IOResult res;
	ULONG nb;

	isave->BeginChunk (kChunkModifier);
	Modifier::Save(isave);
	isave->EndChunk ();

	isave->BeginChunk(kChunkSelLevel);
	res = isave->Write(&selLevel, sizeof(selLevel), &nb);
	isave->EndChunk();

	isave->BeginChunk (kChunkPreserveMapSettings);
	mPreserveMapSettings.Save (isave);
	isave->EndChunk();

	isave->BeginChunk(kChunkMeshPaintHandler);
	MeshPaintHandler::Save(isave);
	isave->EndChunk();

	int num = mPointRange.Count();
	if (num) {
		isave->BeginChunk (kChunkPointRangeNum);
		isave->Write (&num, sizeof(int), &nb);
		isave->EndChunk ();

		isave->BeginChunk (kChunkPointRange);
		isave->Write (mPointRange.Addr(0), sizeof(int)*num, &nb);
		isave->EndChunk ();
	}

	num = mPointNode.Count();
	if (num) {
		isave->BeginChunk (kChunkPointNodeNameNum);
		isave->Write (&num, sizeof(int), &nb);
		isave->EndChunk ();

		for (int i=0; i<num; i++) {
			isave->BeginChunk (kChunkPointNodeName);
			//isave->Write (&i, sizeof(int), &nb);
			isave->WriteWString (mPointNode[i]);
			isave->EndChunk ();
		}
	}

	return IO_OK;
}

IOResult EditPolyMod::Load(ILoad *iload) {
	IOResult res;
	ULONG nb;
	int i, num;
	TCHAR *readString;
	TSTR empty;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case kChunkModifier:
			res = Modifier::Load(iload);
			break;
		case kChunkSelLevel:
			res = iload->Read(&selLevel, sizeof(selLevel), &nb);
			break;
		case kChunkPreserveMapSettings:
			res = mPreserveMapSettings.Load (iload);
			break;
		case kChunkMeshPaintHandler:
			res = MeshPaintHandler::Load (iload);
			break;

		case kChunkPointRangeNum:
			res = iload->Read (&num, sizeof(int), &nb);
			if (res != IO_OK) break;
			mPointRange.SetCount (num);
			break;

		case kChunkPointRange:
			res = iload->Read (mPointRange.Addr(0), sizeof(int)*mPointRange.Count(), &nb);
			if (res != IO_OK) break;
			AllocPointControllers (mPointRange[mPointRange.Count()-1]);
			break;

		case kChunkPointNodeNameNum:
			res = iload->Read (&num, sizeof(int), &nb);
			if (res != IO_OK) break;
			mPointNode.ZeroCount();
			for (i=0; i<num; i++) mPointNode.Append (1, &empty);
			i=0;
			break;

		case kChunkPointNodeName:
			res =iload->ReadWStringChunk (&readString);
			if (res != IO_OK) break;
			mPointNode[i] = TSTR(readString);
			i++;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}

const USHORT kChunkVertexSelection = 0x200;
const USHORT kChunkEdgeSelection = 0x210;
const USHORT kChunkFaceSelection = 0x220;
const USHORT kChunkVertexHide = 0x224;
const USHORT kChunkFaceHide = 0x228;
const USHORT kChunkOperationID = 0x230;
const USHORT kChunkOperation = 0x234;
const USHORT kChunkOperationData = 0x238;
const USHORT kChunkMeshPaintHost = 0x240;
const USHORT kChunkCurrentOperationID = 0x260;
const USHORT kChunkCurrentOperationData = 0x268;

IOResult EditPolyMod::SaveLocalData(ISave *isave, LocalModData *ld) {
	ULONG nb;
	EditPolyData *pData = (EditPolyData*)ld;

	isave->BeginChunk(kChunkVertexSelection);
	pData->mVertSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(kChunkEdgeSelection);
	pData->mEdgeSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk(kChunkFaceSelection);
	pData->mFaceSel.Save(isave);
	isave->EndChunk();

	isave->BeginChunk (kChunkVertexHide);
	pData->mVertHide.Save (isave);
	isave->EndChunk ();

	isave->BeginChunk (kChunkFaceHide);
	pData->mFaceHide.Save (isave);
	isave->EndChunk ();

	isave->BeginChunk (kChunkMeshPaintHost);
	((MeshPaintHost *)pData)->Save (isave);	// calls MeshPaintHost implementation.
	isave->EndChunk ();

	// Save the operations - in order!  (Important for reconstructing later.)
	for (PolyOperationRecord *pop = pData->mpOpList; pop != NULL; pop=pop->Next())
	{
		int id = pop->Operation()->OpID ();
		isave->BeginChunk (kChunkOperationID);
		isave->Write (&id, sizeof(int), &nb);
		isave->EndChunk ();

		isave->BeginChunk (kChunkOperation);
		pop->Operation()->Save (isave);
		isave->EndChunk ();

		if (pop->LocalData())
		{
			isave->BeginChunk (kChunkOperationData);
			pop->LocalData()->Save (isave);
			isave->EndChunk ();
		}
	}

	LocalPolyOpData *currentOpData = pData->GetPolyOpData();
	if (currentOpData) {
		isave->BeginChunk (kChunkCurrentOperationID);
		int id = currentOpData->OpID();
		isave->Write (&id, sizeof(int), &nb);
		isave->EndChunk ();

		isave->BeginChunk (kChunkCurrentOperationData);
		currentOpData->Save (isave);
		isave->EndChunk ();
	}

	return IO_OK;
}

IOResult EditPolyMod::LoadLocalData(ILoad *iload, LocalModData **pld) {
	EditPolyData *pData = new EditPolyData;
	*pld = pData;
	IOResult res;
	int id;
	ULONG nb;
	PolyOperation *currentOp=NULL;
	PolyOperationRecord *currentRecord=NULL, *lastRecord=NULL;
	LocalPolyOpData *currentData=NULL;

	while (IO_OK==(res=iload->OpenChunk())) {
		switch(iload->CurChunkID())  {
		case kChunkVertexSelection:
			res = pData->mVertSel.Load(iload);
			break;
		case kChunkEdgeSelection:
			res = pData->mEdgeSel.Load(iload);
			break;
		case kChunkFaceSelection:
			res = pData->mFaceSel.Load(iload);
			break;
		case kChunkVertexHide:
			res = pData->mVertHide.Load (iload);
			break;

		case kChunkFaceHide:
			res = pData->mFaceHide.Load (iload);
			break;

		case kChunkMeshPaintHost:
			res = ((MeshPaintHost *)pData)->Load (iload);	// calls MeshPaintHost implementation.
			break;
		case kChunkOperationID:
			res = iload->Read (&id, sizeof(int), &nb);
			currentOp = GetPolyOperationByID(id)->Clone ();
			currentRecord = new PolyOperationRecord (currentOp);
			break;
		case kChunkOperation:
			res = currentOp->Load (iload);
			if (res != IO_OK) break;
			if (lastRecord == NULL) pData->mpOpList = currentRecord;
			else lastRecord->SetNext (currentRecord);
			lastRecord = currentRecord;
			break;
		case kChunkOperationData:
			// id should still be current...
			currentData = pData->CreateLocalPolyOpData(id);
			if (!currentData) break;
			res = currentData->Load (iload);
			if (res == IO_OK) currentRecord->SetLocalData (currentData);
			else currentData->DeleteThis ();
			currentData = NULL;
			break;

		case kChunkCurrentOperationID:
			res = iload->Read (&id, sizeof(int), &nb);
			break;

		case kChunkCurrentOperationData:
			// id should still be current...
			pData->SetPolyOpData (id);
			if (!pData->GetPolyOpData()) break;
			res = pData->GetPolyOpData()->Load (iload);
			if (res != IO_OK) pData->ClearPolyOpData();
			break;
		}
		iload->CloseChunk();
		if (res!=IO_OK) return res;
	}
	return IO_OK;
}

void EditPolyMod::InvalidateDistanceCache () {
	if (ip==NULL) return;

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);

	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;
		pData->InvalidateDistances ();
	}
}

void EditPolyMod::InvalidateSoftSelectionCache () {
	if (ip==NULL) return;

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts(list,nodes);

	for (int i=0; i<list.Count(); i++) {
		EditPolyData *pData = (EditPolyData*)list[i]->localData;
		if (!pData) continue;
		pData->InvalidateSoftSelection ();
	}
	int softsel = mpParams->GetInt (epm_ss_use, TimeValue(0));
	if (softsel) NotifyDependents (FOREVER, PART_SELECT, REFMSG_CHANGE);
}

class RescalerEnumProc : public ModContextEnumProc {
	float mScale;
public:
	void SetScale (float scale) { mScale=scale; }
	BOOL proc(ModContext *mc);
};

BOOL RescalerEnumProc::proc (ModContext *mc) {
	EditPolyData *pData = (EditPolyData*)mc->localData;
	if (!pData) return true;
	pData->RescaleWorldUnits (mScale);
	pData->FreeCache ();
	return true;
}

static RescalerEnumProc theRescalerEnumProc;

void EditPolyMod::RescaleWorldUnits (float f) {
	theRescalerEnumProc.SetScale (f);
	EnumModContexts (&theRescalerEnumProc);
	EpModLocalDataChanged (PART_ALL);	// Could affect everything - whether welds occur, etc.
}

void EditPolyMod::ResetOperationParams () {
	// This method clears all animations and temporary parameters.

	if (mSliceMode) {
		slicePreviewMode = false;
		ExitSliceMode ();
	}

	// Clear the animations on all the operation parameters.
	for (int i=0; i<mpParams->NumParams (); i++)
	{
		if (mpParams->GetController (i) == NULL) continue;
		mpParams->RemoveController (i,0);
	}

	// Clear the Extrude Along Spline node:
	if (mpParams->GetINode (epm_extrude_spline_node)) {
		INode *nullNode = NULL;
		mpParams->SetValue (epm_extrude_spline_node, ip->GetTime(), nullNode);
	}

	// And these parameters should be reset to their default values:
	mpParams->Reset (epm_bridge_target_1);
	mpParams->Reset (epm_bridge_target_2);
	mpParams->Reset (epm_bridge_twist_1);
	mpParams->Reset (epm_bridge_twist_2);
	mpParams->Reset (epm_bridge_selected);

	if (mpSliceControl->IsAnimated()) {
		Matrix3 currentSlicePlane = EpGetSlicePlaneTM (ip->GetTime());
		ReplaceReference (EDIT_SLICEPLANE_REF, NewDefaultMatrix3Controller(), true);
		Point3 N = currentSlicePlane.GetRow (2);
		Point3 p = currentSlicePlane.GetTrans();

		SuspendAnimate();
		AnimateOff();
		SuspendSetKeyMode();

		EpSetSlicePlane (N, p, ip->GetTime());

		ResumeSetKeyMode();
		ResumeAnimate ();
	}

	if (mpParams->GetInt (epm_smooth_group_set))
		mpParams->SetValue (epm_smooth_group_set, TimeValue(0), 0);
	if (mpParams->GetInt (epm_smooth_group_clear))
		mpParams->SetValue (epm_smooth_group_clear, TimeValue(0), 0);

	// Clear all vertex animations:
	DeletePointControllers ();
}

class RemoveLocalPolyOpDataRestore : public RestoreObj {
private:
	EditPolyData *mpData;
	LocalPolyOpData *mpPolyOpData;
public:
	RemoveLocalPolyOpDataRestore (EditPolyData *pData) : mpData(pData) { mpPolyOpData = pData->GetPolyOpData(); }
	void Restore (int isUndo) { mpData->SetPolyOpData (mpPolyOpData); }
	void Redo () { mpData->RemovePolyOpData (); }
	int Size () { return 8; }
	TSTR Description() { return TSTR(_T("RemoveLocalPolyOpData")); }
};

void EditPolyMod::EpModCancel ()
{
	if (ip == NULL) return;

	PolyOperation *pOp = GetPolyOperation ();
	if (pOp == NULL) return;
	if (pOp->OpID() == ep_op_null) return;

	theHold.Begin ();

	// Cancel out of the paint deform mode, if applicable.
	if (pOp->OpID() == ep_op_paint_deform) {
		if( InPaintMode() ) EndPaintMode();
		if (IsPaintDataActive (PAINTMODE_DEFORM)) DeactivatePaintData( PAINTMODE_DEFORM, true);
		// (false for the last argument would generate a "Revert" we don't need.)
	}

	pOp = NULL;	// must be done using this now.

	bool mre = macroRecorder->Enabled()!=0;
	if (mre) macroRecorder->Disable();
	mpParams->SetValue (epm_current_operation, TimeValue(0), ep_op_null);
	if (mre) macroRecorder->Enable ();

	ResetOperationParams ();

	// Clear all local operation data:
	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts (list, nodes);
	for (int i=0; i<list.Count(); i++) {
		if (!list[i]->localData) continue;
		EditPolyData *pData = (EditPolyData *) list[i]->localData;
		if (!pData->GetPolyOpData()) continue;

		theHold.Put (new RemoveLocalPolyOpDataRestore (pData));
		pData->RemovePolyOpData ();
	}
	nodes.DisposeTemporary ();

	theHold.Accept (GetString (IDS_EDITPOLY_CANCEL_OPERATION));

	EpModLocalDataChanged (PART_TOPO|PART_GEOM|PART_SELECT|PART_VERTCOLOR|PART_TEXMAP);

	if (macroRecorder->Enabled())
	{
		macroRecorder->FunctionCall (_T("$.modifiers[#Edit_Poly].Cancel"), 0, 0);
		macroRecorder->EmitScript();
	}
}

void EditPolyMod::EpModCommitUnlessAnimating (TimeValue t)
{
	int animationMode = mpParams->GetInt (epm_animation_mode, t);
	PolyOperation *pOp = GetPolyOperation ();
	if (pOp && !pOp->CanAnimate()) animationMode = false;
	if (animationMode) return;
	EpModCommit (t, true);
}

void EditPolyMod::EpModCommit (TimeValue t)
{
	EpModCommit (t, true);
}

void EditPolyMod::UpdateTransformData (TimeValue t, Interval & valid, EditPolyData *pData) {
	if (!pData->GetPolyOpData()) return;
	if (pData->GetPolyOpData()->OpID() != ep_op_transform) return;
	if (mPointControl.Count() == 0) return;
	LocalTransformData *pTransform = (LocalTransformData*)pData->GetPolyOpData();

	for (int j=0; j<mPointNode.Count(); j++) {
		if (mPointNode[j] == pTransform->GetNodeName()) break;
	}
	if (j==mPointNode.Count()) return;

	int limit = mPointRange[j+1] - mPointRange[j];
	if (limit>pTransform->NumOffsets()) limit = pTransform->NumOffsets();
	for (int i=0; i<limit; i++) {
		int k = i+mPointRange[j];
		if (k>=mPointControl.Count()) break;
		if (mPointControl[k] == NULL) continue;
		mPointControl[k]->GetValue (t, pTransform->OffsetPointer(i), valid);
	}
}

void EditPolyMod::EpModCommit (TimeValue t, bool macroRecord, bool clearAfter)
{
	if (ip==NULL) return;

	int editType = mpParams->GetInt (epm_current_operation, t);
	if (editType == ep_op_null) return;

	PolyOperation *pOp = GetPolyOperation ();

	if (pOp == NULL)
	{
		mpParams->SetValue (epm_current_operation, t, ep_op_null);
		return;
	}

	UpdateCache (t);

	if (pOp->OpID() != ep_op_paint_deform) {
		if( InPaintMode() ) EndPaintMode();
		if (IsPaintDataActive(PAINTMODE_DEFORM)) DeactivatePaintData (PAINTMODE_DEFORM, true);
	}

	ModContextList list;
	INodeTab nodes;
	ip->GetModContexts (list, nodes);

	for (int i=0; i<list.Count(); i++)
	{
		EditPolyData *pData = (EditPolyData *)list[i]->localData;
		if (!pData || !pData->GetMesh()) continue;

		MNMesh & mesh = *(pData->GetMesh());

		// We need an instance of our op for each local mod data:
		PolyOperation *localOp = pOp->Clone ();
		localOp->GetValues (this, t, FOREVER);	// Don't affect validity, since we're "freezing" these parameters.
		localOp->RecordSelection (mesh, this, pData);
		localOp->SetUserFlags (mesh);
		if (localOp->ModContextTM())
		{
			if (list[i]->tm) *(localOp->ModContextTM()) = Inverse(*(list[i]->tm));
			else localOp->ModContextTM()->IdentityMatrix();
		}
		localOp->GetNode (this, t, FOREVER);

		// If we're doing a transform and have point controllers,
		// update the local transform data.
		if (localOp->OpID() == ep_op_transform) UpdateTransformData (t, FOREVER, pData);

		bool ret = localOp->Apply (mesh, pData->GetPolyOpData());
		if (ret && pOp->CanDelete())
		{
			pData->CollapseDeadSelections (this, mesh);
			mesh.CollapseDeadStructs ();
		}

		if (ret && pOp->ChangesSelection())
		{
			BitArray sel;
			mesh.getVerticesByFlag (sel, MN_SEL);
			pData->SetVertSel (sel, this, t);
			mesh.getEdgesByFlag (sel, MN_SEL);
			pData->SetEdgeSel (sel, this, t);
			mesh.getFacesByFlag (sel, MN_SEL);
			pData->SetFaceSel (sel, this, t);
			mesh.getVerticesByFlag (sel, MN_HIDDEN);
			pData->SetVertHide (sel, this);
			mesh.getFacesByFlag (sel, MN_HIDDEN);
			pData->SetFaceHide (sel, this);

			if (pData->GetPaintSelCount()) {
				if (pData->GetPaintSelCount() != mesh.numv) pData->SetPaintSelCount (mesh.numv);
				float *softsel = pData->GetPaintSelValues();
				for (int j=0; j<mesh.numv; j++) if (mesh.v[j].GetFlag(MN_SEL)) softsel[j] = 1.0f;
			}
		}
		else
		{
			// Just make sure we have the right number of everything.
			pData->SynchBitArrays ();
		}

		if (theHold.Holding ()) {
			theHold.Put (new AddOperationRestoreObj (this, pData));
		} else {
			DebugPrint ("Edit Poly WARNING: Operation being committed without restore object.\n");
			DbgAssert(0);
		}
		pData->PushOperation (localOp);
	}
	nodes.DisposeTemporary();

	if (clearAfter)
	{
		// Clean up the paint deform data.
		if (pOp->OpID() == ep_op_paint_deform) {
			if( InPaintMode() ) EndPaintMode();
			if (IsPaintDataActive(PAINTMODE_DEFORM)) DeactivatePaintData (PAINTMODE_DEFORM, true);
		}

		pOp->Reset ();

		pOp = NULL;	// must be done using this now.

		bool mre = macroRecorder->Enabled()!=0;
		if (mre) macroRecorder->Disable();
		mpParams->SetValue (epm_current_operation, t, ep_op_null);
		if (mre) macroRecorder->Enable ();

		ResetOperationParams ();
	}

	EpModLocalDataChanged (PART_TOPO|PART_GEOM|PART_SELECT|PART_VERTCOLOR|PART_TEXMAP);

	if (macroRecord && macroRecorder->Enabled())
	{
		macroRecorder->FunctionCall (_T("$.modifiers[#Edit_Poly].Commit"), 0, 0);
		macroRecorder->EmitScript();
	}
}

void EditPolyMod::EpModCommitAndRepeat (TimeValue t)
{
	if (ip==NULL) return;

	int editType = mpParams->GetInt (epm_current_operation);
	if (editType == ep_op_null) return;

	PolyOperation *pOp = GetPolyOperation ();

	EpModCommit (t, false, false);

	if (macroRecorder->Enabled())
	{
		macroRecorder->FunctionCall (_T("$.modifiers[#Edit_Poly].CommitAndRepeat"), 0, 0);
		macroRecorder->EmitScript();
	}
}

int EditPolyMod::GetPolyOperationID ()
{
	return mpParams->GetInt (epm_current_operation);
}

PolyOperation *EditPolyMod::GetPolyOperation ()
{
	int operation = mpParams->GetInt (epm_current_operation);
	if ((mpCurrentOperation != NULL) && (mpCurrentOperation->OpID() == operation))
		return mpCurrentOperation;

	if (mpCurrentOperation != NULL)
	{
		mpCurrentOperation->DeleteThis ();
		mpCurrentOperation = NULL;
	}

	if (operation == ep_op_null) return NULL;

	PolyOperation *staticPolyOp = GetPolyOperationByID (operation);
	if (staticPolyOp == NULL) return NULL;
	mpCurrentOperation = staticPolyOp->Clone();
	return mpCurrentOperation;
}

PolyOperation *EditPolyMod::GetPolyOperationByID (int operationId)
{
	for (int i=0; i<mOpList.Count (); i++)
		if (operationId == mOpList[i]->OpID ()) return mOpList[i];
	return NULL;
}

void EditPolyMod::EpSetLastOperation (int op) {
	ICustButton *pButton = NULL;
	HWND hGeom = GetDlgHandle (ep_geom);
	if (hGeom) pButton = GetICustButton (GetDlgItem (hGeom, IDC_REPEAT_LAST));

	switch (op) {
	case ep_op_null:
	case ep_op_unhide_vertex:
	case ep_op_unhide_face:
	case ep_op_ns_copy:
	case ep_op_ns_paste:
	case ep_op_reset_plane:
	case ep_op_remove_iso_verts:
	case ep_op_remove_iso_map_verts:
	case ep_op_cut:
	case ep_op_toggle_shaded_faces:
	case ep_op_get_stack_selection:
		// Don't modify button or mLastOperation.
		break;
	case ep_op_hide_vertex:
	case ep_op_hide_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_HIDE));
		break;
	case ep_op_hide_unsel_vertex:
	case ep_op_hide_unsel_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_HIDE_UNSELECTED));
		break;
	case ep_op_cap:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_CAP));
		break;
	case ep_op_delete_vertex:
	case ep_op_delete_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_DELETE));
		break;
	case ep_op_remove_vertex:
	case ep_op_remove_edge:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_REMOVE));
		break;
	case ep_op_detach_vertex:
	case ep_op_detach_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_DETACH));
		break;
	case ep_op_split:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SPLIT));
		break;
	case ep_op_break:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_BREAK));
		break;
	case ep_op_collapse_vertex:
	case ep_op_collapse_edge:
	case ep_op_collapse_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_COLLAPSE));
		break;
	case ep_op_weld_vertex:
	case ep_op_weld_edge:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_WELD_SEL));
		break;
	case ep_op_slice:
	case ep_op_slice_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SLICE));
		break;
	case ep_op_create_shape:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_CREATE_SHAPE_FROM_EDGES));
		break;
	case ep_op_make_planar:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_MAKE_PLANAR));
		break;
	case ep_op_meshsmooth:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_MESHSMOOTH));
		break;
	case ep_op_tessellate:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_TESSELLATE));
		break;
	case ep_op_flip_face:
	case ep_op_flip_element:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_FLIP_NORMALS));
		break;
	case ep_op_retriangulate:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_RETRIANGULATE));
		break;
	case ep_op_autosmooth:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_AUTOSMOOTH));
		break;
	case ep_op_extrude_vertex:
	case ep_op_extrude_edge:
	case ep_op_extrude_face:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_EXTRUDE));
		break;
	case ep_op_bevel:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_BEVEL));
		break;
	case ep_op_chamfer_vertex:
	case ep_op_chamfer_edge:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_CHAMFER));
		break;
	case ep_op_inset:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_INSET));
		break;
	case ep_op_outline:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_OUTLINE));
		break;
	case ep_op_hinge_from_edge:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_HINGE_FROM_EDGE));
		break;
	case ep_op_connect_edge:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_CONNECT_EDGES));
		break;
	case ep_op_connect_vertex:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_CONNECT_VERTICES));
		break;
	case ep_op_extrude_along_spline:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_EXTRUDE_ALONG_SPLINE));
		break;
	case ep_op_sel_grow:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SELECTION_GROW));
		break;
	case ep_op_sel_shrink:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SELECTION_SHRINK));
		break;
	case ep_op_select_loop:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SELECT_EDGE_LOOP));
		break;
	case ep_op_select_ring:
		mLastOperation = op;
		if (pButton) pButton->SetTooltip (true, GetString (IDS_SELECT_EDGE_RING));
		break;
	}
	if (pButton) ReleaseICustButton (pButton);
}

// For Maxscript access:
// Methods to get information about our mesh.

int EditPolyMod::EpMeshGetNumVertices (INode *node)
{
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	return mesh->numv;
}

Point3 EditPolyMod::EpMeshGetVertex (int vertIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return Point3(0,0,0);
	if ((vertIndex<0) || (vertIndex>=mesh->numv)) return Point3(0,0,0);
	return mesh->v[vertIndex].p;
}

int EditPolyMod::EpMeshGetVertexFaceCount (int vertIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((vertIndex<0) || (vertIndex>=mesh->numv) || !mesh->vfac) return -1;
	return mesh->vfac[vertIndex].Count();
}

int EditPolyMod::EpMeshGetVertexFace (int vertIndex, int whichFace, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((vertIndex<0) || (vertIndex>=mesh->numv) || !mesh->vfac) return -1;
	if ((whichFace<0) || (whichFace>=mesh->vfac[vertIndex].Count())) return -1;
	return mesh->vfac[vertIndex][whichFace];
}

int EditPolyMod::EpMeshGetVertexEdgeCount (int vertIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((vertIndex<0) || (vertIndex>=mesh->numv) || !mesh->vedg) return 0;
	return mesh->vedg[vertIndex].Count();
}

int EditPolyMod::EpMeshGetVertexEdge (int vertIndex, int whichEdge, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((vertIndex<0) || (vertIndex>=mesh->numv) || !mesh->vedg) return -1;
	if ((whichEdge<0) || (whichEdge>=mesh->vedg[vertIndex].Count())) return -1;
	return mesh->vedg[vertIndex][whichEdge];
}

int EditPolyMod::EpMeshGetNumEdges (INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	return mesh->nume;
}

int EditPolyMod::EpMeshGetEdgeVertex (int edgeIndex, int end, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((edgeIndex<0) || (edgeIndex>=mesh->nume)) return -1;
	if ((end<0) || (end>1)) return -1;
	return mesh->e[edgeIndex][end];
}

int EditPolyMod::EpMeshGetEdgeFace (int edgeIndex, int side, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((edgeIndex<0) || (edgeIndex>=mesh->nume)) return -1;
	if ((side<0) || (side>1)) return -1;
	return side ? mesh->e[edgeIndex].f2 : mesh->e[edgeIndex].f1;
}

int EditPolyMod::EpMeshGetNumFaces(INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	return mesh->numf;
}

int EditPolyMod::EpMeshGetFaceDegree (int faceIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return 0;
	return mesh->f[faceIndex].deg;
}

int EditPolyMod::EpMeshGetFaceVertex (int faceIndex, int corner, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return -1;
	if ((corner<0) || (corner>=mesh->f[faceIndex].deg)) return -1;
	return mesh->f[faceIndex].vtx[corner];
}

int EditPolyMod::EpMeshGetFaceEdge (int faceIndex, int side, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return -1;
	if ((side<0) || (side>=mesh->f[faceIndex].deg)) return -1;
	return mesh->f[faceIndex].edg[side];
}

int EditPolyMod::EpMeshGetFaceDiagonal (int faceIndex, int diagonal, int end, INode *node) {
	MNMesh *mesh = EpModGetMesh(node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return -1;
	if ((diagonal<0) || (diagonal>=mesh->f[faceIndex].deg-2)) return -1;
	if ((end<0) || (end>1)) return -1;
	return mesh->f[faceIndex].diag[diagonal*2+end];
}

int EditPolyMod::EpMeshGetFaceMaterial (int faceIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return 0;
	return mesh->f[faceIndex].material;
}

DWORD EditPolyMod::EpMeshGetFaceSmoothingGroup (int faceIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return 0;
	return mesh->f[faceIndex].smGroup;
}

int EditPolyMod::EpMeshGetNumMapChannels (INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	return mesh->numm;
}

bool EditPolyMod::EpMeshGetMapChannelActive (int mapChannel, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return false;
	if ((mapChannel <= -NUM_HIDDENMAPS) || (mapChannel >= mesh->numm)) return false;
	return mesh->M(mapChannel)->GetFlag (MN_DEAD) ? false : true;
}

int EditPolyMod::EpMeshGetNumMapVertices (int mapChannel, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if (!EpMeshGetMapChannelActive(mapChannel)) return 0;
	return mesh->M(mapChannel)->numv;
}

UVVert EditPolyMod::EpMeshGetMapVertex (int mapChannel, int vertIndex, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return UVVert(0,0,0);
	if (!EpMeshGetMapChannelActive(mapChannel)) return UVVert(0,0,0);
	if ((vertIndex < 0) || (vertIndex >= mesh->M(mapChannel)->numv)) return UVVert(0,0,0);
	return mesh->M(mapChannel)->v[vertIndex];
}

int EditPolyMod::EpMeshGetMapFaceVertex (int mapChannel, int faceIndex, int corner, INode *node) {
	MNMesh *mesh = EpModGetMesh (node);
	if (!mesh) return 0;
	if (!EpMeshGetMapChannelActive(mapChannel)) return -1;
	if ((faceIndex<0) || (faceIndex>=mesh->numf)) return -1;
	if ((corner<0) || (corner>=mesh->f[faceIndex].deg)) return -1;
	return mesh->M(mapChannel)->f[faceIndex].tv[corner];
}

//--- Named selection sets -----------------------------------------

class EditPolyAddSetRestore : public RestoreObj {
public:
	BitArray set;		
	DWORD id;
	TSTR name;
	GenericNamedSelSetList *setList;

	EditPolyAddSetRestore (GenericNamedSelSetList *sl, DWORD i, TSTR &n) : setList(sl), id(i), name(n) { }
	void Restore(int isUndo) {
		set  = *setList->GetSet (id);
		setList->RemoveSet(id);
	}
	void Redo() { setList->InsertSet (set, id, name); }
			
	TSTR Description() {return TSTR(_T("Add NS Set"));}
};

class EditPolyAddSetNameRestore : public RestoreObj {
public:		
	TSTR name;
	DWORD id;
	int index;	// location in list.

	EditPolyMod *et;
	Tab<TSTR*> *sets;
	Tab<DWORD> *ids;

	EditPolyAddSetNameRestore (EditPolyMod *e, DWORD idd, Tab<TSTR*> *s,Tab<DWORD> *i) : et(e), sets(s), ids(i), id(idd) { }
	void Restore(int isUndo);
	void Redo();				
	TSTR Description() {return TSTR(_T("Add Set Name"));}
};

void EditPolyAddSetNameRestore::Restore(int isUndo) {
	int sct = sets->Count();
	for (index=0; index<sct; index++) if ((*ids)[index] == id) break;
	if (index >= sct) return;

	name = *(*sets)[index];
	delete (*sets)[index];
	sets->Delete (index, 1);
	ids->Delete (index, 1);
	if (et->ip) et->ip->NamedSelSetListChanged();
}

void EditPolyAddSetNameRestore::Redo() {
	TSTR *nm = new TSTR(name);
	if (index < sets->Count()) {
		sets->Insert (index, 1, &nm);
		ids->Insert (index, 1, &id);
	} else {
		sets->Append (1, &nm);
		ids->Append (1, &id);
	}
	if (et->ip) et->ip->NamedSelSetListChanged();
}

class EditPolyDeleteSetRestore : public RestoreObj {
public:
	BitArray set;
	DWORD id;
	TSTR name;
	GenericNamedSelSetList *setList;

	EditPolyDeleteSetRestore(GenericNamedSelSetList *sl,DWORD i, TSTR & n) {
		setList = sl;
		id = i;
		BitArray *ptr = setList->GetSet(id);
		if (ptr) set = *ptr;
		name = n;
	}
	void Restore(int isUndo) { setList->InsertSet(set, id, name); }
	void Redo() { setList->RemoveSet(id); }
	TSTR Description() {return TSTR(_T("Delete Set"));}
};

class EditPolyDeleteSetNameRestore : public RestoreObj {
public:		
	TSTR name;
	DWORD id;
	EditPolyMod *et;
	Tab<TSTR*> *sets;
	Tab<DWORD> *ids;

	EditPolyDeleteSetNameRestore(Tab<TSTR*> *s,EditPolyMod *e,Tab<DWORD> *i, DWORD id) {
		sets = s; et = e;
		this->id = id;
		ids = i;
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			name = *(*sets)[index];
			}
		}   		
	void Restore(int isUndo) {			
		TSTR *nm = new TSTR(name);			
		//sets->Insert(index,1,&nm);
		sets->Append(1,&nm);
		ids->Append(1,&id);
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
	void Redo() {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			sets->Delete(index,1);
			ids->Delete(index,1);
			}
		//sets->Delete(index,1);
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
			
	TSTR Description() {return TSTR(_T("Delete Set Name"));}
};

class EditPolySetNameRestore : public RestoreObj {
public:
	TSTR undo, redo;
	DWORD id;
	Tab<TSTR*> *sets;
	Tab<DWORD> *ids;
	EditPolyMod *et;
	EditPolySetNameRestore(Tab<TSTR*> *s,EditPolyMod *e,Tab<DWORD> *i,DWORD id) {
		this->id = id;
		ids = i;
		sets = s; et = e;
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			undo = *(*sets)[index];
			}
		}

	void Restore(int isUndo) {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
				}
			}
		if (index>=0) {
			redo = *(*sets)[index];
			*(*sets)[index] = undo;
			}			
		if (et->ip) et->ip->NamedSelSetListChanged();
		}
	void Redo() {
		int index = -1;
		for (int j=0; j<sets->Count(); j++) {
			if ((*ids)[j]==id) {
				index = j;
				break;
			}
		}
		if (index>=0) {
			*(*sets)[index] = redo;
		}
		if (et->ip) et->ip->NamedSelSetListChanged();
	}
			
	TSTR Description() {return TSTR(_T("Set Name"));}
};

int EditPolyMod::FindSet(TSTR &setName, int nsl) {
	for (int i=0; i<namedSel[nsl].Count(); i++) {
		if (setName == *namedSel[nsl][i]) return i;
	}
	return -1;
}

DWORD EditPolyMod::AddSet(TSTR &setName,int nsl) {
	DWORD id = 0;
	TSTR *name = new TSTR(setName);
	int nsCount = namedSel[nsl].Count();

	// Find an empty id to assign to this set.
	BOOL found = FALSE;
	while (!found) {
		found = TRUE;
		for (int i=0; i<ids[nsl].Count(); i++) {
			if (ids[nsl][i]!=id) continue;
			id++;
			found = FALSE;
			break;
		}
	}

	// Find location in alphabetized list:
	for (int pos=0; pos<nsCount; pos++) if (setName < *(namedSel[nsl][pos])) break;
	if (pos == nsCount) {
		namedSel[nsl].Append(1,&name);
		ids[nsl].Append(1,&id);
	} else {
		namedSel[nsl].Insert (pos, 1, &name);
		ids[nsl].Insert (pos, 1, &id);
	}

	return id;
}

void EditPolyMod::RemoveSet(TSTR &setName,int nsl) {
	int i = FindSet(setName,nsl);
	if (i<0) return;
	delete namedSel[nsl][i];
	namedSel[nsl].Delete(i,1);
	ids[nsl].Delete(i,1);
}

void EditPolyMod::UpdateSetNames () {
	if (!ip) return;

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i=0; i<mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if ( !meshData ) continue;
		for (int nsl=0; nsl<3; nsl++) {
			// Make sure the namedSel array is in alpha order.
			// (Crude bubble sort since we expect that it will be.)
			int j, k, kmax = namedSel[nsl].Count();
			for (k=1; k<kmax; k++) {
				if (*(namedSel[nsl][k-1]) < *(namedSel[nsl][k])) continue;
				for (j=0; j<k-1; j++) {
					if (*(namedSel[nsl][j]) > *(namedSel[nsl][k])) break;
				}
				// j now represents the point at which k should be inserted.
				TSTR *hold = namedSel[nsl][k];
				DWORD dhold = ids[nsl][k];
				int j2;
				for (j2=k; j2>j; j2--) {
					namedSel[nsl][j2] = namedSel[nsl][j2-1];
					ids[nsl][j2] = ids[nsl][j2-1];
				}
				namedSel[nsl][j] = hold;
				ids[nsl][j] = dhold;
			}

			GenericNamedSelSetList & gnsl = meshData->GetNamedSel(nsl);
			// Check for old, unnamed or misnamed sets with ids.
			for (k=0; k<gnsl.Count(); k++) {
				for (j=0; j<ids[nsl].Count(); j++) if (ids[nsl][j] == gnsl.ids[k]) break;
				if (j == ids[nsl].Count()) continue;
				if (gnsl.names[k] && !(*(gnsl.names[k]) == *(namedSel[nsl][j]))) {
					delete gnsl.names[k];
					gnsl.names[k] = NULL;
				}
				if (gnsl.names[k]) continue;
				gnsl.names[k] = new TSTR(*(namedSel[nsl][j]));
			}
			gnsl.Alphabetize ();

			// Now check lists against each other, adding any missing elements.
			for (j=0; j<gnsl.Count(); j++) {
				if (*(gnsl.names[j]) == *(namedSel[nsl][j])) continue;
				if (j>= namedSel[nsl].Count()) {
					TSTR *nname = new TSTR(*gnsl.names[j]);
					DWORD nid = gnsl.ids[j];
					namedSel[nsl].Append (1, &nname);
					ids[nsl].Append (1, &nid);
					continue;
				}
				if (*(gnsl.names[j]) > *(namedSel[nsl][j])) {
					BitArray baTemp;
					gnsl.InsertSet (j, baTemp, ids[nsl][j], *(namedSel[nsl][j]));
					continue;
				}
				// Otherwise:
				TSTR *nname = new TSTR(*gnsl.names[j]);
				DWORD nid = gnsl.ids[j];
				namedSel[nsl].Insert (j, 1, &nname);
				ids[nsl].Insert (j, 1, &nid);
			}
			for (; j<namedSel[nsl].Count(); j++) {
				BitArray baTemp;
				gnsl.AppendSet (baTemp, ids[nsl][j], *(namedSel[nsl][j]));
			}
		}
	}

	nodes.DisposeTemporary();
}

void EditPolyMod::ClearSetNames() {
	for (int i=0; i<3; i++) {
		for (int j=0; j<namedSel[i].Count(); j++) {
			delete namedSel[i][j];
			namedSel[i][j] = NULL;
		}
	}
}

void EditPolyMod::ActivateSubSelSet(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	int nsl = namedSetLevel[selLevel];
	int index = FindSet (setName, nsl);	
	if (index<0 || !ip) return;

	theHold.Begin ();
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;
		if (theHold.Holding() && !meshData->GetFlag(kEPDataHeld))
			theHold.Put(new EditPolySelectRestore(this,meshData));

		BitArray *set = NULL;

		switch (nsl) {
		case NS_VERTEX:
			set = meshData->vselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->mVertSel.GetSize()) {
					set->SetSize(meshData->mVertSel.GetSize(),TRUE);
				}
				meshData->SetVertSel (*set, this, ip->GetTime());
			}
			break;

		case NS_FACE:
			set = meshData->fselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->mFaceSel.GetSize()) {
					set->SetSize(meshData->mFaceSel.GetSize(),TRUE);
				}
				meshData->SetFaceSel (*set, this, ip->GetTime());
			}
			break;

		case NS_EDGE:
			set = meshData->eselSet.GetSet(ids[nsl][index]);
			if (set) {
				if (set->GetSize()!=meshData->mEdgeSel.GetSize()) {
					set->SetSize(meshData->mEdgeSel.GetSize(),TRUE);
				}
				meshData->SetEdgeSel (*set, this, ip->GetTime());
			}
			break;
		}
	}
	
	nodes.DisposeTemporary();
	LocalDataChanged ();
	theHold.Accept (GetString (IDS_SELECT));
	ip->RedrawViews(ip->GetTime());
}

void EditPolyMod::NewSetFromCurSel(TSTR &setName) {
	ModContextList mcList;
	INodeTab nodes;
	DWORD id = -1;
	int nsl = namedSetLevel[selLevel];
	int index = FindSet(setName, nsl);
	if (index<0) {
		id = AddSet(setName, nsl);
		if (theHold.Holding()) theHold.Put (new EditPolyAddSetNameRestore (this, id, &namedSel[nsl], &ids[nsl]));
	} else id = ids[nsl][index];

	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;
		
		BitArray *set = NULL;

		switch (nsl) {
		case NS_VERTEX:	
			if (index>=0 && (set = meshData->vselSet.GetSet(id))) {
				*set = meshData->mVertSel;
			} else {
				meshData->vselSet.InsertSet (meshData->mVertSel,id, setName);
				if (theHold.Holding()) theHold.Put (new EditPolyAddSetRestore (&(meshData->vselSet), id, setName));
			}
			break;

		case NS_FACE:
			if (index>=0 && (set = meshData->fselSet.GetSet(id))) {
				*set = meshData->mFaceSel;
			} else {
				meshData->fselSet.InsertSet(meshData->mFaceSel,id, setName);
				if (theHold.Holding()) theHold.Put (new EditPolyAddSetRestore (&(meshData->fselSet), id, setName));
			}
			break;

		case NS_EDGE:
			if (index>=0 && (set = meshData->eselSet.GetSet(id))) {
				*set = meshData->mEdgeSel;
			} else {
				meshData->eselSet.InsertSet(meshData->mEdgeSel, id, setName);
				if (theHold.Holding()) theHold.Put (new EditPolyAddSetRestore (&(meshData->eselSet), id, setName));
			}
			break;
		}
	}	
	nodes.DisposeTemporary();
}

void EditPolyMod::RemoveSubSelSet(TSTR &setName) {
	int nsl = namedSetLevel[selLevel];
	int index = FindSet (setName, nsl);
	if (index<0 || !ip) return;		

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	DWORD id = ids[nsl][index];

	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;		

		switch (nsl) {
		case NS_VERTEX:	
			if (theHold.Holding()) theHold.Put(new EditPolyDeleteSetRestore(&meshData->vselSet,id, setName));
			meshData->vselSet.RemoveSet(id);
			break;

		case NS_FACE:
			if (theHold.Holding()) theHold.Put(new EditPolyDeleteSetRestore(&meshData->fselSet,id, setName));
			meshData->fselSet.RemoveSet(id);
			break;

		case NS_EDGE:
			if (theHold.Holding()) theHold.Put(new EditPolyDeleteSetRestore(&meshData->eselSet,id, setName));
			meshData->eselSet.RemoveSet(id);
			break;
		}
	}
	
	if (theHold.Holding()) theHold.Put(new EditPolyDeleteSetNameRestore(&(namedSel[nsl]),this,&(ids[nsl]),id));
	RemoveSet (setName, nsl);
	ip->ClearCurNamedSelSet();
	nodes.DisposeTemporary();
}

void EditPolyMod::SetupNamedSelDropDown() {
	if (selLevel == EPM_SL_OBJECT) return;
	ip->ClearSubObjectNamedSelSets();
	int nsl = namedSetLevel[selLevel];
	for (int i=0; i<namedSel[nsl].Count(); i++)
		ip->AppendSubObjectNamedSelSet(*namedSel[nsl][i]);
	UpdateNamedSelDropDown ();
}

int EditPolyMod::NumNamedSelSets() {
	int nsl = namedSetLevel[selLevel];
	return namedSel[nsl].Count();
}

TSTR EditPolyMod::GetNamedSelSetName(int i) {
	int nsl = namedSetLevel[selLevel];
	return *namedSel[nsl][i];
}

void EditPolyMod::SetNamedSelSetName(int i,TSTR &newName) {
	int nsl = namedSetLevel[selLevel];
	if (theHold.Holding()) theHold.Put(new EditPolySetNameRestore(&namedSel[nsl],this,&ids[nsl],ids[nsl][i]));
	*namedSel[nsl][i] = newName;
}

void EditPolyMod::NewSetByOperator(TSTR &newName,Tab<int> &sets,int op) {
	int nsl = namedSetLevel[selLevel];
	DWORD id = AddSet(newName, nsl);
	if (theHold.Holding())
		theHold.Put(new EditPolyAddSetNameRestore(this, id, &namedSel[nsl], &ids[nsl]));

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;
	
		BitArray bits;
		GenericNamedSelSetList *setList;

		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;
		case NS_FACE: setList = &meshData->fselSet; break;			
		case NS_EDGE:   setList = &meshData->eselSet; break;			
		}		

		bits = (*setList)[sets[0]];

		for (int i=1; i<sets.Count(); i++) {
			switch (op) {
			case NEWSET_MERGE:
				bits |= (*setList)[sets[i]];
				break;

			case NEWSET_INTERSECTION:
				bits &= (*setList)[sets[i]];
				break;

			case NEWSET_SUBTRACT:
				bits &= ~((*setList)[sets[i]]);
				break;
			}
		}
		setList->InsertSet (bits, id, newName);
		if (theHold.Holding()) theHold.Put(new EditPolyAddSetRestore(setList, id, newName));
	}
}

void EditPolyMod::NSCopy() {
	int index = SelectNamedSet();
	if (index<0) return;
	if (!ip) return;

	int nsl = namedSetLevel[selLevel];
	MeshNamedSelClip *clip = new MeshNamedSelClip(*namedSel[nsl][index]);

	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts(mcList,nodes);

	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;

		GenericNamedSelSetList *setList;
		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;				
		case NS_FACE: setList = &meshData->fselSet; break;			
		case NS_EDGE: setList = &meshData->eselSet; break;			
		}		

		BitArray *bits = new BitArray(*setList->sets[index]);
		clip->sets.Append(1,&bits);
	}
	SetMeshNamedSelClip(clip, namedClipLevel[selLevel]);
	
	// Enable the paste button
	HWND hWnd = GetDlgHandle (ep_geom);
	if (!hWnd) return;
	ICustButton *but;
	but = GetICustButton(GetDlgItem(hWnd,IDC_PASTE_NS));
	but->Enable();
	ReleaseICustButton(but);
}

void EditPolyMod::NSPaste() {
	int nsl = namedSetLevel[selLevel];
	MeshNamedSelClip *clip = GetMeshNamedSelClip(namedClipLevel[selLevel]);
	if (!clip) return;	
	TSTR name = clip->name;
	if (!GetUniqueSetName(name)) return;

	EpModAboutToChangeSelection ();

	ModContextList mcList;
	INodeTab nodes;
	theHold.Begin();

	DWORD id = AddSet (name, nsl);	
	theHold.Put(new EditPolyAddSetNameRestore(this, id, &namedSel[nsl], &ids[nsl]));	

	ip->GetModContexts(mcList,nodes);
	for (int i = 0; i < mcList.Count(); i++) {
		EditPolyData *meshData = (EditPolyData*)mcList[i]->localData;
		if (!meshData) continue;

		GenericNamedSelSetList *setList;
		switch (nsl) {
		case NS_VERTEX: setList = &meshData->vselSet; break;
		case NS_EDGE: setList = &meshData->eselSet; break;
		case NS_FACE: setList = &meshData->fselSet; break;
		}
				
		if (i>=clip->sets.Count()) {
			BitArray bits;
			setList->InsertSet(bits,id,name);
		} else setList->InsertSet(*clip->sets[i],id,name);
		if (theHold.Holding()) theHold.Put(new EditPolyAddSetRestore(setList, id, name));
	}	
	
	ActivateSubSelSet(name);
	ip->SetCurNamedSelSet(name);
	theHold.Accept(GetString (IDS_PASTE_NS));
	SetupNamedSelDropDown();
}

static INT_PTR CALLBACK EditPolyPickSetDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg) {
	case WM_INITDIALOG:	{
		Tab<TSTR*> *setList = (Tab<TSTR*>*)lParam;
		for (int i=0; i<setList->Count(); i++) {
			int pos  = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_ADDSTRING,0,
				(LPARAM)(TCHAR*)*(*setList)[i]);
			SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_SETITEMDATA,pos,i);
		}
		break;
	}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDC_NS_LIST:
			if (HIWORD(wParam)!=LBN_DBLCLK) break;
			// fall through

		case IDOK:
			int sel;
			sel = SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETCURSEL,0,0);
			if (sel!=LB_ERR) {
				int res =SendDlgItemMessage(hWnd,IDC_NS_LIST,LB_GETITEMDATA,sel,0);
				EndDialog(hWnd,res);
				break;
			}
			// fall through

		case IDCANCEL:
			EndDialog(hWnd,-1);
			break;
		}
		break;

	default:
		return FALSE;
	}
	return TRUE;
}

int EditPolyMod::SelectNamedSet() {
	Tab<TSTR*> &setList = namedSel[namedSetLevel[selLevel]];
	if (!ip) return false;
	return DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_NAMEDSET_SEL),
		ip->GetMAXHWnd(), EditPolyPickSetDlgProc, (LPARAM)&setList);
}

void EditPolyMod::UpdateNamedSelDropDown () {
	if (!ip) return;
	if (selLevel == EPM_SL_OBJECT) {
		ip->ClearCurNamedSelSet ();
		return;
	}
	// See if this selection matches a named set
	ModContextList mcList;
	INodeTab nodes;
	ip->GetModContexts (mcList, nodes);
	BitArray nselmatch;
	nselmatch.SetSize (namedSel[namedSetLevel[selLevel]].Count());
	nselmatch.SetAll ();
	int nd, i, foundone=FALSE;
	for (nd=0; nd<mcList.Count(); nd++) {
		EditPolyData *d = (EditPolyData *) mcList[nd]->localData;
		if (!d) continue;
		foundone = TRUE;
		switch (selLevel) {
		case EPM_SL_VERTEX:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->vselSet.sets[i]) == d->mVertSel)) nselmatch.Clear(i);
			}
			break;
		case EPM_SL_EDGE:
		case EPM_SL_BORDER:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->eselSet.sets[i]) == d->mEdgeSel)) nselmatch.Clear(i);
			}
			break;
		default:
			for (i=0; i<nselmatch.GetSize(); i++) {
				if (!nselmatch[i]) continue;
				if (!(*(d->fselSet.sets[i]) == d->mFaceSel)) nselmatch.Clear(i);
			}
			break;
		}
		if (nselmatch.NumberSet () == 0) break;
	}
	if (foundone && nselmatch.NumberSet ()) {
		for (i=0; i<nselmatch.GetSize(); i++) if (nselmatch[i]) break;
		ip->SetCurNamedSelSet (*(namedSel[namedSetLevel[selLevel]][i]));
	} else ip->ClearCurNamedSelSet ();
}

static INT_PTR CALLBACK PickSetNameDlgProc (HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	static TSTR *name;

	switch (msg) {
	case WM_INITDIALOG: {
		name = (TSTR*)lParam;
		ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
		edit->SetText(*name);
		ReleaseICustEdit(edit);
		break;
		}

	case WM_COMMAND:
		switch (LOWORD(wParam)) {
		case IDOK: {
			ICustEdit *edit =GetICustEdit(GetDlgItem(hWnd,IDC_SET_NAME));
			TCHAR buf[256];
			edit->GetText(buf,256);
			*name = TSTR(buf);
			ReleaseICustEdit(edit);
			EndDialog(hWnd,1);
			break;
			}

		case IDCANCEL:
			EndDialog(hWnd,0);
			break;
		}
		break;

	default:
		return FALSE;
	};
	return TRUE;
}

BOOL EditPolyMod::GetUniqueSetName(TSTR &name) {
	while (1) {				
		Tab<TSTR*> &setList = namedSel[namedSetLevel[selLevel]];

		BOOL unique = TRUE;
		for (int i=0; i<setList.Count(); i++) {
			if (name==*setList[i]) {
				unique = FALSE;
				break;
			}
		}
		if (unique) break;

		if (!ip) return FALSE;
		if (!DialogBoxParam (hInstance, MAKEINTRESOURCE(IDD_NAMEDSET_PASTE),
			ip->GetMAXHWnd(), PickSetNameDlgProc, (LPARAM)&name)) return FALSE;
	}
	return TRUE;
}
