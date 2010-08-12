#include "StdAfx.h"
#include "FullScreenEffect.h"

using namespace zerO;

CFullScreenEffect::CFullScreenEffect(void) :
CResource(RESUORCE_FULLSCREENEFFECT)
{
}

CFullScreenEffect::~CFullScreenEffect(void)
{
}

bool CFullScreenEffect::Restore()
{
	FLOAT sx = (FLOAT)GAMEHOST.GetBackBufferSurfaceDesc().Width;
	FLOAT sy = (FLOAT)GAMEHOST.GetBackBufferSurfaceDesc().Height;

	m_Vertices[0].Position = D3DXVECTOR4(0 , sy, 0.0f, 1.0f);
	m_Vertices[1].Position = D3DXVECTOR4(0 ,  0, 0.0f, 1.0f);
	m_Vertices[2].Position = D3DXVECTOR4(sx, sy, 0.0f, 1.0f);
	m_Vertices[3].Position = D3DXVECTOR4(sx,  0, 0.0f, 1.0f);
	m_Vertices[0].UV       = D3DXVECTOR2(0.0f, 1.0f);
	m_Vertices[1].UV       = D3DXVECTOR2(0.0f, 0.0f);
	m_Vertices[2].UV       = D3DXVECTOR2(1.0f, 1.0f);
	m_Vertices[3].UV       = D3DXVECTOR2(1.0f, 0.0f);

	return true;
}

void CFullScreenEffect::Render(CTexturePrinter& TexturePrinter)
{
	DEVICE.SetTextureStageState( 0, D3DTSS_COLOROP, D3DTOP_MODULATE );
	DEVICE.SetTextureStageState( 0, D3DTSS_COLORARG1, D3DTA_TEXTURE );
	DEVICE.SetTextureStageState( 0, D3DTSS_COLORARG2, D3DTA_CURRENT );
	DEVICE.SetTextureStageState( 0, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1 );
	DEVICE.SetTextureStageState( 0, D3DTSS_ALPHAARG1, D3DTA_TEXTURE );
	DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
	DEVICE.SetFVF(D3DFVF_XYZRHW | D3DFVF_TEX1);
	DEVICE.SetTexture( 0, TexturePrinter.GetTexture(0)->GetTexture() );
	DEVICE.DrawPrimitiveUP( D3DPT_TRIANGLESTRIP, 2, m_Vertices, sizeof(VERTEX) );
}