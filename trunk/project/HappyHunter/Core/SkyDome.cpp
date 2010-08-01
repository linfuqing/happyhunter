#include "StdAfx.h"
#include "SkyDome.h"
#include "d3dutils.h"
#include "Camera.h"

using namespace zerO;

CSkyDome::CSkyDome(void)
{
}

CSkyDome::~CSkyDome(void)
{
}

bool CSkyDome::Create(zerO::FLOAT fTheta, zerO::FLOAT fPhi, zerO::FLOAT fRadius)
{
	INT nTheta, nPhi;

	D3DXVECTOR3 UV;

	UINT i, n = 0, uNumVertices = (UINT)(360 / fTheta * 90 / fPhi * 4);

	LPVERTEX pVertices;

	m_uNumTriangles = uNumVertices - 2;

	DEBUG_NEW(pVertices, VERTEX[uNumVertices]);

	memset(pVertices, 0, sizeof(VERTEX) * uNumVertices);

	for(nPhi = 0; nPhi <= ( 90 - fPhi ); nPhi += (INT)fPhi)
	{
		for(nTheta = 0; nTheta <= (360 - fTheta ); nTheta+=(INT)fTheta )
		{
			pVertices[n].x = fRadius * sinf( TO_RADIAN(nPhi) ) * cosf(  TO_RADIAN(nTheta) );
			pVertices[n].y = fRadius * cosf( TO_RADIAN(nPhi) );
			pVertices[n].z = fRadius * sinf( TO_RADIAN(nPhi) ) * sinf(  TO_RADIAN(nTheta) );

			UV.x = pVertices[n].x;
			UV.y = pVertices[n].y;
			UV.z = pVertices[n].z;

			D3DXVec3Normalize(&UV, &UV);

			pVertices[n].u = atan2(UV.x, UV.y) / (D3DX_PI * 2) + 0.5f;
			pVertices[n].v = asinf(UV.z)       /  D3DX_PI      + 0.5f;

			n ++;

			pVertices[n].x = fRadius * sinf( TO_RADIAN(nPhi + fPhi) )*cosf( TO_RADIAN(nTheta) );
			pVertices[n].y = fRadius * cosf( TO_RADIAN(nPhi + fPhi) );
			pVertices[n].z = fRadius * sinf( TO_RADIAN(nPhi + fPhi) )*sinf( TO_RADIAN(nTheta) );
			
			UV.x = pVertices[n].x;
			UV.y = pVertices[n].y;
			UV.z = pVertices[n].z;

			D3DXVec3Normalize(&UV, &UV);

			pVertices[n].u = atan2(UV.x, UV.y) / (D3DX_PI * 2) + 0.5f;
			pVertices[n].v = asinf(UV.z)       /  D3DX_PI      + 0.5f;

			n ++;

			pVertices[n].x = fRadius * sinf( TO_RADIAN(nPhi) ) * cosf(  TO_RADIAN(nTheta + fTheta) );
			pVertices[n].y = fRadius * cosf( TO_RADIAN(nPhi) );
			pVertices[n].z = fRadius * sinf( TO_RADIAN(nPhi) ) * sinf(  TO_RADIAN(nTheta + fTheta) );

			UV.x = pVertices[n].x;
			UV.y = pVertices[n].y;
			UV.z = pVertices[n].z;

			D3DXVec3Normalize(&UV, &UV);

			pVertices[n].u = atan2(UV.x, UV.y) / (D3DX_PI * 2) + 0.5f;
			pVertices[n].v = asinf(UV.z)       /  D3DX_PI      + 0.5f;

			n ++;

			if( nPhi > - 90 && nPhi < 90 )
			{
				pVertices[n].x = fRadius * sinf( TO_RADIAN(nPhi + fPhi) ) * cosf(  TO_RADIAN(nTheta + fTheta) );
				pVertices[n].y = fRadius * cosf( TO_RADIAN(nPhi + fPhi) );
				pVertices[n].z = fRadius * sinf( TO_RADIAN(nPhi + fPhi) ) * sinf(  TO_RADIAN(nTheta + fTheta) );

				UV.x = pVertices[n].x;
				UV.y = pVertices[n].y;
				UV.z = pVertices[n].z;

				D3DXVec3Normalize(&UV, &UV);

				pVertices[n].u = atan2(UV.x, UV.y) / (D3DX_PI * 2) + 0.5f;
				pVertices[n].v = asinf(UV.z)       /  D3DX_PI      + 0.5f;

				n ++;
			}
		}
	}

	for(i = 0; i < uNumVertices - 3; i ++ )
	{
		INT i0 = i;
		INT i1 = (i + 1);
		INT i2 = (i + 2);

		if( (pVertices[i0].u - pVertices[i1].u) > 0.9f )
			pVertices[i1].u += 1.0f;

		if( (pVertices[i1].u - pVertices[i0].u) > 0.9f )
			pVertices[i0].u += 1.0f;

		if( (pVertices[i0].u - pVertices[i2].u) > 0.9f )
			pVertices[i2].u += 1.0f;

		if( (pVertices[i2].u - pVertices[i0].u) > 0.9f )
			pVertices[i0].u += 1.0f;

		if( (pVertices[i1].u - pVertices[i2].u) > 0.9f )
			pVertices[i2].u += 1.0f;

		if( (pVertices[i2].u - pVertices[i1].u) > 0.9f )
			pVertices[i1].u += 1.0f;

		if( (pVertices[i0].v - pVertices[i1].v) > 0.8f )
			pVertices[i1].v += 1.0f;

		if( (pVertices[i1].v - pVertices[i0].v) > 0.8f )
			pVertices[i0].v += 1.0f;

		if( (pVertices[i0].v - pVertices[i2].v) > 0.8f )
			pVertices[i2].v += 1.0f;

		if( (pVertices[i2].v - pVertices[i0].v) > 0.8f )
			pVertices[i0].v += 1.0f;

		if( (pVertices[i1].v - pVertices[i2].v) > 0.8f )
			pVertices[i2].v += 1.0f;

		if( (pVertices[i2].v - pVertices[i1].v) > 0.8f )
			pVertices[i1].v += 1.0f;
	}

	bool bResult = m_VertexBuffer.Create(uNumVertices, sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, pVertices, D3DFVF_XYZ | D3DFVF_TEX1);

	DEBUG_DELETE_ARRAY(pVertices);

	return bResult;
}

void CSkyDome::Render()
{
	DEVICE.SetVertexShader(NULL);
	DEVICE.SetPixelShader(NULL);

	DEVICE.SetTransform( D3DTS_WORLD,      &GetMatrix() ); 
	DEVICE.SetTransform( D3DTS_VIEW,       &CAMERA.GetViewMatrix() );
	DEVICE.SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );

	DEVICE.SetRenderState(D3DRS_LIGHTING, false);

	//DEVICE.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);
	//DEVICE.SetRenderState(D3DRS_FILLMODE, D3DFILL_WIREFRAME);

	m_VertexBuffer.Activate(0, 0, true);

	DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 , m_uNumTriangles);
}
