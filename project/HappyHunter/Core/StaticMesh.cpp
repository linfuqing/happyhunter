#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "StaticMesh.h"
#include "Camera.h"

using namespace zerO;

CStaticMesh::CStaticMesh() :
m_pMesh(NULL),
m_pAdjacencyBuffer(NULL),
m_dwNumMaterials(0),
m_strEffectFile(TEXT(""))
{
}

CStaticMesh::~CStaticMesh()
{
	Destroy();
}

void CStaticMesh::__GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d)
{
	if (pMesh == NULL)
		return;

	DWORD dwVertexNum = pMesh->GetNumVertices();

	LPD3DXMESH pTempMesh;
	pMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, BoxVertex::FVF_Flags, &DEVICE, &pTempMesh);
	
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	pTempMesh->GetVertexBuffer(&pVertexBuffer);

	FLOAT maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
	FLOAT minX = 0.0f, minY = 0.0f, minZ = 0.0f;
	BoxVertex* pVertices;
	pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);
	for(DWORD i = 0; i< dwVertexNum; i ++)
	{
		if(pVertices[i].p.x > maxX)
			maxX = pVertices[i].p.x;
		if(pVertices[i].p.y > maxY)
			maxY = pVertices[i].p.y;
		if(pVertices[i].p.z > maxZ)
			maxZ = pVertices[i].p.z;

		if(pVertices[i].p.x < minX)
			minX = pVertices[i].p.x;
		if(pVertices[i].p.y < minY)
			minY = pVertices[i].p.y;
		if(pVertices[i].p.z < minZ)
			minZ = pVertices[i].p.z;
	}
	pVertexBuffer->Unlock();

	rect3d.Set(minX, maxX, minY, maxY, minZ, maxZ);

	DEBUG_RELEASE(pVertexBuffer);
	DEBUG_RELEASE(pTempMesh);
}

HRESULT CStaticMesh::__GenerateDeclMesh(LPD3DXMESH& pMesh)
{
	if (pMesh == NULL)
		return S_FALSE;

	HRESULT hr;

	LPD3DXMESH pMeshSysMem = pMesh;
	D3DVERTEXELEMENT9 decl[]=
	{
		{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		D3DDECL_END()
	};

	LPD3DXMESH  pMeshSysMem2  = NULL;
	hr = pMeshSysMem->CloneMesh(D3DXMESH_MANAGED, decl, &DEVICE, &pMeshSysMem2);
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}
	
	//确保顶点包含法线
	hr = D3DXComputeNormals(pMeshSysMem2,NULL);
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//计算切线
	hr = D3DXComputeTangent( pMeshSysMem2, 0, 0, 0, true, NULL );
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	D3DVERTEXELEMENT9 decl2[]=
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		D3DDECL_END()
	};

	hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl2, &DEVICE, &pMesh );
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//释放临时网格模型对象
	DEBUG_RELEASE(pMeshSysMem);
	DEBUG_RELEASE(pMeshSysMem2);

	return hr;
}

bool CStaticMesh::Create(const PBASICCHAR meshFile)
{
	//创建效果
	if( !m_RenderMethod.LoadEffect( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;

	//加载网格模型
	LPD3DXBUFFER pD3DXMtrlBuffer;

	HRESULT hr;

	hr = D3DXLoadMeshFromX( meshFile, D3DXMESH_MANAGED, 
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
			//创建纹理
#ifdef _UNICODE	
			BASICCHAR szFile[MAX_PATH];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, szFile);
			BASICSTRING texFile;
			GetRealPath(meshFile, texFile, TEXT("/"), szFile);
			if( !pSurface->LoadTexuture((PBASICCHAR)texFile.c_str(), 0) )
				return false;

			BASICSTRING normalMapFile;
			GetRealPath((PBASICCHAR)texFile.c_str(), normalMapFile, TEXT("."), TEXT("-normalmap.tga"), true);
			if( !pSurface->LoadTexuture((PBASICCHAR)normalMapFile.c_str(), 1) )
				return false;
#else
			if( !pSurface->LoadTexuture(d3dxMaterials[i].pTextureFilename, 0) )
				return false;
#endif
			m_RenderMethod.SetSurface(pSurface);
		}
	}

	DEBUG_RELEASE( pD3DXMtrlBuffer );

	__GenerateDeclMesh(m_pMesh);
	__GetBoundBox(m_pMesh, m_LocalRect);

	return true;
}

bool CStaticMesh::Destroy()
{
	m_RenderMethod.DestroySurface();

	DEBUG_RELEASE(m_pAdjacencyBuffer);
	DEBUG_RELEASE(m_pMesh);

	m_pAdjacencyBuffer = NULL;
	m_pMesh            = NULL;

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

	D3DXVECTOR4 vecEyePos = D3DXVECTOR4(CAMERA.GetPosition(), 0.0f); 
	m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecEye", &vecEyePos);

	D3DXMATRIXA16 matView = CAMERA.GetViewMatrix();
	D3DXMATRIXA16 matProj = CAMERA.GetProjectionMatrix();
	D3DXMATRIXA16 matWVP = m_WorldMatrix * matView * matProj;

	m_RenderMethod.GetEffect()->GetEffect()->SetMatrix( "matWorld", &m_WorldMatrix );
	m_RenderMethod.GetEffect()->GetEffect()->SetMatrix( "matWorldViewProj", &matWVP );

	const D3DLIGHT9* pLight = LIGHTMANAGER.GetLight(0);

	D3DXVECTOR4 LightDirection(pLight->Direction, 1.0f);

	if(pLight)
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("vecLightDir", &LightDirection);
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
		/*m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(m_RenderMethod.GetSurface(i)->GetMaterial().Diffuse));
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);*/
		m_RenderMethod.GetEffect()->GetEffect()->SetTexture("ColorMap", m_RenderMethod.GetSurface(i)->GetTexture(0)->GetTexture());
		m_RenderMethod.GetEffect()->GetEffect()->SetTexture("BumpMap", m_RenderMethod.GetSurface(i)->GetTexture(1)->GetTexture());

		/*m_RenderMethod.GetEffect()->SetMatrix( CEffect::WORLD_VIEW_PROJECTION, m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
		m_RenderMethod.GetEffect()->SetParameter(CEffect::DIFFUSE_MATERIAL_COLOR, &m_RenderMethod.GetSurface(i)->GetMaterial().Diffuse);
		m_RenderMethod.GetEffect()->SetParameter(CEffect::AMBIENT_MATERIAL_COLOR, &ambEmm);*/

		//依照更新标志进行更新
		if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
			m_RenderMethod.GetEffect()->Begin();

		m_RenderMethod.GetEffect()->GetEffect()->BeginPass(pEntry->uRenderPass);

		//绘制
		if (m_pMesh != NULL)
			m_pMesh->DrawSubset( i );

		m_RenderMethod.GetEffect()->GetEffect()->EndPass();

		m_RenderMethod.GetEffect()->End();
	}
}