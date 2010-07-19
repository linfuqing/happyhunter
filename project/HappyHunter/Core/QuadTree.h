#pragma once

#include "Camera.h"
#include "BitFlag.h"
#include "basicutils.h"

namespace zerO
{
	inline UINT32 GetMaskY(UINT8 uMinY, UINT8 uMaxY)
	{
		UINT32 uHigh     = FLAG(uMaxY),
			uLow         = FLAG(uMinY),
			uSetMask     = uHigh - 1,
			uClearMask   = uLow  - 1,
			uResult      = uSetMask;

		if (uMinY)
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
		friend class CQuadTreeNode;
	public:
		CQuadTreeObject(void);
		~CQuadTreeObject(void);

		/*CQuadTreeNode* GetParentQuadTreeNode()const;
		UINT32 GetMaskZ()const;

		CQuadTreeObject* GetTreeForward()const;
		CQuadTreeObject* GetTreeRear()const;*/

		virtual void PrepareForRender();

		CQuadTreeObject* GetNext()const;

		/*void SetQueadTree(CQuadTreeNode* pParentQuadTreeNode, UINT32 uMaskZ);*/

		void AttachToQuadTree(CQuadTree* pParent);

		void DetachFromSearchList();

		void DetachFromQuadTree();

		/*void ClearListData();*/
	private:
		/*void __SetTreeForward(CQuadTreeObject* pTreeForward);
		void __SetTreeRear(CQuadTreeObject* pTreeRear);

		void __SetSearchForward(CQuadTreeObject* pTreeForward);
		void __SetSearchRear(CQuadTreeObject* pTreeRear);*/

		void __AttachToSearchList(CQuadTreeObject* pForward, CQuadTreeObject* pRear);

		CQuadTreeObject* m_pTreeForward;
		CQuadTreeObject* m_pTreeRear;

		CQuadTreeObject* m_pSearchForward;
		CQuadTreeObject* m_pSearchRear;

		CQuadTree*     m_pParentQuadTree;
		CQuadTreeNode* m_pParentQuadTreeNode;
		UINT32 m_uMaskY;
	};

	/*inline CQuadTreeObject* CQuadTreeObject::GetTreeForward()const
	{
		return m_pTreeForward;
	}

	inline CQuadTreeObject* CQuadTreeObject::GetTreeRear()const
	{
		return m_pTreeRear;
	}*/

	inline CQuadTreeObject* CQuadTreeObject::GetNext()const
	{
		return m_pSearchForward;
	}

	/*inline void CQuadTreeObject::__SetTreeForward(CQuadTreeObject* pTreeForward)
	{
		m_pTreeForward = pTreeForward;
	}

	inline void CQuadTreeObject::__SetTreeRear(CQuadTreeObject* pTreeRear)
	{
		m_pTreeRear = pTreeRear;
	}

	inline void CQuadTreeObject::__SetSearchForward(CQuadTreeObject* pForward)
	{
		m_pSearchForward = pForward;
	}

	inline void CQuadTreeObject::__SetSearchRear(CQuadTreeObject* pRear)
	{
		m_pSearchRear = pRear;
	}*/

	/*inline void CQuadTreeObject::SetQueadTree(CQuadTreeNode* pParentQuadTreeNode, UINT32 uMaskZ)
	{
		m_pParentQuadTreeNode = pParentQuadTreeNode;
		m_uMaskZ              = uMaskZ;
	}

	inline CQuadTreeNode* CQuadTreeObject::GetParentQuadTreeNode()const
	{
		return m_pParentQuadTreeNode;
	}

	inline UINT32 CQuadTreeObject::GetMaskZ()const
	{
		return m_uMaskZ;
	}*/

	inline void CQuadTreeObject::__AttachToSearchList(CQuadTreeObject* pForward, CQuadTreeObject* pRear)
	{
		m_pSearchForward = pForward;
		m_pSearchRear    = pRear;

		if(pForward)
			pForward->m_pSearchRear = this;

		if(pRear)
			pRear->m_pSearchForward = this;
	}

	inline void CQuadTreeObject::DetachFromSearchList()
	{
		if(m_pSearchForward)
			m_pSearchForward->m_pSearchRear = m_pSearchRear;

		if(m_pSearchRear)
			m_pSearchRear->m_pSearchForward = m_pSearchForward;

		m_pSearchForward = NULL;
		m_pSearchRear    = NULL;
	}

	/*inline void CQuadTreeObject::ClearSearchListData()
	{
		m_pSearchForward = NULL;
		m_pSearchRear    = NULL;
	}*/

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
			UINT32 uMaskY,
			const LPFRUSTUM pFrustum);

		void AddMemberToSearchList(
			CQuadTreeObject** ppListHead,
			CQuadTreeObject** ppListTail,
			UINT32 uMaskY,
			const CRectangle3D& WorldRect,
			const LPFRUSTUM pFrustum); 

		void Setup(CQuadTreeNode* pParent, CQuadTreeNode* pChild1, CQuadTreeNode* pChild2, CQuadTreeNode* pChild3, CQuadTreeNode* pChild4);

		UINT32 GetLocalMaskY()const;
		UINT32 GetWorldMaskY()const;
	private:
		void __DescendantMemberAdded(UINT32 uMaskY);
		void __DescendantMemberRemoved();

		void __RebuildLocalMaskY();
		void __RebuildWorldMaskY();

		UINT32 m_uLocalMaskY;
		UINT32 m_uWorldMaskY;

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

	inline UINT32 CQuadTreeNode::GetLocalMaskY()const
	{
		return m_uLocalMaskY;
	}

	inline UINT32 CQuadTreeNode::GetWorldMaskY()const
	{
		return m_uWorldMaskY;
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
		bool Destroy();

		CQuadTreeObject* SearchObject(const CRectangle3D& WorldRectangle, const LPFRUSTUM pFrustum = NULL);

		UINT32 AddObject(CQuadTreeObject* pObject);
	private:
		CQuadTreeNode* m_pLevelNodes[MAXINUM_TREE_DEPTH];
		D3DXVECTOR3 m_Scale;
		D3DXVECTOR3 m_Offset;

		UINT m_uDepth;

		void __FindTreeNodeInfo(const CQuadTreeRectangle& WorldByteRect, UINT& uLevel, UINT& uLevelX, UINT& uLevelZ);
		CQuadTreeNode* __FindTreeNode(const CQuadTreeRectangle& WorldByteRect);
		CQuadTreeNode* __GetNodeFromLevelXZ(INT nLevel, UINT x, UINT z);
	};

	inline CQuadTreeNode* CQuadTree::__GetNodeFromLevelXZ(INT nLevel, UINT x, UINT z)
	{
		if(nLevel >= 0 && nLevel < (INT)m_uDepth)
			return &m_pLevelNodes[nLevel][(z << nLevel) + x];

		return NULL;
	}

	inline void CQuadTree::__FindTreeNodeInfo(const CQuadTreeRectangle& WorldByteRect, UINT& uLevel, UINT& uLevelX, UINT& uLevelZ)
	{
		UINT8 uPatternX   = WorldByteRect.GetMinX() ^ WorldByteRect.GetMaxX(),
			  uPatternZ   = WorldByteRect.GetMinZ() ^ WorldByteRect.GetMaxZ(),
			  uBitPattern = MAX(uPatternX, uPatternZ);

		INT nHighBit      = uBitPattern ? (HighestBitSet(uBitPattern) + 1) : 0;

		UINT uShift;

		uLevel = MAXINUM_TREE_DEPTH - nHighBit - MININUM_TREE_DEPTH;

		uLevel = MIN(uLevel, m_uDepth - 1);

		uShift = MAXINUM_TREE_DEPTH - uLevel - MININUM_TREE_DEPTH;

		uLevelX = WorldByteRect.GetMaxX() >> uShift;
		uLevelZ = WorldByteRect.GetMaxZ() >> uShift;
	}

	inline CQuadTreeNode* CQuadTree::__FindTreeNode(const CQuadTreeRectangle& WorldByteRect)
	{
		UINT uLevel, uLevelX, uLevelZ;

		__FindTreeNodeInfo(WorldByteRect, uLevel, uLevelX, uLevelZ);

		return __GetNodeFromLevelXZ(uLevel, uLevelX, uLevelZ);
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
