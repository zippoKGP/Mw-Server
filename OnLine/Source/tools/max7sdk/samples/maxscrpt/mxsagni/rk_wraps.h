// \{def_visible_primitive(\([^"]*\)\}+\{\("[^\;]*\)\}
// def_struct_primitive(\1struct, \2\3
#include "buildver.h"

def_visible_primitive( CreatePreview,		"CreatePreview");
def_visible_primitive( GetMatLibFileName, 	"GetMatLibFileName");

def_visible_primitive( SetSysCur, 			"SetSysCur");

def_visible_primitive( GetSnapState, 		"GetSnapState");
def_visible_primitive( GetSnapMode, 		"GetSnapMode");
def_visible_primitive( GetCrossing, 		"GetCrossing");
def_visible_primitive( SetToolBtnState, 	"SetToolBtnState");
def_visible_primitive( GetCoordCenter, 		"GetCoordCenter");
def_visible_primitive( SetCoordCenter, 		"SetCoordCenter");

def_visible_primitive( EnableUndo,			"enableUndo");
def_visible_primitive( EnableCoordCenter, 	"EnableCoordCenter");
def_visible_primitive( GetRefCoordSys, 		"GetRefCoordSys");
def_visible_primitive( SetRefCoordSys, 		"SetRefCoordSys");
def_visible_primitive( EnableRefCoordSys, 	"EnableRefCoordSys");
def_visible_primitive( GetNumAxis, 			"GetNumAxis");
def_visible_primitive( LockAxisTripods, 	"LockAxisTripods");
def_visible_primitive( AxisTripodLocked, 	"AxisTripodLocked");

def_visible_primitive( CompleteRedraw, 		"CompleteRedraw");
def_visible_primitive( ExclusionListDlg, 	"ExclusionListDlg");
def_visible_primitive( MaterialBrowseDlg, 	"MaterialBrowseDlg");
def_visible_primitive( HitByNameDlg, 		"HitByNameDlg");
def_visible_primitive( NodeColorPicker, 	"NodeColorPicker");
def_visible_primitive( FileOpenMatLib, 		"FileOpenMatLib");
#ifndef NO_MATLIB_SAVING // orb 01-09-2002
#ifndef USE_CUSTOM_MATNAV // orb 08-23-2001 removing mtllib saving
def_visible_primitive( FileSaveMatLib, 		"FileSaveMatLib");
def_visible_primitive( FileSaveAsMatLib, 	"FileSaveAsMatLib");
#endif
#endif
def_visible_primitive( LoadDefaultMatLib, 	"LoadDefaultMatLib");

def_visible_primitive( PushPrompt, 			"PushPrompt");
def_visible_primitive( PopPrompt, 			"PopPrompt");
def_visible_primitive( ReplacePrompt, 		"ReplacePrompt");
def_visible_primitive( DisplayTempPrompt, 	"DisplayTempPrompt");
def_visible_primitive( RemoveTempPrompt, 	"RemoveTempPrompt");

def_visible_primitive( DisableStatusXYZ, 	"DisableStatusXYZ");
def_visible_primitive( EnableStatusXYZ, 	"EnableStatusXYZ");
def_visible_primitive( SetStatusXYZ, 		"SetStatusXYZ");

def_visible_primitive( GetGridSpacing, 		"GetGridSpacing");
def_visible_primitive( GetGridMajorLines, 	"GetGridMajorLines");

def_visible_primitive( EnableShowEndRes, 	"EnableShowEndRes");
def_visible_primitive( AppendSubSelSet, 	"AppendSubSelSet");
def_visible_primitive( ClearSubSelSets, 	"ClearSubSelSets");
def_visible_primitive( ClearCurSelSet, 		"ClearCurSelSet");
def_visible_primitive( EnableSubObjSel, 	"EnableSubObjSel");
def_visible_primitive( IsSubSelEnabled, 	"IsSubSelEnabled");

/*
def_visible_primitive( RegAxisChangeCB, 	"RegAxisChangeCB");
def_visible_primitive( UnRegAxisChangeCB, 	"UnRegAxisChangeCB");
def_visible_primitive( RegDeleteUser, 		"RegDeleteUser");
def_visible_primitive( UnRegDeleteUser, 	"UnRegDeleteUser");
def_visible_primitive( SetGetEnvMap, 		"SetGetEnvMap");
*/
def_visible_primitive( IsCPEdgeOnInView, 	"IsCPEdgeOnInView");
def_visible_primitive( NodeInvalRect, 		"NodeInvalRect");

def_visible_primitive( SetVPortBGColor, 	"SetVPortBGColor");
def_visible_primitive( GetVPortBGColor, 	"GetVPortBGColor");
def_visible_primitive( SetBkgImageAspect, 	"SetBkgImageAspect");
def_visible_primitive( GetBkgImageAspect, 	"GetBkgImageAspect");

def_visible_primitive( SetBkgImageAnimate, 	"SetBkgImageAnimate");
def_visible_primitive( GetBkgImageAnimate, 	"GetBkgImageAnimate");

#ifndef WEBVERSION	// russom - 04/10/02
def_visible_primitive( SetBkgFrameRange, 	"SetBkgFrameRange");
def_visible_primitive( GetBkgRangeVal, 		"GetBkgRangeVal");
#endif // WEBVERSION
def_visible_primitive( SetBkgORType, 		"SetBkgORType");
def_visible_primitive( GetBkgORType, 		"GetBkgORType");

#ifndef WEBVERSION	// russom - 04/10/02
def_visible_primitive( SetBkgStartTime, 	"SetBkgStartTime");
def_visible_primitive( GetBkgStartTime, 	"GetBkgStartTime");
def_visible_primitive( SetBkgSyncFrame, 	"SetBkgSyncFrame");
def_visible_primitive( GetBkgSyncFrame, 	"GetBkgSyncFrame");
def_visible_primitive( GetBkgFrameNum, 		"GetBkgFrameNum");
#endif // WEBVERSION

def_visible_primitive( SetUseEnvironmentMap, "SetUseEnvironmentMap");
def_visible_primitive( GetUseEnvironmentMap, "GetUseEnvironmentMap");

def_visible_primitive( DeSelectNode, 		"DeSelectNode");
def_visible_primitive( BoxPickNode, 		"BoxPickNode");
def_visible_primitive( CirclePickNode, 		"CirclePickNode");
def_visible_primitive( FencePickNode, 		"FencePickNode");

def_visible_primitive( FlashNodes, 			"FlashNodes");

#ifndef NO_DRAFT_RENDERER
def_visible_primitive( SetUseDraftRenderer, "SetUseDraftRenderer");
def_visible_primitive( GetUseDraftRenderer, "GetUseDraftRenderer");
#endif // NO_DRAFT_RENDERER

def_visible_primitive( ClearNodeSelection, 	"ClearNodeSelection");
def_visible_primitive( DisableSceneRedraw, 	"DisableSceneRedraw");
def_visible_primitive( EnableSceneRedraw, 	"EnableSceneRedraw");
def_visible_primitive( IsSceneRedrawDisabled,"IsSceneRedrawDisabled");

def_visible_primitive( GetKeyStepsPos, 		"GetKeyStepsPos");
def_visible_primitive( SetKeyStepsPos, 		"SetKeyStepsPos");

def_visible_primitive( GetKeyStepsRot, 		"GetKeyStepsRot");
def_visible_primitive( SetKeyStepsRot, 		"SetKeyStepsRot");

def_visible_primitive( GetKeyStepsScale, 	"GetKeyStepsScale");
def_visible_primitive( SetKeyStepsScale, 	"SetKeyStepsScale");

def_visible_primitive( GetKeyStepsSelOnly, 	"GetKeyStepsSelOnly");
def_visible_primitive( SetKeyStepsSelOnly, 	"SetKeyStepsSelOnly");

def_visible_primitive( GetKeyStepsUseTrans, "GetKeyStepsUseTrans");
def_visible_primitive( SetKeyStepsUseTrans, "SetKeyStepsUseTrans");

//def_visible_primitive( RegExitMAXCallback, 	"RegExitMAXCallback");
//def_visible_primitive( UnRegExitMAXCallback,"UnRegExitMAXCallback");

def_visible_primitive( AssignNewName, 				"AssignNewName");
def_visible_primitive( ForceCompleteRedraw,			"ForceCompleteRedraw");
def_visible_primitive( GetBackGroundController, 	"GetBackGroundController");
def_visible_primitive( GetBackGround, 				"GetBackGround");
def_visible_primitive( GetCommandPanelTaskMode, 	"GetCommandPanelTaskMode");
def_visible_primitive( GetNamedSelSetItem, 			"GetNamedSelSetItem");
def_visible_primitive( GetNamedSelSetItemCount, 	"GetNamedSelSetItemCount");
def_visible_primitive( GetNamedSelSetName, 			"GetNamedSelSetName");
def_visible_primitive( GetNumNamedSelSets, 			"GetNumNamedSelSets");

#ifndef WEBVERSION
def_visible_primitive( GetRendApertureWidth, 		"GetRendApertureWidth");
def_visible_primitive( SetRendApertureWidth, 		"SetRendApertureWidth");
#endif
def_visible_primitive( GetRendImageAspect, 			"GetRendImageAspect");
def_visible_primitive( NamedSelSetListChanged, 		"NamedSelSetListChanged");
//def_visible_primitive( NotifyVPDisplayCBChanged,	"NotifyVPDisplayCBChanged");
def_visible_primitive( OkMtlForScene, 				"OkMtlForScene");
def_visible_primitive( RescaleWorldUnits, 			"RescaleWorldUnits");
def_visible_primitive( SetBackGround, 				"SetBackGround");
def_visible_primitive( SetBackGroundController, 	"SetBackGroundController");
def_visible_primitive( SetCommandPanelTaskMode, 	"SetCommandPanelTaskMode");
def_visible_primitive( SetCurNamedSelSet, 			"SetCurNamedSelSet");
def_visible_primitive( StopCreating, 				"StopCreating");

// graphics window methods
def_struct_primitive( getDriverString, 			gw, "getDriverString");
def_struct_primitive( isPerspectiveView, 		gw, "isPerspectiveView");
def_struct_primitive( setSkipCount, 			gw, "setSkipCount");
def_struct_primitive( getSkipCount, 			gw, "getSkipCount");
def_struct_primitive( querySupport, 			gw, "querySupport");

def_struct_primitive( getMaxLights, 			gw, "getMaxLights");
def_struct_primitive( setTransform, 			gw, "setTransform");

def_struct_primitive( setPos, 					gw, "setPos");
def_struct_primitive( getWinSizeX, 				gw, "getWinSizeX");
def_struct_primitive( getWinSizeY, 				gw, "getWinSizeY");
def_struct_primitive( getWinDepth, 				gw, "getWinDepth");
def_struct_primitive( getHitherCoord, 			gw, "getHitherCoord");
def_struct_primitive( getYonCoord, 				gw, "getYonCoord");
def_struct_primitive( getViewportDib,			gw, "getViewportDib");

def_struct_primitive( resetUpdateRect, 			gw, "resetUpdateRect");
def_struct_primitive( enlargeUpdateRect, 		gw, "enlargeUpdateRect");
def_struct_primitive( getUpdateRect, 			gw, "getUpdateRect");
def_struct_primitive( updateScreen, 			gw, "updateScreen");
def_struct_primitive( setRndLimits, 			gw, "setRndLimits");
def_struct_primitive( getRndLimits, 			gw, "getRndLimits");
def_struct_primitive( getRndMode, 				gw, "getRndMode");

def_struct_primitive( hTransPoint, 				gw, "hTransPoint");
def_struct_primitive( wTransPoint, 				gw, "wTransPoint");
def_struct_primitive( transPoint, 				gw, "transPoint");
def_struct_primitive( getFlipped, 				gw, "getFlipped");

def_struct_primitive( setColor, 				gw, "setColor");
def_struct_primitive( clearScreen, 				gw, "clearScreen");
//def_struct_primitive( lightVertex, 				gw, "lightVertex");

def_struct_primitive( hText, 					gw, "hText");
def_struct_primitive( hMarker,					gw, "hMarker");
def_struct_primitive( hPolyline, 				gw, "hPolyline");
def_struct_primitive( hPolygon, 				gw, "hPolygon");
def_struct_primitive( hTriStrip, 				gw, "hTriStrip");

def_struct_primitive( wText, 					gw, "wText");
def_struct_primitive( wMarker,					gw, "wMarker");
def_struct_primitive( wPolyline, 				gw, "wPolyline");
def_struct_primitive( wPolygon, 				gw, "wPolygon");
def_struct_primitive( wTriStrip, 				gw, "wTriStrip");

def_struct_primitive( text, 					gw, "text");
def_struct_primitive( marker, 					gw, "marker");
def_struct_primitive( polyline, 				gw, "polyline");
def_struct_primitive( polygon, 					gw, "polygon");

def_struct_primitive( NonScalingObjectSize, 	gw, "NonScalingObjectSize");
def_struct_primitive( GetPointOnCP, 			gw, "GetPointOnCP");
def_struct_primitive( GetCPDisp, 				gw, "GetCPDisp");
def_struct_primitive( GetVPWorldWidth, 			gw, "GetVPWorldWidth");
def_struct_primitive( MapCPToWorld, 			gw, "MapCPToWorld");
def_struct_primitive( SnapPoint, 				gw, "SnapPoint");
def_struct_primitive( SnapLength, 				gw, "SnapLength");
def_struct_primitive( IsPerspView, 				gw, "IsPerspView");
def_struct_primitive( GetFocalDist, 			gw, "GetFocalDist");

def_struct_primitive( getTextExtent_gw, 		gw, "getTextExtent");

def_visible_primitive( loadDllsFromDir,			"loadDllsFromDir");
def_visible_primitive( SilentMode, 				"SilentMode");
def_visible_primitive( SetSilentMode, 			"SetSilentMode");
def_visible_primitive( GetDir, 					"GetDir");
def_visible_primitive( SetDir, 					"SetDir");

def_visible_primitive( SelectSaveBitMap, 		"SelectSaveBitMap");

def_visible_primitive( GetUIColor, 				"GetUIColor");
def_visible_primitive( SetUIColor, 				"SetUIColor");
def_visible_primitive( GetDefaultUIColor, 		"GetDefaultUIColor");

def_visible_primitive( MatchPattern, 			"MatchPattern");
//def_visible_primitive( CreateArcballDialog, 	"CreateArcballDialog");

def_visible_primitive( GetCVertMode, 			"GetCVertMode");
#ifndef NO_MOTION_BLUR	// russom - 04/10/02
def_visible_primitive( GetImageBlurMultController, 		"GetImageBlurMultController");
#endif	// NO_MOTION_BLUR
def_visible_primitive( GetInheritVisibility, 	"GetInheritVisibility");
def_visible_primitive( GetPosTaskWeight, 		"GetPosTaskWeight");
def_visible_primitive( GetRenderID, 			"GetRenderID");
def_visible_primitive( GetRotTaskWeight, 		"GetRotTaskWeight");
def_visible_primitive( GetShadeCVerts, 			"GetShadeCVerts");
def_visible_primitive( GetTaskAxisState, 		"GetTaskAxisState");
//def_visible_primitive( GetTaskAxisStateBits, 	"GetTaskAxisStateBits");
def_visible_primitive( GetTrajectoryON, 		"GetTrajectoryON");
def_visible_primitive( IsBoneOnly, 				"IsBoneOnly");
def_visible_primitive( SetCVertMode, 			"SetCVertMode");
#ifndef NO_MOTION_BLUR	// russom - 04/10/02
def_visible_primitive( SetImageBlurMultController,"SetImageBlurMultController");
def_visible_primitive( SetImageBlurMultiplier, 	"SetImageBlurMultiplier");
#endif	// NO_MOTION_BLUR
def_visible_primitive( SetInheritVisibility, 	"SetInheritVisibility");
#ifndef NO_MOTION_BLUR	// russom - 04/10/02
def_visible_primitive( SetMotBlur, 				"SetMotBlur");
#endif	// NO_MOTION_BLUR
def_visible_primitive( SetPosTaskWeight, 		"SetPosTaskWeight");
def_visible_primitive( SetRenderable, 			"SetRenderable");
def_visible_primitive( SetRenderID, 			"SetRenderID");
def_visible_primitive( SetRotTaskWeight, 		"SetRotTaskWeight");
def_visible_primitive( SetShadeCVerts, 			"SetShadeCVerts");
def_visible_primitive( SetTaskAxisState, 		"SetTaskAxisState");
def_visible_primitive( SetTrajectoryON, 		"SetTrajectoryON");
def_visible_primitive( GetTMController, 		"GetTMController");
def_visible_primitive( GetVisController, 		"GetVisController");
def_visible_primitive( is_group_head,			"isGroupHead");
def_visible_primitive( is_group_member,			"isGroupMember");
def_visible_primitive( is_open_group_member,	"isOpenGroupMember");
def_visible_primitive( is_open_group_head,		"isOpenGroupHead");
def_visible_primitive( set_group_member,		"setGroupMember");
def_visible_primitive( set_group_head,			"setGroupHead");
def_visible_primitive( set_group_open,			"setGroupOpen");
//def_visible_primitive( set_group_member_open,	"setGroupMemberOpen");

/*
def_visible_primitive( CanInstanceController, 	"CanInstanceController");
def_visible_primitive( CloneControl, 			"CloneControl");
def_visible_primitive( GetXController, 			"GetXController");
def_visible_primitive( GetYController, 			"GetYController");
def_visible_primitive( GetZController, 			"GetZController");
*/
def_visible_primitive( CreateLockKey, 			"CreateLockKey");
def_visible_primitive( MirrorIKConstraints, 	"MirrorIKConstraints");
def_visible_primitive( NodeIKParamsChanged, 	"NodeIKParamsChanged");
def_visible_primitive( OKToBindToNode, 			"OKToBindToNode");

def_visible_primitive( IntersectRayEx, 			"IntersectRayEx");
def_visible_primitive( ArbAxis, 				"ArbAxis");
def_visible_primitive( degToRad, 				"DegToRad");
def_visible_primitive( radToDeg, 				"radToDeg");
def_visible_primitive( RAMPlayer, 				"RAMPlayer");
// RK 5/12/99, Commenting as Shiva does not use C-Dilla
/*
def_visible_primitive( IsEmergencyLicense, 		"IsEmergencyLicense");
def_visible_primitive( IsNetworkLicense, 		"IsNetworkLicense");
def_visible_primitive( IsTrialLicense, 			"IsTrialLicense");
def_visible_primitive( SetNetServer, 			"SetNetServer");	// Should not be exposed
*/
def_visible_primitive( IsNetServer, 			"IsNetServer");

// Access to atmos/effects active checbox
#ifndef NO_RENDEREFFECTS	// russom - 03/26/02
def_visible_primitive( fx_set_active,			"setActive");
def_visible_primitive( fx_is_active,			"isActive");
#endif // NO_RENDEREFFECTS

#ifndef NO_OBJECT_BOOL
// boolean compound object access
def_struct_primitive( createBooleanObject, 	boolObj, "createBooleanObject");
def_struct_primitive( setOperandB, 			boolObj, "setOperandB");
def_struct_primitive( GetOperandSel, 		boolObj, "GetOperandSel");
def_struct_primitive( SetOperandSel, 		boolObj, "SetOperandSel");
def_struct_primitive( GetBoolOp, 			boolObj, "GetBoolOp");
def_struct_primitive( SetBoolOp, 			boolObj, "SetBoolOp");
def_struct_primitive( GetBoolCutType, 		boolObj, "GetBoolCutType");
def_struct_primitive( SetBoolCutType, 		boolObj, "SetBoolCutType");
def_struct_primitive( GetDisplayResult, 	boolObj, "GetDisplayResult");
def_struct_primitive( SetDisplayResult, 	boolObj, "SetDisplayResult");
def_struct_primitive( GetShowHiddenOps, 	boolObj, "GetShowHiddenOps");
def_struct_primitive( SetShowHiddenOps, 	boolObj, "SetShowHiddenOps");
def_struct_primitive( GetUpdateMode, 		boolObj, "GetUpdateMode");
def_struct_primitive( SetUpdateMode, 		boolObj, "SetUpdateMode");
def_struct_primitive( GetOptimize, 			boolObj, "GetOptimize");
def_struct_primitive( SetOptimize, 			boolObj, "SetOptimize");
#endif

// particle system access
// This functions are not really useful since, the value's set change whenever 
// the time slider is moved
//def_visible_primitive( SetParticlePos, 			"SetParticlePos");
//def_visible_primitive( SetParticleVelocity, 	"SetParticleVelocity");
//def_visible_primitive( SetParticleAge, 			"SetParticleAge");


// trackbar access
def_struct_primitive( get_trackbar_next_key,	trackbar, "GetNextKeyTime");
def_struct_primitive( get_trackbar_prev_key,	trackbar, "GetPreviousKeyTime");

// file property access
def_struct_primitive( fp_num_properties,	fileProperties,	"getNumProperties");
def_struct_primitive( fp_get_property_name,	fileProperties,	"getPropertyName");
def_struct_primitive( fp_get_property_val,	fileProperties,	"getPropertyValue");
def_struct_primitive( fp_get_items,			fileProperties,	"getItems");
def_struct_primitive( fp_find_property,		fileProperties,	"findProperty");
def_struct_primitive( fp_add_property,		fileProperties,	"addProperty");
def_struct_primitive( fp_delete_property,	fileProperties,	"deleteProperty");


//watje
def_visible_primitive( SetSelectFilter, 		"SetSelectFilter");
def_visible_primitive( GetSelectFilter, 		"GetSelectFilter");
def_visible_primitive( GetNumberSelectFilters, 		"GetNumberSelectFilters");

def_visible_primitive( SetDisplayFilter, 		"SetDisplayFilter");
def_visible_primitive( GetDisplayFilter, 		"GetDisplayFilter");
def_visible_primitive( GetNumberDisplayFilters, 		"GetNumberDisplayFilters");

def_visible_primitive( GetSelectFilterName, 		"GetSelectFilterName");
def_visible_primitive( GetDisplayFilterName, 		"GetDisplayFilterName");

// LAM: Added these 5/2/01
#ifndef NO_REGIONS
def_visible_primitive( SetRenderType,	"SetRenderType");
def_visible_primitive( GetRenderType, 	"GetRenderType");
#endif // NO_REGIONS
