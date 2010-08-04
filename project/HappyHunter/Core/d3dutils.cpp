#include "StdAfx.h"
#include "d3dutils.h"

using namespace zerO;

void zerO::UpdateSkinnedMesh(
                  const D3DXMATRIX *pBoneTransforms,
				  zerO::UINT uNumBones,
                  const void* pVerticesSrc,
                  void* pVerticesDst,
				  zerO::UINT uNumTotalVertices,
                  zerO::UINT uStride,
				  zerO::PUINT puNumBoneInfluences,
				  LPDWORD* ppdwVerticesIndices,
				  zerO::PFLOAT* ppfWeights,
				  const D3DXVECTOR3* pTangentSrc,
				  D3DXVECTOR3* pTangentDst
                  )

{
	UINT i, uNumVertices, uOffsetByte, uNormalByte, uIndex, VectorSize = sizeof(D3DXVECTOR3);
	
	FLOAT fWeight;

	UINT8 *pDest = (zerO::PUINT8)pVerticesDst;

	D3DXVECTOR3 *Src, Temp;

	const UINT8 *pSrc = (zerO::PUINT8)pVerticesSrc;

	memcpy(pDest, pSrc, uNumTotalVertices * uStride);

	for(i = 0; i < uNumBones; i++)
	{
	   uNumVertices = puNumBoneInfluences[i];

	   if(uNumVertices <= 0)
		   continue;

	   while(uNumVertices --)
	   {
		   uIndex = ppdwVerticesIndices[i][uNumVertices];

		   fWeight = ppfWeights[i][uNumVertices];

		   uOffsetByte = uIndex * uStride;

		   uNormalByte = uOffsetByte + VectorSize;

		   Src = (D3DXVECTOR3 *)(pSrc + uOffsetByte);

		   D3DXVec3TransformCoord(&Temp, Src, &pBoneTransforms[i]);

		   *( (D3DXVECTOR3 *)(pDest + uOffsetByte) ) += (Temp - *Src) * fWeight;

		   Src = (D3DXVECTOR3 *)(pSrc + uNormalByte);  //·¨Ïß

		   D3DXVec3TransformNormal(&Temp, Src, &pBoneTransforms[i]);

		   *( (D3DXVECTOR3 *)(pDest + uNormalByte) ) += (Temp - *Src) * fWeight;
	   }
	}

	if(pTangentSrc && pTangentDst)
	{
		memcpy(pTangentDst, pTangentSrc, uNumTotalVertices * VectorSize);

		for(i = 0; i < uNumBones; i++)
		{
		   uNumVertices = puNumBoneInfluences[i];

		   if(uNumVertices <= 0)
			   continue;

		   while(uNumVertices --)
		   {

			   uIndex = ppdwVerticesIndices[i][uNumVertices];

			   fWeight = ppfWeights[i][uNumVertices];

			   D3DXVec3TransformNormal(&Temp, &pTangentSrc[uIndex], &pBoneTransforms[i]);

			   pTangentDst[uIndex] += (Temp - pTangentSrc[uIndex]) * fWeight;
		   }
		}
	}
}