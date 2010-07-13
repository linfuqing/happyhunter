#pragma once

#include "QuadTree.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "Texture.h"
#include "RenderMethod.h"

namespace zerO
{
	class CTerrain;

	class CTerrainSection :
		public CQuadTreeObject
	{
		typedef struct
		{
			FLOAT	    fHeight;
			D3DXVECTOR3 Normal;
		}VERTEX, * LPVERTEX;
	public:
		CTerrainSection(void);
		~CTerrainSection(void);

		CVertexBuffer& GetVertexBuffer();

		UINT GetSectorX()const;
		UINT GetSectorY()const;

		bool ApplyForRender();
		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		virtual bool Create(
			CSceneNode* pRootNode,
			CTerrain* pParent, 
			UINT uSectorX, 
			UINT uSectorY, 
			UINT uHeightMapX,
			UINT uHeightMapY,
			UINT VerticesX, 
			UINT VerticesY, 
			const CRectangle2D& WorldRect);
	protected:
		virtual bool __BuildVertexBuffer();

		CTerrain* m_pTerrain;
		CVertexBuffer m_VertexBuffer;
		UINT m_uHeightMapX;
		UINT m_uHeightMapY;
		UINT m_uSectorX; 
		UINT m_uSectorY; 
		UINT m_VerticesX; 
		UINT m_VerticesY; 
	};

	inline CVertexBuffer& CTerrainSection::GetVertexBuffer()
	{
		return m_VertexBuffer;
	}

	inline UINT CTerrainSection::GetSectorX()const
	{
		return m_uSectorX;
	}

	inline UINT CTerrainSection::GetSectorY()const
	{
		return m_uSectorY;
	}

	class CTerrain
	{
		typedef struct
		{
			D3DXVECTOR2 Position;
			D3DXVECTOR2 UV;
		}VERTEX, * LPVERTEX;
	public:
		CTerrain(void);
		~CTerrain(void);

		UINT GetTableWidth()const;
		UINT GetTableHeight()const;

		UINT GetTableIndex(UINT uMapX, UINT uMapY)const;

		FLOAT GetHeight(UINT x, UINT y)const;
		FLOAT GetHeight(UINT uIndex)const;
		const D3DXVECTOR3& GetNormal(UINT x, UINT y)const;

		CRenderMethod& GetRenderMethod();

		void Render();

		bool Destroy();

		virtual void SetQuadTree(CQuadTree* pQuadTree);

		virtual bool Create(CSceneNode* pRootNode, CTexture* pHeightMap, const CRectangle3D& WorldExtents, UINT8 uShift);

		virtual bool SubmitSection(CTerrainSection* pSection)const;

		virtual void RenderSection(CTerrainSection* pSection, UINT32 uFlag, const CRenderQueue::LPRENDERENTRY pEntry)const;
	protected:
		void _BuildHeightAndNormalTables(LPDIRECT3DTEXTURE9 pTexture);

		virtual bool _SetVertexDescription();
		virtual bool _BuildVertexBuffer();
		virtual bool _BuildIndexBuffer();

		virtual bool _AllocateSectors();

		CTerrainSection* m_pSector;

		CVertexBuffer m_VertexBuffer;
		CIndexBuffer m_IndexBuffer;

		CRenderMethod m_RenderMethod;

		CRectangle3D m_WorldExtents;

		CSceneNode* m_pRootNode;

		D3DXVECTOR3 m_WorldSize;
		D3DXVECTOR3 m_MapScale;

		D3DXVECTOR2 m_SectorSize;

		UINT8 m_uSectorShift;
		UINT8 m_uSectorUnits;
		UINT8 m_uSectorVertices;

		UINT m_uSectorCountX;
		UINT m_uSectorCountY;

		UINT m_uTableWidth;
		UINT m_uTableHeight;
		PFLOAT m_pfHeightTable;
		LPD3DXVECTOR3 m_pNormalTable;
	};

	inline UINT CTerrain::GetTableWidth()const
	{
		return m_uTableWidth;
	}
	
	inline UINT CTerrain::GetTableHeight()const
	{
		return m_uTableHeight;
	}

	inline UINT CTerrain::GetTableIndex(UINT uMapX, UINT uMapY)const
	{
		uMapX = uMapX < m_uTableWidth  ? uMapX : (m_uTableWidth  - 1);
		uMapY = uMapY < m_uTableHeight ? uMapY : (m_uTableHeight - 1);

		return uMapY * m_uTableWidth + uMapX;
	}

	inline FLOAT CTerrain::GetHeight(UINT x, UINT y)const
	{
		if (x >= m_uTableWidth) 
			x = m_uTableWidth - 1;

		if (y >= m_uTableHeight) 
			y = m_uTableHeight - 1;

		return m_pfHeightTable[y * m_uTableWidth + x];
	}

	inline FLOAT CTerrain::GetHeight(UINT uIndex)const
	{
		DEBUG_ASSERT(uIndex < m_uTableWidth * m_uTableHeight, "Invalid index");

		return m_pfHeightTable[uIndex];
	}

	inline const D3DXVECTOR3& CTerrain::GetNormal(UINT x, UINT y)const
	{
		if (x >= m_uTableWidth) 
			x = m_uTableWidth - 1;

		if (y >= m_uTableHeight) 
			y = m_uTableHeight - 1;

		return m_pNormalTable[(y * m_uTableWidth) + x];
	}

	inline CRenderMethod& CTerrain::GetRenderMethod()
	{
		return m_RenderMethod;
	}

	class CRoamTerrainSection :
		public CTerrainSection
	{
	public:
		typedef enum
		{
			TOTAL_DETAIL_LEVELS = 9,
			TOTAL_VARIANCES     = 1 << TOTAL_DETAIL_LEVELS
		}CONSTANTS;

		typedef struct TRIANGLETREENODE
		{
			TRIANGLETREENODE* pBase;
			TRIANGLETREENODE* pLeftNeighbor;
			TRIANGLETREENODE* pRightNeighbor;
			TRIANGLETREENODE* pLeftChild;
			TRIANGLETREENODE* pRightChild;
		}TRIANGLETREENODE, * LPTRIANGLETREENODE;

		CRoamTerrainSection();
		~CRoamTerrainSection();

		bool Create(
			CSceneNode* pRootNode,
			CTerrain* pParent, 
			UINT uSectorX, UINT uSectorY, 
			UINT uHeightMapX, UINT uHeightMapY,
			UINT VerticesX, UINT VerticesY, 
			const CRectangle2D& WorldRect);

		void Reset();
		void Tessellate(FLOAT fScale, FLOAT fLimit);
		void BuildTriangleList();

		void PrepareForRender();

		FLOAT GetQueueSortValue()const;
		UINT16 GetTotalIndices()const;
		CIndexBuffer& GetIndexBuffer();

	private:
		void __ComputeVariance();

		void __Split(LPTRIANGLETREENODE pTriangle);
		void __RecursiveTessellate( 
			LPTRIANGLETREENODE pTriangle, 
			FLOAT fDistanceA, 
			FLOAT fDistanceB, 
			FLOAT fDistanceC, 
			PFLOAT pfTree,
			UINT16 uIndex,
			FLOAT fScale, 
			FLOAT fLimit);

		void __RecursiveBuildTriangleList( 
			LPTRIANGLETREENODE pTriangle,  
			UINT16 uCornerA, 
			UINT16 uCornerB, 
			UINT16 uCornerC);

		FLOAT __RecursiveComputeVariance(	
			UINT16 uCornerA, 
			UINT16 uCornerB,
			UINT16 uCornerC,
			FLOAT fHeightA, 
			FLOAT fHeightB, 
			FLOAT fHeightC,
			PFLOAT pfTree,
			UINT16 uIndex);

		FLOAT m_fVarianceTreeA[TOTAL_VARIANCES];
		FLOAT m_fVarianceTreeB[TOTAL_VARIANCES];
		FLOAT m_fDistance0;
		FLOAT m_fDistance1;
		FLOAT m_fDistance2;
		FLOAT m_fDistance3;
		FLOAT m_fQueueSortValue;
		UINT16 m_uMaxIndices;
		UINT16 m_uTotalIndices;
		PUINT16 m_puIndexList;

		CIndexBuffer m_IndexBuffer;

		TRIANGLETREENODE m_RootTriangleA;
		TRIANGLETREENODE m_RootTriangleB;

		LPTRIANGLETREENODE m_pLeftNeighborOfA;
		LPTRIANGLETREENODE m_pRightNeighborOfA;
		LPTRIANGLETREENODE m_pLeftNeighborOfB;
		LPTRIANGLETREENODE m_pRightNeighborOfB;
	};

	inline FLOAT CRoamTerrainSection::GetQueueSortValue()const
	{
		return m_fQueueSortValue;
	}

	inline UINT16 CRoamTerrainSection::GetTotalIndices()const
	{
		return m_uTotalIndices;
	}

	inline CIndexBuffer& CRoamTerrainSection::GetIndexBuffer()
	{
		return m_IndexBuffer;
	}

	class CRoamTerrain :
		public CTerrain
	{
		typedef CRoamTerrainSection::TRIANGLETREENODE TRIANGLETREENODE, * LPTRIANGLETREENODE;

		typedef enum
		{
			TOTAL_DETAIL_LEVELS = CRoamTerrainSection::TOTAL_DETAIL_LEVELS,
			TESSELLATION_QUEUE_SIZE = CRoamTerrainSection::TOTAL_VARIANCES << 1,
			MAXINUM_TRIANGLE_TREE_NODES = TESSELLATION_QUEUE_SIZE * 64
		}CONSTANTS;

	public:
		CRoamTerrain();
		~CRoamTerrain();

		void SetTessellationParameters(FLOAT fScale, FLOAT fLimit);
		void SetQuadTree(CQuadTree* pQuadTree);

		bool Create(
			CSceneNode* pRootNode, 
			CTexture* pHeightMap, 
			const CRectangle3D& WorldExtents, 
			UINT8 uShift);

		bool SubmitSection(CTerrainSection* pSection)const;

		void RenderSection(CTerrainSection* pSection, UINT32 uFlag, const CRenderQueue::LPRENDERENTRY pEntry)const;

		void Reset();
		bool AddToTessellationQueue(CRoamTerrainSection* pSection);
		void ProcessTessellationQueue();

		LPTRIANGLETREENODE RequestTriangleNode();
		CRoamTerrainSection* GetSection(INT nSectionX, INT nSectionY);

	private:
		CRoamTerrainSection* m_pRoamSection;
		LPTRIANGLETREENODE m_pTriangleNodePool;
		UINT32 m_uNextTriangleNode;
		CRoamTerrainSection** m_ppTessellationQueue;
		UINT32 m_uTessellationQueueCount;

		FLOAT m_fScale;
		FLOAT m_fLimit;

		bool _AllocateSectors();
	};

	inline void CRoamTerrain::SetTessellationParameters(FLOAT fScale, FLOAT fLimit)
	{
		m_fScale = fScale;
		m_fLimit = fLimit;
	}

	inline CRoamTerrainSection* CRoamTerrain::GetSection(INT nSectionX, INT nSectionY)
	{
		CRoamTerrainSection* pSection = NULL;

		if (nSectionX >= 0 && nSectionX < (INT)m_uSectorCountX && nSectionY >=0 && nSectionY < (INT)m_uSectorCountY)
		{
			pSection = &m_pRoamSection[nSectionY * m_uSectorCountX + nSectionX];
		}
		else
		{
			//
		}

		return pSection;
	}
}
