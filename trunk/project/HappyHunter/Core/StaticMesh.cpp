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
	//����Ч��
	if( !m_RenderMethod.LoadEffect( (PBASICCHAR)m_strMeshFile.c_str() ) )
		return false;

	//��������ģ��
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

	LPD3DXMESH pTempMesh;  //��ʱ����ģ�ͱ���

	//����ԭʼ����ģ��
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

	//�ۺ��ظ��Ķ���
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

	//��鴦���򻯺������ģ���Ƿ���Ч
	hr = D3DXValidMesh( m_pMesh, (DWORD*)m_pAdjacencyBuffer->GetBufferPointer(), NULL );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//���ɲ��ϸ������ģ��
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
		if (m_pPMesh != NULL)
			m_pPMesh->DrawSubset( i );

		m_RenderMethod.GetEffect()->GetEffect()->EndPass();

		m_RenderMethod.GetEffect()->End();
	}
}