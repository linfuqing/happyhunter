#pragma once

#include "Camera.h"
#include "BitFlag.h"
#include "basicutils.h"

namespace zerO
{
	inline UINT32 GetMaskZ(UINT8 uMinZ, UINT8 uMaxZ)
	{
		UINT32 uHigh     = FLAG(uMaxZ),
			uLow         = FLAG(uMinZ),
			uSetMask     = uHigh - 1,
			uClearMask   = uLow  - 1,
			uResult      = uSetMask;

		if (uMinZ)
			uResult &= ~uClearMask;

		uResult |= uHigh;
		uResult |= uLow;

		return uResult;
	}

	class CQuadTreeRectangle :
		public CBasicRectangle3D<UINT8>
	{
	public:
		void Convert(const CRectangle3D& Rect, const D3DXVECTOR3& Offset, const D3DXVECTOR3& Scale);
	};

	class CQuadTreeNode;

	class CQuadTree;

	class CQuadTreeObject :
		public CSceneNode
	{
	public:
		CQuadTreeObject(void);
		~CQuadTreeObject(void);

		CQuadTreeNode* GetParentQuadTreeNode()const;
		UINT32 GetMaskZ()const;

		CQuadTreeObject* GetForward()const;
		CQuadTreeObject* GetRear()const;

		void SetForward(CQuadTreeObject* pForward);
		void SetRear(CQuadTreeObject* pRear);

		void SetQueadTrre(CQuadTreeNode* pParentQuadTreeNode, UINT32 uMaskZ);

		void AttachToList(CQuadTreeObject* pForward, CQuadTreeObject* pRear);

		void AttachToQuadTree(CQuadTree* pParent);

		void DetachFromList();

		void DetachFromQuadTree();

		void ClearListData();
	private:
		CQuadTreeObject* m_pForward;
		CQuadTreeObject* m_pRear;

		CQuadTree*     m_pParentQuadTree;
		CQuadTreeNode* m_pParentQuadTreeNode;
		UINT32 m_uMaskZ;
	};

	inline CQuadTreeObject* CQuadTreeObject::GetForward()const
	{
		return m_pForward;
	}

	inline CQuadTreeObject* CQuadTreeObject::GetRear()const
	{
		return m_pRear;
	}

	inline void CQuadTreeObject::SetForward(CQuadTreeObject* pForward)
	{
		m_pForward = pForward;
	}

	inline void CQuadTreeObject::SetRear(CQuadTreeObject* pRear)
	{
		m_pRear = pRear;
	}


	inline void CQuadTreeObject::SetQueadTrre(CQuadTreeNode* pParentQuadTreeNode, UINT32 uMaskZ)
	{
		m_pParentQuadTreeNode = pParentQuadTreeNode;
		m_uMaskZ               = uMaskZ;
	}

	inline CQuadTreeNode* CQuadTreeObject::GetParentQuadTreeNode()const
	{
		return m_pParentQuadTreeNode;
	}

	inline UINT32 CQuadTreeObject::GetMaskZ()const
	{
		return m_uMaskZ;
	}

	inline void CQuadTreeObject::AttachToList(CQuadTreeObject* pForward, CQuadTreeObject* pRear)
	{
		m_pForward = pForward;
		m_pRear    = pRear;

		if(pForward)
			pForward->SetRear(this);

		if(pRear)
			pRear->SetForward(this);
	}

	inline void CQuadTreeObject::DetachFromList()
	{
		if(m_pForward)
			m_pForward->SetRear(m_pRear);

		if(m_pRear)
			m_pRear->SetForward(m_pForward);

		m_pForward = NULL;
		m_pRear    = NULL;
	}

	inline void CQuadTreeObject::ClearListData()
	{
		m_pForward = NULL;
		m_pRear    = NULL;
	}

	class CQuadTreeNode
	{
	public:
		CQuadTreeNode();
		~CQuadTreeNode();

		UINT32 AddMember(CQuadTreeObject* pObject, const CQuadTreeRectangle& Rect);
		void RemoveMember(CQuadTreeObject* pObject);

		void AddMemberToSearchList(
			CQuadTreeObject** ppListHead,
			CQuadTreeObject** ppListTail,
			UINT32 uMaskZ,
			const LPFRUSTUM pFrustum);

		void AddMemberToSearchList(
			CQuadTreeObject** ppListHead,
			CQuadTreeObject** ppListTail,
			UINT32 uMaskZ,
			const CRectangle3D& WorldRect,
			const LPFRUSTUM pFrustum); 

		void Setup(CQuadTreeNode* pParent, CQuadTreeNode* pChild1, CQuadTreeNode* pChild2, CQuadTreeNode* pChild3, CQuadTreeNode* pChild4);

		UINT32 GetLocalMaskZ()const;
		UINT32 GetWorldMaskZ()const;
	private:
		void __DescendantMemberAdded(UINT32 uMaskZ);
		void __DescendantMemberRemoved();

		void __RebuildLocalMaskZ();
		void __RebuildWorldMaskZ();

		UINT32 m_uLocalMaskZ;
		UINT32 m_uWorldMaskZ;

		CQuadTreeObject* m_pMembers;

		CQuadTreeNode* m_pParent;
		CQuadTreeNode* m_pChildren[4];
	};
		
	inline void CQuadTreeNode::Setup(CQuadTreeNode* pParent, CQuadTreeNode* pChild1, CQuadTreeNode* pChild2, CQuadTreeNode* pChild3, CQuadTreeNode* pChild4)
	{
		DEBUG_ASSERT(m_pParent == NULL, "Parent node already set.");

		m_pParent = pParent;
		m_pChildren[0] = pChild1;
		m_pChildren[1] = pChild2;
		m_pChildren[2] = pChild3;
		m_pChildren[3] = pChild4;
	}

	inline UINT32 CQuadTreeNode::GetLocalMaskZ()const
	{
		return m_uLocalMaskZ;
	}

	inline UINT32 CQuadTreeNode::GetWorldMaskZ()const
	{
		return m_uWorldMaskZ;
	}

	class CQuadTree
	{
		typedef enum
		{
			MININUM_TREE_DEPTH = 1,
			MAXINUM_TREE_DEPTH = 8/*TOTAL_BITS(UINT8)*/ + MININUM_TREE_DEPTH
		}CONSTANTS;
	public:
		CQuadTree(void);
		~CQuadTree(void);

		void Create(const CRectangle3D& Boundary, UINT uDepth);
		void Destroy();

		CQuadTreeObject* SearchObject(const CRectangle3D& WorldRectangle, const LPFRUSTUM pFrustum = NULL);

		UINT32 AddObject(CQuadTreeObject* pObject);
	private:
		CQuadTreeNode* m_pLevelNodes[MAXINUM_TREE_DEPTH];
		D3DXVECTOR3 m_Scale;
		D3DXVECTOR3 m_Offset;

		UINT m_uDepth;

		void __FindTreeNodeInfo(const CQuadTreeRectangle& WorldByteRect, UINT& uLevel, UINT& uLevelX, UINT& uLevelY);
		CQuadTreeNode* __FindTreeNode(const CQuadTreeRectangle& WorldByteRect);
		CQuadTreeNode* __GetNodeFromLevelXY(INT nLevel, UINT x, UINT y);
	};

	inline CQuadTreeNode* CQuadTree::__GetNodeFromLevelXY(INT nLevel, UINT x, UINT y)
	{
		if(nLevel >= 0 && nLevel < (INT)m_uDepth)
			return &m_pLevelNodes[nLevel][(y << nLevel) + x];

		return NULL;
	}

	inline void CQuadTree::__FindTreeNodeInfo(const CQuadTreeRectangle& WorldByteRect, UINT& uLevel, UINT& uLevelX, UINT& uLevelY)
	{
		UINT8 uPatternX   = WorldByteRect.GetMinX() ^ WorldByteRect.GetMaxX(),
			  uPatternY   = WorldByteRect.GetMinY() ^ WorldByteRect.GetMaxY(),
			  uBitPattern = MAX(uPatternX, uPatternY);

		INT nHighBit      = uBitPattern ? HighestBitSet(uBitPattern) + 1 : 0;

		UINT uShift;

		uLevel = MAXINUM_TREE_DEPTH - nHighBit - 1;

		uLevel = MIN(uLevel, m_uDepth - 1);

		uShift = MAXINUM_TREE_DEPTH - uLevel - 1;

		uLevelX = WorldByteRect.GetMaxX() >> uShift;
		uLevelY = WorldByteRect.GetMaxY() >> uShift;
	}

	inline CQuadTreeNode* CQuadTree::__FindTreeNode(const CQuadTreeRectangle& WorldByteRect)
	{
		UINT uLevel, uLevelX, uLevelY;

		__FindTreeNodeInfo(WorldByteRect, uLevel, uLevelX, uLevelY);

		return __GetNodeFromLevelXY(uLevel, uLevelX, uLevelY);
	}

	
	inline UINT32 CQuadTree::AddObject(CQuadTreeObject* pObject)
	{
		DEBUG_ASSERT(m_uDepth, "The quad tree has not been created.");

		CQuadTreeRectangle ByteRect;

		ByteRect.Convert(pObject->GetWorldRectangle(), m_Offset, m_Scale);

		CQuadTreeNode* pNode = __FindTreeNode(ByteRect);

		DEBUG_ASSERT(pNode, "Failed to locate quad tree node.");

		return pNode->AddMember(pObject, ByteRect);
	}
}
