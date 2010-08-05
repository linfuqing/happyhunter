#ifndef D3DUTILS_H
#define D3DUTILS_H

#include "datatype.h"

namespace zerO
{
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
}

#endif