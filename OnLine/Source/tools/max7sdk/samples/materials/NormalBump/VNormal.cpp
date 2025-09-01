/**********************************************************************
 *<
	FILE: VNormal.cpp

	DESCRIPTION:

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#include "VNormal.h"



//===========================================================================
//
// VNormalArray
//
// Holds a normal vector and related data at each vertex & smoothing group;
// that is, one piece of data per vertex per smoothing group at the vertex.
//
//===========================================================================

void VNormalArray::AddNode( int vertIndex, int smGroup ) {
	if( smGroup==0 ) return;
	ListNode* node = &(lists[vertIndex]);
	while( (node->next!=NULL) && (node->smGroup!=0) && !(node->smGroup&smGroup) )
		node = node->next;
	if( (node->smGroup!=0) && !(node->smGroup&smGroup) ) {
		node->next = new ListNode;
		node->next->Init();
		node->next->smGroup = smGroup;
	}
	else node->smGroup |= smGroup;
}

int VNormalArray::FindNode( int vertIndex, int smGroup ) {
	if( smGroup==0 ) return -1;
	int index = indices[vertIndex];
	ListNode* node = &(lists[vertIndex]);
	while( (node!=NULL) && !(node->smGroup&smGroup) )
		node = node->next, index++;
	return (node==NULL?  -1 : index );
}

void VNormalArray::Init( Mesh* mesh ) {
	Free();

	// Allocate the lookup list
	int numVerts = mesh->numVerts;
	lists.SetCount(numVerts);
	for( int i=0; i<numVerts; i++ ) lists[i].Init();

	// Initialize the list values
	int numTri = mesh->numFaces;
	BOOL anySmoothed = FALSE, anyUnsmoothed = FALSE;

	for( i=0; i<numTri; i++ ) {
		DWORD smGroup = mesh->faces[i].smGroup;
		if( smGroup==0 )
			anyUnsmoothed = TRUE;
		else {
			anySmoothed = TRUE;
			Face fc = mesh->faces[i];
			//GenTri tri = mesh->GetTri(i);
			AddNode( fc.v[0], smGroup );
			AddNode( fc.v[1], smGroup );
			AddNode( fc.v[2], smGroup );
		}
	}

	// Initialize the indices
	int curIndex=0;
	if( anySmoothed ) {
		indices.SetCount( numVerts );
		for( i=0; i<numVerts; i++, curIndex++ ) {
			indices[i] = curIndex;
			ListNode* node = &(lists[i]);
			while( node->next!=NULL ) node=node->next, curIndex++;
		}
	}

	nodeCount = (anySmoothed? curIndex : 0);
	faceCount = (anyUnsmoothed? numTri : 0);
	valid = TRUE;
}

void VNormalArray::Free() {
	for( int i=0; i<lists.Count(); i++ ) {
		ListNode *prev, *next = lists[i].next;
		while( next!=NULL ) {
			prev = next, next = next->next;
			delete prev;
		}
	}

	for( i=0; i<vertexTangentBasisSet.Count(); i++ )
		if( vertexTangentBasisSet[i]!=NULL )	delete[] vertexTangentBasisSet[i];
	for( i=0; i<faceTangentBasisSet.Count(); i++ )
		if( faceTangentBasisSet[i]!=NULL )		delete[] faceTangentBasisSet[i];

	indices.SetCount(0);
	lists.SetCount(0);

	//normals.SetCount(0);
	vertexTangentBasisSet.SetCount(0);
	faceTangentBasisSet.SetCount(0);

	nodeCount = faceCount = 0;
	valid = FALSE;
}

BOOL VNormalArray::Valid() { return valid; }

#define zeroVector Point3(0,0,0)

/*
void VNormalArray::InitNormals() {
	if( normals.Count()<count ) {
		normals.SetCount(count);
		for( int i=0; i<count; i++ ) normals[i]=zeroVector;
	}
}

BOOL VNormalArray::ValidNormals() {
	return (valid && (normals.Count()==count)? TRUE:FALSE);
}

Point3& VNormalArray::GetNormal( int vertIndex, DWORD smGroup ) {
	int index = Find( vertIndex, smGroup );
	DbgAssert( (index>=0) && (index<count) && (normals.Count()==count) );
	return normals[index];
}

void VNormalArray::AddNormal( int vertIndex, DWORD smGroup, Point3& normal ) {
	int index = Find( vertIndex, smGroup );
	DbgAssert( (index>=0) && (index<count) && ValidNormals() );
	normals[index] += normal;

}

void VNormalArray::NormalizeNormals(  ) {
	DbgAssert( ValidNormals() );
	for( int i=0; i<normals.Count(); i++ )
		normals[i] = normals[i].Normalize();
}
*/

void VNormalArray::InitTangentBasis(int mapChannel) {
	int m = mapChannel + NUM_HIDDENMAPS;
	TangentBasis* null = NULL;

	if( vertexTangentBasisSet.Count()<(m+1) ) {
		for( int i=vertexTangentBasisSet.Count(); i<(m+1); i++ )
			vertexTangentBasisSet.Append(1,&null);
		if( nodeCount>0 )	vertexTangentBasisSet[m] = new TangentBasis[ nodeCount ];
		else				vertexTangentBasisSet[m] = null;
		for( i=0; i<nodeCount; i++ ) vertexTangentBasisSet[m][i]=zeroTangentBasis;
	}

	if( faceTangentBasisSet.Count()<(m+1) ) {
		for( int i=faceTangentBasisSet.Count(); i<(m+1); i++ )
			faceTangentBasisSet.Append(1,&null);
		if( faceCount>0 )	faceTangentBasisSet[m] = new TangentBasis[ faceCount ];
		else				faceTangentBasisSet[m] = null;
		for( i=0; i<faceCount; i++ ) faceTangentBasisSet[m][i]=zeroTangentBasis;
	}}

BOOL VNormalArray::ValidTangentBasis(int mapChannel) {
	int m = mapChannel + NUM_HIDDENMAPS;
	if( !valid || (vertexTangentBasisSet.Count()<=m) || (faceTangentBasisSet.Count()<=m) )
		return FALSE;

	return ((vertexTangentBasisSet[m]!=NULL) || (faceTangentBasisSet[m]!=NULL)? TRUE:FALSE);
}

TangentBasis& VNormalArray::GetVertexTangentBasis( int vertIndex, int smGroup, int mapChannel ) {
	int index = FindNode( vertIndex, smGroup );
	int m = mapChannel + NUM_HIDDENMAPS;
	DbgAssert( (index>=0) && (index<nodeCount) && ValidTangentBasis(mapChannel) );
	return vertexTangentBasisSet[m][index];
}

void VNormalArray::AddVertexTangentBasis( int vertIndex, int smGroup, int mapChannel, TangentBasis& tangentBasis ) {
	int index = FindNode( vertIndex, smGroup );
	int m = mapChannel + NUM_HIDDENMAPS;
	DbgAssert( (index>=0) && (index<nodeCount) && ValidTangentBasis(mapChannel) );
	vertexTangentBasisSet[m][index].uBasis += tangentBasis.uBasis;
	vertexTangentBasisSet[m][index].vBasis += tangentBasis.vBasis;
}

TangentBasis& VNormalArray::GetFaceTangentBasis( int faceIndex, int mapChannel ) {
	int m = mapChannel + NUM_HIDDENMAPS;
	DbgAssert( (faceIndex>=0) && (faceIndex<faceCount) && ValidTangentBasis(mapChannel) );
	return faceTangentBasisSet[m][faceIndex];
}

void VNormalArray::AddFaceTangentBasis( int faceIndex, int mapChannel, TangentBasis& tangentBasis ) {
	int m = mapChannel + NUM_HIDDENMAPS;
	DbgAssert( (faceIndex>=0) && (faceIndex<faceCount) && ValidTangentBasis(mapChannel) );
	faceTangentBasisSet[m][faceIndex].uBasis += tangentBasis.uBasis;
	faceTangentBasisSet[m][faceIndex].vBasis += tangentBasis.vBasis;
}

void VNormalArray::NormalizeTangentBasis( int mapChannel ) {
	DbgAssert( ValidTangentBasis(mapChannel) );
	int m = mapChannel + NUM_HIDDENMAPS;
	for( int i=0; i<nodeCount; i++ ) {
		vertexTangentBasisSet[m][i].uBasis = vertexTangentBasisSet[m][i].uBasis.Normalize();
		vertexTangentBasisSet[m][i].vBasis = vertexTangentBasisSet[m][i].vBasis.Normalize();
	}
	for( int i=0; i<faceCount; i++ ) {
		faceTangentBasisSet[m][i].uBasis = faceTangentBasisSet[m][i].uBasis.Normalize();
		faceTangentBasisSet[m][i].vBasis = faceTangentBasisSet[m][i].vBasis.Normalize();
	}
}



//===========================================================================
//
// Class VNormalMgr
//
// Holds a normal vector and related data at each vertex & smoothing group;
// that is, one piece of data per vertex per smoothing group at the vertex.
//
//===========================================================================

VNormalMgr theVNormalMgr;


VNormalMgr::VNormalMgr() {
	InitializeCriticalSection( &criticalSection );
}

VNormalMgr::~VNormalMgr() {
	Free();
	DeleteCriticalSection( &criticalSection );
}


void VNormalMgr::Free() {
	for( int i=0; i<vnormalArrays.Count(); i++ )
		if( vnormalArrays[i]!=NULL )
			delete vnormalArrays[i];
	vnormalArrays.SetCount(0);
}

void VNormalMgr::Init( int nodeID, int mapChannel ) {
	EnterCriticalSection( &criticalSection );
	if( Valid(nodeID,mapChannel) ) { // initialized while waiting on the semaphore?
		LeaveCriticalSection( &criticalSection );
		return;
	}

	VNormalArray* null = NULL;
	int lo = vnormalArrays.Count(), hi = nodeID;
	for( int i=lo; i<=hi; i++ ) vnormalArrays.Append( 1, &null );

	if( vnormalArrays[nodeID]!=NULL ) delete vnormalArrays[nodeID];
	vnormalArrays[nodeID] = new VNormalArray;

	LeaveCriticalSection( &criticalSection );
}

BOOL VNormalMgr::Valid( int nodeID, int mapChannel ) {
	if( vnormalArrays.Count()<=nodeID ) return FALSE;
	VNormalArray *vnormalArray = vnormalArrays[nodeID];
	if( vnormalArray==NULL ) return FALSE;
	return TRUE;
}

void VNormalMgr::InitTangentBasis( ShadeContext& sc, int mapChannel ) {
	int nodeID = sc.NodeID();
	RenderGlobalContext* rgc = sc.globContext;
	if( rgc==NULL ) return;
	RenderInstance* inst = rgc->GetRenderInstance(nodeID);
	if( (inst==NULL) || (inst->mesh==NULL) ) return;

	InitTangentBasis( *(inst->mesh), inst->objToCam, nodeID, mapChannel );
}

void VNormalMgr::InitTangentBasis( Mesh& mesh, Matrix3& tm, int nodeID, int mapChannel ) {
	Init(nodeID,mapChannel); //should be harmless if already initialized

	EnterCriticalSection( &criticalSection );
	if( ValidTangentBasis(nodeID,mapChannel) ) { // initialized while waiting on the semaphore?
		LeaveCriticalSection( &criticalSection );
		return;
	}

	VNormalArray* vnormalArray = vnormalArrays[nodeID];
	vnormalArray->Init(&mesh);
	vnormalArray->InitTangentBasis(mapChannel);

	TangentBasis tangentBasis;

	MeshMap& map = mesh.Map(mapChannel);
	int numFaces = mesh.numFaces;
	Point3 *geomVerts = mesh.verts, *mapVerts = map.tv;
	Point3 mapTri[3], geomTri[3], bv[2];

	if( (mesh.numMaps<=mapChannel) || (map.fnum<=0) || (map.vnum<=0) || (map.tf==NULL) || (map.tv==NULL) )
		numFaces = 0; //Error checking; Bail out if the map is not valid

	for (int i=0; i<numFaces; i++ )
	{
		Face& geomFace = mesh.faces[i];
		geomTri[0] = geomVerts[ geomFace.v[0] ];
		geomTri[1] = geomVerts[ geomFace.v[1] ];
		geomTri[2] = geomVerts[ geomFace.v[2] ];

		TVFace& mapFace = map.tf[i];
		mapTri[0] = mapVerts[ mapFace.t[0] ];
		mapTri[1] = mapVerts[ mapFace.t[1] ];
		mapTri[2] = mapVerts[ mapFace.t[2] ];

		ComputeTangentAndBinormal( mapTri, geomTri, bv );
		tangentBasis.uBasis = tm.VectorTransform( bv[0] );
		tangentBasis.vBasis = tm.VectorTransform( bv[1] );

		if( geomFace.smGroup==0 )
			vnormalArray->AddFaceTangentBasis( i, mapChannel, tangentBasis );
		else {
			vnormalArray->AddVertexTangentBasis( geomFace.v[0], geomFace.smGroup, mapChannel, tangentBasis );
			vnormalArray->AddVertexTangentBasis( geomFace.v[1], geomFace.smGroup, mapChannel, tangentBasis );
			vnormalArray->AddVertexTangentBasis( geomFace.v[2], geomFace.smGroup, mapChannel, tangentBasis );
		}
	}

	vnormalArray->NormalizeTangentBasis( mapChannel );
	LeaveCriticalSection( &criticalSection );
}

BOOL VNormalMgr::ValidTangentBasis( int nodeID, int mapChannel ) {
	if( !Valid( nodeID, mapChannel ) ) return FALSE;

	VNormalArray *vnormalArray = vnormalArrays[nodeID];
	if( !vnormalArray->ValidTangentBasis(mapChannel) ) return FALSE;
	return TRUE;
}

VNormalArray* VNormalMgr::GetVNormalArray( int nodeID ) {
	DbgAssert( nodeID<vnormalArrays.Count() );
	return vnormalArrays[nodeID];
}

void VNormalMgr::GetTangentBasis( ShadeContext& sc, int mapChannel, TangentBasis& tangentBasis ) {
	DbgAssert( sc.globContext!=NULL );
	int nodeID = sc.NodeID();
	RenderInstance* inst = sc.globContext->GetRenderInstance(nodeID);
	DbgAssert( (inst!=NULL) && (inst->mesh!=NULL) );

	int faceIndex = sc.FaceNumber();
	Face& f = inst->mesh->faces[faceIndex];
	DWORD smGroup = f.smGroup;

	DbgAssert( ValidTangentBasis( nodeID, mapChannel ) );
	VNormalArray *vnormalArray = GetVNormalArray( nodeID );

	if( smGroup==0 )
		tangentBasis = vnormalArray->GetFaceTangentBasis( faceIndex, mapChannel );
	else {
		DWORD *v = f.v;
		TangentBasis& b0 = vnormalArray->GetVertexTangentBasis( v[0], smGroup, mapChannel ); //returned in camera space
		TangentBasis& b1 = vnormalArray->GetVertexTangentBasis( v[1], smGroup, mapChannel ); //returned in camera space
		TangentBasis& b2 = vnormalArray->GetVertexTangentBasis( v[2], smGroup, mapChannel ); //returned in camera space

		Point3 bary = sc.BarycentricCoords();
		tangentBasis.uBasis = ((bary.x*b0.uBasis) + (bary.y*b1.uBasis) + (bary.z*b2.uBasis)).Normalize();
		tangentBasis.vBasis = ((bary.x*b0.vBasis) + (bary.y*b1.vBasis) + (bary.z*b2.vBasis)).Normalize();
	}
}