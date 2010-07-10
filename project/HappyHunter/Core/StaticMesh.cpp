#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "StaticMesh.h"

using namespace zerO;

CStaticMesh::CStaticMesh() :
m_pMesh(NULL),
m_pPMesh(NULL),
m_pAdjacencyBuffer(NULL),
m_dwNumMaterials(0),
m_strMeshFile(TEXT("")),
m_strEffectFile(TEXT("HLSLPMesh.fx"))
{
}

CStaticMesh::~CStaticMesh()
{
	/*m_RenderMethod.Destory();*/
}

bool CStaticMesh::Create()
{
	//创建效果
	if( !m_RenderMethod.LoadEffect( (PBASICCHAR)m_strMeshFile.c_str() ) )
		return false;

	//加载网格模型
	LPD3DXBUFFER pD3DXMtrlBuffer;

	HRESULT hr;

	hr = D3DXLoadMeshFromX( m_strEffectFile.c_str(), D3DXMESH_MANAGED, 
		&DEVICE, &m_pAdjacencyBuffer, 
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, 
		&m_pMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//提取材质和纹理文件路径
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	for( DWORD i = 0; i< m_dwNumMaterials; i ++ )
	{
		// MatD3D属性里没有环境光的值设置,当它被加载时,需要设置它
		d3dxMaterials[i].MatD3D.Ambient = d3dxMaterials[i].MatD3D.Diffuse;

		CSurface* pSurface = NULL;
		DEBUG_NEW(pSurface, CSurface);
		pSurface->SetMaterial(d3dxMaterials[i].MatD3D);

		if( d3dxMaterials[i].pTextureFilename != NULL )
		{
			CTexture* pTex = NULL;
			DEBUG_NEW(pTex, CTexture);
			//创建纹理
#ifdef _UNICODE
			WCHAR wszBuf[MAX_PATH];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, wszBuf);

			if( !pTex->Load( wszBuf ) )
				return false;
#else
			if( !pTex->Load( d3dxMaterials[i].pTextureFilename ) )
				return false;
#endif
			pSurface->SetTexture(pTex, 0);
			m_RenderMethod.SetSurface(pSurface);
		}
	}

	DEBUG_RELEASE( pD3DXMtrlBuffer );

	LPD3DXMESH pTempMesh;  //临时网格模型变量

	//整理原始网格模型
	hr = D3DXCleanMesh( D3DXCLEAN_SIMPLIFICATION, m_pMesh, 
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), &pTempMesh, 
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), NULL );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	DEBUG_RELEASE( m_pMesh );
	m_pMesh = pTempMesh;

	//熔合重复的顶点
	D3DXWELDEPSILONS Epsilons;
	ZeroMemory( &Epsilons, sizeof(D3DXWELDEPSILONS) );
	hr = D3DXWeldVertices( m_pMesh, D3DXWELDEPSILONS_WELDPARTIALMATCHES,
		&Epsilons, 
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),  
		(DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
		NULL, NULL );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//检查处理、简化后的网格模型是否有效
	hr = D3DXValidMesh( m_pMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), NULL );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//生成层次细节网格模型
	hr = D3DXGeneratePMesh( m_pMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(),
		NULL, NULL, 1, D3DXMESHSIMP_VERTEX, &m_pPMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	DWORD cVerticesMin = m_pPMesh->GetMinVertices();
	DWORD cVerticesMax = m_pPMesh->GetMaxVertices();

	hr = m_pPMesh->SetNumVertices(cVerticesMax);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	return true;
}

bool CStaticMesh::ApplyForRender()
{
	UINT uTotalPass = m_RenderMethod.GetEffect()->GetTechniqueDesc().Passes, i;

	for (i = 0; i < uTotalPass; i ++)
	{
		//锁定整个队列
		zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

		//将信息需求传送到优化队列
		pRenderEntry->hEffect      = m_RenderMethod.GetEffect()->GetHandle();
		pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
		pRenderEntry->hModel       = RESOURCE_MODEL;
		pRenderEntry->uRenderPass  = (zerO::UINT8)i;
		pRenderEntry->pParent      = this;

		//解锁
		GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
	}

	return true;
}

void CStaticMesh::Update()
{
	CSceneNode::Update();
}

void CStaticMesh::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	for(DWORD i = 0; i < m_dwNumMaterials; i ++)
	{
		D3DXCOLOR color1(m_RenderMethod.GetSurface(i)->GetMaterial().Ambient);
		D3DXCOLOR color2(0.25, 0.25, 0.25, 1.0);
		D3DXCOLOR ambEmm;
		D3DXColorModulate(&ambEmm, &color1, &color2);
		ambEmm += D3DXCOLOR(m_RenderMethod.GetSurface(i)->GetMaterial().Emissive);

		//设置材质属性
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(m_RenderMethod.GetSurface(i)->GetMaterial().Diffuse));
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);
		m_RenderMethod.GetEffect()->GetEffect()->SetTexture("ColorMap", m_RenderMethod.GetSurface(i)->GetTexture(0)->GetTexture());

		//依照更新标志进行更新
		if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
			m_RenderMethod.GetEffect()->Begin();

		m_RenderMethod.GetEffect()->GetEffect()->BeginPass(pEntry->uRenderPass);

		//绘制
		if (m_pPMesh != NULL)
			m_pPMesh->DrawSubset( i );

		m_RenderMethod.GetEffect()->GetEffect()->EndPass();

		m_RenderMethod.GetEffect()->End();
	}
}