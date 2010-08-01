#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "StaticMesh.h"
#include "Camera.h"

using namespace zerO;

CStaticMesh::CStaticMesh() :
m_strEffectFile(TEXT(""))
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

	if( !m_Mesh.Load(meshFile) )
		return false;

	m_LocalRect = m_Mesh.GetRectangle();

	return true;
}

bool CStaticMesh::ApplyForRender()
{
	UINT uTotalPass = m_RenderMethod.GetEffect()->GetTechniqueDesc().Passes, i, j;

	for(i = 0; i < m_Mesh.GetSurfacesNumber(); i ++)
	{
		for (j = 0; j < uTotalPass; j ++)
		{
			//锁定整个队列
			zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

			//将信息需求传送到优化队列
			pRenderEntry->hEffect      = m_RenderMethod.GetEffect()->GetHandle();
			pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
			pRenderEntry->hModel       = m_Mesh.GetHandle();
			pRenderEntry->uModelParamB = (zerO::UINT16)i;
			pRenderEntry->uRenderPass  = (zerO::UINT8)j;
			pRenderEntry->hSurface     = m_Mesh.GetSurfaces()[i].GetHandle();
			pRenderEntry->pParent      = this;

			//解锁
			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}
	}

	return true;
}

void CStaticMesh::Update()
{
	CSprite::Update();

	D3DXVECTOR4 vecEyePos = D3DXVECTOR4(CAMERA.GetWorldPosition(), 0.0f); 
	m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecEye", &vecEyePos);

	D3DLIGHT9 Light/*LIGHTMANAGER.GetLight(0)*/;

	DEVICE.GetLight(0, &Light);

	D3DXVECTOR4 LightDirection(Light.Direction, 1.0f);

	m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecLightDir", &LightDirection);
}

void CStaticMesh::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	//依照更新标志进行更新
	if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
	{
		//m_RenderMethod.GetEffect()->Begin();
		m_RenderMethod.GetEffect()->SetMatrix( CEffect::WORLD                , m_WorldMatrix                                    );
		m_RenderMethod.GetEffect()->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
	}

	if( TEST_BIT(uFlag, zerO::CRenderQueue::MODEL_PARAMB) )
		m_RenderMethod.GetEffect()->SetSurface(&m_Mesh.GetSurfaces()[pEntry->uModelParamB]);

	//m_RenderMethod.GetEffect()->GetEffect()->BeginPass(pEntry->uRenderPass);

	m_RenderMethod.GetEffect()->GetEffect()->CommitChanges();
	//绘制
	if (m_Mesh.GetMesh() != NULL)
		m_Mesh.GetMesh()->DrawSubset(pEntry->uModelParamB);

	//m_RenderMethod.GetEffect()->GetEffect()->EndPass();
}