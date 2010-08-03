#include "StdAfx.h"
#include "Model.h"
#include "basicutils.h"

using namespace zerO;

CAllocateHierarchy::CAllocateHierarchy() :
m_pBoneMatrices(NULL), 
m_pMesh(NULL),
m_NumBoneMatricesMax(0),
m_uNumContainers(0),
m_Type(HARDWARE)
{
}

CAllocateHierarchy::~CAllocateHierarchy()
{
	DEBUG_DELETE_ARRAY(m_pBoneMatrices);
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
//Desc: 生成蒙皮网格模型(含有每个顶点的混合权重、索引和一个骨骼组合表)
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::__GenerateSkinnedMesh(MODELCONTAINER *pMeshContainer)
{
	HRESULT hr = S_OK;
	if( pMeshContainer->pSkinInfo == NULL )
		return hr;

	if(m_Type == HARDWARE)
	{
		//获取当前设备的能力
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

		DEBUG_RELEASE(pMeshContainer->MeshData.pMesh);
		DEBUG_RELEASE(pMeshContainer->pBoneCombinationBuf);

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
	}
	else if(m_Type == SOFTWARE)
	{

		hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( NULL, &pMeshContainer->NumAttributeGroups );
	        if( FAILED( hr ) )
	            return hr;

	    DEBUG_DELETE(pMeshContainer->pAttributeTable);
	    pMeshContainer->pAttributeTable = new D3DXATTRIBUTERANGE[pMeshContainer->NumAttributeGroups];
	    if( pMeshContainer->pAttributeTable == NULL )
	    {
	        hr = E_OUTOFMEMORY;
	        return hr;
	    }

	    hr = pMeshContainer->MeshData.pMesh->GetAttributeTable( pMeshContainer->pAttributeTable, NULL );
	    if( FAILED( hr ) )
	        return hr;
	}

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
		DEBUG_DELETE_ARRAY(m_pBoneMatrices); 
		DEBUG_NEW(m_pBoneMatrices, D3DXMATRIXA16[m_NumBoneMatricesMax]);
		if( m_pBoneMatrices == NULL )
		{
			hr = E_OUTOFMEMORY;
			return hr;
		}
	}

	return hr;
}

HRESULT CAllocateHierarchy::__GenerateDeclMesh(MODELCONTAINER *pMeshContainer)
{
	HRESULT hr;

	if (pMeshContainer == NULL)
		return S_FALSE;

	LPD3DXMESH pMeshSysMem = pMeshContainer->MeshData.pMesh;

	LPD3DXMESH  pMeshSysMem2  = NULL;

	if(m_Type == HARDWARE)
	{
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

		hr = pMeshSysMem->CloneMesh(D3DXMESH_MANAGED, decl, &DEVICE, &pMeshSysMem2);
		if (FAILED(hr))
			return hr;

		//确保顶点包含法线
		hr = D3DXComputeNormals(pMeshSysMem2,NULL);
		if (FAILED(hr))
			return hr;

		//计算切线
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

		hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl2, &DEVICE, &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
			return hr;
	}
	else if(m_Type == SOFTWARE)
	{
		D3DVERTEXELEMENT9   decl[]   = 
		{ 
			{   0,   0,	   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,   0   }, 
			{   0,   12,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,   0   }, 
			{   0,   24,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,   0   }, 
			{   0,   32,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TANGENT,   0   }, 
			D3DDECL_END(), 
		}; 

		hr = pMeshSysMem->CloneMesh(D3DXMESH_MANAGED, decl, &DEVICE, &pMeshSysMem2);
		if (FAILED(hr))
			return hr;

		//确保顶点包含法线
		hr = D3DXComputeNormals(pMeshSysMem2,NULL);
		if (FAILED(hr))
			return hr;

		//计算切线
		hr = D3DXComputeTangent( pMeshSysMem2, 0, 0, 0, true, NULL );
		if (FAILED(hr))
			return hr;

		D3DVERTEXELEMENT9   decl2[]   = 
		{ 
			{   0,   0,	   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,   0   }, 
			{   0,   16,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,   0   }, 
			{   0,   28,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,   0   }, 
			{   0,   36,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TANGENT,   0   }, 
			D3DDECL_END(), 
		}; 

		//pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
		hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl2, &DEVICE, &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
			return hr;

		//pMeshContainer->MeshData.pMesh = pMeshSysMem2;

		if(pMeshContainer->pSkinInfo)
		{
			UINT uNumVertices = pMeshContainer->pOrigMesh->GetNumVertices();

			pMeshContainer->puMeshBuffer  = new UINT8[pMeshContainer->pOrigMesh->GetNumBytesPerVertex() * uNumVertices];
			pMeshContainer->pTangentBuffer = new D3DXVECTOR3[uNumVertices];
			pMeshContainer->pTangentInfo   = new D3DXVECTOR3[uNumVertices];

			if(pMeshContainer->puMeshBuffer == NULL 
				|| pMeshContainer->pTangentBuffer == NULL
				|| pMeshContainer->pTangentInfo == NULL)
			{
				hr = E_OUTOFMEMORY;
				DestroyMeshContainer(pMeshContainer);
				return hr;
			}

			zerO::PUINT8 puVertices;

			zerO::UINT uVertexSize = pMeshSysMem2->GetNumBytesPerVertex(), uTangentOffset = uVertexSize - sizeof(D3DXVECTOR3);

			pMeshSysMem2->LockVertexBuffer(0, (void**)&puVertices);

			for(zerO::UINT i = 0; i < uNumVertices; i ++)
			{
				pMeshContainer->pTangentInfo[i] = *( (D3DXVECTOR3*)(puVertices + uTangentOffset) );

				puVertices += uVertexSize;
			}

			pMeshSysMem2->UnlockVertexBuffer();
		}
	}
	else
		return S_FALSE;
	
	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

	//确保顶点包含法线
	//hr = D3DXComputeNormals(pMeshSysMem2,NULL);
	//if (FAILED(hr))
	//	return hr;

	////计算切线
	//hr = D3DXComputeTangent( pMeshSysMem2, 0, 0, 0, true, NULL );
	//if (FAILED(hr))
	//	return hr;

	//if(m_Type == SOFTWARE)
	//{
	//	
	//}

	/*D3DVERTEXELEMENT9   decl2[]   = 
	{ 
		{   0,   0,	   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_POSITION,   0   }, 
		{   0,   16,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_NORMAL,   0   }, 
		{   0,   28,   D3DDECLTYPE_FLOAT2,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TEXCOORD,   0   }, 
		{   0,   36,   D3DDECLTYPE_FLOAT4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDWEIGHT,   0   }, 
		{   0,   52,   D3DDECLTYPE_UBYTE4,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BLENDINDICES,   0   }, 
		{   0,   56,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_BINORMAL,   0   }, 
		{   0,   68,   D3DDECLTYPE_FLOAT3,   D3DDECLMETHOD_DEFAULT,   D3DDECLUSAGE_TANGENT,   0   }, 
		D3DDECL_END(), 
	}; */

	/*pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	hr = pMeshSysMem2->CloneMesh(D3DXMESH_MANAGED, decl, &DEVICE, &pMeshContainer->MeshData.pMesh );
	if (FAILED(hr))
		return hr;*/

	//pMeshContainer->MeshData.pMesh = pMeshSysMem2;

	//释放临时网格模型对象
	DEBUG_RELEASE(pMeshSysMem);
	DEBUG_RELEASE(pMeshSysMem2);

	m_pMesh = pMeshContainer->MeshData.pMesh;

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: 创建框架, 仅仅是分配内存和初始化,还没有对其成员赋予合适的值
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    MODELFRAME *pFrame;
	
    *ppNewFrame = NULL;
	
    //创建框架结构对象
	pFrame = new MODELFRAME;
    if (pFrame == NULL) 
    {
        return E_OUTOFMEMORY;
    }
	
	//为框架指定名称
    hr = __AllocateName(Name, (LPSTR*)&pFrame->Name);
	if (FAILED(hr))
	{
		delete pFrame;
		return hr;
	}
	
	//初始化MODELFRAME结构其它成员变量
    D3DXMatrixIdentity(&pFrame->TransformationMatrix);
    D3DXMatrixIdentity(&pFrame->CombinedTransformationMatrix);
	
    pFrame->pMeshContainer = NULL;
    pFrame->pFrameSibling = NULL;
    pFrame->pFrameFirstChild = NULL;
	
    *ppNewFrame = pFrame;

	return hr;
}

//-----------------------------------------------------------------------------
// Desc: 在这里是调用了成员函数 __GenerateSkinnedMesh(pMeshContainer);
//       是在这里加载了蒙皮信息
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
    MODELCONTAINER *pMeshContainer = NULL;
    UINT NumFaces;       //网格中的面数,在填充网格容器结构的邻接信息成员时使用
    UINT iMaterial;      //纹理操作时的循环变量
    UINT cBones;         //当前网格模型骨骼总数
    LPD3DXMESH pMesh    = NULL;
    *ppNewMeshContainer = NULL;
	
    if (pMeshData->Type != D3DXMESHTYPE_MESH)
    {
        return E_FAIL;
    }
	
    pMesh = pMeshData->pMesh;

	//为网格容器分配内存
	pMeshContainer = new MODELCONTAINER;
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(MODELCONTAINER));
	
	//填充网格容器结构MODELCONTAINER的成员

	//为网格指定名称
	hr = __AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	
	NumFaces = pMesh->GetNumFaces();

	 // if no normals are in the mesh, add them
 //   if( !( pMesh->GetFVF() & D3DFVF_NORMAL ) )
	//{
 //       pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

 //   // clone the mesh to make room for the normals
 //       hr = pMesh->CloneMeshFVF( pMesh->GetOptions(),
 //                                   pMesh->GetFVF() | D3DFVF_NORMAL,
 //                                   &DEVICE, &pMeshContainer->MeshData.pMesh );
 //       if( FAILED( hr ) )
 //           return hr;

 //   // get the new pMesh pointer back out of the mesh container to use
 //   // NOTE: we do not release pMesh because we do not have a reference to it yet
 //       pMesh = pMeshContainer->MeshData.pMesh;

 //   // now generate the normals for the pmesh
 //       D3DXComputeNormals( pMesh, NULL );
 //   }
 //   else  // if no normals, just add a reference to the mesh for the mesh container
	//{
 //       pMeshContainer->MeshData.pMesh = pMesh;
 //       pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;

 //       pMesh->AddRef();
 //   }

	pMeshContainer->MeshData.pMesh = pMesh;

	pMesh->AddRef();

	pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
	
    //为网格模型准备材质和纹理
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
	pMeshContainer->pSurfaces    = new CSurface[pMeshContainer->NumMaterials];
	pMeshContainer->pAdjacency   = new DWORD[NumFaces*3];

    if ((pMeshContainer->pSurfaces == NULL)
		|| (pMeshContainer->pAdjacency == NULL) )
	{
		hr = E_OUTOFMEMORY;
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  


	D3DMATERIAL9 Material;
    memset( &Material, 0, sizeof(D3DMATERIAL9) );

   if (NumMaterials > 0)            
    {    
		//提取纹理文件, 创建纹理对象
        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
			Material         = pMaterials[iMaterial].MatD3D;
			Material.Ambient = Material.Diffuse;

			pMeshContainer->pSurfaces[iMaterial].SetMaterial(Material);

            if (pMaterials[iMaterial].pTextureFilename != NULL)
            {
#ifdef _UNICODE
                WCHAR szFile[MAX_PATH];
				//从纹理文件路径提取纹理文件名
				RemovePathFromFileName(pMaterials[iMaterial].pTextureFilename, szFile);
				BASICSTRING texFile;
				GetRealPath(m_strFilePath, texFile, TEXT("/"), szFile);
				pMeshContainer->pSurfaces[iMaterial].LoadTexture( (PBASICCHAR)texFile.c_str(), 0 );

				BASICSTRING normalMapFile;
				GetRealPath((PBASICCHAR)texFile.c_str(), normalMapFile, TEXT("."), TEXT("-normalmap.tga"), true);
				pMeshContainer->pSurfaces[iMaterial].LoadTexture( (PBASICCHAR)normalMapFile.c_str(), 1 );
#else
				if( FAILED( D3DXCreateTextureFromFile( &DEVICE, pMeshContainer->pMaterials[iMaterial].pTextureFilename,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;
#endif
			}
        }
    }
    else
    {
        Material.Diffuse.r = 0.5f;
        Material.Diffuse.g = 0.5f;
        Material.Diffuse.b = 0.5f;
        Material.Specular  = Material.Diffuse;

		pMeshContainer->pSurfaces[0].SetMaterial(Material);
    }

    //如果当前网格包含蒙皮信息
    if (pSkinInfo != NULL)
    {
		//加载蒙皮网格信息
        pMeshContainer->pSkinInfo = pSkinInfo; 
        pSkinInfo->AddRef();
		
		//保留原网格信息
        pMeshContainer->pOrigMesh = pMesh;
        pMesh->AddRef();

		//获取骨骼数量
        cBones = pSkinInfo->GetNumBones();

		//为每块骨骼分配保存初始变换矩阵的内存空间
		pMeshContainer->pBoneOffsetMatrices = new D3DXMATRIX[cBones];
        if (pMeshContainer->pBoneOffsetMatrices == NULL) 
        {
			hr = E_OUTOFMEMORY;
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}
        
		//保存每块骨骼的初始变换矩阵
        for (UINT iBone = 0; iBone < cBones; iBone++)
        {
            pMeshContainer->pBoneOffsetMatrices[iBone] = *(pMeshContainer->pSkinInfo->GetBoneOffsetMatrix(iBone));
        }
		
		//生成蒙皮网格模型
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

	m_uNumContainers ++;

    return hr;
}

//-----------------------------------------------------------------------------
// Desc: 释放框架
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyFrame(LPD3DXFRAME pFrameToFree) 
{
	if(pFrameToFree != NULL)
	{
		DEBUG_DELETE_ARRAY( pFrameToFree->Name );
		DEBUG_DELETE( pFrameToFree );
	}

    return S_OK; 
}


//-----------------------------------------------------------------------------
// Desc: 释放网格容器
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::DestroyMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
	if(pMeshContainerBase == NULL)
		return S_OK;

	// 先转为扩展型以免内存泄漏
    MODELCONTAINER *pMeshContainer = (MODELCONTAINER*)pMeshContainerBase;
	
    DEBUG_DELETE_ARRAY( pMeshContainer->Name );
    DEBUG_DELETE_ARRAY( pMeshContainer->pAdjacency );
    DEBUG_DELETE_ARRAY(pMeshContainer->puMeshBuffer);
	DEBUG_DELETE_ARRAY(pMeshContainer->pTangentBuffer);
	DEBUG_DELETE_ARRAY(pMeshContainer->pTangentInfo);
    DEBUG_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );
	DEBUG_DELETE_ARRAY( pMeshContainer->pSurfaces);
	DEBUG_DELETE_ARRAY(pMeshContainer->ppBoneMatrixPtrs);

    DEBUG_RELEASE( pMeshContainer->pBoneCombinationBuf );
    DEBUG_RELEASE( pMeshContainer->MeshData.pMesh );
    DEBUG_RELEASE( pMeshContainer->pSkinInfo );
    DEBUG_RELEASE( pMeshContainer->pOrigMesh );

    DEBUG_DELETE( pMeshContainer );

    return S_OK;
}


#define ANIM_TRANSITION_TIME 0.25		//平滑变换时间


CModel::CModel(void) :
CResource(RESOURCE_MODEL),
m_bPlayAnim(true),
m_pAnimController(NULL),
m_ppContainers(NULL),
m_pFrameRoot(NULL),
m_uCurrentTrack(0),
m_fTimeCurrent(0.0f),
m_uPlayTime(0),
m_fFrameTime(0.0f),
m_lfTotalFrameTime(0.0),
m_uControlPlayTime(0),
m_uNumContainers(0)
{
}

CModel::~CModel(void)
{
	Destroy();
}

bool CModel::Destroy()
{
	if(m_pFrameRoot)
	{
		__ReleaseFrame(m_pFrameRoot);

		D3DXFrameDestroy(m_pFrameRoot, &m_Alloc);

		m_pFrameRoot = NULL;
	}

    DEBUG_RELEASE(m_pAnimController);
	m_pAnimController = NULL;

	DEBUG_DELETE_ARRAY(m_ppContainers);
	m_ppContainers = NULL;

	return true;
}

bool CModel::Restore()
{
	if (m_pAnimController == NULL)
		return false;

	m_pAnimController->ResetTime();
	m_pAnimController->AdvanceTime( m_fTimeCurrent, NULL );

	// Initialize current track
	if( m_szASName[0] != '\0' )
	{
		DWORD dwActiveSet = GetAnimIndex( m_szASName );
		LPD3DXANIMATIONSET pAS = NULL;
		m_pAnimController->GetAnimationSet( dwActiveSet, &pAS );
		m_pAnimController->SetTrackAnimationSet( m_uCurrentTrack, pAS );
		DEBUG_RELEASE( pAS );
	}

	m_pAnimController->SetTrackEnable( m_uCurrentTrack, TRUE );
	m_pAnimController->SetTrackWeight( m_uCurrentTrack, 1.0f );
	m_pAnimController->SetTrackSpeed( m_uCurrentTrack, 1.0f );

	return true;
}

bool CModel::Disable()
{
	if (m_pAnimController == NULL)
		return false;

	LPD3DXANIMATIONSET pAS = NULL;
	m_pAnimController->GetTrackAnimationSet(m_uCurrentTrack, &pAS);
	if( pAS->GetName() )
		strcpy( m_szASName, pAS->GetName() );
	DEBUG_RELEASE(pAS);

	return true;
}

//-----------------------------------------------------------------------------
// Desc: 从文件加载蒙皮网格模型
//-----------------------------------------------------------------------------
bool CModel::Load(const PBASICCHAR pcFileName)
{
    HRESULT hr;

	LPD3DXMESH pMesh;
	hr = D3DXLoadMeshFromX(pcFileName, D3DXMESH_MANAGED, 
		&DEVICE, NULL, 
		NULL, NULL, NULL, 
		&pMesh );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	/*__GetBoundBox(pMesh, m_LocalRect);*/
	DEBUG_RELEASE(pMesh);
	pMesh = NULL;

	wcscpy(m_Alloc.m_strFilePath, pcFileName);

	LPD3DXANIMATIONCONTROLLER pAnimController;

	//从.X文件加载层次框架和动画数据
    hr = D3DXLoadMeshHierarchyFromX(pcFileName, D3DXMESH_MANAGED, &DEVICE, 
		                            &m_Alloc, NULL, &m_pFrameRoot, &pAnimController);
	
	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	pAnimController->CloneAnimationController(
		pAnimController->GetMaxNumAnimationOutputs(),
		pAnimController->GetMaxNumAnimationSets(),
		pAnimController->GetMaxNumTracks(),
		pAnimController->GetMaxNumEvents(),
		&m_pAnimController);

	DEBUG_RELEASE(pAnimController);

	DEBUG_NEW(m_ppContainers, LPMODELCONTAINER[m_Alloc.GetContainersNumber()]);

	//建立各级框架的组合变换矩阵
    hr = __SetupFrame(m_pFrameRoot);  

	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	return true;
}

void CModel::Update(zerO::FLOAT fElapsedAppTime)
{
	if( 0.0f == fElapsedAppTime ) 
        return;
	
	m_fTimeCurrent += fElapsedAppTime;

	if (m_bPlayAnim && m_pAnimController != NULL)
	{
		if(m_uControlPlayTime == 0 || m_uPlayTime < m_uControlPlayTime)
			m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

		// 计算动画播放次数
		m_fFrameTime += fElapsedAppTime;
		if(m_fFrameTime >= m_lfTotalFrameTime)
		{
			m_fFrameTime = 0.0f;
			m_uPlayTime++;
		}
	}
}

void CModel::Update(const LPD3DXMATRIX pMatrix)
{
	__UpdateFrameMatrices(m_pFrameRoot, pMatrix);  //调用子函数
}

void CModel::SetAnimation( zerO::UINT index, DWORD dwControlPlayTime, bool bSmooth )
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

	m_uPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_uControlPlayTime = dwControlPlayTime;

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

void CModel::SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime, bool bSmooth )
{
	if(m_pAnimController == NULL)
		return;
	
	if(!strcmp(m_strNowAnimSetName.c_str(), pName))
		return;
	
	m_strNowAnimSetName = pName;

	m_uPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_uControlPlayTime = dwControlPlayTime;

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

void CModel::AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet)
{
	DWORD dwNewTrack = ( m_uCurrentTrack == 0 ? 1 : 0 );

	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );

	m_pAnimController->UnkeyAllTrackEvents( m_uCurrentTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	m_pAnimController->KeyTrackEnable( m_uCurrentTrack, FALSE, m_fTimeCurrent + ANIM_TRANSITION_TIME );
	m_pAnimController->KeyTrackSpeed( m_uCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_uCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );

	m_uCurrentTrack = dwNewTrack;
}

zerO::UINT CModel::GetPlayTime() 
{ 
	// 如果没有动画控制器，则默认播放了一次
	if(m_pAnimController == NULL)
		m_uPlayTime = 1;

	return m_uPlayTime; 
}

zerO::UINT CModel::GetAnimIndex( char sString[] )
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	zerO::UINT dwRet = 0xffffffff;

	for( zerO::UINT i = 0; i < m_pAnimController->GetNumAnimationSets(); ++ i )
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

bool CModel::CanPlay(LPSTR pName, DWORD count)
{
	if (strcmp(m_strNowAnimSetName.c_str(), pName) == 0 
		|| GetPlayTime() >= count)
		return true;

	return false;
}

bool CModel::__SetupFrame(LPD3DXFRAME pFrame)
{
    if (pFrame->pMeshContainer != NULL)
    {
		( (LPMODELCONTAINER)pFrame->pMeshContainer )->pParent = (LPMODELFRAME)pFrame;

        if( !__SetupMeshContainer(pFrame->pMeshContainer) )
			return false;  //调用子函数
    }

    if (pFrame->pFrameSibling != NULL)
    {
        if( !__SetupFrame(pFrame->pFrameSibling) )
			return false;   //递归
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        if( !__SetupFrame(pFrame->pFrameFirstChild) )
			return false;//递归
    }

    return true;
}

bool CModel::__SetupMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT iBone, cBones;  // cBones表示骨骼数量，iBone表示循环变量
    MODELFRAME *pFrame;

	//先强制转为扩展型
    MODELCONTAINER *pMeshContainer = (MODELCONTAINER*)pMeshContainerBase;

	pMeshContainer->uIndex             = m_uNumContainers;

	m_ppContainers[m_uNumContainers ++] = pMeshContainer;

    // 只有蒙皮网格模型才有骨骼矩阵
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//得到骨骼数量
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//申请存储骨骼矩阵的空间
		DEBUG_NEW(pMeshContainer->ppBoneMatrixPtrs, D3DXMATRIX*[cBones]);

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//找到框架
            pFrame = (MODELFRAME*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return false;

			//将框架的组合变换矩阵赋值给对应的骨骼的复合变换矩阵
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

	return true;
}

//-----------------------------------------------------------------------------
// Desc:计算各个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
void CModel::__UpdateFrameMatrices(LPD3DXFRAME pFrameBase, const LPD3DXMATRIX pParentMatrix)
{
    MODELFRAME *pFrame = (MODELFRAME*)pFrameBase;

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

void CModel::__ReleaseFrame( LPD3DXFRAME pFrameBase )
{
    LPMODELFRAME pFrame = (LPMODELFRAME)pFrameBase;
    LPMODELCONTAINER pMeshContainer;

    pMeshContainer = (LPMODELCONTAINER)pFrame->pMeshContainer;

    while( pMeshContainer != NULL )
    {
        DEBUG_DELETE(pMeshContainer->pAttributeTable);

        pMeshContainer = (LPMODELCONTAINER)pMeshContainer->pNextMeshContainer;
    }

    if( pFrame->pFrameSibling != NULL )
    {
        __ReleaseFrame( pFrame->pFrameSibling );
    }

    if( pFrame->pFrameFirstChild != NULL )
    {
        __ReleaseFrame( pFrame->pFrameFirstChild );
    }
}