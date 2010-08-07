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

	template <typename Type>
	inline bool ElmentEquals(Type lhs, Type rhs, Type ref)
	{
		return abs(lhs - rhs) < ref;
	}

	template <typename Type1, typename Type2>
	inline bool MoveBuffer(Type1& out, const Type1& poor, Type2 poorLen, Type2 ref, Type2 step)
	{
		if (!ElmentEquals(poorLen, 0.0f, ref))
		{
			out += poor * step;
			return true;
		}

		return false;
	}
}

#endif