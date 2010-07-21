#include "StdAfx.h"
#include "SkyBox.h"
#include "Camera.h"

using namespace zerO;
CSkyBox::CSkyBox(void)
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
	static CSurface::TEXITUREFLAGTYPE TextureFlag = 0;

	m_VertexBuffer.Activate(0, 0, true);

	TextureFlag = m_RenderMethod.GetSurface()->GetTextureFlag();

	DEVICE.SetTransform( D3DTS_WORLD,      &GetMatrix() ); 
	DEVICE.SetTransform( D3DTS_VIEW,       &CAMERA.GetViewMatrix() );
	DEVICE.SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );

	DEVICE.SetVertexShader(NULL);
	DEVICE.SetPixelShader(NULL);

	DEVICE.SetMaterial( &m_RenderMethod.GetSurface()->GetMaterial() );

	DEVICE.SetRenderState(D3DRS_LIGHTING, FALSE);

#define SET_TEXTURE(i) \
	if( TEST_BIT(TextureFlag, i) ) \
		DEVICE.SetTexture( 0, m_RenderMethod.GetSurface()->GetTexture(i)->GetTexture() )

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

#undef SET_TEXTURE
}