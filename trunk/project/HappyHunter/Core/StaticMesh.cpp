#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "StaticMesh.h"
#include "Camera.h"

using namespace zerO;

CStaticMesh::CStaticMesh() :
m_strEffectFile(TEXT("")),
m_pMesh(NULL),
m_pShadow(NULL),
m_bIsCreated(false),
m_bIsVisibleShadow(true),
m_bIsCulled(false)
{
}

CStaticMesh::~CStaticMesh()
{
	Destroy();
}

bool CStaticMesh::Create(const PBASICCHAR meshFile)
{
	//创建效果
	if( !m_RenderMethod.LoadEffect( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;

	DEBUG_NEW(m_pMesh, CMesh);

	if( !m_pMesh->Load(meshFile) )
		return false;

	DEBUG_NEW(m_pShadow, CShadowVolume);

	if( !m_pShadow->Create(*m_pMesh->GetMesh(), *this) )
		return false;

	m_LocalRect = m_pMesh->GetRectangle();

	m_bIsCreated = true;

	return true;
}

bool CStaticMesh::Destroy()
{
	if(m_bIsCreated)
		DEBUG_DELETE(m_pMesh);

	DEBUG_DELETE(m_pShadow);

	return true;
}

void CStaticMesh::Clone(CStaticMesh& StaticMesh)const
{
	CSprite::Clone(StaticMesh);

	m_RenderMethod.Clone(StaticMesh.m_RenderMethod);

	StaticMesh.m_pMesh   = m_pMesh;
}

bool CStaticMesh::ApplyForRender()
{
	if(m_bIsCulled)
		return true;

	CEffect* pEffect = m_RenderMethod.GetEffect();
	if(pEffect)
	{
		UINT uTotalPass = pEffect->GetTechniqueDesc().Passes, i, j;

		for(i = 0; i < m_pMesh->GetSurfacesNumber(); i ++)
		{
			for (j = 0; j < uTotalPass; j ++)
			{
				//锁定整个队列
				zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

				//将信息需求传送到优化队列
				pRenderEntry->hEffect      = m_RenderMethod.GetEffect()->GetHandle();
				pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
				pRenderEntry->hModel       = m_pMesh->GetHandle();
				pRenderEntry->uModelParamB = (zerO::UINT16)i;
				pRenderEntry->uRenderPass  = (zerO::UINT8)j;
				pRenderEntry->hSurface     = m_pMesh->GetSurfaces()[i].GetHandle();
				pRenderEntry->pParent      = this;

				//解锁
				GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
			}
		}

		return true;
	}

	return false;
}

void CStaticMesh::Update()
{
	CSprite::Update();

	m_bIsCulled = !CAMERA.GetFrustum().TestHit(m_WorldRect);
}

void CStaticMesh::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
	{
		D3DXVECTOR4 vecEyePos = D3DXVECTOR4(CAMERA.GetWorldPosition(), 0.0f); 
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecEye", &vecEyePos);

		const D3DLIGHT9* pLight = LIGHTMANAGER.GetLight(0);

		D3DXVECTOR4 LightDirection(pLight->Direction, 1.0f);

		m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecLightDir", &LightDirection);
	}

	//依照更新标志进行更新
	if( TEST_BIT(uFlag, zerO::CRenderQueue::PARENT) )
	{
		m_RenderMethod.GetEffect()->SetMatrix( CEffect::WORLD                , m_WorldMatrix                                    );
		m_RenderMethod.GetEffect()->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
	}

	if( TEST_BIT(uFlag, zerO::CRenderQueue::MODEL_PARAMB) )
		m_RenderMethod.GetEffect()->SetSurface(&m_pMesh->GetSurfaces()[pEntry->uModelParamB]);

	m_RenderMethod.GetEffect()->GetEffect()->CommitChanges();

	//绘制
	if (m_pMesh && m_pMesh->GetMesh() != NULL)
		m_pMesh->GetMesh()->DrawSubset(pEntry->uModelParamB);

	if(m_pShadow)
		m_pShadow->SetVisible(m_bIsVisibleShadow);
}