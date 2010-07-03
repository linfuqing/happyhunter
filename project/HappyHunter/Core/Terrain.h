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

		bool Create(
			CSceneNode* pRootNode,
			CTerrain* pParent, 
			UINT uSectorX, UINT uSectorY, 
			UINT uHeightMapX, UINT uHeightMapY,
			UINT VerticesX, UINT VerticesY, 
			const CRectangle2D& WorldRect);

		CVertexBuffer& GetVertexBuffer();

		UINT GetSectorX()const;
		UINT GetSectorY()const;

		bool ApplyForRender();
		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	private:
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

		bool Create(
			CSceneNode* pRootNode, 
			CTexture* pHeightMap, 
			const CRectangle3D& WorldExtents, 
			UINT8 uShift);

		FLOAT GetHeight(UINT x, UINT y)const;
		FLOAT GetHeight(UINT uIndex)const;
		const D3DXVECTOR3& GetNormal(UINT x, UINT y)const;

		CRenderMethod& GetRenderMethod();

		void SetQuadTree(CQuadTree* pQuadTree);

		void Render();

		void Destroy();

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
}
