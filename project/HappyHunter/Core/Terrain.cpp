#include "StdAfx.h"
#include "Terrain.h"

using namespace zerO;

const D3DVERTEXELEMENT9 g_TERRAIN_VERTEX_DESCRIPTION[] =
{
	{ 0, 0, D3DDECLTYPE_FLOAT2, 
		D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_POSITION, 0 },
	{ 0, 8, D3DDECLTYPE_FLOAT2, 
		D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_TEXCOORD, 0 },

	{ 1, 0, D3DDECLTYPE_FLOAT1, 
		D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_POSITION, 1 },
	{ 1, 4, D3DDECLTYPE_FLOAT3, 
		D3DDECLMETHOD_DEFAULT, 
		D3DDECLUSAGE_NORMAL,   0 },
	D3DDECL_END()
};

CTerrainSection::CTerrainSection(void) :
	m_pTerrain(NULL),
		m_uHeightMapX(0),
		m_uHeightMapY(0),
		m_uSectorX(0),
		m_uSectorY(0), 
		m_VerticesX(0),
		m_VerticesY(0)
{
}

CTerrainSection::~CTerrainSection(void)
{
}

bool CTerrainSection::Create(
			CSceneNode* pRootNode,
			CTerrain* pParent, 
			zerO::UINT uSectorX, 
			zerO::UINT uSectorY, 
			zerO::UINT uHeightMapX, 
			zerO::UINT uHeightMapY,
			zerO::UINT VerticesX,
			zerO::UINT VerticesY, 
			const CRectangle2D& WorldRect)
{
	if(m_pTerrain)
	{
		DEBUG_WARNING("Section had be Created.");
		return false;
	}

	if(pRootNode)
		pRootNode->AddChild(this);

	m_pTerrain    = pParent;
	m_VerticesX    = VerticesX;
	m_VerticesY    = VerticesY;
	m_uSectorX    = uSectorX;
	m_uSectorY    = uSectorY;
	m_uHeightMapX = uHeightMapX;
	m_uHeightMapY = uHeightMapY;

	m_WorldRect.GetMaxX() = WorldRect.GetMaxX();
	m_WorldRect.GetMinX() = WorldRect.GetMinX();
	m_WorldRect.GetMaxY() = WorldRect.GetMaxY();
	m_WorldRect.GetMinY() = WorldRect.GetMinY();

	bool bResult  = __BuildVertexBuffer();

	m_LocalRect   = m_WorldRect;

	return bResult;
}

bool CTerrainSection::__BuildVertexBuffer()
{
	LPVERTEX pVertices;
	DEBUG_NEW(pVertices, VERTEX[m_VerticesX * m_VerticesY]);

	FLOAT fHeight = m_pTerrain->GetHeight(m_uHeightMapX, m_uHeightMapY);

	m_WorldRect.GetMinZ() = fHeight;
	m_WorldRect.GetMaxZ() = fHeight;

	UINT x, y;

	for (y = 0; y < m_VerticesY; ++y)
	{
		for (x = 0; x < m_VerticesX; ++x)
		{
			fHeight =  m_pTerrain->GetHeight(m_uHeightMapX + x, m_uHeightMapY + y);

			pVertices[y * m_VerticesX + x].fHeight = fHeight;
			pVertices[y * m_VerticesX + x].Normal  = m_pTerrain->GetNormal(m_uHeightMapX + x, m_uHeightMapY + y);

			m_WorldRect.GetMinZ() = MIN(m_WorldRect.GetMinZ(), fHeight);
			m_WorldRect.GetMaxZ() = MAX(m_WorldRect.GetMaxZ(), fHeight);
		}
	}

	HRESULT hr = m_VertexBuffer.Create(
		m_VerticesX * m_VerticesY, 
		sizeof(VERTEX), 
		D3DUSAGE_WRITEONLY,
		D3DPOOL_MANAGED,
		pVertices);

	DEBUG_DELETE_ARRAY(pVertices);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
}

bool CTerrainSection::ApplyForRender()
{
	return m_pTerrain->SubmitSection(this);
}

void CTerrainSection::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	m_pTerrain->RenderSection(this, uFlag, pEntry);
}

CTerrain::CTerrain(void) :
m_pSector(NULL),
m_pRootNode(NULL),

m_uSectorShift(0),
m_uSectorUnits(0),
m_uSectorVertices(0),

m_uSectorCountX(0),
m_uSectorCountY(0),

m_uTableWidth(0),
m_uTableHeight(0),
m_pfHeightTable(NULL),
m_pNormalTable(NULL)
{
}

CTerrain::~CTerrain(void)
{
	Destroy();
}

void CTerrain::Render()
{
	if(m_pSector)
	{
		UINT uCount = m_uSectorCountX * m_uSectorCountY, i;

		for(i = 0; i < uCount; i ++)
			m_pSector[i].ApplyForRender();
	}
}

void CTerrain::SetQuadTree(CQuadTree* pQuadTree)
{
	if(m_pSector)
	{
		UINT uTotalSectors = m_uSectorCountX * m_uSectorCountY;

		for(UINT i = 0; i < uTotalSectors; i ++)
			m_pSector[i].AttachToQuadTree(pQuadTree);
	}
}

bool CTerrain:: Create(
					   CSceneNode* pRootNode, 
					   CTexture* pHeightMap, 
					   const CRectangle3D& WorldExtents, 
					   zerO::UINT8 uShift)
{
	m_uSectorShift    = uShift;
	m_uSectorUnits = 1 << uShift;
	m_uSectorVertices = m_uSectorUnits + 1; 

	m_pRootNode      = pRootNode;
	m_WorldExtents   = WorldExtents;

	WorldExtents.GetSize(m_WorldSize);
	
	m_uTableWidth    = pHeightMap->GetWidth();
	m_uTableHeight   = pHeightMap->GetHeight();

	m_MapScale.x     = m_WorldSize.x / m_uTableWidth;
	m_MapScale.y     = m_WorldSize.y / m_uTableHeight;
	m_MapScale.z     = m_WorldSize.z / 0xff;

	_BuildHeightAndNormalTables( pHeightMap->GetTexture() );

	m_uSectorCountX = m_uTableWidth  >> uShift;
	m_uSectorCountY = m_uTableHeight >> uShift;

	m_SectorSize.x = m_WorldSize.x / m_uSectorCountX;
	m_SectorSize.y = m_WorldSize.y / m_uSectorCountY;

	if( _BuildVertexBuffer() && _SetVertexDescription() && _BuildIndexBuffer() && _AllocateSectors() )
		return true;
	
	Destroy();

	return false;
}

void CTerrain::_BuildHeightAndNormalTables(LPDIRECT3DTEXTURE9 pTexture)
{
	DEBUG_DELETE_ARRAY(m_pfHeightTable);
	DEBUG_DELETE_ARRAY(m_pNormalTable);

	UINT x, y, uTableIndex, uMapIndex, uPitch;

	DEBUG_NEW(m_pfHeightTable, FLOAT[m_uTableWidth * m_uTableHeight]);
	DEBUG_NEW(m_pNormalTable, D3DXVECTOR3[m_uTableWidth * m_uTableHeight]);

	HRESULT hr;

	D3DLOCKED_RECT LockedRect;

	hr = pTexture->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	PUINT8 pMap;
	
	pMap = (PUINT8)LockedRect.pBits;

	uPitch = (LockedRect.Pitch >> 2) - m_uTableWidth;

	uTableIndex = 0;
	uMapIndex   = 0;

	for(y = 0; y < m_uTableHeight; y ++)
	{
		for(x = 0; x < m_uTableWidth; x ++)
			m_pfHeightTable[uTableIndex ++] = pMap[(uMapIndex ++) << 2] * m_MapScale.z + m_WorldExtents.GetMinZ();

		uMapIndex += uPitch;
	}

	hr = pTexture->UnlockRect(0);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	LPDIRECT3DTEXTURE9 pTemp;
	D3DXVECTOR3 Normal;

	hr = D3DXCreateTexture(
			&DEVICE,
			m_uTableWidth,
			m_uTableHeight,
			1,
			0,
			D3DFMT_A8R8G8B8,
			D3DPOOL_SCRATCH,
			&pTemp);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	hr = D3DXComputeNormalMap(pTemp, pTexture, NULL, 0, D3DX_CHANNEL_RED, m_uTableWidth * m_WorldSize.z / m_WorldSize.x);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	hr = pTemp->LockRect(0, &LockedRect, NULL, D3DLOCK_READONLY);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	pMap = (PUINT8)LockedRect.pBits;

	uPitch = LockedRect.Pitch - (m_uTableWidth << 2);

	uTableIndex = 0;
	uMapIndex   = 0;

	for(y = 0; y < m_uTableHeight; y ++)
	{
		for(x = 0; x < m_uTableWidth; x ++)
		{
			Normal.z = pMap[uMapIndex ++] - 127.5f;
			Normal.y = pMap[uMapIndex ++] - 127.5f;
			Normal.x = pMap[uMapIndex ++] - 127.5f;

			D3DXVec3Normalize(&m_pNormalTable[uTableIndex ++], &Normal);

			uMapIndex ++;
		}

		uMapIndex += uPitch;
	}

	hr = pTemp->UnlockRect(0);

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	pTemp->Release();
}


bool CTerrain::_SetVertexDescription()
{
	return m_VertexBuffer.SetVertexDescription(
	sizeof(g_TERRAIN_VERTEX_DESCRIPTION) / sizeof(D3DVERTEXELEMENT9), g_TERRAIN_VERTEX_DESCRIPTION);
}

bool CTerrain::_BuildVertexBuffer()
{
	D3DXVECTOR2 CellSize(m_SectorSize.x / m_uSectorUnits, m_SectorSize.y / m_uSectorUnits);

	D3DXVECTOR2 Vertex(0.0f,0.0f);
	LPVERTEX pVertices;
	
	DEBUG_NEW(pVertices, VERTEX[m_uSectorVertices * m_uSectorVertices]);

	UINT x, y, uIndex = 0;
	for(y = 0; y < m_uSectorVertices; y ++)
	{
		Vertex.x = 0;

		for(x = 0; x < m_uSectorVertices; x ++)
		{
			pVertices[uIndex].Position = Vertex;
			pVertices[uIndex].UV.x = (float)x / (m_uSectorVertices - 1);
			pVertices[uIndex].UV.y = (float)y / (m_uSectorVertices - 1);

			Vertex.x += CellSize.x;

			uIndex ++;
		}

		Vertex.y += CellSize.y;
	}

	/*VERTEX bitch[289];

	memcpy( bitch, pVertices, sizeof(bitch) );*/

	HRESULT hr = m_VertexBuffer.Create(
		m_uSectorVertices * m_uSectorVertices, 
		sizeof(VERTEX), 
		D3DUSAGE_WRITEONLY, 
		D3DPOOL_MANAGED, 
		pVertices);

	DEBUG_DELETE_ARRAY(pVertices);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	return true;
}

bool CTerrain::_BuildIndexBuffer()
{
	return m_IndexBuffer.CreateSingleStripGrid(
		m_uSectorVertices,
		m_uSectorVertices,
		1,
		1,
		m_uSectorVertices,
		D3DUSAGE_WRITEONLY,
		D3DPOOL_MANAGED);
}

bool CTerrain::_AllocateSectors()
{
	DEBUG_NEW(m_pSector, CTerrainSection[m_uSectorCountX * m_uSectorCountY]);

	D3DXVECTOR2 SectorPosition;
	CRectangle2D SectorRectangle;

	UINT x, y, uPixelX, uPixelY, uIndex = 0;

	for(y=0; y < m_uSectorCountY; y ++)
	{
		for(x = 0; x < m_uSectorCountX; x ++)
		{
			SectorPosition.x = m_WorldExtents.GetMinX() + x * m_SectorSize.x;
			SectorPosition.y = m_WorldExtents.GetMinY() + y * m_SectorSize.y;

			SectorRectangle.Set(
				SectorPosition.x, 
				SectorPosition.x + m_SectorSize.x,
				SectorPosition.y, 
				SectorPosition.y + m_SectorSize.y);

			uPixelX = x << m_uSectorShift;
			uPixelY = y << m_uSectorShift;

			if(!m_pSector[uIndex].Create(
				m_pRootNode, 
				this, 
				x, 
				y, 
				uPixelX, 
				uPixelY, 
				m_uSectorVertices, 
				m_uSectorVertices, 
				SectorRectangle) )
				return false;

			uIndex ++;
		}
	}

	return true;
}

bool CTerrain::Destroy()
{
	DEBUG_DELETE_ARRAY(m_pSector);
	DEBUG_DELETE_ARRAY(m_pfHeightTable);
	DEBUG_DELETE_ARRAY(m_pNormalTable);

	m_pSector      = NULL;
	m_pfHeightTable = NULL;
	m_pNormalTable = NULL;

	return true;
}

bool CTerrain::SubmitSection(CTerrainSection* pSection)const
{
	CEffect* pEffect = m_RenderMethod.GetEffect();

	if (pEffect)
	{
		UINT uTotalPass = pEffect->GetTechniqueDesc().Passes, i;

		for (i = 0; i < uTotalPass; i ++)
		{
			CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();
			
			pRenderEntry->hEffect      = pEffect->GetHandle();
			pRenderEntry->hSurface     = m_RenderMethod.GetSurface()->GetHandle();
			pRenderEntry->uModelType   = CRenderQueue::RENDERENTRY::BUFFER_TYPE;
			pRenderEntry->hModel       = m_VertexBuffer.GetHandle();
			pRenderEntry->uModelParamA = pSection->GetVertexBuffer().GetHandle();
			pRenderEntry->uModelParamB = m_IndexBuffer.GetHandle();
			pRenderEntry->uRenderPass  = i;
			pRenderEntry->pParent      = pSection;
			pRenderEntry->uUserData    = 0;

			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}

		return true;
	}

	return false;
}

void CTerrain::RenderSection(CTerrainSection* pSection, zerO::UINT32 uFlag, const CRenderQueue::LPRENDERENTRY pEntry)const
{
	CEffect* pEffect = m_RenderMethod.GetEffect();
	
	if (pEffect)
	{	
		if( TEST_BIT(uFlag, CRenderQueue::EFFECT) )
			pEffect->Begin();

		pEffect->GetEffect()->BeginPass(pEntry->uRenderPass);

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
			m_VertexBuffer.Activate(0, 0, true);

		if ( TEST_BIT(uFlag, CRenderQueue::MODEL_PARAMA) )
			pSection->GetVertexBuffer().Activate(1, 0, false);

		if ( TEST_BIT(uFlag, CRenderQueue::MODEL_PARAMB) )
			m_IndexBuffer.Activate();

		if ( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			pEffect->SetSurface( m_RenderMethod.GetSurface() );

		D3DXVECTOR4 SectorOffset(
			1.0f,
			1.0f,
			m_WorldExtents.GetMinX() + ( m_SectorSize.x * pSection->GetSectorX() ),
			m_WorldExtents.GetMinY() + ( m_SectorSize.y * pSection->GetSectorY() ) );



		D3DXVECTOR4 UVScaleOffset(
			1.0f / (m_uSectorCountX + 1),
			1.0f / (m_uSectorCountY + 1),
			(FLOAT)pSection->GetSectorX(),
			(FLOAT)pSection->GetSectorY() );

		pEffect->SetParameter(
			CEffect::POSITION, 
			(D3DXVECTOR4*)&SectorOffset);

		pEffect->SetParameter(
			CEffect::UV, 
			(D3DXVECTOR4*)&UVScaleOffset);

		pEffect->GetEffect()->CommitChanges();

		HRESULT hr = DEVICE.DrawIndexedPrimitive(
			m_IndexBuffer.GetType(),
			0,
			0,
			m_uSectorVertices * m_uSectorVertices,
			0,
			m_IndexBuffer.GetPrimitiveCount() );

		pEffect->GetEffect()->EndPass();
	}
}

CRoamTerrainSection::CRoamTerrainSection() :
m_fDistance0(0.0f),
m_fDistance1(0.0f),
m_fDistance2(0.0f),
m_fDistance3(0.0f),
m_fQueueSortValue(0.0f),
m_uMaxIndices(0),
m_uTotalIndices(0),
m_puIndexList(NULL)
{
}

CRoamTerrainSection::~CRoamTerrainSection()
{
}

bool CRoamTerrainSection::Create(
			CSceneNode* pRootNode,
			CTerrain* pParent, 
			zerO::UINT uSectorX, 
			zerO::UINT uSectorY, 
			zerO::UINT uHeightMapX, 
			zerO::UINT uHeightMapY,
			zerO::UINT VerticesX, 
			zerO::UINT VerticesY, 
			const CRectangle2D& WorldRect)
{
	bool bResult= CTerrainSection::Create(pRootNode, pParent, uSectorX, uSectorY, uHeightMapX, uHeightMapY,VerticesX, VerticesY, WorldRect);

	m_uMaxIndices = VerticesX * VerticesY * 2 * 3;

	m_IndexBuffer.Create(D3DPT_TRIANGLELIST, m_uMaxIndices, D3DUSAGE_DYNAMIC, D3DPOOL_DEFAULT, NULL);

	CRoamTerrain* pTerrain = (CRoamTerrain*)m_pTerrain;

	m_pLeftNeighborOfA  = NULL;
	m_pRightNeighborOfA = NULL;
	m_pLeftNeighborOfB  = NULL;
	m_pRightNeighborOfB = NULL;

	CRoamTerrainSection* pNorthSection = pTerrain->GetSection(uSectorX    , uSectorY - 1);
	CRoamTerrainSection* pSouthSection = pTerrain->GetSection(uSectorX    , uSectorY + 1);
	CRoamTerrainSection* pEastSection  = pTerrain->GetSection(uSectorX + 1, uSectorY    );
	CRoamTerrainSection* pWestSection  = pTerrain->GetSection(uSectorX - 1, uSectorY    );

	if (pNorthSection)
		m_pLeftNeighborOfA  = &pNorthSection->m_RootTriangleB;
	if (pSouthSection)
		m_pLeftNeighborOfB  = &pSouthSection->m_RootTriangleA;
	if (pEastSection)
		m_pRightNeighborOfB = &pEastSection->m_RootTriangleA;
	if (pWestSection)
		m_pRightNeighborOfA = &pWestSection->m_RootTriangleB;

	Reset();

	__ComputeVariance();

	return bResult;
}

void CRoamTerrainSection::Reset()
{
	m_RootTriangleA.pLeftChild     = NULL;
	m_RootTriangleA.pRightChild    = NULL;
	m_RootTriangleB.pLeftChild     = NULL;
	m_RootTriangleB.pRightChild    = NULL;

	m_RootTriangleA.pBase          = &m_RootTriangleB;
	m_RootTriangleB.pBase          = &m_RootTriangleA;

	m_RootTriangleA.pLeftNeighbor  = m_pLeftNeighborOfA;
	m_RootTriangleA.pRightNeighbor = m_pRightNeighborOfA;
	m_RootTriangleB.pLeftNeighbor  = m_pLeftNeighborOfB;
	m_RootTriangleB.pRightNeighbor = m_pRightNeighborOfB;
}

void CRoamTerrainSection::Tessellate(zerO::FLOAT fScale, zerO::FLOAT fLimit)
{
	__RecursiveTessellate(&m_RootTriangleA, m_fDistance1, m_fDistance2, m_fDistance0, m_fVarianceTreeA, 1, fScale, fLimit);
	__RecursiveTessellate(&m_RootTriangleB, m_fDistance3, m_fDistance0, m_fDistance2, m_fVarianceTreeB, 1, fScale, fLimit);
}

void CRoamTerrainSection::BuildTriangleList()
{
	if( !m_IndexBuffer.Lock(0, (void**)&m_puIndexList) )
		return;

	m_uTotalIndices = 0;

	__RecursiveBuildTriangleList(&m_RootTriangleA, 0        , 16   , 16*17);

	__RecursiveBuildTriangleList(&m_RootTriangleB, (17*17)-1, 16*17, 16   );

	if( !m_IndexBuffer.Unlock() )
		return;

	m_puIndexList = NULL;
}

void CRoamTerrainSection::PrepareForRender()
{
	D3DXVECTOR2 Corner0( m_WorldRect.GetMinX(), m_WorldRect.GetMinY() );
	D3DXVECTOR2 Corner1( m_WorldRect.GetMinX(), m_WorldRect.GetMaxY() );
	D3DXVECTOR2 Corner2( m_WorldRect.GetMaxX(), m_WorldRect.GetMaxY() );
	D3DXVECTOR2 Corner3( m_WorldRect.GetMaxX(), m_WorldRect.GetMinY() );

	const D3DXVECTOR3 CameraPosition = CAMERA.GetPosition();

	D3DXVECTOR2 ViewPoint(CameraPosition.x, CameraPosition.y);

	Corner0 -= ViewPoint;
	Corner1 -= ViewPoint;
	Corner2 -= ViewPoint;
	Corner3 -= ViewPoint;

	m_fDistance0 = D3DXVec2Length(&Corner0);
	m_fDistance1 = D3DXVec2Length(&Corner1);
	m_fDistance2 = D3DXVec2Length(&Corner2);
	m_fDistance3 = D3DXVec2Length(&Corner3);

	m_fQueueSortValue = MIN(m_fDistance0     , m_fDistance1);
	m_fQueueSortValue = MIN(m_fQueueSortValue, m_fDistance2);
	m_fQueueSortValue = MIN(m_fQueueSortValue, m_fDistance3);

	( (CRoamTerrain*)m_pTerrain )->AddToTessellationQueue(this);
}

void CRoamTerrainSection::__ComputeVariance()
{
	CRoamTerrain* pTerrain = (CRoamTerrain*)m_pTerrain;

	UINT uIndex0 = pTerrain->GetTableIndex(m_uHeightMapX                  , m_uHeightMapY                  );
	UINT uIndex1 = pTerrain->GetTableIndex(m_uHeightMapX                  , m_uHeightMapY + m_VerticesY - 1);
	UINT uIndex2 = pTerrain->GetTableIndex(m_uHeightMapX + m_VerticesX - 1, m_uHeightMapY + m_VerticesY - 1);
	UINT uIndex3 = pTerrain->GetTableIndex(m_uHeightMapX + m_VerticesX - 1, m_uHeightMapY                  );

	FLOAT fHeight0 = pTerrain->GetHeight(uIndex0);
	FLOAT fHeight1 = pTerrain->GetHeight(uIndex1);
	FLOAT fHeight2 = pTerrain->GetHeight(uIndex2);
	FLOAT fHeight3 = pTerrain->GetHeight(uIndex3);

	__RecursiveComputeVariance(uIndex1, uIndex2, uIndex0, fHeight1, fHeight2, fHeight0, m_fVarianceTreeA, 1);
	__RecursiveComputeVariance(uIndex3, uIndex0, uIndex2, fHeight3, fHeight0, fHeight2, m_fVarianceTreeB, 1);
}

void CRoamTerrainSection::__Split(LPTRIANGLETREENODE pTriangle)
{
	if(pTriangle->pLeftChild)
		return;

	if( pTriangle->pBase && (pTriangle->pBase->pBase != pTriangle) )
		__Split(pTriangle->pBase);

	CRoamTerrain* pTerrain = (CRoamTerrain*)m_pTerrain;
	pTriangle->pLeftChild  = pTerrain->RequestTriangleNode();
	pTriangle->pRightChild = pTerrain->RequestTriangleNode();

	DEBUG_ASSERT(pTriangle->pLeftChild  != pTriangle, "recursive link");
	DEBUG_ASSERT(pTriangle->pRightChild != pTriangle, "recursive link");

	if (!pTriangle->pLeftChild || !pTriangle->pRightChild)
	{
		pTriangle->pLeftChild  = 0;
		pTriangle->pRightChild = 0;
		return;
	}

	pTriangle->pLeftChild->pBase           = pTriangle->pLeftNeighbor;
	pTriangle->pLeftChild->pLeftNeighbor   = pTriangle->pRightChild;

	pTriangle->pRightChild->pBase          = pTriangle->pRightNeighbor;
	pTriangle->pRightChild->pRightNeighbor = pTriangle->pLeftChild;

	if (pTriangle->pLeftNeighbor != NULL)
	{
		if (pTriangle->pLeftNeighbor->pBase == pTriangle)
			pTriangle->pLeftNeighbor->pBase = pTriangle->pLeftChild;
		else if (pTriangle->pLeftNeighbor->pLeftNeighbor == pTriangle)
			pTriangle->pLeftNeighbor->pLeftNeighbor = pTriangle->pLeftChild;
		else if (pTriangle->pLeftNeighbor->pRightNeighbor == pTriangle)
			pTriangle->pLeftNeighbor->pRightNeighbor = pTriangle->pLeftChild;
		else
			DEBUG_ERROR("Invalid Left Neighbor!");
	}

	if (pTriangle->pRightNeighbor != NULL)
	{
		if (pTriangle->pRightNeighbor->pBase == pTriangle)
			pTriangle->pRightNeighbor->pBase = pTriangle->pRightChild;
		else if (pTriangle->pRightNeighbor->pRightNeighbor == pTriangle)
			pTriangle->pRightNeighbor->pRightNeighbor = pTriangle->pRightChild;
		else if (pTriangle->pRightNeighbor->pLeftNeighbor == pTriangle)
			pTriangle->pRightNeighbor->pLeftNeighbor = pTriangle->pRightChild;
		else
			DEBUG_ERROR("Invalid Right Neighbor!");
	}

	if (pTriangle->pBase != NULL)
	{
		if ( pTriangle->pBase->pLeftChild )
		{
			pTriangle->pBase->pLeftChild->pRightNeighbor = pTriangle->pRightChild;
			pTriangle->pBase->pRightChild->pLeftNeighbor = pTriangle->pLeftChild;
			pTriangle->pLeftChild->pRightNeighbor        = pTriangle->pBase->pRightChild;
			pTriangle->pRightChild->pLeftNeighbor        = pTriangle->pBase->pLeftChild;
		}
		else
			__Split(pTriangle->pBase);
	}
	else
	{
		pTriangle->pLeftChild->pRightNeighbor = NULL;
		pTriangle->pRightChild->pLeftNeighbor = NULL;
	}
}

void CRoamTerrainSection::__RecursiveTessellate( 
						   LPTRIANGLETREENODE pTriangle, 
						   zerO::FLOAT fDistanceA, 
						   zerO::FLOAT fDistanceB, 
						   zerO::FLOAT fDistanceC, 
						   zerO::PFLOAT pfTree,
						   zerO::UINT16 uIndex,
						   zerO::FLOAT fScale, 
						   zerO::FLOAT fLimit)
{
	if( (uIndex << 1) + 1 < TOTAL_VARIANCES )
	{
		FLOAT fMidDist = (fDistanceB + fDistanceC) * 0.5f;
		
		if(!pTriangle->pLeftChild)
		{
			FLOAT fRatio = (pfTree[uIndex] * fScale) / (fMidDist + 0.0001f);

			if (fRatio > fLimit)
				__Split(pTriangle);
		}

		if(pTriangle->pLeftChild)
		{
			DEBUG_ASSERT(pTriangle->pLeftChild , "invalid triangle node");
			DEBUG_ASSERT(pTriangle->pRightChild, "invalid triangle node");

			__RecursiveTessellate(pTriangle->pLeftChild , fMidDist, fDistanceA, fDistanceB, pfTree,  uIndex << 1     , fScale, fLimit);
			__RecursiveTessellate(pTriangle->pRightChild, fMidDist, fDistanceA, fDistanceB, pfTree, (uIndex << 1) + 1, fScale, fLimit);
		}
	}
}

void CRoamTerrainSection::__RecursiveBuildTriangleList( 
								  LPTRIANGLETREENODE pTriangle,  
								  zerO::UINT16 uCornerA, 
								  zerO::UINT16 uCornerB, 
								  zerO::UINT16 uCornerC)
{
	if(pTriangle->pLeftChild)
	{
		DEBUG_ASSERT(pTriangle->pRightChild, "invalid triangle node");
		
		UINT16 uMidPoint = (uCornerB + uCornerC) >> 1;

		__RecursiveBuildTriangleList(pTriangle->pLeftChild,  uMidPoint, uCornerA, uCornerB);
		__RecursiveBuildTriangleList(pTriangle->pRightChild, uMidPoint, uCornerC, uCornerA);
	}
	else if(m_uTotalIndices + 3 < m_uMaxIndices)
	{
		m_puIndexList[m_uTotalIndices ++] = uCornerC;
		m_puIndexList[m_uTotalIndices ++] = uCornerB;
		m_puIndexList[m_uTotalIndices ++] = uCornerA;
	}
}

zerO::FLOAT CRoamTerrainSection::__RecursiveComputeVariance(	
								 zerO::UINT16 uCornerA, 
								 zerO::UINT16 uCornerB,
								 zerO::UINT16 uCornerC,
								 zerO::FLOAT fHeightA, 
								 zerO::FLOAT fHeightB, 
								 zerO::FLOAT fHeightC,
								 zerO::PFLOAT pfTree,
								 zerO::UINT16 uIndex)
{
	if (uIndex < TOTAL_DETAIL_LEVELS)
	{
		UINT16 uMidPoint           = (uCornerB + uCornerC) >> 1;
		FLOAT  fMidHeight          = m_pTerrain->GetHeight(uMidPoint);
		FLOAT  fInterpolatedHeight = (fHeightB + fHeightC) * 0.5f;
		FLOAT  fVariance           = abs(fMidHeight - fInterpolatedHeight);

		FLOAT fLeft  = __RecursiveComputeVariance( uMidPoint, uCornerA, uCornerB, fMidHeight, fHeightA, fHeightB, pfTree,      uIndex << 1  );

		FLOAT fRight = __RecursiveComputeVariance( uMidPoint, uCornerC, uCornerA, fMidHeight, fHeightC, fHeightA, pfTree, 1 + (uIndex << 1) );

		fVariance = MAX(fVariance, fLeft ); 
		fVariance = MAX(fVariance, fRight); 

		pfTree[uIndex] = fVariance;

		return fVariance;
	}

	return 0.0f;
}

CRoamTerrain::CRoamTerrain() :
m_pRoamSection(NULL),
m_pTriangleNodePool(NULL),
m_uNextTriangleNode(0),
m_ppTessellationQueue(NULL),
m_uTessellationQueueCount(0)
{
}

CRoamTerrain::~CRoamTerrain()
{
}

void CRoamTerrain::SetQuadTree(CQuadTree* pQuadTree)
{
	if(m_pRoamSection)
	{
		UINT uTotalSecctors = m_uSectorCountX * m_uSectorCountY;

		for(UINT i = 0; i < uTotalSecctors; i ++)
			m_pRoamSection[i].AttachToQuadTree(pQuadTree);
	}
}

bool CRoamTerrain::Create(CSceneNode* pRootNode, CTexture* pHeightMap, const CRectangle3D& WorldExtents, zerO::UINT8 uShift)
{
	uShift = 4;

	bool bResult = CTerrain::Create(pRootNode, pHeightMap, WorldExtents, uShift);

	if (bResult)
	{
		DEBUG_NEW(m_pTriangleNodePool, TRIANGLETREENODE[MAXINUM_TRIANGLE_TREE_NODES]);
		DEBUG_NEW(m_ppTessellationQueue, CRoamTerrainSection*[TESSELLATION_QUEUE_SIZE]);

		memset(m_pTriangleNodePool, 0, sizeof(TRIANGLETREENODE) * MAXINUM_TRIANGLE_TREE_NODES);
		memset(m_ppTessellationQueue, 0, sizeof(CRoamTerrainSection*) * TESSELLATION_QUEUE_SIZE);
	}

	return bResult;
}

bool CRoamTerrain::SubmitSection(CTerrainSection* pSection)const
{
	CEffect* pEffect = m_RenderMethod.GetEffect();

	if (pEffect)
	{
		UINT uTotalPass = pEffect->GetTechniqueDesc().Passes, i;

		CRoamTerrainSection* pRoamSection=(CRoamTerrainSection*)pSection;

		for (i = 0; i < uTotalPass; i ++)
		{
			CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();
			
			pRenderEntry->hEffect      = pEffect->GetHandle();
			pRenderEntry->hSurface     = m_RenderMethod.GetSurface()->GetHandle();
			pRenderEntry->uModelType   = CRenderQueue::RENDERENTRY::BUFFER_TYPE;
			pRenderEntry->hModel       = m_VertexBuffer.GetHandle();
			pRenderEntry->uModelParamA = pRoamSection->GetVertexBuffer().GetHandle();
			pRenderEntry->uModelParamB = pRoamSection->GetIndexBuffer().GetHandle();
			pRenderEntry->uRenderPass  = i;
			pRenderEntry->pParent      = pSection;
			pRenderEntry->uUserData    = 0;

			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}

		return true;
	}

	return false;
}

void CRoamTerrain::RenderSection(CTerrainSection* pSection, zerO::UINT32 uFlag, const CRenderQueue::LPRENDERENTRY pEntry)const
{
	CEffect* pEffect = m_RenderMethod.GetEffect();
	
	if (pEffect)
	{	
		CRoamTerrainSection* pRoamSection=(CRoamTerrainSection*)pSection;

		if( TEST_BIT(uFlag, CRenderQueue::EFFECT) )
			pEffect->Begin();

		pEffect->GetEffect()->BeginPass(pEntry->uRenderPass);

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
			m_VertexBuffer.Activate(0, 0, true);

		if ( TEST_BIT(uFlag, CRenderQueue::MODEL_PARAMA) )
			pRoamSection->GetVertexBuffer().Activate(1, 0, false);

		if ( TEST_BIT(uFlag, CRenderQueue::MODEL_PARAMB) )
			pRoamSection->GetIndexBuffer().Activate();

		if ( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			pEffect->SetSurface( m_RenderMethod.GetSurface() );

		D3DXVECTOR4 SectorOffset(
			1.0f,
			1.0f,
			m_WorldExtents.GetMinX() + ( m_SectorSize.x * pSection->GetSectorX() ),
			m_WorldExtents.GetMinY() + ( m_SectorSize.y * pSection->GetSectorY() ) );



		D3DXVECTOR4 UVScaleOffset(
			1.0f / (m_uSectorCountX + 1),
			1.0f / (m_uSectorCountY + 1),
			(FLOAT)pSection->GetSectorX(),
			(FLOAT)pSection->GetSectorY() );

		pEffect->SetParameter(
			CEffect::POSITION, 
			(D3DXVECTOR4*)&SectorOffset);

		pEffect->SetParameter(
			CEffect::UV, 
			(D3DXVECTOR4*)&UVScaleOffset);

		pEffect->GetEffect()->CommitChanges();

		UINT uTotalPolys = pRoamSection->GetTotalIndices() / 3;

		HRESULT hr = DEVICE.DrawIndexedPrimitive(
			pRoamSection->GetIndexBuffer().GetType(),
			0,
			0,
			m_uSectorVertices * m_uSectorVertices,
			0,
			uTotalPolys);

		pEffect->GetEffect()->EndPass();
	}
}

void CRoamTerrain::Reset()
{
	m_uTessellationQueueCount = 0;
	m_uNextTriangleNode       = 0;

	UINT uTotal = m_uSectorCountY * m_uSectorCountX;

	for (UINT i = 0; i < uTotal; i ++)
	{
		m_pRoamSection[i].Reset();
	}
}

bool CRoamTerrain::AddToTessellationQueue(CRoamTerrainSection* pSection)
{
	if (m_uTessellationQueueCount < TESSELLATION_QUEUE_SIZE)
	{
		m_ppTessellationQueue[m_uTessellationQueueCount] = pSection;

		m_uTessellationQueueCount ++;

		return true;
	}

	DEBUG_WARNING("increase the size of the ROAM tessellation queue");

	return false;
}

typedef CRoamTerrainSection* LPROAMTERRAINSECTION;

bool IsWrap(
		const LPROAMTERRAINSECTION& Source, 
		const LPROAMTERRAINSECTION& Target)
{
	return Source->GetQueueSortValue() < Target->GetQueueSortValue();
};

void CRoamTerrain::ProcessTessellationQueue()
{
	Sort<LPROAMTERRAINSECTION>(m_ppTessellationQueue, m_ppTessellationQueue, m_uTessellationQueueCount, IsWrap);

	UINT i;

	for(i = 0; i < m_uTessellationQueueCount; i ++)
		m_ppTessellationQueue[i]->Tessellate(m_fScale, m_fLimit);

	for(i = 0; i < m_uTessellationQueueCount; i ++)
		m_ppTessellationQueue[i]->BuildTriangleList();
}

CRoamTerrain::LPTRIANGLETREENODE CRoamTerrain::RequestTriangleNode()
{
	LPTRIANGLETREENODE pNode = 0;

	if (m_uNextTriangleNode < MAXINUM_TRIANGLE_TREE_NODES)
	{
		pNode = &m_pTriangleNodePool[m_uNextTriangleNode];
		memset( pNode, 0 ,sizeof(TRIANGLETREENODE) );

		m_uNextTriangleNode ++;
	}

	return pNode;
}

bool CRoamTerrain::_AllocateSectors()
{
	DEBUG_NEW(m_pRoamSection, CRoamTerrainSection[m_uSectorCountX * m_uSectorCountY]);

	D3DXVECTOR2 SectorPosition;
	CRectangle2D SectorRectangle;

	UINT x, y, uPixelX, uPixelY, uIndex = 0;

	for(y=0; y < m_uSectorCountY; y ++)
	{
		for(x = 0; x < m_uSectorCountX; x ++)
		{
			SectorPosition.x = m_WorldExtents.GetMinX() + x * m_SectorSize.x;
			SectorPosition.y = m_WorldExtents.GetMinY() + y * m_SectorSize.y;

			SectorRectangle.Set(
				SectorPosition.x, 
				SectorPosition.x + m_SectorSize.x,
				SectorPosition.y, 
				SectorPosition.y + m_SectorSize.y);

			uPixelX = x << m_uSectorShift;
			uPixelY = y << m_uSectorShift;

			if(!m_pRoamSection[uIndex].Create(
				m_pRootNode, 
				this, 
				x, 
				y, 
				uPixelX, 
				uPixelY, 
				m_uSectorVertices, 
				m_uSectorVertices, 
				SectorRectangle) )
				return false;

			uIndex ++;
		}
	}

	return true;
}
