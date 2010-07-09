#include "StdAfx.h"
#include "basicutils.h"
#include "RenderQueue.h"
#include "SkinMesh.h"

using namespace zerO;

//=============================================================================
//Desc: AllocateHierarchy.cpp
//=============================================================================

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
		DEBUG_NEW(*pNewName, CHAR[cbLength]);
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
// Desc: 创建框架, 仅仅是分配内存和初始化,还没有对其成员赋予合适的值
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::CreateFrame(LPCSTR Name, LPD3DXFRAME *ppNewFrame)
{
    HRESULT hr = S_OK;
    D3DXFRAME_DERIVED *pFrame;
	
    *ppNewFrame = NULL;
	
    //创建框架结构对象
	DEBUG_NEW(pFrame, D3DXFRAME_DERIVED);
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
	
	//初始化D3DXFRAME_DERIVED结构其它成员变量
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
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = NULL;
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
	
    if (pMesh->GetFVF() == 0)
    {
        return E_FAIL;
    }

	//为网格容器分配内存
	DEBUG_NEW(pMeshContainer, D3DXMESHCONTAINER_DERIVED);
    if (pMeshContainer == NULL)
    {
        return E_OUTOFMEMORY;
    }
    memset(pMeshContainer, 0, sizeof(D3DXMESHCONTAINER_DERIVED));
	
	//填充网格容器结构D3DXMESHCONTAINER_DERIVED的成员

	//为网格指定名称
	hr = __AllocateName(Name, &pMeshContainer->Name);
	if (FAILED(hr))
	{
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}      
	
    NumFaces = pMesh->GetNumFaces();

	//确保网格顶点包含法线
    if (!(pMesh->GetFVF() & D3DFVF_NORMAL))
    {
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        hr = pMesh->CloneMeshFVF( pMesh->GetOptions(), 
			                      pMesh->GetFVF() | D3DFVF_NORMAL, 
								  &DEVICE, 
			                      &pMeshContainer->MeshData.pMesh );
		if (FAILED(hr))
		{
			DestroyMeshContainer(pMeshContainer);
			return hr;
		}

		pMesh = pMeshContainer->MeshData.pMesh;
        D3DXComputeNormals( pMesh, NULL );
    }
    else 
    {
        pMeshContainer->MeshData.pMesh = pMesh;
        pMeshContainer->MeshData.Type = D3DXMESHTYPE_MESH;
        pMesh->AddRef();
    }
	
    //为网格模型准备材质和纹理
    pMeshContainer->NumMaterials = max(1, NumMaterials); 
	DEBUG_NEW(pMeshContainer->pMaterials, D3DXMATERIAL[pMeshContainer->NumMaterials]);
	DEBUG_NEW(pMeshContainer->ppTextures, LPDIRECT3DTEXTURE9[pMeshContainer->NumMaterials]);
	DEBUG_NEW(pMeshContainer->pAdjacency, DWORD[NumFaces*3]);
    if ((pMeshContainer->pAdjacency == NULL) || (pMeshContainer->pMaterials == NULL)
		|| (pMeshContainer->ppTextures == NULL))
	{
		hr = E_OUTOFMEMORY;
		DestroyMeshContainer(pMeshContainer);
		return hr;
	}

	memcpy(pMeshContainer->pAdjacency, pAdjacency, sizeof(DWORD) * NumFaces*3);  
    memset(pMeshContainer->ppTextures, 0, sizeof(LPDIRECT3DTEXTURE9) * pMeshContainer->NumMaterials);
	
   if (NumMaterials > 0)            
    {    
        //复制材质属性, 设置材质环境光属性
        memcpy(pMeshContainer->pMaterials, pMaterials, sizeof(D3DXMATERIAL) * NumMaterials); 
		pMeshContainer->pMaterials->MatD3D.Ambient = pMeshContainer->pMaterials->MatD3D.Diffuse;
       
		//提取纹理文件, 创建纹理对象
        for (iMaterial = 0; iMaterial < NumMaterials; iMaterial++) 
        {
            if (pMeshContainer->pMaterials[iMaterial].pTextureFilename != NULL)
            {
#ifdef _UNICODE
                WCHAR wszBuf[MAX_PATH];
				//从纹理文件路径提取纹理文件名
				RemovePathFromFileName(pMeshContainer->pMaterials[iMaterial].pTextureFilename, wszBuf);
                if( FAILED( D3DXCreateTextureFromFile( &DEVICE, wszBuf,
                                                        &pMeshContainer->ppTextures[iMaterial] ) ) )
                    pMeshContainer->ppTextures[iMaterial] = NULL;
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
		DEBUG_NEW(pMeshContainer->pBoneOffsetMatrices, D3DXMATRIX[cBones]);
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
	
    *ppNewMeshContainer = pMeshContainer;
    pMeshContainer = NULL;
    return hr;
}

//-----------------------------------------------------------------------------
//Desc: 生成蒙皮网格模型(含有每个顶点的混合权重、索引和一个骨骼组合表)
//-----------------------------------------------------------------------------
HRESULT CAllocateHierarchy::__GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer)
{
	HRESULT hr = S_OK;
	if( pMeshContainer->pSkinInfo == NULL )
		return hr;

	DEBUG_RELEASE( pMeshContainer->MeshData.pMesh );
	DEBUG_RELEASE( pMeshContainer->pBoneCombinationBuf );

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

	// FVF has to match our declarator. Vertex shaders are not as forgiving as FF pipeline
	DWORD NewFVF = (pMeshContainer->MeshData.pMesh->GetFVF() & D3DFVF_POSITION_MASK) | D3DFVF_NORMAL | D3DFVF_TEX1 | D3DFVF_LASTBETA_UBYTE4;
	if (NewFVF != pMeshContainer->MeshData.pMesh->GetFVF())
	{
		LPD3DXMESH pMesh;
		hr = pMeshContainer->MeshData.pMesh->CloneMeshFVF(pMeshContainer->MeshData.pMesh->GetOptions(), NewFVF, &DEVICE, &pMesh);
		if (!FAILED(hr))
		{
			pMeshContainer->MeshData.pMesh->Release();
			pMeshContainer->MeshData.pMesh = pMesh;
			pMesh = NULL;
		}
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

    UINT iMaterial;
	// 先转为扩展型以免内存泄漏
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;
	
    DEBUG_DELETE_ARRAY( pMeshContainer->Name );
    DEBUG_DELETE_ARRAY( pMeshContainer->pAdjacency );
    DEBUG_DELETE_ARRAY( pMeshContainer->pMaterials );
    DEBUG_DELETE_ARRAY( pMeshContainer->pBoneOffsetMatrices );
	
    if (pMeshContainer->ppTextures != NULL)
    {
        for (iMaterial = 0; iMaterial < pMeshContainer->NumMaterials; iMaterial++)
        {
            DEBUG_RELEASE( pMeshContainer->ppTextures[iMaterial] );
        }
    }
    DEBUG_DELETE_ARRAY( pMeshContainer->ppTextures );

    DEBUG_DELETE_ARRAY( pMeshContainer->ppBoneMatrixPtrs );
    DEBUG_RELEASE( pMeshContainer->pBoneCombinationBuf );
    DEBUG_RELEASE( pMeshContainer->MeshData.pMesh );
    DEBUG_RELEASE( pMeshContainer->pSkinInfo );
    DEBUG_RELEASE( pMeshContainer->pOrigMesh );
    DEBUG_DELETE( pMeshContainer );
    return S_OK;
}


//----------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------------
// Desc: 构造函数和析构函数  Construction/Destruction
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
m_strMeshFile(TEXT("")),
m_strEffectFile(TEXT("HLSLSkinMesh.fx"))
{
	DEBUG_NEW(m_pAlloc, CAllocateHierarchy);
}


//-----------------------------------------------------------------------------
// Desc: 构造函数和析构函数 
//-----------------------------------------------------------------------------
CSkinMesh::~CSkinMesh()
{
	D3DXFrameDestroy(m_pFrameRoot, m_pAlloc);
    DEBUG_RELEASE(m_pAnimController);
	/*DEBUG_RELEASE(m_pEffect);*/
	DEBUG_DELETE(m_pAlloc);
}


//-----------------------------------------------------------------------------
// Desc:创建并加载蒙皮网格模型
//-----------------------------------------------------------------------------
bool CSkinMesh::Create()
{
	HRESULT hr;

	if ( !m_Effect.Load( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;
	
	hr = __LoadFromXFile((PBASICCHAR)m_strMeshFile.c_str());
	if(FAILED(hr))
		return false;

	return true;
}


//-----------------------------------------------------------------------------
// Desc: 从文件加载蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::__LoadFromXFile(WCHAR *strFileName)
{
    HRESULT hr;

	//从.X文件加载层次框架和动画数据
    hr = D3DXLoadMeshHierarchyFromX(strFileName, D3DXMESH_MANAGED, &DEVICE, 
		                            m_pAlloc, NULL, &m_pFrameRoot, &m_pAnimController);

	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	//建立各级框架的组合变换矩阵
    hr = __SetupBoneMatrixPointers(m_pFrameRoot);  

	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}

	return S_OK;
}


//--------------------------------------------------------------------------
// Desc: 仅在__LoadFromXFile中调用。调用子函数__SetupBoneMatrixPointersOnMesh()
//       安置好各级框架(实际上是各个骨骼)的组合变换矩阵。
// 注意: 在这里其实并没有计算出各个骨骼的组合变换矩阵，只是为每个矩阵开辟了相应
//       的存储空间，真正的计算是在函数CSkinMesh::__UpdateFrameMatrices()中完成的。
//---------------------------------------------------------------------------
HRESULT CSkinMesh::__SetupBoneMatrixPointers(LPD3DXFRAME pFrame)
{
    HRESULT hr;

    if (pFrame->pMeshContainer != NULL)
    {
        hr = __SetupBoneMatrixPointersOnMesh(pFrame->pMeshContainer);  //调用子函数
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameSibling != NULL)
    {
        hr = __SetupBoneMatrixPointers(pFrame->pFrameSibling);   //递归
        if (FAILED(hr))
            return hr;
    }

    if (pFrame->pFrameFirstChild != NULL)
    {
        hr = __SetupBoneMatrixPointers(pFrame->pFrameFirstChild);  //递归
        if (FAILED(hr))
            return hr;
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 仅在__SetupBoneMatrixPointers()中被调用，设置每个骨骼的组合变换矩阵
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::__SetupBoneMatrixPointersOnMesh(LPD3DXMESHCONTAINER pMeshContainerBase)
{
    UINT iBone, cBones;  // cBones表示骨骼数量，iBone表示循环变量
    D3DXFRAME_DERIVED *pFrame;

	//先强制转为扩展型
    D3DXMESHCONTAINER_DERIVED *pMeshContainer = (D3DXMESHCONTAINER_DERIVED*)pMeshContainerBase;

    // 只有蒙皮网格模型才有骨骼矩阵
    if (pMeshContainer->pSkinInfo != NULL)
    {
		//得到骨骼数量
        cBones = pMeshContainer->pSkinInfo->GetNumBones();

		//申请存储骨骼矩阵的空间
		DEBUG_NEW(pMeshContainer->ppBoneMatrixPtrs, D3DXMATRIX*[cBones]);
		if (pMeshContainer->ppBoneMatrixPtrs == NULL)
            return E_OUTOFMEMORY;

        for (iBone = 0; iBone < cBones; iBone++)
        {
			//找到框架
            pFrame = (D3DXFRAME_DERIVED*)D3DXFrameFind(m_pFrameRoot, pMeshContainer->pSkinInfo->GetBoneName(iBone));
            if (pFrame == NULL)
                return E_FAIL;

			//将框架的组合变换矩阵赋值给对应的骨骼的复合变换矩阵
            pMeshContainer->ppBoneMatrixPtrs[iBone] = &pFrame->CombinedTransformationMatrix;
		}
    }

    return S_OK;
}


//-----------------------------------------------------------------------------
// Desc: 绘制框架
//       用函数CSkinMesh::__DrawFrame()绘制框架
//-----------------------------------------------------------------------------
void CSkinMesh::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	__DrawFrame(m_pFrameRoot, pEntry, uFlag);  //调用子函数
}

//-----------------------------------------------------------------------------
// Desc: 更新框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::__UpdateFrameMatrices()更新框架
//-----------------------------------------------------------------------------
void CSkinMesh::Update()
{
	CSceneNode::Update();

	FLOAT fElapsedAppTime = TIME;

	if( 0.0f == fElapsedAppTime ) 
        return;
	
	m_fTimeCurrent += fElapsedAppTime;

	if (m_bPlayAnim && m_pAnimController != NULL)
	{
		if(m_dwControlPlayTime == 0 || m_dwPlayTime < m_dwControlPlayTime)
			m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

		// 计算动画播放次数
		m_fFrameTime += fElapsedAppTime;
		if(m_fFrameTime >= m_lfTotalFrameTime)
		{
			m_fFrameTime = 0.0f;
			m_dwPlayTime++;
		}
	}

	__UpdateFrameMatrices(m_pFrameRoot, &m_WorldMatrix);  //调用子函数
}


//-----------------------------------------------------------------------------
// Desc:计算各个骨骼的组合变换矩阵
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
// Desc: 绘制框架.
//       先用CSkinMesh::__DrawMeshContainer()绘制一个LPD3DXMESHCONTAINER类型
//       的变量pMeshContainer.然后递归绘制同一级框架和子一级框架。
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
	HRESULT hr;
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
	//检查是否是蒙皮网格模型
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
					D3DXMatrixMultiply(&m_pAlloc->m_pBoneMatrices[iPaletteEntry], &matTemp, &m_matView);
				}
			}
			m_Effect.GetEffect()->SetMatrixArray( "mWorldMatrixArray", m_pAlloc->m_pBoneMatrices, pMeshContainer->NumPaletteEntries);

			D3DXCOLOR color1(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Ambient);
			D3DXCOLOR color2(0.25, 0.25, 0.25, 1.0);
			D3DXCOLOR ambEmm;
			D3DXColorModulate(&ambEmm, &color1, &color2);
			ambEmm += D3DXCOLOR(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Emissive);

			//设置材质属性
			m_Effect.GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[pBoneComb[iAttrib].AttribId].MatD3D.Diffuse));
			m_Effect.GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);

			//设置纹理
			m_Effect.GetEffect()->SetTexture( "ColorMap", pMeshContainer->ppTextures[pBoneComb[iAttrib].AttribId] );
			//设置当前骨骼数量
			m_Effect.GetEffect()->SetInt( "CurNumBones", pMeshContainer->NumInfl -1);

			//依照更新标志进行更新
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

			//设置材质属性
			m_Effect.GetEffect()->SetVector("MaterialDiffuse", (D3DXVECTOR4*)&(pMeshContainer->pMaterials[iMaterial].MatD3D.Diffuse));
			m_Effect.GetEffect()->SetVector("MaterialAmbient", (D3DXVECTOR4*)&ambEmm);
			//设置纹理
			m_Effect.GetEffect()->SetTexture( "ColorMap", pMeshContainer->ppTextures[iMaterial] );
			
			//依照更新标志进行更新
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
	// 如果没有动画控制器，则默认播放了一次
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

bool CSkinMesh::ApplyForRender()
{
	UINT uTotalPass = m_Effect.GetTechniqueDesc().Passes, i;

	for (i = 0; i < uTotalPass; i ++)
	{
		//锁定整个队列
		zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

		//将信息需求传送到优化队列
		pRenderEntry->hEffect      = m_Effect.GetHandle();
		pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
		pRenderEntry->hModel       = RESOURCE_MODEL;
		pRenderEntry->uRenderPass  = (zerO::UINT8)i;
		pRenderEntry->pParent      = this;

		//解锁
		GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
	}

	return true;
}

//-----------------------------------------------------------------------------
// Desc: 释放蒙皮网格模型
//-----------------------------------------------------------------------------
HRESULT CSkinMesh::Destory()
{
	delete this;
	return S_OK;
}

HRESULT CSkinMesh::Reset()
{
	HRESULT hr;
	hr = m_Effect.GetEffect()->OnResetDevice();
	if ( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return S_FALSE;
	}
	m_pAnimController->ResetTime();
	m_pAnimController->AdvanceTime( m_fTimeCurrent, NULL );

	// Initialize current track
	if( m_szASName[0] != '\0' )
	{
		DWORD dwActiveSet = GetAnimIndex( m_szASName );
		LPD3DXANIMATIONSET pAS = NULL;
		m_pAnimController->GetAnimationSet( dwActiveSet, &pAS );
		m_pAnimController->SetTrackAnimationSet( m_dwCurrentTrack, pAS );
		DEBUG_RELEASE( pAS );
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
	LPD3DXANIMATIONSET pAS = NULL;
	m_pAnimController->GetTrackAnimationSet(m_dwCurrentTrack, &pAS);
	if( pAS->GetName() )
		strcpy( m_szASName, pAS->GetName() );
	DEBUG_RELEASE(pAS);
	return hr;
}