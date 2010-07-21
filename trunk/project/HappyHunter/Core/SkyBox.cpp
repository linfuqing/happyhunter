#include "StdAfx.h"
#include "SkyBox.h"
#include "Camera.h"

using namespace zerO;
CSkyBox::CSkyBox(void) :
m_fCloudSpeedX(0.0f),
m_fCloudSpeedY(0.0f)
{
}

CSkyBox::~CSkyBox(void)
{
}

bool CSkyBox::Create(zerO::FLOAT fSize)
{
	m_fHalfSize = fSize * 0.5f;

	///
	//----------------------------------
	//         ______
	//        |      |
	//        |      |
	//  ______|______|_____________
	// |      |      |      |      |
	// |      |      |      |      |
	// |______|______|______|______|
	//        |      |
	//        |      |
	//        |______|
	//
	//-----------------------------------
	///

	BOX Box;

	Box.Top.Vertices[0].x = - m_fHalfSize;
	Box.Top.Vertices[0].y =   m_fHalfSize;
	Box.Top.Vertices[0].z = - m_fHalfSize;

	Box.Top.Vertices[1].x =   m_fHalfSize;
	Box.Top.Vertices[1].y =   m_fHalfSize;
	Box.Top.Vertices[1].z = - m_fHalfSize;

	Box.Top.Vertices[2].x = - m_fHalfSize;
	Box.Top.Vertices[2].y =   m_fHalfSize;
	Box.Top.Vertices[2].z =   m_fHalfSize;

	Box.Top.Vertices[3].x =   m_fHalfSize;
	Box.Top.Vertices[3].y =   m_fHalfSize;
	Box.Top.Vertices[3].z =   m_fHalfSize;

	Box.Bottom.Vertices[0].x = - m_fHalfSize;
	Box.Bottom.Vertices[0].y = - m_fHalfSize;
	Box.Bottom.Vertices[0].z =   m_fHalfSize;

	Box.Bottom.Vertices[1].x =   m_fHalfSize;
	Box.Bottom.Vertices[1].y = - m_fHalfSize;
	Box.Bottom.Vertices[1].z =   m_fHalfSize;

	Box.Bottom.Vertices[2].x = - m_fHalfSize;
	Box.Bottom.Vertices[2].y = - m_fHalfSize;
	Box.Bottom.Vertices[2].z = - m_fHalfSize;

	Box.Bottom.Vertices[3].x =   m_fHalfSize;
	Box.Bottom.Vertices[3].y = - m_fHalfSize;
	Box.Bottom.Vertices[3].z = - m_fHalfSize;

	Box.Left.Vertices[0].x = - m_fHalfSize;
	Box.Left.Vertices[0].y =   m_fHalfSize;
	Box.Left.Vertices[0].z = - m_fHalfSize;

	Box.Left.Vertices[1].x = - m_fHalfSize;
	Box.Left.Vertices[1].y =   m_fHalfSize;
	Box.Left.Vertices[1].z =   m_fHalfSize;

	Box.Left.Vertices[2].x = - m_fHalfSize;
	Box.Left.Vertices[2].y = - m_fHalfSize;
	Box.Left.Vertices[2].z = - m_fHalfSize;

	Box.Left.Vertices[3].x = - m_fHalfSize;
	Box.Left.Vertices[3].y = - m_fHalfSize;
	Box.Left.Vertices[3].z =   m_fHalfSize;

	Box.Right.Vertices[0].x =   m_fHalfSize;
	Box.Right.Vertices[0].y =   m_fHalfSize;
	Box.Right.Vertices[0].z =   m_fHalfSize;

	Box.Right.Vertices[1].x =   m_fHalfSize;
	Box.Right.Vertices[1].y =   m_fHalfSize;
	Box.Right.Vertices[1].z = - m_fHalfSize;

	Box.Right.Vertices[2].x =   m_fHalfSize;
	Box.Right.Vertices[2].y = - m_fHalfSize;
	Box.Right.Vertices[2].z =   m_fHalfSize;

	Box.Right.Vertices[3].x =   m_fHalfSize;
	Box.Right.Vertices[3].y = - m_fHalfSize;
	Box.Right.Vertices[3].z = - m_fHalfSize;

	Box.Front.Vertices[0].x = - m_fHalfSize;
	Box.Front.Vertices[0].y =   m_fHalfSize;
	Box.Front.Vertices[0].z =   m_fHalfSize;

	Box.Front.Vertices[1].x =   m_fHalfSize;
	Box.Front.Vertices[1].y =   m_fHalfSize;
	Box.Front.Vertices[1].z =   m_fHalfSize;

	Box.Front.Vertices[2].x = - m_fHalfSize;
	Box.Front.Vertices[2].y = - m_fHalfSize;
	Box.Front.Vertices[2].z =   m_fHalfSize;

	Box.Front.Vertices[3].x =   m_fHalfSize;
	Box.Front.Vertices[3].y = - m_fHalfSize;
	Box.Front.Vertices[3].z =   m_fHalfSize;

	Box.Back.Vertices[0].x =   m_fHalfSize;
	Box.Back.Vertices[0].y =   m_fHalfSize;
	Box.Back.Vertices[0].z = - m_fHalfSize;

	Box.Back.Vertices[1].x = - m_fHalfSize;
	Box.Back.Vertices[1].y =   m_fHalfSize;
	Box.Back.Vertices[1].z = - m_fHalfSize;

	Box.Back.Vertices[2].x =   m_fHalfSize;
	Box.Back.Vertices[2].y = - m_fHalfSize;
	Box.Back.Vertices[2].z = - m_fHalfSize;

	Box.Back.Vertices[3].x = - m_fHalfSize;
	Box.Back.Vertices[3].y = - m_fHalfSize;
	Box.Back.Vertices[3].z = - m_fHalfSize;

	Box.Top.Vertices[0].u = 0.0f;
	Box.Top.Vertices[0].v = 0.0f;

	Box.Top.Vertices[1].u = 1.0f;
	Box.Top.Vertices[1].v = 0.0f;

	Box.Top.Vertices[2].u = 0.0f;
	Box.Top.Vertices[2].v = 1.0f;

	Box.Top.Vertices[3].u = 1.0f;
	Box.Top.Vertices[3].v = 1.0f;

	Box.Bottom.Vertices[0].u = 0.0f;
	Box.Bottom.Vertices[0].v = 0.0f;

	Box.Bottom.Vertices[1].u = 1.0f;
	Box.Bottom.Vertices[1].v = 0.0f;

	Box.Bottom.Vertices[2].u = 0.0f;
	Box.Bottom.Vertices[2].v = 1.0f;

	Box.Bottom.Vertices[3].u = 1.0f;
	Box.Bottom.Vertices[3].v = 1.0f;

	Box.Left.Vertices[0].u = 0.0f;
	Box.Left.Vertices[0].v = 0.0f;

	Box.Left.Vertices[1].u = 1.0f;
	Box.Left.Vertices[1].v = 0.0f;

	Box.Left.Vertices[2].u = 0.0f;
	Box.Left.Vertices[2].v = 1.0f;

	Box.Left.Vertices[3].u = 1.0f;
	Box.Left.Vertices[3].v = 1.0f;

	Box.Right.Vertices[0].u = 0.0f;
	Box.Right.Vertices[0].v = 0.0f;

	Box.Right.Vertices[1].u = 1.0f;
	Box.Right.Vertices[1].v = 0.0f;

	Box.Right.Vertices[2].u = 0.0f;
	Box.Right.Vertices[2].v = 1.0f;

	Box.Right.Vertices[3].u = 1.0f;
	Box.Right.Vertices[3].v = 1.0f;

	Box.Front.Vertices[0].u = 0.0f;
	Box.Front.Vertices[0].v = 0.0f;

	Box.Front.Vertices[1].u = 1.0f;
	Box.Front.Vertices[1].v = 0.0f;

	Box.Front.Vertices[2].u = 0.0f;
	Box.Front.Vertices[2].v = 1.0f;

	Box.Front.Vertices[3].u = 1.0f;
	Box.Front.Vertices[3].v = 1.0f;

	Box.Back.Vertices[0].u = 0.0f;
	Box.Back.Vertices[0].v = 0.0f;

	Box.Back.Vertices[1].u = 1.0f;
	Box.Back.Vertices[1].v = 0.0f;

	Box.Back.Vertices[2].u = 0.0f;
	Box.Back.Vertices[2].v = 1.0f;

	Box.Back.Vertices[3].u = 1.0f;
	Box.Back.Vertices[3].v = 1.0f;

	return m_VertexBuffer.Create(24, sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, (void*)&Box, D3DFVF_XYZ | D3DFVF_TEX1);
}

void CSkyBox::Update()
{
	CBackground::Update();
}

void CSkyBox::Render()
{
#define SET_TEXTURE(i) \
	if( TEST_BIT(m_RenderMethod.GetSurface()->GetTextureFlag(), i) ) \
	{\
		pEffect->SetTexture( 0, *m_RenderMethod.GetSurface()->GetTexture(i) );\
		pEffect->GetEffect()->CommitChanges();\
	}

	CEffect* pEffect = m_RenderMethod.GetEffect();

	if(pEffect)
	{
		DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, true         );
		DEVICE.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA   );
		DEVICE.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

		pEffect->Begin();

		pEffect->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, GetMatrix() * CAMERA.GetViewProjectionMatrix() );

		m_VertexBuffer.Activate(0, 0, true);

		const D3DMATERIAL9& MATERIAL = m_RenderMethod.GetSurface()->GetMaterial();
		pEffect->SetParameter(CEffect::AMBIENT_MATERIAL_COLOR, &MATERIAL.Ambient);
		pEffect->SetParameter(CEffect::DIFFUSE_MATERIAL_COLOR, &MATERIAL.Diffuse);
		pEffect->SetParameter(CEffect::EMISSIVE_MATERIAL_COLOR, &MATERIAL.Emissive);
		pEffect->SetParameter(CEffect::SPECULAR_MATERIAL_COLOR, &MATERIAL.Specular);
		pEffect->SetParameter(CEffect::SPECULAR_MATERIAL_POWER, &MATERIAL.Power);

		UINT uTotalPass = pEffect->GetTechniqueDesc().Passes, i;

		D3DXVECTOR2 UVOffset(0.0f, 0.0f);

		static FLOAT fUVOffsetX = 0.0f, fUVOffsetY = 0.0f;

		pEffect->SetParameter(CEffect::UV, &UVOffset);

		for (i = 0; i < uTotalPass; i ++)
		{
			pEffect->GetEffect()->BeginPass(i);

			SET_TEXTURE(0);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 , 2);

			SET_TEXTURE(1);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 4 , 2);

			SET_TEXTURE(2);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 8 , 2);

			SET_TEXTURE(3);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 12, 2);

			SET_TEXTURE(4);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 16, 2);

			SET_TEXTURE(5);
			DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 20, 2);

			if( TEST_BIT(m_RenderMethod.GetSurface()->GetTextureFlag(), 6) ) 
			{
				fUVOffsetX += m_fCloudSpeedX;
				fUVOffsetY += m_fCloudSpeedY;

				UVOffset.x = fUVOffsetX;
				UVOffset.y = fUVOffsetY;
				pEffect->SetParameter(CEffect::UV, &UVOffset);

				pEffect->SetTexture( 0, *m_RenderMethod.GetSurface()->GetTexture(6) );
				pEffect->GetEffect()->CommitChanges();
				DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0 , 2);

				UVOffset.x = 0.0f;
				UVOffset.y = 0.0f;
				pEffect->SetParameter(CEffect::UV, &UVOffset);
			}

			pEffect->GetEffect()->EndPass();
		}

		pEffect->End();

		DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false        );
	}

#undef SET_TEXTURE
}