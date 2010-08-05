#ifndef D3DUTILS_H
#define D3DUTILS_H

#include "datatype.h"

namespace zerO
{
#define USER_PI ((zerO::FLOAT)  3.141592654f)
#define TO_DEGREE(x) ( (x) * 180.0f / D3DX_PI )
#define TO_RADIAN(x) ( (x) * D3DX_PI / 180.0f )

	void UpdateSkinnedMesh(    
		const D3DXMATRIX *pBoneTransforms,  
		UINT uNumBones,     
		const void* pVerticesSrc,    
		void* pVerticesDst, 
		UINT uNumTotalVertices,    
		UINT uStride,
		PUINT puNumBoneInfluences, 
		LPDWORD* ppdwVerticesIndices,
		PFLOAT* ppfWeights,
		const D3DXVECTOR3* pTangentSrc = NULL,
		D3DXVECTOR3* pTangentDst = NULL);

	void DirectionToRotation(FLOAT& x, FLOAT& y, const D3DXVECTOR3& Direction);
	void SceneDirectionToRotation(FLOAT& x, FLOAT& y, const D3DXVECTOR3& Direction);

	// »º¶¯Ð§¹û£¨»º³å£©
	bool MoveBuffer(
		D3DXVECTOR3& outRot, 
		D3DXVECTOR3& outTrans, 
		const D3DXVECTOR3& rotPoor, 
		const D3DXVECTOR3& transPoor, 
		FLOAT rotRef, 
		FLOAT transRef, 
		FLOAT step);

	inline bool FloatEquals(FLOAT lhs, FLOAT rhs, FLOAT ref)
	{
		return abs(lhs - rhs) < ref;
	}
}

#endif