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

// EditPolySelectRestore --------------------------------------------------

EditPolySelectRestore::EditPolySelectRestore(EditPolyMod *pMod, EditPolyData *pData) : mpMod(pMod), mpData(pData) {
	mLevel = mpMod->GetEPolySelLevel();
	mpData->SetFlag (kEPDataHeld);

	switch (mLevel) {
	case EPM_SL_OBJECT: DbgAssert(0); break;
	case EPM_SL_VERTEX:
		mUndoSel = mpData->mVertSel; break;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		mUndoSel = mpData->mEdgeSel;
		break;
	default:
		mUndoSel = mpData->mFaceSel;
		break;
	}
}

EditPolySelectRestore::EditPolySelectRestore(EditPolyMod *pMod, EditPolyData *pData, int sLevel) : mpMod(pMod), mpData(pData), mLevel(sLevel) {
	mpData->SetFlag (kEPDataHeld);

	switch (mLevel) {
	case EPM_SL_OBJECT:
		DbgAssert(0); break;
	case EPM_SL_VERTEX:
		mUndoSel = mpData->mVertSel; break;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		mUndoSel = mpData->mEdgeSel; break;
	default:
		mUndoSel = mpData->mFaceSel; break;
	}
}

void EditPolySelectRestore::After () {
	switch (mLevel) {			
	case EPM_SL_VERTEX:
		mRedoSel = mpData->mVertSel; break;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		mRedoSel = mpData->mEdgeSel; break;
	default:
		mRedoSel = mpData->mFaceSel; break;
	}
}

void EditPolySelectRestore::Restore(int isUndo) {
	if (isUndo && mRedoSel.GetSize() == 0) After ();
	switch (mLevel) {
	case EPM_SL_VERTEX:
		mpData->mVertSel = mUndoSel; break;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		mpData->mEdgeSel = mUndoSel; break;
	default:
		mpData->mFaceSel = mUndoSel; break;
	}
	mpMod->EpModLocalDataChanged (PART_SELECT);
}

void EditPolySelectRestore::Redo() {
	switch (mLevel) {		
	case EPM_SL_VERTEX:
		mpData->mVertSel = mRedoSel; break;
	case EPM_SL_EDGE:
	case EPM_SL_BORDER:
		mpData->mEdgeSel = mRedoSel; break;
	default:
		mpData->mFaceSel = mRedoSel; break;
	}
	mpMod->EpModLocalDataChanged (PART_SELECT);
}

EditPolyHideRestore::EditPolyHideRestore(EditPolyMod *pMod, EditPolyData *pData) : mpMod(pMod), mpData(pData) {
	mpData->SetFlag (kEPDataHeld);
	mFaceLevel = (mpMod->GetMNSelLevel() == MNM_SL_FACE);

	if (mFaceLevel) mUndoHide = mpData->mFaceHide;
	else mUndoHide = mpData->mVertHide;
}

EditPolyHideRestore::EditPolyHideRestore(EditPolyMod *pMod, EditPolyData *pData, bool isFaceLevel)
			: mpMod(pMod), mpData(pData), mFaceLevel(isFaceLevel) {
	mpData->SetFlag (kEPDataHeld);

	if (mFaceLevel) mUndoHide = mpData->mFaceHide;
	else mUndoHide = mpData->mVertHide;
}

void EditPolyHideRestore::After () {
	if (mFaceLevel) mRedoHide = mpData->mFaceHide;
	else mRedoHide = mpData->mVertHide;
}

void EditPolyHideRestore::Restore(int isUndo) {
	if (isUndo && mRedoHide.GetSize() == 0) After ();
	if (mFaceLevel) mpData->mFaceHide = mUndoHide;
	else mpData->mVertHide = mUndoHide;
	mpMod->EpModLocalDataChanged (PART_DISPLAY);
}

void EditPolyHideRestore::Redo() {
	if (mFaceLevel) mpData->mFaceHide = mRedoHide;
	else mpData->mVertHide = mRedoHide;
	mpMod->EpModLocalDataChanged (PART_DISPLAY);
}

