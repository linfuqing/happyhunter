#include "StdAfx.h"
#include "Billboard.h"
#include "Camera.h"

using namespace zerO;

CBillboard::CBillboard(void) :
m_bIsPlay(false),
m_bIsCreated(false),
m_LockType(UNLOCK)
{
}

CBillboard::~CBillboard(void)
{
	Destroy();
}

void CBillboard::Clone(CBillboard& Billboard)const
{
	CSprite::Clone(Billboard);

	Billboard.m_fWidth        = m_fWidth;
	Billboard.m_fHeight       = m_fHeight;
	Billboard.m_fOffsetU      = m_fOffsetU;
	Billboard.m_fOffsetV      = m_fOffsetV;

	Billboard.m_CurrentUV     = m_CurrentUV;
	Billboard.m_MaxUV         = m_MaxUV;

	Billboard.m_bIsPlay       = m_bIsPlay;

	Billboard.m_LockType      = m_LockType;

	Billboard.m_pVertexBuffer = m_pVertexBuffer;

	memcpy( Billboard.m_RenderData, m_RenderData, sizeof(m_RenderData) );
	m_RenderMethod.Clone(Billboard.m_RenderMethod);
}

bool CBillboard::Create(zerO::FLOAT fWidth, zerO::FLOAT fHeight, const D3DXVECTOR3* pCenter)
{
	m_fWidth          = fWidth;
	m_fHeight         = fHeight;

	FLOAT fHalfWidth  = fWidth  / 2.0f;
	FLOAT fHalfHeight = fHeight / 2.0f;

	m_fOffsetU = 1.0f;
	m_fOffsetV = 1.0f;

	m_CurrentUV.Set(0.0f, 1.0f, 0.0f, 1.0f);
	m_MaxUV.Set(0.0f, 1.0f, 0.0f, 1.0f);

	m_RenderData[0].Position = D3DXVECTOR3(- fHalfWidth, - fHalfHeight, 0.0f);
	m_RenderData[0].UV       = D3DXVECTOR2(0.0f, 1.0f);

	m_RenderData[1].Position = D3DXVECTOR3(- fHalfWidth,   fHalfHeight, 0.0f);
	m_RenderData[1].UV       = D3DXVECTOR2(0.0f, 0.0f);

	m_RenderData[2].Position = D3DXVECTOR3(  fHalfWidth, - fHalfHeight, 0.0f);
	m_RenderData[2].UV       = D3DXVECTOR2(1.0f, 1.0f);

	m_RenderData[3].Position = D3DXVECTOR3(  fHalfWidth,   fHalfHeight, 0.0f);
	m_RenderData[3].UV       = D3DXVECTOR2(1.0f, 0.0f);

	if(pCenter)
	{
		m_RenderData[0].Position += *pCenter;
		m_RenderData[1].Position += *pCenter;
		m_RenderData[2].Position += *pCenter;
		m_RenderData[3].Position += *pCenter;
	}

	DEBUG_NEW(m_pVertexBuffer, CVertexBuffer);

	if( !m_pVertexBuffer->Create(4, sizeof(VERTEX), D3DUSAGE_WRITEONLY, D3DPOOL_MANAGED, m_RenderData, D3DFVF_XYZ | D3DFVF_TEX1) )
		return false;

	m_bIsCreated = true;

	return true;
}

bool CBillboard::Destroy()
{
	if(m_bIsCreated)
		DEBUG_DELETE(m_pVertexBuffer);

	return true;
}

void CBillboard::Update()
{
	CSprite::Update();
}

bool CBillboard::ApplyForRender()
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
			pRenderEntry->hModel       = m_pVertexBuffer->GetHandle();
			pRenderEntry->uRenderPass  = i;
			pRenderEntry->pParent      = this;

			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}

		return true;
	}

	return false;
}

void CBillboard::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	CEffect* pEffect = m_RenderMethod.GetEffect();
	
	if (pEffect)
	{	
		DEVICE.SetRenderState(D3DRS_ALPHATESTENABLE, TRUE);
		DEVICE.SetRenderState(D3DRS_ALPHAREF, 0x0000000);
		DEVICE.SetRenderState(D3DRS_ALPHAFUNC, D3DCMP_GREATER);

		DEVICE.SetRenderState(D3DRS_CULLMODE, D3DCULL_NONE);

		if( TEST_BIT(uFlag, CRenderQueue::PARENT) )
		{
			D3DXMATRIX Matrix;
			
			switch(m_LockType)
			{
			case LOCK_XYZ:
				Matrix = CAMERA.GetWorldMatrix();

				Matrix._41 = m_WorldMatrix._41;
				Matrix._42 = m_WorldMatrix._42;
				Matrix._43 = m_WorldMatrix._43;
				break;
			default:
				Matrix = m_WorldMatrix;
			}

			pEffect->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, Matrix * CAMERA.GetViewProjectionMatrix() );

			if(m_bIsPlay)
			{

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

				m_RenderData[1].UV.x = m_CurrentUV.GetMinX();
				m_RenderData[1].UV.y = m_CurrentUV.GetMinY();

				m_RenderData[0].UV.x = m_CurrentUV.GetMinX();
				m_RenderData[0].UV.y = m_CurrentUV.GetMaxY();

				m_RenderData[3].UV.x = m_CurrentUV.GetMaxX();
				m_RenderData[3].UV.y = m_CurrentUV.GetMinY();

				m_RenderData[2].UV.x = m_CurrentUV.GetMaxX();
				m_RenderData[2].UV.y = m_CurrentUV.GetMaxY();

				void* pVertices;
				m_pVertexBuffer->Lock(0, &pVertices);

				memcpy( pVertices, m_RenderData, sizeof(m_RenderData) );

				m_pVertexBuffer->Unlock();
			}
		}

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
			m_pVertexBuffer->Activate(0, 0, true);

		if ( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			pEffect->SetSurface( m_RenderMethod.GetSurface() );

		pEffect->GetEffect()->CommitChanges();

		DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2);

		DEVICE.SetRenderState(D3DRS_CULLMODE, D3DCULL_CCW);
		DEVICE.SetRenderState(D3DRS_ALPHATESTENABLE, FALSE);
	}
}