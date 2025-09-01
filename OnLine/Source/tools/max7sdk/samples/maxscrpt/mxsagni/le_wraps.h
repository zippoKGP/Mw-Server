
/*Interface class and miscellaneous functions*/

def_visible_primitive( ConfigureBitmapPaths,	"ConfigureBitmapPaths");
//def_visible_primitive( PushAxisConstraints,		"PushAxisConstraints");
//def_visible_primitive( PopAxisConstraints,		"PopAxisConstraints");
#ifndef NO_ATMOSPHERICS	// russom - 04/11/02
def_visible_primitive( EditAtmosphere,			"EditAtmosphere");
#endif
def_visible_primitive( CheckForSave,			"CheckForSave");


/* Object Class */ 
def_visible_primitive( GetPolygonCount,			"GetPolygonCount");
def_visible_primitive( GetTriMeshFaceCount,		"GetTriMeshFaceCount");
def_visible_primitive( IsPointSelected,			"IsPointSelected");
def_visible_primitive( NumMapsUsed,				"NumMapsUsed");
def_visible_primitive( IsShapeObject,			"IsShapeObject");
def_visible_primitive( PointSelection,			"PointSelection");
def_visible_primitive( NumSurfaces,				"NumSurfaces");
def_visible_primitive( IsSurfaceUVClosed,		"IsSurfaceUVClosed");

/* Miscellaneous functions */
def_visible_primitive( DeselectHiddenEdges,		"DeselectHiddenEdges");
def_visible_primitive( DeselectHiddenFaces,		"DeselectHiddenFaces");
def_visible_primitive( AverageSelVertCenter,	"AverageSelVertCenter");
def_visible_primitive( AverageSelVertNormal,	"AverageSelVertNormal");
def_visible_primitive( MatrixFromNormal,		"MatrixFromNormal");

/* Patch Objects */
#ifndef NO_PATCHES
def_visible_primitive( SetPatchSteps,			"SetPatchSteps");
def_visible_primitive( GetPatchSteps,			"GetPatchSteps");
#endif

/* Euler Angles */
def_visible_primitive( GetEulerQuatAngleRatio,	"GetEulerQuatAngleRatio");
def_visible_primitive( GetEulerMatAngleRatio,	"GetEulerMatAngleRatio");

/* IK controller stuff */

#ifndef WEBVERSION
def_struct_primitive( GetStartTime,		ik,		"GetStartTime");
def_struct_primitive( SetStartTime,		ik,		"SetStartTime");
def_struct_primitive( GetEndTime,		ik,		"GetEndTime");
def_struct_primitive( SetEndTime,		ik,		"SetEndTime");
def_struct_primitive( SetPosThreshold,	ik,		"SetPosThreshold");
def_struct_primitive( GetPosThreshold,	ik,		"GetPosThreshold");
def_struct_primitive( SetRotThreshold,	ik,		"SetRotThreshold");
def_struct_primitive( GetRotThreshold,	ik,		"GetRotThreshold");
#endif //WEBVERSION
def_struct_primitive( GetIterations,	ik,		"GetIterations");
def_struct_primitive( SetIterations,	ik,		"SetIterations");


/*SystemTools  -  Utility functions to pool the state of the system
  Added by AF (09/28/00)                                         */
def_struct_primitive( IsDebugging,			systemTools,	"IsDebugging");
def_struct_primitive( NumberOfProcessors,	systemTools,	"NumberOfProcessors");
def_struct_primitive( IsWindows9x,			systemTools,	"IsWindows9x");
def_struct_primitive( IsWindows98or2000,	systemTools,	"IsWindows98or2000");
def_struct_primitive( GetScreenWidth,		systemTools,	"GetScreenWidth");
def_struct_primitive( GetScreenHeight,		systemTools,	"GetScreenHeight");
