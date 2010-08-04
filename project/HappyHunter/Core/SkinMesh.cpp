#include "StdAfx.h"
#include "basicutils.h"
#include "d3dutils.h"
#include "RenderQueue.h"
#include "SkinMesh.h"
#include "Camera.h"

using namespace zerO;
//--------------------------------------------------------------------
// Desc: 构造函数和析构函数  Construction/Destruction
//--------------------------------------------------------------------
CSkinMesh::CSkinMesh() :
m_strEffectFile(TEXT("")),
m_bIsCreated(false)
{
}


//-----------------------------------------------------------------------------
// Desc: 构造函数和析构函数 
//-----------------------------------------------------------------------------
CSkinMesh::~CSkinMesh()
{
	Destroy();
}

void CSkinMesh::__ApplyForRenderFrame(LPD3DXFRAME pFrame)
{
	LPD3DXMESHCONTAINER pMeshContainer;
	
    pMeshContainer = pFrame->pMeshContainer;
    while (pMeshContainer != NULL)
    {
        __ApplyForRenderMeshContainer(pMeshContainer, pFrame);
        pMeshContainer = pMeshContainer->pNextMeshContainer;
    }
	
    if (pFrame->pFrameSibling != NULL)
    {
        __ApplyForRenderFrame(pFrame->pFrameSibling);
    }
	
    if (pFrame->pFrameFirstChild != NULL)
    {
        __ApplyForRenderFrame(pFrame->pFrameFirstChild);
    }
}

void CSkinMesh::__ApplyForRenderMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase)
{
	MODELCONTAINER *pMeshContainer = (MODELCONTAINER*)pMeshContainerBase;
	MODELFRAME *pFrame = (MODELFRAME*)pFrameBase;

	UINT uMaterial;
	UINT uAttrib;
	UINT uPass;
	UINT uTotalPass         = m_pEffect->GetTechniqueDesc().Passes;
	UINT uNumBoneInfluences = pMeshContainer->NumInfl -1;

	CSurface* pSurface;
	LPD3DXBONECOMBINATION pBoneComb;

	//检查是否是蒙皮网格模型
	if (pMeshContainer->pSkinInfo != NULL)
	{
		if(m_pModel->GetAllocateHierarchy().GetType() == CAllocateHierarchy::SOFTWARE)
		{
			for( uAttrib = 0; uAttrib < pMeshContainer->NumAttributeGroups; uAttrib++ )
            {
				uMaterial = pMeshContainer->pAttributeTable[uAttrib].AttribId;
				pSurface  = &pMeshContainer->pSurfaces[uMaterial];

				for (uPass = 0; uPass < uTotalPass; uPass ++)
				{
					//锁定整个队列
					zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

					//将信息需求传送到优化队列
					pRenderEntry->hEffect      = m_pEffect->GetHandle();
					pRenderEntry->uBoneCount   = uNumBoneInfluences;
					pRenderEntry->uDetailLevel = 0;
					pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
					pRenderEntry->hModel       = m_pModel->GetHandle();
					pRenderEntry->uModelParamA = pMeshContainer->uIndex;
					pRenderEntry->uModelParamB = uMaterial;
					pRenderEntry->uRenderPass  = uPass;
					pRenderEntry->hSurface     = pSurface->GetHandle();
					pRenderEntry->pParent      = this;
					pRenderEntry->uUserData    = uMaterial;

					//解锁
					GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
				}
            }
		}
		else if(m_pModel->GetAllocateHierarchy().GetType() == CAllocateHierarchy::HARDWARE)
		{
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>(pMeshContainer->pBoneCombinationBuf->GetBufferPointer());
			for (uAttrib = 0; uAttrib < pMeshContainer->NumAttributeGroups; uAttrib ++)
			{ 
				uMaterial = pBoneComb[uAttrib].AttribId;
				pSurface  = &pMeshContainer->pSurfaces[uMaterial];

				for (uPass = 0; uPass < uTotalPass; uPass ++)
				{
					//锁定整个队列
					zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

					//将信息需求传送到优化队列
					pRenderEntry->hEffect      = m_pEffect->GetHandle();
					pRenderEntry->uBoneCount   = uNumBoneInfluences;
					pRenderEntry->uDetailLevel = 0;
					pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
					pRenderEntry->hModel       = m_pModel->GetHandle();
					pRenderEntry->uModelParamA = pMeshContainer->uIndex;
					pRenderEntry->uModelParamB = uAttrib;
					pRenderEntry->uRenderPass  = uPass;
					pRenderEntry->hSurface     = pSurface->GetHandle();
					pRenderEntry->pParent      = this;
					pRenderEntry->uUserData    = uMaterial;

					//解锁
					GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
				}
			}
		}
	}
	else
	{
		for (uMaterial = 0; uMaterial < pMeshContainer->NumMaterials; uMaterial++)
		{
			pSurface  = &pMeshContainer->pSurfaces[uMaterial];

			for (uPass = 0; uPass < uTotalPass; uPass ++)
			{
				//锁定整个队列
				zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();

				//将信息需求传送到优化队列
				pRenderEntry->hEffect      = m_pEffect->GetHandle();
				pRenderEntry->uBoneCount   = 0;
				pRenderEntry->uDetailLevel = 0;
				pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::MODEL_TYPE;
				pRenderEntry->hModel       = m_pModel->GetHandle();
				pRenderEntry->uModelParamA = pMeshContainer->uIndex;
				pRenderEntry->uModelParamB = uMaterial;
				pRenderEntry->uRenderPass  = uPass;
				pRenderEntry->hSurface     = pSurface->GetHandle();
				pRenderEntry->pParent      = this;
				pRenderEntry->uUserData    = uMaterial;

				//解锁
				GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
			}
		}
	}
}

void CSkinMesh::Clone(CSkinMesh& SkinMesh)const
{
	CSprite::Clone(SkinMesh);

	SkinMesh.m_pEffect = m_pEffect;
	SkinMesh.m_pModel  = m_pModel;
}

//-----------------------------------------------------------------------------
// Desc:创建并加载蒙皮网格模型
//-----------------------------------------------------------------------------
bool CSkinMesh::Create(const PBASICCHAR fileName)
{
	DEBUG_NEW(m_pEffect, CEffect);

	if ( !m_pEffect->Load( (PBASICCHAR)m_strEffectFile.c_str() ) )
		return false;

	DEBUG_NEW(m_pModel, CModel);

	if( !m_pModel->Create(fileName, this) )
		return false;

	m_bIsCreated = true;

	return true;
}

bool CSkinMesh::Destroy()
{
	if(m_bIsCreated)
	{
		DEBUG_DELETE(m_pEffect);
		DEBUG_DELETE(m_pModel);

		m_bIsCreated = false;
	}

	m_pEffect = NULL;
	m_pModel  = NULL;

	return true;
}

bool CSkinMesh::ApplyForRender()
{
	if(m_pEffect->GetEffect() && m_pModel)
		__ApplyForRenderFrame( m_pModel->GetFrameRoot() );

	return true;
}

//-----------------------------------------------------------------------------
// Desc: 绘制框架
//       用函数CSkinMesh::__DrawFrame()绘制框架
//-----------------------------------------------------------------------------
void CSkinMesh::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	LPMODELCONTAINER pMeshContainer = m_pModel->GetModelContainer(pEntry->uModelParamA);
	LPMODELFRAME pFrame = pMeshContainer->pParent;

	UINT uNumBoneInfluences = pMeshContainer->NumInfl - 1;

	D3DXMATRIXA16 matTemp;

	if(pMeshContainer->UseSoftwareVP)
		DEVICE.SetSoftwareVertexProcessing(TRUE);

	if( TEST_BIT(uFlag, zerO::CRenderQueue::SURFACE) )
		m_pEffect->SetSurface(&pMeshContainer->pSurfaces[pEntry->uUserData]);

	if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
	{
		static D3DXMATRIX s_Matrix(
			1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			0.0f, 0.0f, 0.0f, 1.0f);

		m_pModel->Update(&s_Matrix);

		const D3DLIGHT9* pLight = LIGHTMANAGER.GetLight(0);

		if(pLight)
		{
			D3DXVECTOR4 LightDirection(pLight->Direction, 1.0f);
			m_pEffect->GetEffect()->SetVector("vecLightDir", &LightDirection);
		}
	}

	if(m_pModel->GetAllocateHierarchy().GetType() == CAllocateHierarchy::SOFTWARE)
	{
		if(pMeshContainer->pSkinInfo)
		{
			if( TEST_BIT(uFlag, zerO::CRenderQueue::MODEL_PARAMA) )
			{
				DWORD cBones; 
				DWORD iBone;
				PBYTE pbVerticesSrc;
				PBYTE pbVerticesDest;

				cBones = pMeshContainer->pSkinInfo->GetNumBones();

				// set up bone transforms
				for( iBone = 0; iBone < cBones; ++iBone )
				{
					D3DXMatrixMultiply
						(
						&m_pModel->GetBoneMatrices()[iBone],                 // output
						&pMeshContainer->pBoneOffsetMatrices[iBone],
						pMeshContainer->ppBoneMatrixPtrs[iBone]
						);
				}

				pMeshContainer->pOrigMesh->LockVertexBuffer( D3DLOCK_READONLY, ( LPVOID* )&pbVerticesSrc );
				pMeshContainer->MeshData.pMesh->LockVertexBuffer( 0, ( LPVOID* )&pbVerticesDest );

				// generate skinned mesh
				//pMeshContainer->pSkinInfo->UpdateSkinnedMesh(m_pModel->GetBoneMatrices(), NULL, pbVerticesSrc, pMeshContainer->puBuffer);
				UpdateSkinnedMesh(
					m_pModel->GetBoneMatrices(),
					pMeshContainer->pSkinInfo->GetNumBones(),
					pbVerticesSrc, 
					pMeshContainer->puMeshBuffer, 
					pMeshContainer->pOrigMesh->GetNumVertices(),
					pMeshContainer->pOrigMesh->GetNumBytesPerVertex(),
					pMeshContainer->puNumBoneInfluences,
					pMeshContainer->ppdwVerticesIndices,
					pMeshContainer->ppfWeights,
					pMeshContainer->pTangentInfo,
					pMeshContainer->pTangentBuffer
					);

				zerO::PUINT8 puMeshBuffer = pMeshContainer->puMeshBuffer;

				D3DXVECTOR3* pTangentBuffer = pMeshContainer->pTangentBuffer;

				zerO::UINT uNumBytesPerVertexSrc  = pMeshContainer->pOrigMesh->GetNumBytesPerVertex(),
						   uNumBytesPerVertexDest = pMeshContainer->MeshData.pMesh->GetNumBytesPerVertex(),
						   uNumVertices           = pMeshContainer->pOrigMesh->GetNumVertices(),
						   uPosition4Size         = sizeof(D3DXVECTOR4),
						   uPosition3Size         = sizeof(D3DXVECTOR3),
						   uTangentOffset         = uNumBytesPerVertexDest - sizeof(D3DXVECTOR3);

				for(zerO::UINT i = 0; i < uNumVertices; i ++)
				{
					*(D3DXVECTOR4*)(pbVerticesDest                 ) = D3DXVECTOR4(*(D3DVECTOR*)puMeshBuffer, 1.0f);
					*(D3DXVECTOR3*)(pbVerticesDest + uPosition4Size) = *(D3DXVECTOR3*)(puMeshBuffer + uPosition3Size);
					*(D3DXVECTOR3*)(pbVerticesDest + uTangentOffset) = pTangentBuffer[i];


					puMeshBuffer   += uNumBytesPerVertexSrc;
					pbVerticesDest += uNumBytesPerVertexDest;
				}

				pMeshContainer->pOrigMesh->UnlockVertexBuffer();
				pMeshContainer->MeshData.pMesh->UnlockVertexBuffer();

				//D3DXComputeNormals(pMeshContainer->MeshData.pMesh,NULL);

				////计算切线
				//D3DXComputeTangent(pMeshContainer->MeshData.pMesh, 0, 0, 0, true, NULL);

				if(pMeshContainer->pShadow)
					pMeshContainer->pShadow->SetMeshData(*pMeshContainer->MeshData.pMesh);
			}

			if( TEST_BIT(uFlag, zerO::CRenderQueue::PARENT) )
			{
				m_pEffect->SetMatrix(CEffect::WORLD, m_WorldMatrix);
				m_pEffect->SetMatrix(CEffect::WORLD_VIEW_PROJECTION, m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
			}
		}
		else
		{
			m_pEffect->SetMatrix(CEffect::WORLD, pFrame->CombinedTransformationMatrix * m_WorldMatrix);
			m_pEffect->SetMatrix(CEffect::WORLD_VIEW_PROJECTION, pFrame->CombinedTransformationMatrix * m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
		}
	}
	else if(m_pModel->GetAllocateHierarchy().GetType() == CAllocateHierarchy::HARDWARE)
	{
		if( TEST_BIT(uFlag, zerO::CRenderQueue::PARENT) )
		{
			m_pEffect->SetMatrix(CEffect::WORLD, m_WorldMatrix);
			m_pEffect->SetMatrix(CEffect::WORLD_VIEW_PROJECTION, m_WorldMatrix * CAMERA.GetViewProjectionMatrix() );
		}

		if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT_PARAM) )
				//设置当前骨骼数量
				m_pEffect->SetParameter(CEffect::BONE_INFLUENCES_NUMBER, (void*)&uNumBoneInfluences);

		if(pMeshContainer->pSkinInfo)
		{
			UINT iMatrixIndex;
			UINT iPaletteEntry;
			LPD3DXBONECOMBINATION pBoneComb;
			
			pBoneComb = reinterpret_cast<LPD3DXBONECOMBINATION>( pMeshContainer->pBoneCombinationBuf->GetBufferPointer() );

			for (iPaletteEntry = 0; iPaletteEntry < pMeshContainer->NumPaletteEntries; ++iPaletteEntry)
			{
				iMatrixIndex = pBoneComb[pEntry->uModelParamB].BoneId[iPaletteEntry];

				if (iMatrixIndex != UINT_MAX)
				{
					D3DXMatrixMultiply(
						&m_pModel->GetBoneMatrices()[iPaletteEntry], 
						&pMeshContainer->pBoneOffsetMatrices[iMatrixIndex], 
						pMeshContainer->ppBoneMatrixPtrs[iMatrixIndex]);
				}
			}

			m_pEffect->SetMatrixArray(CEffect::WORLD_MATRIX_ARRAY, m_pModel->GetBoneMatrices(), pMeshContainer->NumPaletteEntries);
		}
		else
		{
			matTemp = pFrame->CombinedTransformationMatrix;
			m_pEffect->SetMatrixArray(CEffect::WORLD_MATRIX_ARRAY, &matTemp, 1);
		}
	}
	else
		return;

	m_pEffect->GetEffect()->CommitChanges();

	//绘制
	pMeshContainer->MeshData.pMesh->DrawSubset(pEntry->uModelParamB);

	if(pMeshContainer->UseSoftwareVP)
		DEVICE.SetSoftwareVertexProcessing(FALSE);
}


//-----------------------------------------------------------------------------
// Desc: 更新框架
//       (1)用m_pAnimController->AdvanceTime()设置时间，m_pAnimController是
//          类LPD3DXANIMATIONCONTROLLER的一个对象
//       (2)用函数CSkinMesh::__UpdateFrameMatrices()更新框架
//-----------------------------------------------------------------------------
void CSkinMesh::Update()
{
	CSprite::Update();

	if(m_bIsCreated)
		m_pModel->Update(ELAPSEDTIME);

	/*D3DXVECTOR4 eyePos(CAMERA.GetPosition(), 0.0f);
	m_pEffect->GetEffect()->SetVector( "vecEye", &eyePos);*/
}

//
//void CSkinMesh::SetAnimation( zerO::UINT index, DWORD dwControlPlayTime, bool bSmooth )
//{
//	if( index >= m_pAnimController->GetNumAnimationSets() )
//		return;
//
//	LPD3DXANIMATIONSET pAnimSet = NULL;
//	m_pAnimController->GetAnimationSet( index, &pAnimSet );
//
//	if(pAnimSet)
//	{
//		if(!strcmp(m_strNowAnimSetName.c_str(), pAnimSet->GetName()))
//			return;
//
//		m_strNowAnimSetName = pAnimSet->GetName();
//	}
//
//	m_dwPlayTime = 0;
//	m_fFrameTime = 0.0f;
//	m_dwControlPlayTime = dwControlPlayTime;
//
//	if( pAnimSet )
//	{
//		m_lfTotalFrameTime = pAnimSet->GetPeriod();
//		if(bSmooth)
//			AnimationChangeSmooth(pAnimSet);
//		else
//		{
//			DWORD dwNewTrack = 0;
//			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
//			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
//		}
//		pAnimSet->Release();
//	}
//}
//
//void CSkinMesh::SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime, bool bSmooth )
//{
//	if(m_pAnimController == NULL)
//		return;
//	
//	if(!strcmp(m_strNowAnimSetName.c_str(), pName))
//		return;
//	
//	m_strNowAnimSetName = pName;
//
//	m_dwPlayTime = 0;
//	m_fFrameTime = 0.0f;
//	m_dwControlPlayTime = dwControlPlayTime;
//
//	LPD3DXANIMATIONSET pAnimSet = NULL;
//	m_pAnimController->SetTrackPosition(0, 0.0);
//	m_pAnimController->GetAnimationSetByName( pName, &pAnimSet );
//	if( pAnimSet )
//	{
//		m_lfTotalFrameTime = pAnimSet->GetPeriod();
//		if(bSmooth)
//			AnimationChangeSmooth(pAnimSet);
//		else
//		{
//			DWORD dwNewTrack = 0;
//			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
//			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
//		}
//		pAnimSet->Release();
//	}
//}
//
//void CSkinMesh::AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet)
//{
//	DWORD dwNewTrack = ( m_dwCurrentTrack == 0 ? 1 : 0 );
//
//	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
//
//	m_pAnimController->UnkeyAllTrackEvents( m_dwCurrentTrack );
//	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );
//
//	m_pAnimController->KeyTrackEnable( m_dwCurrentTrack, FALSE, m_fTimeCurrent + ANIM_TRANSITION_TIME );
//	m_pAnimController->KeyTrackSpeed( m_dwCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
//	m_pAnimController->KeyTrackWeight( m_dwCurrentTrack, 0.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
//	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
//	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
//	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, m_fTimeCurrent, ANIM_TRANSITION_TIME, D3DXTRANSITION_LINEAR );
//
//	m_dwCurrentTrack = dwNewTrack;
//}
//
//DWORD CSkinMesh::GetPlayTime() 
//{ 
//	// 如果没有动画控制器，则默认播放了一次
//	if(m_pAnimController == NULL)
//		m_dwPlayTime = 1;
//
//	return m_dwPlayTime; 
//}
//
//DWORD CSkinMesh::GetAnimIndex( char sString[] )
//{
//	HRESULT hr;
//	LPD3DXANIMATIONSET pAS;
//	DWORD dwRet = 0xffffffff;
//
//	for( DWORD i = 0; i < m_pAnimController->GetNumAnimationSets(); ++ i )
//	{
//		hr = m_pAnimController->GetAnimationSet( i, & pAS );
//		if( FAILED( hr ) )
//			continue;
//
//		if( pAS->GetName() &&
//			!strncmp( pAS->GetName(), sString, min( strlen( pAS->GetName() ), strlen( sString ) ) ) )
//		{
//			dwRet = i;
//			pAS->Release();
//			break;
//		}
//
//		pAS->Release();
//	}
//
//	return dwRet;
//}
//
//bool CSkinMesh::CanPlay(LPSTR pName, DWORD count)
//{
//	if (strcmp(m_strNowAnimSetName.c_str(), pName) == 0 
//		|| GetPlayTime() >= count)
//		return true;
//
//	return false;
//}
