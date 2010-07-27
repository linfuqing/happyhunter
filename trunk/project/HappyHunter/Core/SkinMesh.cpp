#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "SkinMesh.h"
#include "Camera.h"

#ifndef SAFE_DELETE
#define SAFE_DELETE(p)       { if (p) { delete (p);     (p)=NULL; } }
#endif    
#ifndef SAFE_DELETE_ARRAY
#define SAFE_DELETE_ARRAY(p) { if (p) { delete[] (p);   (p)=NULL; } }
#endif    
#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=NULL; } }
#endif

using namespace zerO;

CAllocateHierarchy::CAllocateHierarchy() :
m_pBoneMatrices(NULL), 
m_NumBoneMatricesMax(0),
m_pMesh(NULL)
{
}

CAllocateHierarchy::~CAllocateHierarchy()
{
	//DEBUG_DELETE_ARRAY(m_pBoneMatrices);
	//m_pBoneMatrices   = NULL;
	SAFE_DELETE_ARRAY(m_pBoneMatrices);
	SAFE_RELEASE(m_pMesh);
}

//-----------------------------------------------------------------------------
// Desc: 
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::__AllocateName( LPCSTR Name, LPSTR *pNewName )
{
    UINT cbLength;

    if( Name != NULL )
    {
        cbLength = (UINT)strlen(Name) + 1;
		//DEBUG_NEW(*pNewName, CHAR[cbLength]);
		*pNewName = new CHAR[cbLength];
        if (*pNewName == NULL)
            return E_OUTOFMEMORY;
        memcpy( *pNewName, Name, cbLength*sizeof(CHAR) );
    }
    else
    {
        *pNewName = NULL;
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
//Desc: ������Ƥ����ģ��(����ÿ������Ļ��Ȩ�ء�������һ��������ϱ�)
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::__GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr = S_OK;
	if( pMeshContainer->pSkinInfo == NULL )
		return hr;

	//DEBUG_RELEASE( pMeshContainer->MeshData.pMesh );
	//DEBUG_RELEASE( pMeshContainer->pBoneCombinationBuf );
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);

	//��ȡ��ǰ�豸������
	D3DCAPS9 d3dCaps;
    DEVICE.GetDeviceCaps( &d3dCaps );

	pMeshContainer->NumPaletteEntries = min(26, pMeshContainer->pSkinInfo->GetNumBones());
	DWORD Flags = D3DXMESHOPT_VERTEXCACHE;

	if (d3dCaps.VertexShaderVersion >= D3DVS_VERSION(1, 1))
	{
		pMeshContainer->UseSoftwareVP = false;
		Flags |= D3DXMESH_MANAGED;
	}
	else
	{
		pMeshContainer->UseSoftwareVP = true;
		Flags |= D3DXMESH_SYSTEMMEM;
	}

	//DEBUG_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);

	hr = pMeshContainer->pSkinInfo->ConvertToIndexedBlendedMesh 
		                                                      ( pMeshContainer->pOrigMesh,
		                                                        Flags, 
		                                                        pMeshContainer->NumPaletteEntries, 
		                                                        pMeshContainer->pAdjacency, 
		                                                        NULL, NULL, NULL,             
		                                                        &pMeshContainer->NumInfl,
		                                                        &pMeshContainer->NumAttributeGroups, 
		                                                        &pMeshContainer->pBoneCombinationBuf, 
		                                                        &pMeshContainer->MeshData.pMesh);
	if (FAILED(hr))
		return hr;

	hr = __GenerateDeclMesh(pMeshContainer); 
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	D3DVERTEXELEMENT9 pDecl[MAX_FVF_DECL_SIZE];
	LPD3DVERTEXELEMENT9 pDeclCur;
	hr = pMeshContainer->MeshData.pMesh->GetDeclaration(pDecl);
	if (FAILED(hr))
		return hr;

	pDeclCur = pDecl;
	while (pDeclCur->Stream != 0xff)
	{
		if ((pDeclCur->Usage == D3DDECLUSAGE_BLENDINDICES) && (pDeclCur->UsageIndex == 0))
			pDeclCur->Type = D3DDECLTYPE_D3DCOLOR;
		pDeclCur++;
	}

	hr = pMeshContainer->MeshData.pMesh->UpdateSemantics(pDecl);
	if (FAILED(hr))
		return hr;

	// allocate a buffer for bone matrices,
	if( m_NumBoneMatricesMax < pMeshContainer->pSkinInfo->GetNumBones() )
	{
		m_NumBoneMatricesMax = pMeshContainer->pSkinInfo->GetNumBones();

		// Allocate space for blend matrices
		//DEBUG_DELETE_ARRAY(m_pBoneMatrices); 
		//m_pBoneMatrices = NULL;
		SAFE_DELETE_ARRAY(m_pBoneMatrices);
		//DEBUG_NEW(m_pBoneMatrices, D3DXMATRIXA16[m_NumBoneMatricesMax]);
		m_pBoneMatrices = new D3DXMATRIXA16[m_NumBoneMatricesMax];
		if( m_pBoneMatrices == NULL )
		{
			hr = E_OUTOFMEMORY;
			return hr;
		}
	}

	m_pMesh = pMeshContainer->MeshData.pMesh;

	return hr;
}

HRESULT CAllocateHierarchy::__GenerateDeclMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr;

	if (pMeshContainer == NULL)
		return S_FALSE;

	LPD3DXMESH pMeshSysMem = pMeshContainer->MeshData.pMesh;
	D3DVERTEXELEMENT9   decl[]   = 
	{ 
		{   0,   0,	   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,   0   }, 
		{   0,   12,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,   0   }, 
		{   0,   24,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,   0   }, 
		{   0,   32,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDWEIGHT,   0   }, 
		{   0,   48,   D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDINDICES,   0   }, 
		{   0,   52,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BINORMAL,   0   }, 
		{   0,   64,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TANGENT,   0   }, 
		D3DDECL_END(), 
	}; 

	LPD3DXMESH  pMeshSysMem2  = NULL;
	hr = pMeshSysMem->CloneMesh(D3DXMESH_MANAGED, decl, &DEVICE, &pMeshSysMem2);
	if (FAILED(hr))
		return hr;
	
	//ȷ�������������
	hr = D3DXComputeNormals(pMeshSysMem2,NULL);
	if (FAILED(hr))
		return hr;

	//��������
	hr = D3DXComputeTangent( pMeshSysMem2, 0, 0, 0, true, NULL );
	if (FAILED(hr))
		return hr;

	D3DVERTEXELEMENT9   decl2[]   = 
	{ 
		{   0,   0,	   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,   0   }, 
		{   0,   16,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,   0   }, 
		{   0,   28,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,   0   }, 
		{   0,   36,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDWEIGHT,   0   }, 
		{   0,   52,   D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDINDICES,   0   }, 
		{   0,   56,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BINORMAL,   0   }, 
		{   0,   68,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TANGENT,   0   }, 
		D3DDECL_END(), 
	}; 

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl2, &DEVICE, &pMeshContainer->MeshData.pMesh );
	if (FAILED(hr))
		return hr;

	//�ͷ���ʱ����ģ�Ͷ���
	SAFE_RELEASE(pMeshSysMem);
	SAFE_RELEASE(pMeshSysMem2);

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: �������, �����Ƿ����ڴ�ͳ�ʼ��,��û�ж����Ա������ʵ�ֵ
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    //������ܽṹ����
	//DEBUG_NEW(pFrame, D3DXFRAME_DERIVED);
	pFrame = new D3DXFRAME_DERIVED;
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//Ϊ���ָ������
    hr = __AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//��ʼ��D3DXFRAME_DERIVED�ṹ������Ա����
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
	
    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;
	
    *ppNewFrame = pFrame;
    pFrame = NULL;

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: �������ǵ����˳�Ա���� __GenerateSkinnedMesh(pMeshContainer);
//       ���������������Ƥ��Ϣ
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateMeshContainer(LPCSTR Name, 
                                                CONST D3DXMESHDATA *pMeshData,
                                                CONST D3DXMATERIAL *pMaterials, 
                                                CONST D3DXEFFECTINSTANCE *pEffectInstances, 
                                                DWORD NumMaterials, 
                                                CONST DWORD *pAdjacency, 
                                                LPD3DXSKININFO pSkinInfo, 
                                                LPD3DXMESHCONTAINER *ppNewMeshContainer) 
{
    HRESULT hr;
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
    UINT NumFaces;       //�����е�����,��������������ṹ���ڽ���Ϣ��Աʱʹ��
    UINT iMaterial;      //�������ʱ��ѭ������
    UINT cBones;         //��ǰ����ģ�͹�������
    LPD3DXMESH pMesh    = NULL;
    *ppNewMeshContainer = NULL;
	
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        return E_FAIL;
    }
	
    pMesh = pMeshData->pMesh;

	//Ϊ�������������ڴ�
	//DEBUG_NEW(pMeshContainer, D3DXMESHCONTAINER_DERIVED);
	pMeshContainer = new D3DXMESHCONTAINER_DERIVED;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//������������ṹD3DXMESHCONTAINER_DERIVED�ĳ�Ա

	//Ϊ����ָ������
	hr = __AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	
    NumFaces = pMesh->GetNumFaces();

    pMeshContainer->MeshData.pMesh = pMesh;
    pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	pMesh->AddRef();
	
    //Ϊ����ģ��׼�����ʺ�����
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
	//DEBUG_NEW(pMeshContainer->pMaterials, D3DXMATERIAL[pMeshContainer->NumMaterials]);
	//DEBUG_NEW(pMeshContainer->ppTextures, LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials]);
	//DEBUG_NEW(pMeshContainer->pAdjacency, DWORD[NumFaces*3]);
	pMeshContainer->pMaterials = new D3DXMATERIAL[pMeshContainer->NumMaterials];
	pMeshContainer->ppTextures = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->ppBumpMaps = new LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency = new DWORD[NumFaces*3];
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL) || (pMeshContainer->ppBumpMaps == NULL))
	{
		hr = E_OUTOFMEMORY;
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	memset(pMeshContainer->ppBumpMaps, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	
   if (NumMaterials > 0)            
    {    
        //���Ʋ�������, ���ò��ʻ���������
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;
       
		//��ȡ�����ļ�, �����������
        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
#ifdef _UNICODE
                WCHAR szFile[MAX_PATH];
				//�������ļ�·����ȡ�����ļ���
				RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, szFile);
				BASICSTRING texFile;
				GetRealPath(m_strFilePath, texFile, TEXT("/"), szFile);
				if( FAILED( D3DXCreateTextureFromFile( &DEVICE, (PBASICCHAR)texFile.c_str(),
					&pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;

				BASICSTRING normalMapFile;
				GetRealPath((PBASICCHAR)texFile.c_str(), normalMapFile, TEXT("."), TEXT("-normalmap.tga"), true);
				if( FAILED( D3DXCreateTextureFromFile( &DEVICE, normalMapFile.c_str(), &pMeshContainer->ppBumpMaps[iMaterial] ) ) )
					pMeshContainer->ppBumpMaps[iMaterial] = NULL;
#else
				if( FAILED( D3DXCreateTextureFromFile( &DEVICE, pMeshContainer->pMaterials[iMaterial].pTextureFilename,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;
#endif

                pMeshContainer->pMaterials[iMaterial].pTextureFilename = NULL;
			}
        }
    }
    else
    {
        pMeshContainer->pMaterials[0].pTextureFilename = NULL;
        memset(&pMeshContainer->pMaterials[0].MatD3D, 0, sizeof(D3DMATERIAL9));
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.r = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.g = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Diffuse.b = 0.5f;
        pMeshContainer->pMaterials[0].MatD3D.Specular = pMeshContainer->pMaterials[0].MatD3D.Diffuse;
    }

    //�����ǰ���������Ƥ��Ϣ
    if (pSkinInfo != NULL)
    {
		//������Ƥ������Ϣ
        pMeshContainer->pSkinInfo = pSkinInfo; 
        pSkinInfo->AddRef();
		
		//����ԭ������Ϣ
        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

		//��ȡ��������
        cBones = pSkinInfo->GetNumBones();

		//Ϊÿ��������䱣���ʼ�任������ڴ�ռ�
		//DEBUG_NEW(pMeshContainer->pBoneOffsetMatrices, D3DXMATRIX[cBones]);
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL) 
        {
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
        
		//����ÿ������ĳ�ʼ�任����
        for (UINT iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }
		
		//������Ƥ����ģ��
		hr = __GenerateSkinnedMesh(pMeshContainer); 
        if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
    }
	else
	{
		hr = __GenerateDeclMesh(pMeshContainer); 
		if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
	}
	
    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;

    return hr;
}

//-----------------------------------------------------------------------------
// Desc: �ͷſ��
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	if(pFrameToFree != NULL)
	{
		//DEBUG_DELETE_ARRAY( pFrameToFree->Name );
		SAFE_DELETE_ARRAY(pFrameToFree->Name);
		//DEBUG_DELETE( pFrameToFree );
		SAFE_DELETE(pFrameToFree);

		/*pFrameToFree->Name = NULL;
		pFrameToFree       = NULL;*/
	}
    return S_OK; 
}


//-----------------------------------------------------------------------------
// Desc: �ͷ���������
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

    UINT iMaterial;
	// ��תΪ��չ�������ڴ�й©
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	
    /*DEBUG_DELETE_ARRAY( pMeshContainer->Name );
    DEBUG_DELETE_ARRAY( pMeshContainer->pAdjacency );
    DEBUG_DELETE_ARRAY( pMeshContainer->pMaterials );
    DEBUG_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );

	pMeshContainer->Name                = NULL;
	pMeshContainer->pAdjacency          = NULL;
	pMeshContainer->pMaterials          = NULL;
	pMeshContainer->pBoneOffsetMatrices = NULL;*/
	SAFE_DELETE_ARRAY(pMeshContainer->Name);
	SAFE_DELETE_ARRAY(pMeshContainer->pAdjacency);
	SAFE_DELETE_ARRAY(pMeshContainer->pMaterials);
	SAFE_DELETE_ARRAY(pMeshContainer->pBoneOffsetMatrices);
	
    if (pMeshContainer->ppTextures != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            //DEBUG_RELEASE( pMeshContainer->ppTextures[iMaterial] );
			SAFE_RELEASE(pMeshContainer->ppTextures[iMaterial]);

			pMeshContainer->ppTextures[iMaterial] = NULL;
        }
    }
   /* DEBUG_DELETE_ARRAY( pMeshContainer->ppTextures );

    DEBUG_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );

	pMeshContainer->ppTextures       = NULL;
	pMeshContainer->ppBoneMatrixPtrs = NULL;*/
	SAFE_DELETE_ARRAY(pMeshContainer->ppTextures);

	if (pMeshContainer->ppBumpMaps != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            SAFE_RELEASE( pMeshContainer->ppBumpMaps[iMaterial] );
        }
    }
	SAFE_DELETE_ARRAY( pMeshContainer->ppBumpMaps );

	SAFE_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);

    //DEBUG_RELEASE( pMeshContainer->pBoneCombinationBuf );
    //DEBUG_RELEASE( pMeshContainer->MeshData.pMesh );
    //DEBUG_RELEASE( pMeshContainer->pSkinInfo );
    //DEBUG_RELEASE( pMeshContainer->pOrigMesh );
    //DEBUG_DELETE( pMeshContainer );
	SAFE_RELEASE(pMeshContainer->pBoneCombinationBuf);
	SAFE_RELEASE(pMeshContainer->MeshData.pMesh);
	SAFE_RELEASE(pMeshContainer->pSkinInfo);
	SAFE_RELEASE(pMeshContainer->pOrigMesh);
	SAFE_DELETE(pMeshContainer);

	//pMeshContainer->pBoneCombinationBuf = NULL;
	//pMeshContainer->MeshData.pMesh      = NULL;
	//pMeshContainer->pSkinInfo           = NULL;
	//pMeshContainer->pOrigMesh           = NULL;
	//pMeshContainerBase                  = NULL;

    return S_OK;
}


//----------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------
// Desc: ���캯������������  Construction/Destruction
//--------------------------------------------------------------------
CSkinMesh::CSkinMesh() :
m_bPlayAnim(true),
m_pAnimController(NULL),
m_pFrameRoot(NULL),
m_dwCurrentTrack(0),
m_fTimeCurrent(0.0f),
m_dwPlayTime(0),
m_fFrameTime(0.0f),
m_lfTotalFrameTime(0.0),
m_dwControlPlayTime(0),
m_strEffectFile(TEXT(""))
{
}


//-----------------------------------------------------------------------------
// Desc: ���캯������������ 
//-----------------------------------------------------------------------------
CSkinMesh::~CSkinMesh()
{
	Destroy();
}

//-----------------------------------------------------------------------------
// Desc: ���ļ�������Ƥ����ģ��
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::__LoadFromXFile( const PBASICCHAR strFileName )
{
    HRESULT hr;

	LPD3DXMESH pMesh;
	hr = D3DXLoadMeshFromX( strFileName, D3DXMESH_MANAGED, 
		&DEVICE, NULL, 
		NULL, NULL, NULL, 
		&pMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	__GetBoundBox(pMesh, m_LocalRect);
	//DEBUG_RELEASE(pMesh);
	SAFE_RELEASE(pMesh);

	wcscpy(m_Alloc.m_strFilePath, strFileName);
	//��.X�ļ����ز�ο�ܺͶ�������
    hr = D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, &DEVICE, 
		                            &m_Alloc, NULL, &m_pFrameRoot, &m_pAnimController);

	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	//����������ܵ���ϱ任����
    hr = __SetupBoneMatrixPointers(m_pFrameRoot);  

	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	return S_OK;
}

void CSkinMesh::__GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d)
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

	//DEBUG_RELEASE(pVertexBuffer);
	//DEBUG_RELEASE(pTempMesh);
	SAFE_RELEASE(pVertexBuffer);
	SAFE_RELEASE(pTempMesh);
}


//--------------------------------------------------------------------------
// Desc: ����__LoadFromXFile�е��á������Ӻ���__SetupBoneMatrixPointersOnMesh()
//       ���úø������(ʵ�����Ǹ�������)����ϱ任����
// ע��: ��������ʵ��û�м����������������ϱ任����ֻ��Ϊÿ�����󿪱�����Ӧ
//       �Ĵ洢�ռ䣬�����ļ������ں���CSkinMesh::__UpdateFrameMatrices()����ɵġ�
//---------------------------------------------------------------------------
HRESULT CSkinMesh::__SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        hr = __SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //�����Ӻ���
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = __SetupBoneMatrixPointers(pFrame->pFrameSibling);   //�ݹ�
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = __SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //�ݹ�
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: ����__SetupBoneMatrixPointers()�б����ã�����ÿ����������ϱ任����
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::__SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT iBone, cBones;  // cBones��ʾ����������iBone��ʾѭ������
    D3DXFRAME_DERIVED *pFrame;

	//��ǿ��תΪ��չ��
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // ֻ����Ƥ����ģ�Ͳ��й�������
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//�õ���������
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//����洢��������Ŀռ�
		//DEBUG_NEW(pMeshContainer->ppBoneMatrixPtrs, D3DXMATRIX*[cBones]);
		pMeshContainer->ppBoneMatrixPtrs = new D3DXMATRIX*[cBones];
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//�ҵ����
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return E_FAIL;

			//����ܵ���ϱ任����ֵ����Ӧ�Ĺ����ĸ��ϱ任����
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

    return S_OK;
}

//-----------------------------------------------------------------------------
// Desc:���������������ϱ任����
//-----------------------------------------------------------------------------
VOID CSkinMesh::__UpdateFrameMatrices(LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix)
{
    D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;

    if (pParentMatrix != NULL)
        D3DXMatrixMultiply(&pFrame->CombinedTransformationMatrix, &pFrame->TransformationMatrix, pParentMatrix);
    else
        pFrame->CombinedTransformationMatrix = pFrame->TransformationMatrix;

    if (pFrame->pFrameSibling != NULL)
    {
        __UpdateFrameMatrices(pFrame->pFrameSibling, pParentMatrix);
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        __UpdateFrameMatrices(pFrame->pFrameFirstChild, &pFrame->CombinedTransformationMatrix);
    }
}


//-----------------------------------------------------------------------------
// Desc: ���ƿ��.
//       ����CSkinMesh::__DrawMeshContainer()����һ��LPD3DXMESHCONTAINER����
//       �ı���pMeshContainer.Ȼ��ݹ����ͬһ����ܺ���һ����ܡ�
//-----------------------------------------------------------------------------
VOID CSkinMesh::__DrawFrame( LPD3DXFRAME pFrame, CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag )
{
    LPD3DXMESHCONTAINER pMeshContainer;
	
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
        __DrawMeshContainer(pMeshContainer, pFrame, pEntry, uFlag );
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        __DrawFrame( pFrame->pFrameSibling, pEntry, uFlag );
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        __DrawFrame( pFrame->pFrameFirstChild, pEntry, uFlag );
    }
}


//-----------------------------------------------------------------------------
// Name: __DrawMeshContainer()
// Desc: Called to render a mesh in the hierarchy
//-----------------------------------------------------------------------------
void CSkinMesh::__DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag  )
{
	D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	D3DXFRAME_DERIVED *pFrame = (D3DXFRAME_DERIVED*)pFrameBase;
	UINT iMaterial;
	UINT iAttrib;
	LPD3DXBONECOMBINATION pBoneComb;

	UINT iMatrixIndex;
	UINT iPaletteEntry;
	D3DXMATRIXA16 matTemp;
	D3DCAPS9 d3dCaps;
	DEVICE.GetDeviceCaps( &d3dCaps );

	if (pMeshContainer->UseSoftwareVP)
	{
		DEVICE.SetSoftwareVertexProcessing(TRUE);
	}

	//����Ƿ�����Ƥ����ģ��
	if (pMeshContainer->pSkinInfo != NULL)
	{
		pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
		for (iAttrib = 0; iAttrib < pMeshContainer->NumAttributeGroups; iAttrib++)
		{ 
			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[iAttrib].BoneId[iPaletteEntry];
				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply(&matTemp, &pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
					D3DXMatrixMultiply(&m_Alloc.m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
				}
			}
			m_Effect.GetEffect()->SetMatrixArray( "mWorldMatrixArray", m_Alloc.m_pBoneMatrices, pMeshContainer->NumPaletteEntries);

			D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
			D3DXCOLOR color2(0.25, 0.25, 0.25, 1.0);
			D3DXCOLOR ambEmm;
			D3DXColorModulate(&ambEmm, &color1, &color2);
			ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

			//���ò�������
			m_Effect.GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
			m_Effect.GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

			//��������
			m_Effect.GetEffect()->SetTexture( "ColorMap", pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
			m_Effect.GetEffect()->SetTexture( "BumpMap", pMeshContainer->ppBumpMaps[pBoneComb[iAttrib].AttribId] );
			//���õ�ǰ��������
			m_Effect.GetEffect()->SetInt( "CurNumBones", pMeshContainer->NumInfl -1 );

			//���ո��±�־���и���
			if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
				m_Effect.Begin();

			m_Effect.GetEffect()->BeginPass(pEntry->uRenderPass);
			pMeshContainer->MeshData.pMesh->DrawSubset( iAttrib );
			m_Effect.GetEffect()->EndPass();

			m_Effect.End();
		}
	}
	else
	{
		matTemp = pFrame->CombinedTransformationMatrix * m_matView;
		m_Effect.GetEffect()->SetMatrixArray( "mWorldMatrixArray", &matTemp, 1);

		for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
		{
			D3DXCOLOR color1(pMeshContainer->pMaterials[iMaterial].MatD3D.Ambient);
			D3DXCOLOR color2(0.25, 0.25, 0.25, 1.0);
			D3DXCOLOR ambEmm;
			D3DXColorModulate(&ambEmm, &color1, &color2);
			ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[iMaterial].MatD3D.Emissive);

			//���ò�������
			m_Effect.GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse));
			m_Effect.GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);
			//��������
			m_Effect.GetEffect()->SetTexture( "ColorMap", pMeshContainer->ppTextures[iMaterial] );
			m_Effect.GetEffect()->SetTexture( "BumpMap", pMeshContainer->ppBumpMaps[iMaterial] );
			
			//���ո��±�־���и���
			if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
				m_Effect.Begin();

			m_Effect.GetEffect()->BeginPass(pEntry->uRenderPass);
			pMeshContainer->MeshData.pMesh->DrawSubset( iMaterial );
			m_Effect.GetEffect()->EndPass();

			m_Effect.End();
		}
	}

	DEVICE.SetVertexShader(NULL);

	if (pMeshContainer->UseSoftwareVP)
	{
		DEVICE.SetSoftwareVertexProcessing(FALSE);
	}  
}

//-----------------------------------------------------------------------------
// Desc:������������Ƥ����ģ��
//-----------------------------------------------------------------------------
bool CSkinMesh::Create(const PBASICCHAR fileName)
{
	HRESULT hr;

	if ( !m_Effect.Load( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;

	hr = __LoadFromXFile( fileName );
	if(FAILED(hr))
		return false;

	return true;
}

bool CSkinMesh::ApplyForRender()
{
	UINT uTotalPass = m_Effect.GetTechniqueDesc().Passes, i;

	for (i = 0; i < uTotalPass; i ++)
	{
		//������������
		zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

		//����Ϣ�����͵��Ż�����
		pRenderEntry->hEffect      = m_Effect.GetHandle();
		pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
		pRenderEntry->hModel       = RESOURCE_MODEL;
		pRenderEntry->uRenderPass  = (zerO::UINT8)i;
		pRenderEntry->pParent      = this;

		//����
		GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Desc: �ͷ���Ƥ����ģ��
//-----------------------------------------------------------------------------
bool CSkinMesh::Destroy()
{
	if(m_pFrameRoot)
		D3DXFrameDestroy(m_pFrameRoot, &m_Alloc);

    //DEBUG_RELEASE(m_pAnimController);
	//m_pAnimController = NULL;
	SAFE_RELEASE(m_pAnimController);

	return true;
}

HRESULT CSkinMesh::Restore()
{
	HRESULT hr;
	hr = m_Effect.GetEffect()->OnResetDevice();
	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	if (m_pAnimController == NULL)
		return S_FALSE;

	m_pAnimController->ResetTime();
	m_pAnimController->AdvanceTime( m_fTimeCurrent, NULL );

	// Initialize current track
	if( m_szASName[0] != '\0' )
	{
		DWORD dwActiveSet = GetAnimIndex( m_szASName );
		LPD3DXANIMATIONSET pAS = NULL;
		m_pAnimController->GetAnimationSet( dwActiveSet, &pAS );
		m_pAnimController->SetTrackAnimationSet( m_dwCurrentTrack, pAS );
		//DEBUG_RELEASE( pAS );
		SAFE_RELEASE(pAS);
	}

	m_pAnimController->SetTrackEnable( m_dwCurrentTrack, TRUE );
	m_pAnimController->SetTrackWeight( m_dwCurrentTrack, 1.0f );
	m_pAnimController->SetTrackSpeed( m_dwCurrentTrack, 1.0f );
	return hr;
}

HRESULT CSkinMesh::Lost()
{
	HRESULT hr;
	hr = m_Effect.GetEffect()->OnLostDevice();
	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	if (m_pAnimController == NULL)
		return S_FALSE;

	LPD3DXANIMATIONSET pAS = NULL;
	m_pAnimController->GetTrackAnimationSet(m_dwCurrentTrack, &pAS);
	if( pAS->GetName() )
		strcpy( m_szASName, pAS->GetName() );
	//DEBUG_RELEASE(pAS);
	SAFE_RELEASE(pAS);
	return hr;
}

//-----------------------------------------------------------------------------
// Desc: ���ƿ��
//       �ú���CSkinMesh::__DrawFrame()���ƿ��
//-----------------------------------------------------------------------------
void CSkinMesh::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	__DrawFrame(m_pFrameRoot, pEntry, uFlag);  //�����Ӻ���
}

//-----------------------------------------------------------------------------
// Desc: ���¿��
//       (1)��m_pAnimController->AdvanceTime()����ʱ�䣬m_pAnimController��
//          ��LPD3DXANIMATIONCONTROLLER��һ������
//       (2)�ú���CSkinMesh::__UpdateFrameMatrices()���¿��
//-----------------------------------------------------------------------------
void CSkinMesh::Update()
{
	CSceneNode::Update();

	m_matView = CAMERA.GetViewMatrix();

	m_Effect.GetEffect()->SetMatrix( "mProj", &CAMERA.GetProjectionMatrix() );
	
	D3DXVECTOR4 eyePos(CAMERA.GetPosition(), 0.0f);
	m_Effect.GetEffect()->SetVector( "vecEye", &eyePos);

	const D3DLIGHT9* pLight = LIGHTMANAGER.GetLight(0);

	D3DXVECTOR4 LightDirection(pLight->Direction, 1.0f);

	if(pLight)
		m_Effect.GetEffect()->SetVector("vecLightDir", &LightDirection);

	FLOAT fElapsedAppTime = ELAPSEDTIME;

	if( 0.0f == fElapsedAppTime ) 
        return;
	
	m_fTimeCurrent += fElapsedAppTime;

	if (m_bPlayAnim && m_pAnimController != NULL)
	{
		if(m_dwControlPlayTime == 0 || m_dwPlayTime < m_dwControlPlayTime)
			m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

		// ���㶯�����Ŵ���
		m_fFrameTime += fElapsedAppTime;
		if(m_fFrameTime >= m_lfTotalFrameTime)
		{
			m_fFrameTime = 0.0f;
			m_dwPlayTime++;
		}
	}

	__UpdateFrameMatrices(m_pFrameRoot, &m_WorldMatrix);  //�����Ӻ���
	m_Effect.GetEffect()->SetMatrix( "matWorld", &m_WorldMatrix );
}

void CSkinMesh::SetAnimation( zerO::UINT index, DWORD dwControlPlayTime, bool bSmooth )
{
	if( index >= m_pAnimController->GetNumAnimationSets() )
		return;

	LPD3DXANIMATIONSET pAnimSet = NULL;
	m_pAnimController->GetAnimationSet( index, &pAnimSet );

	if(pAnimSet)
	{
		if(!strcmp(m_strNowAnimSetName.c_str(), pAnimSet->GetName()))
			return;

		m_strNowAnimSetName = pAnimSet->GetName();
	}

	m_dwPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_dwControlPlayTime = dwControlPlayTime;

	if( pAnimSet )
	{
		m_lfTotalFrameTime = pAnimSet->GetPeriod();
		if(bSmooth)
			AnimationChangeSmooth(pAnimSet);
		else
		{
			DWORD dwNewTrack = 0;
			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
		}
		pAnimSet->Release();
	}
}

void CSkinMesh::SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime, bool bSmooth )
{
	if(m_pAnimController == NULL)
		return;
	
	if(!strcmp(m_strNowAnimSetName.c_str(), pName))
		return;
	
	m_strNowAnimSetName = pName;

	m_dwPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_dwControlPlayTime = dwControlPlayTime;

	LPD3DXANIMATIONSET pAnimSet = NULL;
	m_pAnimController->SetTrackPosition(0, 0.0);
	m_pAnimController->GetAnimationSetByName( pName, &pAnimSet );
	if( pAnimSet )
	{
		m_lfTotalFrameTime = pAnimSet->GetPeriod();
		if(bSmooth)
			AnimationChangeSmooth(pAnimSet);
		else
		{
			DWORD dwNewTrack = 0;
			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
		}
		pAnimSet->Release();
	}
}

void CSkinMesh::AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet)
{
	DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );

	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );

	m_pAnimController->UnkeyAllTrackEvents( m_dwCurrentTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	m_pAnimController->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_fTimeCurrent + ANIM_TRANSITION_TIME );
	m_pAnimController->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

	m_dwCurrentTrack = dwNewTrack;
}

DWORD CSkinMesh::GetPlayTime() 
{ 
	// ���û�ж�������������Ĭ�ϲ�����һ��
	if(m_pAnimController == NULL)
		m_dwPlayTime = 1;

	return m_dwPlayTime; 
}

DWORD CSkinMesh::GetAnimIndex( char sString[] )
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	DWORD dwRet = 0xffffffff;

	for( DWORD i = 0; i < m_pAnimController->GetNumAnimationSets(); ++ i )
	{
		hr = m_pAnimController->GetAnimationSet( i, & pAS );
		if( FAILED( hr ) )
			continue;

		if( pAS->GetName() &&
			!strncmp( pAS->GetName(), sString, min( strlen( pAS->GetName() ), strlen( sString ) ) ) )
		{
			dwRet = i;
			pAS->Release();
			break;
		}

		pAS->Release();
	}

	return dwRet;
}

bool CSkinMesh::CanPlay(LPSTR pName, DWORD count)
{
	if (strcmp(m_strNowAnimSetName.c_str(), pName) == 0 
		|| GetPlayTime() >= count)
		return true;

	return false;
}
