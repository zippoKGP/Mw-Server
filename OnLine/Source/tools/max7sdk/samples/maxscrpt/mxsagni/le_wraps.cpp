/**********************************************************************
 *<
	FILE: le_tclasses.cpp

	DESCRIPTION: Contains the Implementation of the various types and the root class

	CREATED BY: Luis Estrada

	HISTORY:

 *>	Copyright (c) 1994, All Rights Reserved.
 **********************************************************************/

//#include "pch.h"
#include "MAXScrpt.h"
#include "Numbers.h"
#include "3DMath.h"
#include "MAXObj.h"

#include "meshadj.h"
#include "ikctrl.h"

#include "resource.h"

#ifdef ScripterExport
	#undef ScripterExport
#endif
#define ScripterExport __declspec( dllexport )

#include "MXSAgni.h"
#include "ExtClass.h"

#include "defextfn.h"
#	include "namedefs.h"

#include "definsfn.h"
#	include	"le_wraps.h"

#include "agnidefs.h"

/*--------------------------------- TestInterface-----------------------------------*/

/*--------------------------------- Interface Class ------------------------------*/

Value*
ConfigureBitmapPaths_cf(Value** arg_list, int count)
{
	
	check_arg_count (ConfigureBitmapPaths,0,count);

	return MAXScript_interface->ConfigureBitmapPaths() ? &true_value : &false_value;
}


Value*
EditAtmosphere_cf(Value** arg_list, int count)
{
	if (count == 0)
		MAXScript_interface->EditAtmosphere(NULL);
	else
	{
		check_arg_count (EditAtmosphere,2,count);
		MAXScript_interface->EditAtmosphere(arg_list[0]->to_atmospheric(), arg_list[1]->to_node());
	}

	return &ok;
}


Value*
CheckForSave_cf(Value** arg_list, int count)
{
	//bool checkforsave
	check_arg_count (CheckForSave, 0, count);

	return MAXScript_interface->CheckForSave () ? &true_value : &false_value;

}

Value*
MatrixFromNormal_cf(Value** arg_list, int count)
{
	Matrix3 result_matrix;
	check_arg_count (MatrixFromNormal, 1, count);

	Point3 the_normal= arg_list[0]->to_point3();
	
	MatrixFromNormal (the_normal, result_matrix);

	return new Matrix3Value (result_matrix);
	
}



/*-------------------------------------------------- Object Class ---------------*/

/* Parameter needed are the time and the object) */
Value*
GetPolygonCount_cf(Value** arg_list, int count)
{
	int faces=0,vertnum=0;
	one_typed_value_local (Array* total) 

	check_arg_count (GetPolygonCount, 1, count);
	INode* node = arg_list[0]->to_node();

	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	GetPolygonCount (MAXScript_time(), refobj, faces, vertnum);
	
	vl.total = new Array (2);
	vl.total->append (Integer::intern(faces));
	vl.total->append (Integer::intern(vertnum));

	return_value (vl.total);	
}



Value*
GetTriMeshFaceCount_cf(Value** arg_list, int count)
{
	int faces=0,vertnum=0;
	one_typed_value_local (Array* total) 

	check_arg_count (GetTriMeshFaceCount, 1, count);
	INode* node = arg_list[0]->to_node();

	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	GetTriMeshFaceCount (MAXScript_time(), refobj, faces, vertnum);
	
	vl.total = new Array (2);
	vl.total->append (Integer::intern(faces));
	vl.total->append (Integer::intern(vertnum));

	return_value (vl.total);	
}


Value*
NumMapsUsed_cf(Value** arg_list, int count)
{
	check_arg_count (NumMapsUsed, 1, count);
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return Integer::intern (refobj->NumMapsUsed());
}


Value*
IsPointSelected_cf(Value** arg_list, int count)
{
	int pointindex;
	check_arg_count (IsPointSelected, 2, count);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	pointindex = arg_list[1]->to_int()-1;
	if (pointindex < 0)
		throw RuntimeError(GetString(IDS_RK_ISPOINTSELECTED_INDEX_OUT_OF_RANGE));

	return refobj->IsPointSelected(pointindex) ? &true_value : &false_value;
}


Value*
PointSelection_cf(Value** arg_list, int count)
{
	int pointindex;
	check_arg_count (PointSelection, 2, count);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	pointindex = arg_list[1]->to_int()-1;
	if (pointindex < 0)
		throw RuntimeError(GetString(IDS_RK_POINTSELECTION_INDEX_OUT_OF_RANGE));
	return Float::intern(refobj->PointSelection(pointindex));
}


Value*
IsShapeObject_cf(Value** arg_list, int count)
{
	check_arg_count (IsShapeObject, 1, count);
	
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return refobj->IsShapeObject () ? &true_value : &false_value;
}

Value* 
NumSurfaces_cf(Value** arg_list, int count)
{
	check_arg_count (NumSurfaces, 1, count);
	
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;

	return Integer::intern(refobj->NumSurfaces (MAXScript_time()));

}

Value*
IsSurfaceUVClosed_cf (Value** arg_list, int count)
{
	check_arg_count (IsSurfaceUVClosed, 2, count);
	one_typed_value_local (Array* total);

	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	Object* refobj = os.obj;
	int inu;
	int inv;

	int surfnum = arg_list[1]->to_int();
	if (surfnum < 1 || surfnum > refobj->NumSurfaces(MAXScript_time())) 
		throw RuntimeError(GetString(IDS_RK_ISSURFACEUVCLOSED_INDEX_OUT_OF_RANGE));

	refobj->SurfaceClosed (MAXScript_time(), surfnum-1, inu, inv);
	
	vl.total = new Array (2);
	vl.total->append (inu ? &true_value : &false_value);
	vl.total->append (inv ? &true_value : &false_value);

	return_value (vl.total);	
}

/*------------------------------ Miscellaneous Global Functions -------------*/

Value*
DeselectHiddenEdges_cf(Value **arg_list, int count)
{
	Object *obj;

	check_arg_count (DeselectHiddenEdges, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and deselect hidden edges */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	DeselectHiddenEdges ( themesh );

	return &ok;
}
	

Value*
DeselectHiddenFaces_cf(Value **arg_list, int count)
{
	Object *obj;
	check_arg_count (DeselectHiddenFaces, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and deselect hidden edges */
	Mesh &themesh = ((TriObject*) obj)->GetMesh();
	DeselectHiddenFaces ( themesh );

	return &ok;
}


Value*
AverageSelVertCenter_cf(Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (AverageSelVertCenter, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and return the average center of the selected vertices */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	return new Point3Value(AverageSelVertCenter ( themesh ) );

}



Value*
AverageSelVertNormal_cf(Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (AverageSelVertNormal, 1, count);

	INode* node = arg_list[0]->to_node();
	
	// Get the object from the node
	ObjectState os = node->EvalWorldState(MAXScript_time());
	if (os.obj->SuperClassID()==GEOMOBJECT_CLASS_ID) {
		obj = (GeomObject*)os.obj;
		if (obj->ClassID() != GetEditTriObjDesc()->ClassID() &&
			obj->ClassID() != Class_ID(TRIOBJ_CLASS_ID, 0))		
			return &undefined;
	}
	else
		return &undefined;
	
	
	/* get the associated mesh and return the average center of the selected vertices */
	Mesh &themesh = ((TriObject *) obj)->GetMesh();
	return new Point3Value(AverageSelVertNormal ( themesh ) );

}


/* -----------------------------------   Patch Properties ------------------------------- */

#ifndef NO_PATCHES
Value* GetPatchSteps_cf(Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (GetPatchSteps, 1, count);

	// Get the object from the node
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	obj = os.obj;

	if (obj->ClassID() != Class_ID(PATCHOBJ_CLASS_ID,0)) 
		throw RuntimeError (GetString(IDS_RK_PATCH_OPERATION_ON_NONMESH), obj->GetObjectName());

		//throw RuntimeError (_T("Patch operation on non-Patch: "), obj->GetObjectName());

	return Integer::intern( ((PatchObject*)obj)->GetMeshSteps());
}


Value* SetPatchSteps_cf (Value** arg_list, int count)
{
	Object *obj;

	check_arg_count (SetPatchSteps, 2, count);

	// Get the object from the node
	INode* node = arg_list[0]->to_node();
	ObjectState os = node->EvalWorldState(MAXScript_time());
	obj = os.obj;

	if (obj->ClassID() != Class_ID(PATCHOBJ_CLASS_ID,0))
		throw RuntimeError (GetString(IDS_RK_PATCH_OPERATION_ON_NONMESH), obj->GetObjectName());
	if (arg_list[1]->to_int() < 0 ) 
		throw RuntimeError (GetString(IDS_RK_SETPATCHSTEPS_INDEX_OUT_OF_RANGE));

	((PatchObject*)obj)->SetMeshSteps (arg_list[1]->to_int());
	needs_complete_redraw_set();

	if (MAXScript_interface->GetCommandPanelTaskMode() == TASK_MODE_MODIFY)
		if ((BaseObject*)obj == MAXScript_interface->GetCurEditObject())
			obj->NotifyDependents (FOREVER, PART_ALL, REFMSG_CHANGE);
			//MAXScript_interface->SetCommandPanelTaskMode (TASK_MODE_MODIFY);

	return &ok;
}
#endif // NO_PATCHES

/* --------------------------- Euler Angles -------------------------------- */

Value* GetEulerQuatAngleRatio_cf (Value** arg_list, int count)
{
	check_arg_count_with_keys(GetEulerQuatAngleRatio, 4, count);
	Quat quat1,quat2;
	Array *euler1, *euler2;
	int angle,size1,size2;

	def_euler_angles();
	Value* theangle = key_arg(angle)->eval();
	if (theangle == &unsupplied)
		angle = EULERTYPE_XYZ;
	else
		angle = GetID(eulerAngles, elements(eulerAngles), theangle); 

	quat1 = arg_list[0]->to_quat();
	quat2 = arg_list[1]->to_quat();
	type_check(arg_list[2], Array, "QuatAngleRatio");
	type_check(arg_list[3], Array, "QuatAngleRatio");
    euler1 = (Array*)arg_list[2];
	size1 = euler1->size;
	euler2 = (Array*)arg_list[3];
	size2 = euler2->size;
	if (!size1 || !size2)
		return &undefined;

	float* eulerarray1 = new float[size1];
    float* eulerarray2 = new float[size2];

	for (int i=0;i<size1;i++)
		eulerarray1[i] = euler1->data[i]->to_float();
	
	for (i=0;i<size2;i++)
		eulerarray2[i] = euler2->data[i]->to_float();
	
	return Float::intern (GetEulerQuatAngleRatio (quat1, quat2, 
		eulerarray1, eulerarray2, angle));

}

Value* GetEulerMatAngleRatio_cf (Value** arg_list, int count)
{
	check_arg_count_with_keys(GetEulerMatAngleRatio, 4, count);
	Matrix3 mat1, mat2;
	Array *euler1, *euler2;
	int angle,size1,size2;

	def_euler_angles();
	Value* theangle = key_arg(angle)->eval(); 
	if (theangle == &unsupplied)
		angle = EULERTYPE_XYZ;
	else
		angle = GetID(eulerAngles, elements(eulerAngles), theangle); 

	mat1 = arg_list[0]->to_matrix3();
	mat2 = arg_list[1]->to_matrix3();
	type_check(arg_list[2], Array, "MatAngleRatio");
	type_check(arg_list[3], Array, "MatAngleRatio");
    euler1 = (Array*)arg_list[2];
	size1 = euler1->size;
	euler2 = (Array*)arg_list[3];
	size2 = euler2->size;
	if (!size1 || !size2)
		return &undefined;

	float* eulerarray1 = new float[size1];
    float* eulerarray2 = new float[size2];

	for (int i=0;i<size1;i++)
		eulerarray1[i] = euler1->data[i]->to_float();
	
	for (i=0;i<size2;i++)
		eulerarray2[i] = euler2->data[i]->to_float();

	return Float::intern (GetEulerMatAngleRatio (mat1, mat2,
		eulerarray1, eulerarray2, angle)); 
}


Value* GetStartTime_cf (Value** arg_list, int count)
{
	check_arg_count (GetStartTime, 1, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		return MSTime::intern(themaster->GetStartTime());
	}
	else
		return &undefined;
}


Value* SetStartTime_cf (Value** arg_list, int count)
{
	check_arg_count (SetStartTime, 2, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		themaster->SetStartTime(arg_list[1]->to_timevalue());
		needs_redraw_set();
		return (arg_list[1]);
	}
	else
		return &undefined;
}


Value* GetEndTime_cf (Value** arg_list, int count)
{
	check_arg_count (GetEndTime, 1, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		return MSTime::intern(themaster->GetEndTime());
	}
	else
		return &undefined;

}


Value* SetEndTime_cf (Value** arg_list, int count)
{
	check_arg_count (SetEndTime, 2, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		themaster->SetEndTime(arg_list[1]->to_timevalue());
		needs_redraw_set();
		return (arg_list[1]);
	}
	else
		return &undefined;
}


Value* GetIterations_cf (Value** arg_list, int count)
{
	check_arg_count (GetIterations, 1, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		return Integer::intern(themaster->GetIterations());
	}
	else
		return &undefined;
}


Value* SetIterations_cf (Value** arg_list, int count)
{
	check_arg_count (SetIterations, 2, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		themaster->SetIterations (arg_list[1]->to_int());
		needs_redraw_set();
		return (arg_list[1]);
	}
	else
		return &undefined;
}


Value* GetPosThreshold_cf (Value** arg_list, int count)
{
	check_arg_count (GetPosThreshold, 1, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		return Float::intern(themaster->GetPosThresh());
	}
	else
		return &undefined;
}


Value* SetPosThreshold_cf (Value** arg_list, int count)
{
	check_arg_count (SetPosThreshold, 2, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		themaster->SetPosThresh(arg_list[1]->to_float());
		needs_redraw_set();
		return (arg_list[1]);
	}
	else
		return &undefined;

}


Value* GetRotThreshold_cf (Value** arg_list, int count)
{
	check_arg_count (GetRotThreshold, 1, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		return Float::intern(RadToDeg(themaster->GetRotThresh()));
	}
	else
		return &undefined;

}

Value* SetRotThreshold_cf (Value** arg_list, int count)
{
	check_arg_count (SetRotThreshold, 2, count);
	INode* node = arg_list[0]->to_node();

	Control* controller = node->GetTMController();
	// LAM - defect ??? 
	ReferenceTarget* ref = (controller) ? (ReferenceTarget*)controller->GetInterface(I_MASTER) : NULL;
	if (ref && ref->ClassID() == IKMASTER_CLASSID)
	{	IKMasterControl* themaster = (IKMasterControl*)ref;
		themaster->SetRotThresh (DegToRad(arg_list[1]->to_float()));
		needs_redraw_set();
		return (arg_list[1]);
	}
	else
		return &undefined;

}

//System Utility methods
//Added by AF (09/28/00)
//************************************************************************

Value* IsDebugging_cf (Value** arg_list, int count)
{
	check_arg_count (IsDebugging, 0, count);

	return ( IsDebugging() ? &true_value : &false_value );
}

Value* NumberOfProcessors_cf (Value** arg_list, int count)
{
	check_arg_count (NumberOfProcessors, 0, count);

	return Integer::intern (NumberOfProcessors());
}

Value* IsWindows9x_cf (Value** arg_list, int count)
{
	check_arg_count (IsWindows9x, 0, count);

	return ( IsWindows9x() ? &true_value : &false_value );
}

Value* IsWindows98or2000_cf (Value** arg_list, int count)
{
	check_arg_count (IsWindows98or2000, 0, count);

	return ( IsWindows98or2000() ? &true_value : &false_value );
}

Value* GetScreenWidth_cf (Value** arg_list, int count)
{
	check_arg_count (GetScreenWidth, 0, count);

	return Integer::intern (GetScreenWidth());
}

Value* GetScreenHeight_cf (Value** arg_list, int count)
{
	check_arg_count (GetScreenHeight, 0, count);

	return Integer::intern (GetScreenHeight());
}


/* --------------------- plug-in init --------------------------------- */
// this is called by the dlx initializer, register the global vars here
void le_init()
{
//	#include "le_glbls.h"
}

