#include "StdAfx.h"
#include "Billboarding.h"
#include "Camera.h"

using namespace zerO;

CBillboarding::CBillboarding(void) :
m_bIsPlay(false),
m_Type(UNLOCK)
{
}

CBillboarding::~CBillboarding(void)
{
}

bool CBillboarding::Create(zerO::FLOAT fWidth, zerO::FLOAT fHeight)
{
	m_fHalfWidth  = fWidth  / 2.0f;
	m_fHalfHeight = fHeight / 2.0f;

	m_fOffsetU = 1.0f;
	m_fOffsetV = 1.0f;

	m_CurrentUV.Set(0.0f, 1.0f, 0.0f, 1.0f);
	m_MaxUV.Set(0.0f, 1.0f, 0.0f, 1.0f);

	m_RenderData[0].Position = D3DXVECTOR3(- m_fHalfWidth, - m_fHalfHeight, 0.0f);
	m_RenderData[0].UV       = D3DXVECTOR2(0.0f, 0.0f);

	m_RenderData[1].Position = D3DXVECTOR3(- m_fHalfWidth,   m_fHalfHeight, 0.0f);
	m_RenderData[1].UV       = D3DXVECTOR2(0.0f, 1.0f);

	m_RenderData[2].Position = D3DXVECTOR3(  m_fHalfWidth, - m_fHalfHeight, 0.0f);
	m_RenderData[2].UV       = D3DXVECTOR2(1.0f, 0.0f);

	m_RenderData[3].Position = D3DXVECTOR3(  m_fHalfWidth,   m_fHalfHeight, 0.0f);
	m_RenderData[3].UV       = D3DXVECTOR2(1.0f, 1.0f);

	if( !m_VertexBuffer.Create(4, sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, m_RenderData, D3DFVF_XYZ | D3DFVF_TEX1) )
		return false;

	return true;
}

void CBillboarding::Update()
{
	CSceneNode::Update();

	if(!m_bIsPlay)
		return;

	if(m_CurrentUV.GetMaxX() > m_MaxUV.GetMaxX() - m_fOffsetU)
	{
		m_CurrentUV.GetMinX() = m_MaxUV.GetMinX();
		m_CurrentUV.GetMaxX() = m_MaxUV.GetMinX() + m_fOffsetU;
	}
	else
	{
		m_CurrentUV.GetMinX() += m_fOffsetU;
		m_CurrentUV.GetMaxX() += m_fOffsetU;
	}

	if(m_CurrentUV.GetMaxY() > m_MaxUV.GetMaxY() - m_fOffsetV)
	{
		m_CurrentUV.GetMinY() = m_MaxUV.GetMinY();
		m_CurrentUV.GetMaxY() = m_MaxUV.GetMinY() + m_fOffsetV;
	}
	else
	{
		m_CurrentUV.GetMinY() += m_fOffsetV;
		m_CurrentUV.GetMaxY() += m_fOffsetV;
	}

	m_RenderData[0].UV.x = m_CurrentUV.GetMinX();
	m_RenderData[0].UV.y = m_CurrentUV.GetMinY();

	m_RenderData[1].UV.x = m_CurrentUV.GetMinX();
	m_RenderData[1].UV.y = m_CurrentUV.GetMaxY();

	m_RenderData[2].UV.x = m_CurrentUV.GetMaxX();
	m_RenderData[2].UV.y = m_CurrentUV.GetMinY();

	m_RenderData[3].UV.x = m_CurrentUV.GetMaxX();
	m_RenderData[3].UV.y = m_CurrentUV.GetMaxY();

	void* pVertices;
	m_VertexBuffer.Lock(0, &pVertices);

	memcpy( pVertices, m_RenderData, sizeof(m_RenderData) );

	m_VertexBuffer.Unlock();
}

bool CBillboarding::ApplyForRender()
{
	CEffect* pEffect = m_RenderMethod.GetEffect();

	if (pEffect)
	{
		UINT uTotalPass = pEffect->GetTechniqueDesc().Passes, i;

		for (i = 0; i < uTotalPass; i ++)
		{
			CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();
			
			pRenderEntry->hEffect      = pEffect->GetHandle();
			pRenderEntry->hSurface     = m_RenderMethod.GetSurface()->GetHandle();
			pRenderEntry->uModelType   = CRenderQueue::RENDERENTRY::BUFFER_TYPE;
			pRenderEntry->hModel       = m_VertexBuffer.GetHandle();
			pRenderEntry->uRenderPass  = i;
			pRenderEntry->pParent      = this;

			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}

		return true;
	}

	return false;
}

void CBillboarding::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	CEffect* pEffect = m_RenderMethod.GetEffect();
	
	if (pEffect)
	{	
		if( TEST_BIT(uFlag, CRenderQueue::EFFECT) )
		{
			pEffect->Begin();

			D3DXMATRIX Matrix;
			
			switch(m_Type)
			{
			case LOCK_XYZ:
				Matrix = CAMERA.GetWorldMatrix();

				Matrix._41 = m_Position.x;
				Matrix._42 = m_Position.y;
				Matrix._43 = m_Position.z;
				break;
			default:
				Matrix = m_WorldMatrix;
			}

			pEffect->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, Matrix * CAMERA.GetViewProjectionMatrix() );
		}

		pEffect->GetEffect()->BeginPass(pEntry->uRenderPass);

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
			m_VertexBuffer.Activate(0, 0, true);

		if ( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			pEffect->SetSurface( m_RenderMethod.GetSurface() );

		pEffect->GetEffect()->CommitChanges();

		DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		pEffect->GetEffect()->EndPass();
	}
}