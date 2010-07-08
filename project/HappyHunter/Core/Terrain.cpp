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
		for(UINT i = 0; i < m_uSectorCountX * m_uSectorCountY; i ++)
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
	m_uSectorVertices = 1 << uShift;
	m_uSectorUnits    = m_uSectorVertices - 1; 

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

void CTerrain::Destroy()
{
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
			pRenderEntry->uModelType   = CRenderQueue::RENDERENTRY::BUFFER;
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

	m_pIndexBuffer->Create(D3DPT_TRIANGLELIST, m_uMaxIndices, D3DUSAGE_DYNAMIC,D3DPOOL_DEFAULT, NULL);

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
}

void CRoamTerrainSection::Tessellate(zerO::FLOAT fScale, zerO::FLOAT fLimit)
{
}

void CRoamTerrainSection::BuildTriangleList()
{
}

void CRoamTerrainSection::__ComputeVariance()
{
}

void CRoamTerrainSection::__Split(LPTRIANGLETREENODE pTriangle)
{
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
}

void CRoamTerrainSection::__RecursiveBuildTriangleList( 
								  LPTRIANGLETREENODE pTriangle,  
								  zerO::UINT16 uCornerA, 
								  zerO::UINT16 uCornerB, 
								  zerO::UINT16 uCornerC)
{
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
	return true;
}

void CRoamTerrain::RenderSection(CTerrainSection* pSection, zerO::UINT32 uFlag, const CRenderQueue::LPRENDERENTRY pEntry)const
{
}

void CRoamTerrain::Reset()
{
}

bool CRoamTerrain::AddToTessellationQueue(CRoamTerrainSection* pSection)
{
	return true;
}

void CRoamTerrain::ProcessTessellationQueue()
{
}

CRoamTerrain::LPTRIANGLETREENODE CRoamTerrain::RequestTriangleNode()
{
	return NULL;
}

bool CRoamTerrain::__AllocateSectors()
{
	return true;
}
