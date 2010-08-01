#include "StdAfx.h"
#include "Mesh.h"
#include "basicutils.h"

using namespace zerO;

const D3DVERTEXELEMENT9 g_MESH_VERTEX_DESCRIPTION[]=
{
	{0, 0,  D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
	{0, 12, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	{0, 20, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
	{0, 32, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
	D3DDECL_END()
};

CMesh::CMesh(void) :
CResource(RESOURCE_MESH),
m_pMesh(NULL),
m_pSurfaces(NULL),
m_uNumSurfaces(0)
{
}

CMesh::~CMesh(void)
{
	Destroy();
}

void CMesh::SetNormalMap(CTexture* pTexture)
{
	for(UINT i = 0; i < m_uNumSurfaces; i ++)
		m_pSurfaces[i].SetTexture(pTexture, NORMAL);
}

bool CMesh::Load(const PBASICCHAR pcFileName)
{
		//加载网格模型
	LPD3DXBUFFER pD3DXMtrlBuffer;

	HRESULT hr;

	hr = D3DXLoadMeshFromX(
		pcFileName, D3DXMESH_MANAGED, 
		&DEVICE, NULL, 
		&pD3DXMtrlBuffer, NULL, (DWORD*)&m_uNumSurfaces, 
		&m_pMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	DEBUG_NEW(m_pSurfaces, CSurface[m_uNumSurfaces]);

	//提取材质和纹理文件路径
	D3DXMATERIAL* d3dxMaterials = (D3DXMATERIAL*)pD3DXMtrlBuffer->GetBufferPointer();

	for( DWORD i = 0; i< m_uNumSurfaces; i ++ )
	{
		// MatD3D属性里没有环境光的值设置,当它被加载时,需要设置它
		d3dxMaterials[i].MatD3D.Ambient = d3dxMaterials[i].MatD3D.Diffuse;

		CSurface* pSurface = &m_pSurfaces[i];
		pSurface->SetMaterial(d3dxMaterials[i].MatD3D);

		if( d3dxMaterials[i].pTextureFilename != NULL )
		{
			//创建纹理
#ifdef _UNICODE	
			BASICCHAR szFile[MAX_PATH];
			RemovePathFromFileName(d3dxMaterials[i].pTextureFilename, szFile);
			BASICSTRING texFile;
			GetRealPath(pcFileName, texFile, TEXT("/"), szFile);
			if( !pSurface->LoadTexture((PBASICCHAR)texFile.c_str(), 0) )
				return false;

			BASICSTRING normalMapFile;
			GetRealPath((PBASICCHAR)texFile.c_str(), normalMapFile, TEXT("."), TEXT("-normalmap.tga"), true);
			if( !pSurface->LoadTexture((PBASICCHAR)normalMapFile.c_str(), 1) )
				return false;
#else
			if( !pSurface->LoadTexture(d3dxMaterials[i].pTextureFilename, 0) )
				return false;
#endif
		}
	}

	DEBUG_RELEASE( pD3DXMtrlBuffer );

	return __GenerateDeclMesh(m_pMesh) && __GetBoundBox(m_pMesh, m_Rectangle);
}

bool CMesh::__GenerateDeclMesh(LPD3DXMESH& pMesh)
{
	if (pMesh == NULL)
		return false;

	HRESULT hr;

	LPD3DXMESH pMeshSysMem = pMesh;

	LPD3DXMESH  pMeshSysMem2  = NULL;
	hr = pMeshSysMem->CloneMesh(D3DXMESH_MANAGED, g_MESH_VERTEX_DESCRIPTION, &DEVICE, &pMeshSysMem2);
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

	/*D3DVERTEXELEMENT9 decl2[]=
	{
		{0, 0,  D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},
		{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
		{0, 24, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_NORMAL, 0},
		{0, 36, D3DDECLTYPE_FLOAT3, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TANGENT, 0},
		D3DDECL_END()
	};*/

	hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, g_MESH_VERTEX_DESCRIPTION, &DEVICE, &pMesh );
	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	//释放临时网格模型对象
	DEBUG_RELEASE(pMeshSysMem);
	DEBUG_RELEASE(pMeshSysMem2);

	return true;
}

bool CMesh::__GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& Rect3d)
{
	if (pMesh == NULL)
		return false;

	UINT uVertexNum = pMesh->GetNumVertices();

	LPD3DXMESH pTempMesh;
	pMesh->CloneMeshFVF(D3DXMESH_SYSTEMMEM, D3DFVF_XYZ, &DEVICE, &pTempMesh);
	
	LPDIRECT3DVERTEXBUFFER9 pVertexBuffer;
	pTempMesh->GetVertexBuffer(&pVertexBuffer);

	FLOAT maxX = 0.0f, maxY = 0.0f, maxZ = 0.0f;
	FLOAT minX = 0.0f, minY = 0.0f, minZ = 0.0f;

	D3DXVECTOR3* pVertices;

	pVertexBuffer->Lock(0, 0, (void**)&pVertices, 0);

	for(UINT i = 0; i < uVertexNum; i ++)
	{
		if(pVertices[i].x > maxX)
			maxX = pVertices[i].x;
		if(pVertices[i].y > maxY)
			maxY = pVertices[i].y;
		if(pVertices[i].z > maxZ)
			maxZ = pVertices[i].z;

		if(pVertices[i].x < minX)
			minX = pVertices[i].x;
		if(pVertices[i].y < minY)
			minY = pVertices[i].y;
		if(pVertices[i].z < minZ)
			minZ = pVertices[i].z;
	}
	pVertexBuffer->Unlock();

	Rect3d.Set(minX, maxX, minY, maxY, minZ, maxZ);

	DEBUG_RELEASE(pVertexBuffer);
	DEBUG_RELEASE(pTempMesh);

	return true;
}

bool CMesh::Destroy()
{
	DEBUG_RELEASE(m_pMesh);
	DEBUG_DELETE_ARRAY(m_pSurfaces);

	m_pMesh     = NULL;
	m_pSurfaces = NULL;

	return true;
}