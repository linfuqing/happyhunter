#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "StaticMesh.h"

using namespace zerO;

CStaticMesh::CStaticMesh() :
m_pMesh(NULL),
m_pAdjacencyBuffer(NULL),
m_dwNumMaterials(0),
m_strMeshFile(TEXT("")),
m_strEffectFile(TEXT("HLSLPMesh.fx"))
{
}

CStaticMesh::~CStaticMesh()
{
	DEBUG_RELEASE(m_pAdjacencyBuffer);
	DEBUG_RELEASE(m_pMesh);
}

bool CStaticMesh::Create()
{
	//����Ч��
	if( !m_RenderMethod.LoadEffect( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;

	//��������ģ��
	LPD3DXBUFFER pD3DXMtrlBuffer;

	HRESULT hr;

	hr = D3DXLoadMeshFromX( m_strMeshFile.c_str(), D3DXMESH_MANAGED, 
		&DEVICE, &m_pAdjacencyBuffer, 
		&pD3DXMtrlBuffer, NULL, &m_dwNumMaterials, 
		&m_pMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//��ȡ���ʺ������ļ�·��
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	for( DWORD i = 0; i< m_dwNumMaterials; i ++ )
	{
		// MatD3D������û�л������ֵ����,����������ʱ,��Ҫ������
		d3dxMaterials[i].MatD3D.Ambient = d3dxMaterials[i].MatD3D.Diffuse;

		CSurface* pSurface = NULL;
		DEBUG_NEW(pSurface, CSurface);
		pSurface->SetMaterial(d3dxMaterials[i].MatD3D);

		if( d3dxMaterials[i].pTextureFilename != NULL )
		{
			CTexture* pTex = NULL;
			DEBUG_NEW(pTex, CTexture);
			//��������
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

	LPD3DXMESH pMeshSysMem = m_pMesh;
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
	
	//ȷ�������������
	hr = D3DXComputeNormals(pMeshSysMem2,NULL);
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//��������
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

	hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl2, &DEVICE, &m_pMesh );
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//�ͷ���ʱ����ģ�Ͷ���
	DEBUG_RELEASE(pMeshSysMem);
	DEBUG_RELEASE(pMeshSysMem2);

	return true;
}

bool CStaticMesh::ApplyForRender()
{
	UINT uTotalPass = m_RenderMethod.GetEffect()->GetTechniqueDesc().Passes, i;

	for (i = 0; i < uTotalPass; i ++)
	{
		//������������
		zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

		//����Ϣ�����͵��Ż�����
		pRenderEntry->hEffect      = m_RenderMethod.GetEffect()->GetHandle();
		pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
		pRenderEntry->hModel       = RESOURCE_MODEL;
		pRenderEntry->uRenderPass  = (zerO::UINT8)i;
		pRenderEntry->pParent      = this;

		//����
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

		//���ò�������
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(m_RenderMethod.GetSurface(i)->GetMaterial().Diffuse));
		m_RenderMethod.GetEffect()->GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);
		m_RenderMethod.GetEffect()->GetEffect()->SetTexture("ColorMap", m_RenderMethod.GetSurface(i)->GetTexture(0)->GetTexture());

		//���ո��±�־���и���
		if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
			m_RenderMethod.GetEffect()->Begin();

		m_RenderMethod.GetEffect()->GetEffect()->BeginPass(pEntry->uRenderPass);

		//����
		if (m_pMesh != NULL)
			m_pMesh->DrawSubset( i );

		m_RenderMethod.GetEffect()->GetEffect()->EndPass();

		m_RenderMethod.GetEffect()->End();
	}
}