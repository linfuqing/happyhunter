#include "StdAfx.h"
#include "Billboarding.h"
#include "Camera.h"

using namespace zerO;

CBillboarding::CBillboarding(void)
{
}

CBillboarding::~CBillboarding(void)
{
}

CSurface& CBillboarding::GetSurface()
{
	return m_Surface;
}

bool CBillboarding::Create(zerO::FLOAT fWidth, zerO::FLOAT fHeight)
{
	m_fHalfWidth  = fWidth  / 2.0f;
	m_fHalfHeight = fHeight / 2.0f;

	VERTEX Vertices[4];

	Vertices[0].Position = D3DXVECTOR3(- m_fHalfWidth, - m_fHalfHeight, 0.0f);
	Vertices[0].UV       = D3DXVECTOR2(0.0f, 0.0f);

	Vertices[1].Position = D3DXVECTOR3(- m_fHalfWidth,   m_fHalfHeight, 0.0f);
	Vertices[1].UV       = D3DXVECTOR2(0.0f, 1.0f);

	Vertices[2].Position = D3DXVECTOR3(  m_fHalfWidth, - m_fHalfHeight, 0.0f);
	Vertices[2].UV       = D3DXVECTOR2(1.0f, 0.0f);

	Vertices[3].Position = D3DXVECTOR3(  m_fHalfWidth,   m_fHalfHeight, 0.0f);
	Vertices[3].UV       = D3DXVECTOR2(1.0f, 1.0f);

	if( !m_VertexBuffer.Create(4, sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, Vertices, D3DFVF_XYZ | D3DFVF_TEX1) )
		return false;

	return true;
}

void CBillboarding::Update()
{
	CSceneNode::Update();
}

bool CBillboarding::ApplyForRender()
{
	CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();
			
	//pRenderEntry->hEffect      = pEffect->GetHandle();
	pRenderEntry->hSurface     = m_Surface.GetHandle();
	pRenderEntry->uModelType   = CRenderQueue::RENDERENTRY::BUFFER_TYPE;
	pRenderEntry->hModel       = m_VertexBuffer.GetHandle();
	//pRenderEntry->uRenderPass  = i;
	pRenderEntry->pParent      = this;

	GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);

	return true;
}

void CBillboarding::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	/*if( TEST_BIT(uFlag, CRenderQueue::EFFECT) )
			pEffect->Begin();*/

	//pEffect->GetEffect()->BeginPass(pEntry->uRenderPass);

	if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
	{
		DEVICE.SetTransform( D3DTS_WORLD,      &m_WorldMatrix); 
		DEVICE.SetTransform( D3DTS_VIEW,       &CAMERA.GetViewMatrix() );
		DEVICE.SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );
		m_VertexBuffer.Activate(0, 0, true);
	}

	if ( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
		m_Surface.Activate();

	DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);
}