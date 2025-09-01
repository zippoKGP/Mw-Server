	/**********************************************************************
 *<
	FILE: PolyPaint.h

	DESCRIPTION:   Paint Soft Selection; Editable Polygon Mesh Object

	CREATED BY: Michaelson Britt

	HISTORY: created May 2004

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#ifndef __POLYPAINTSEL_H__
#define __POLYPAINTSEL_H__

#include "Max.h"
#include "notify.h"
#include "IPainterInterface.h"

class MeshPaintManager;
class MeshPaintHandler;
class MeshPaintHost;
class MeshPaintHostRestore;
typedef Tab<float>	FloatTab;
typedef Tab<Point3>	Point3Tab;


//-----------------------------------------------------------------------------
//--- Data types

typedef enum {
	PAINTMODE_UNDEF=-1,
	PAINTMODE_OFF=0, PAINTMODE_PAINTSEL, PAINTMODE_ERASESEL, PAINTMODE_BLURSEL,
	PAINTMODE_PAINTPUSHPULL, PAINTMODE_PAINTRELAX, PAINTMODE_ERASEDEFORM,

	//Paint mode categories, used with Activate/DeactivatePaintData()
	PAINTMODE_SEL, //paint and erase selection modes
	PAINTMODE_DEFORM, //push-pull, relax, and erase deform modes
} PaintModeID;

inline BOOL IsPaintSelMode( PaintModeID paintMode ) { // TRUE if the mode is a "Paint Selection" mode, FALSE otherwise
	return ( ((paintMode==PAINTMODE_SEL) || (paintMode==PAINTMODE_PAINTSEL) ||
			  (paintMode==PAINTMODE_ERASESEL) || (paintMode==PAINTMODE_BLURSEL) ) ? TRUE : FALSE );
}

inline BOOL IsPaintDeformMode( PaintModeID paintMode ) { // TRUE if the mode is a "Paint Deform" mode, FALSE otherwise
	return ( ((paintMode==PAINTMODE_DEFORM) || (paintMode==PAINTMODE_PAINTPUSHPULL) ||
			  (paintMode==PAINTMODE_PAINTRELAX) || (paintMode==PAINTMODE_ERASEDEFORM)) ? TRUE : FALSE );
}

typedef enum {
	PAINTAXIS_NONE=0, PAINTAXIS_ORIGNORMS, PAINTAXIS_DEFORMEDNORMS,
	PAINTAXIS_XFORM_X, PAINTAXIS_XFORM_Y, PAINTAXIS_XFORM_Z,
} PaintAxisID;

inline BOOL IsPaintXFormAxis( PaintAxisID paintAxis ) { // TRUE if the axis is xform_x, xform_y or xform_z
	return ((paintAxis==PAINTAXIS_XFORM_X) || (paintAxis==PAINTAXIS_XFORM_Y) || (paintAxis==PAINTAXIS_XFORM_Z)?
			TRUE:FALSE);
}

//Information about one set of paint data (one object) during a paint stroke
typedef struct {
	PaintModeID				paintMode;
	INode*					node;
	MNMesh*					object;

	MeshPaintHandler*		handler;
	MeshPaintHost*			host;
	MeshPaintHostRestore*	restore;	//to undo the changes of this context
	float*					amounts;	//accumulation buffer; used in any paint mode
	BOOL					inverted;	//set to TRUE after values are inverted due to the ALT key

	// Paint Selection data
	int						paintSelCount;
	float					*origSel, *goalSel;
	float					*inputSel, *outputSel;
	// Paint Deformation data
	int						paintDeformCount;
	Point3					*origPositions, *goalPositions;
	Point3					*inputPositions, *outputDeform;
	BitArray				selMask;
	FloatTab				softSelMask;
} PaintDataContext;

typedef struct {
	BOOL					shiftKey, ctrlKey, altKey;
	float					radius, strength, pressure;
} PaintBrushContext;

// Information about one increment along a paint stroke;
// superset of PaintDataContext
typedef struct {
	PaintDataContext*		data;
	PaintBrushContext		brush;
	int						pointGatherCount;
	float*					pointGather;
	ICurve*					curve;
} PaintStrokeContext;

//-----------------------------------------------------------------------------
//--- PainterSettings: save and restore settings of the painter interface

class PainterSettings {
	public:
		BOOL				drawTrace, drawRing, drawNormal, pressureEnable;
		int					pressureAffects;
		float				brushMinSize, brushMaxSize, brushMinStrength, brushMaxStrength;

		void				Init();
		void				Get( IPainterInterface_V5* painter );
		void				Set( IPainterInterface_V5* painter );
};


//-----------------------------------------------------------------------------
//--- MeshPaintHandler: to be implemented by any Modifier (or object) which supports Paint Mesh
//    see also MeshPaintHost
class MeshPaintHandler {
	public:
		//-- Implemented by the client
		virtual void		PrePaintStroke (PaintModeID paintMode)=0;	// prepare to receive paint stroke.

		virtual	void		GetPaintHosts( Tab<MeshPaintHost*>& hosts, Tab<INode*>& nodes ) = 0;
		virtual float		GetPaintValue( PaintModeID paintMode ) = 0;
		virtual PaintAxisID GetPaintAxis( PaintModeID paintMode ) = 0;
		// per-host data...
		virtual MNMesh*		GetPaintObject( MeshPaintHost* host ) = 0; //FIXME: switch to ObjectWrapper
		virtual void		GetPaintInputSel( MeshPaintHost* host, FloatTab& selValues ) = 0;
		virtual void		GetPaintInputVertPos( MeshPaintHost* host, Point3Tab& vertPos ) = 0;
		virtual void		GetPaintMask( MeshPaintHost* host, BitArray& sel, FloatTab& softSel ) = 0;
		// paint actions
		virtual void		ApplyPaintSel( MeshPaintHost* host ) = 0;
		virtual void		ApplyPaintDeform( MeshPaintHost* host ) = 0;
		virtual void		RevertPaintDeform( MeshPaintHost* host ) = 0;
		// notifications
		virtual void		OnPaintModeChanged( PaintModeID prevMode, PaintModeID curMode ) = 0; //when the mode changes
		virtual void		OnPaintDataActivate( PaintModeID paintMode, BOOL onOff ) = 0; //when the data becomes active/unactive
		virtual void		OnPaintDataRedraw( PaintModeID prevMode ) = 0; //to redraw the screen; called after OnPaintDataChanged()
		virtual void		OnPaintBrushChanged() = 0; //when the brush changes

		// Names for restore objects - undo "Paint Soft Selection", "Paint Deformation", etc - should be localized.
		virtual TCHAR *GetPaintUndoString (PaintModeID mode) = 0;


		//-- Implemented by the system
		MeshPaintHandler();
		~MeshPaintHandler() {}
		IOResult			Load(ILoad *iload);
		IOResult			Save(ISave *isave);
		void				BeginEditParams();
		void				EndEditParams();

		void				BeginPaintMode( PaintModeID paintMode );
		void				EndPaintMode();
		PaintModeID			GetPaintMode();
		BOOL				InPaintMode();

		float				GetPaintBrushSize();
		void				SetPaintBrushSize( float size );
		float				GetPaintBrushStrength();
		void				SetPaintBrushStrength( float strength );

		void				UpdatePaintBrush(); //call when brush params change
		void				UpdatePaintObject( MeshPaintHost* host ); //call when the object topology changes

		// Activate the paint data before painting, to allocated the buffers
		void				ActivatePaintData( PaintModeID paintMode );
		void				DeactivatePaintData( PaintModeID paintMode, BOOL accepted=TRUE );
		BOOL				IsPaintDataActive( PaintModeID paintMode );

	protected:
		friend class MeshPaintHandlerRestore;
		friend class MeshPaintMgr;
		PainterSettings*	GetPainterSettings() {return &painterSettings;}
		void				SetPainterSettings( PainterSettings& painterSettings ) {this->painterSettings=painterSettings;}

		enum {SEL_ACTIVE_CHUNK,DEFORM_ACTIVE_CHUNK}; // load/save chunk enums
		BOOL				paintSelActive, paintDeformActive;
		PainterSettings		painterSettings;
};

//-----------------------------------------------------------------------------
//--- MeshPaintHost: to be implemented by any ModData (or object) which supports Paint Mesh
//    see also MeshPaintHandler
class MeshPaintHost {
	public:
		//-- Implemented by the system
		MeshPaintHost();
		~MeshPaintHost();
		IOResult			Load(ILoad *iload);
		IOResult			Save(ISave *isave);

		float*				GetPaintSelValues();
		int					GetPaintSelCount();
		void				SetPaintSelCount( int count );

		Point3*				GetPaintDeformOffsets();
		int					GetPaintDeformCount();
		void				SetPaintDeformCount( int count );

		void				UpdatePaintObject( MNMesh* object );

	protected:
		friend class MeshPaintMgr;
		friend class MeshPaintHandler;
		friend class MeshPaintHostRestore;

		// used internally
		MeshPaintHostRestore* CreatePaintRestore( PaintModeID mode, INode* node, MeshPaintHandler* handler, bool active=true );
		void				DeletePaintRestore( MeshPaintHostRestore* restore );
		void				PutPaintRestore( PaintModeID mode, INode* node, MeshPaintHandler* handler, bool active=true );


		enum {PAINT_SEL_BUF_CHUNK, PAINT_SEL_DEFORM_CHUNK}; // load/save chunk enums
		// buffer may hold more values than are actively used; store a separate usage count
		int					mPaintSelCount, mPaintDeformCount;
		FloatTab			paintSelBuf; //paint selection values
		Point3Tab			paintDeformBuf; //paint deformation offset values
};

//-----------------------------------------------------------------------------
//--- MeshPaintHandlerRestore: Undo object for MeshPaintHandler
class MeshPaintHandlerRestore : public RestoreObj {
	public:	
		MeshPaintHandlerRestore( MeshPaintHandler* handler );
		void				Restore(int isUndo);
		void				Redo();
		int					Size();
	protected:
		MeshPaintHandler*	handler;
		BOOL				paintSelActive, paintDeformActive;
};

//-----------------------------------------------------------------------------
//--- MeshPaintHostRestore: Undo object for MeshPaintHost

class MeshPaintHostRestore : public RestoreObj {
	public:	
		MeshPaintHostRestore( PaintModeID paintMode, INode* node, MeshPaintHandler* handler, MeshPaintHost* host, bool active=true);
		~MeshPaintHostRestore();
		void				Restore(int isUndo);
		void				Redo();
		int					Size();

		TCHAR*				GetName(); //GetString(IDS_RESTOREOBJ_MESHPAINTSEL)
		FloatTab&			GetPaintSel() {return paintSelValues;}
		Point3Tab&			GetPaintDeform() {return paintDeformValues;}
	protected:
		INode*				node;
		MeshPaintHost*		host;
		MeshPaintHandler*	handler;
		PaintModeID			paintMode;
		int					paintSelCount, paintDeformCount;
		FloatTab			paintSelValues;
		Point3Tab			paintDeformValues;
		bool				mActive;
};

//-----------------------------------------------------------------------------
//--- MeshPaintMgr: runs paint sessions for MeshPaint handlers and their hosts

class MeshPaintMgr :
	public IPainterCanvasInterface_V5,
	public IPainterCanvasInterface_V7,
	public ReferenceTarget //must derive from ReferenceTarget to be a PainterCanvas
{
	public:
		MeshPaintMgr();
		~MeshPaintMgr() {Free();}
		void				DeleteThis();
		BOOL				Init( MeshPaintHandler* handler );
		void				Free();

		// Accessor methods
		float				GetPaintValue();
		void				SetPaintValue( float f );
		float				GetBrushSize();
		void				SetBrushSize( float f );
		float				GetBrushStrength();
		void				SetBrushStrength( float f );


		// Mode-related methods
		void				BeginEditParams( MeshPaintHandler* handler );
		void				EndEditParams( MeshPaintHandler* handler );

		PaintModeID			GetMode() {return mPaintMode;}
		void				BeginSession( PaintModeID paintMode );
		void				EndSession( PaintModeID paintMode=PAINTMODE_UNDEF );
		BOOL				InSession() {return (GetMode()!=PAINTMODE_OFF);}

		// Display Painter Interface options
		void				BringUpOptions() {if(pPainter!=NULL) pPainter->BringUpOptions();}

	//-- From ReferenceTarget
		void*				GetInterface( ULONG id );
		RefResult			NotifyRefChanged(Interval changeInt, RefTargetHandle hTarget, PartID& partID,  RefMessage message)
							{return REF_SUCCEED;}


	//-- Painter Canvas methods
		BOOL				StartStroke();
		BOOL				PaintStroke(BOOL hit, IPoint2 mousePos, 
								Point3 worldPoint, Point3 worldNormal,
								Point3 localPoint, Point3 localNormal,
								Point3 bary,  int index,
								BOOL shift, BOOL ctrl, BOOL alt, 
								float radius, float str,
								float pressure, INode *node,
								BOOL mirrorOn,
								Point3 worldMirrorPoint, Point3 worldMirrorNormal,
								Point3 localMirrorPoint, Point3 localMirrorNormal);
		BOOL				EndStroke();
		BOOL				EndStroke(int ct, BOOL *hit,IPoint2 *mousePos, 
								Point3 *worldPoint, Point3 *worldNormal,
								Point3 *localPoint, Point3 *localNormal,
								Point3 *bary,  int *index,
								BOOL *shift, BOOL *ctrl, BOOL *alt, 
								float *radius, float *str,
								float *pressure, INode **node,
								BOOL mirrorOn,
								Point3 *worldMirrorPoint, Point3 *worldMirrorNormal,
								Point3 *localMirrorPoint, Point3 *localMirrorNormal);
		BOOL				CancelStroke();
		BOOL				SystemEndPaintSession();
		void				PainterDisplay(TimeValue t, ViewExp *vpt, int flags);

		//void				CanvasStartPaint() {}	//FIXME: use suspend/resume
		//void				CanvasEndPaint() {}		//FIXME: use suspend/resume

		void				OnPaintBrushChanged();

	protected:
		friend class MeshPaintHostRestore;

		// Helper methods
		PaintDataContext*	BeginDataContext( PaintModeID paintMode, INode* node, MeshPaintHandler* handler, MeshPaintHost* host );
		void				EndDataContext( PaintDataContext* data, BOOL accepted );

		void				InitPaintSel( PaintDataContext* data );			//init the goal values
		void				InitPaintDeform( PaintDataContext* data );		//init the goal positions
		void				UpdatePaintDeform( PaintDataContext* data );	//update the goal positions, mid-stroke
		void				GetPaintInputVertNorm( PaintDataContext* data, Point3Tab& inputNorms );

		void				UpdatePointGather( INode* node, MeshPaintHost* host );
		void				UpdateBrushTracking();

		BOOL				PaintStroke( INode *node, int index, PaintBrushContext brush );
		void				PaintStroke_Sel( PaintStrokeContext* stroke );
		void				PaintStroke_SelBlur( PaintStrokeContext* stroke );
		void				PaintStroke_Deform( PaintStrokeContext* stroke );
		void				EndStroke( BOOL accepted );

		IPainterInterface_V7 *pPainter;
		IPainterInterface_V7* GetPainter();

		PaintModeID			mPaintMode;
		float				paintSelValue,paintDeformValue;

		// Cached values per paint stroke
		MeshPaintHandler*	handler;
		Tab<INode*>			nodes;
		Tab<MeshPaintHost*>	hosts;
		Tab<ObjectState>	objectStates;
		Tab<PaintDataContext*> dataContexts;
		PainterSettings		painterSettingsPrev;
};

extern MeshPaintMgr theMeshPaintMgr;
inline MeshPaintMgr* GetMeshPaintMgr() {return &theMeshPaintMgr;}


#endif //__POLYPAINTSEL_H__
