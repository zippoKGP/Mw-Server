/**********************************************************************
 *<
	FILE: VNormal.h

	DESCRIPTION:

	CREATED BY:

	HISTORY:

 *>	Copyright (c) 2004, All Rights Reserved.
 **********************************************************************/

#include "Max.h"

#ifndef __VNORMAL__H
#define __VNORMAL__H

typedef struct Basis{
	Point3 uBasis, vBasis;
} TangentBasis;

static TangentBasis zeroTangentBasis;// ={uBasis.Set(0,0,0),vBasis.Set(0,0,0)};


//===========================================================================
//
// Class VNormalArray
//
// Holds a normal vector and related data at each vertex & smoothing group;
// that is, one piece of data per vertex per smoothing group at the vertex.
//
//===========================================================================

class VNormalArray {
	public:
		VNormalArray() {valid=FALSE; nodeCount=faceCount=0;}

		~VNormalArray() {Free();}
		void				Init( Mesh* mesh );
		void				Free();
		BOOL				Valid(); //whether Init() has been called

		//void				InitNormals();
		//BOOL				ValidNormals(); //whether the normals channel exists
		//Point3&				GetNormal( int vertIndex, DWORD smGroup );
		//void				AddNormal( int vertIndex, DWORD smGroup, Point3& normal );
		//void				NormalizeNormals();

		void				InitTangentBasis(int mapChannel);
		BOOL				ValidTangentBasis(int mapChannel); //whether the tangent basis channel exists
		TangentBasis&		GetVertexTangentBasis( int vertIndex, int smGroup, int mapChannel );
		void				AddVertexTangentBasis( int vertIndex, int smGroup, int mapChannel, TangentBasis& tangentBasis );
		TangentBasis&		GetFaceTangentBasis( int faceIndex, int mapChannel );
		void				AddFaceTangentBasis( int faceIndex, int mapChannel, TangentBasis& tangentBasis );
		void				NormalizeTangentBasis(int mapChannel);

	protected:
		// A linked list of smoothing groups at a vertex
		class ListNode {
		public:
			DWORD			smGroup;
			ListNode*		next;
			void Init() {smGroup=0;next=NULL;}
		};
		void				AddNode( int vertIndex, int smGroup );
		int					FindNode( int vertIndex, int smGroup );

		// The number of node and face data allocated; may be zero depending on the smoothing group arrangement
		int					nodeCount, faceCount;

		// Lookup data; a base index and a smoothing group linked list, one each per vertex.
		// The Nth smoothing group of vertex M would have an index of (N + indices[M])
		BOOL				valid;
		Tab<int>			indices;
		Tab<ListNode>		lists;

		// Vertex data; one entry per vertex per vertex smoothing.
		// Indices into these arrays must be calculated using Find()
		//int					vertexCount, faceCount; //total number of array items
		//Tab<Point3>			normals;
		Tab<TangentBasis*>  vertexTangentBasisSet;	//one array per channel
		Tab<TangentBasis*>  faceTangentBasisSet;	//one array per channel
};



//===========================================================================
//
// Class VNormalMgr
//
//===========================================================================

// FIXME !! The ShadeContext should support this itself
class VNormalMgr {
	public:
		VNormalMgr();
		~VNormalMgr();
		void				Free(); //free all data

		void				InitTangentBasis( ShadeContext& sc, int mapChannel );
		void				InitTangentBasis( Mesh& mesh, Matrix3& tm, int nodeID, int mapChannel );
		BOOL				ValidTangentBasis( int nodeID, int mapChannel );

		// These assume Init() has already been called
		VNormalArray*		GetVNormalArray( int nodeID );
		void				GetTangentBasis( ShadeContext& sc, int mapChannel, TangentBasis& tangentBasis );

	protected:
		void				Init( int nodeID, int mapChannel );
		BOOL				Valid( int nodeID, int mapChannel );

		CRITICAL_SECTION	criticalSection;
		Tab<VNormalArray*>	vnormalArrays;
};

extern VNormalMgr theVNormalMgr;
inline VNormalMgr* GetVNormalMgr() {return &theVNormalMgr;}


#endif //__VNormal__H