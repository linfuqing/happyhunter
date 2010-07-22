#include "StdAfx.h"
#include "QuadTree.h"

using namespace zerO;

void CQuadTreeRectangle::Convert(const CRectangle3D& Rect, const D3DXVECTOR3& Offset, const D3DXVECTOR3& Scale)
{
	CRectangle3D ConvertRect(Rect);

	ConvertRect += Offset;
	ConvertRect *= Scale;

	ConvertRect.GetMaxX() -= 0.01f;
	ConvertRect.GetMaxY() -= 0.01f;
	ConvertRect.GetMaxZ() -= 0.01f;

	ConvertRect.GetMaxX() = MAX( ConvertRect.GetMaxX(), ConvertRect.GetMinX() );
	ConvertRect.GetMaxY() = MAX( ConvertRect.GetMaxY(), ConvertRect.GetMinY() );
	ConvertRect.GetMaxZ() = MAX( ConvertRect.GetMaxZ(), ConvertRect.GetMinZ() );

	CBasicRectangle3D<INT> IntRect;

	IntRect.GetMaxX() = FLOOR( ConvertRect.GetMaxX() );
	IntRect.GetMinX() = FLOOR( ConvertRect.GetMinX() );
	IntRect.GetMaxY() = FLOOR( ConvertRect.GetMaxY() );
	IntRect.GetMinY() = FLOOR( ConvertRect.GetMinY() );
	IntRect.GetMaxZ() = FLOOR( ConvertRect.GetMaxZ() );
	IntRect.GetMinZ() = FLOOR( ConvertRect.GetMinZ() );

	IntRect.GetMinX() = CLAMP(IntRect.GetMinX(), 0                    , 254);
	IntRect.GetMaxX() = CLAMP(IntRect.GetMaxX(), IntRect.GetMinX() + 1, 255);
	IntRect.GetMinY() = CLAMP(IntRect.GetMinY(), 0                    , 30 );
	IntRect.GetMaxY() = CLAMP(IntRect.GetMaxY(), IntRect.GetMinY() + 1, 31 );
	IntRect.GetMinZ() = CLAMP(IntRect.GetMinZ(), 0                    , 254);
	IntRect.GetMaxZ() = CLAMP(IntRect.GetMaxZ(), IntRect.GetMinZ() + 1, 255);

	m_MaxX = IntRect.GetMaxX();
	m_MinX = IntRect.GetMinX();
	m_MaxY = IntRect.GetMaxY();
	m_MinY = IntRect.GetMinY();
	m_MaxZ = IntRect.GetMaxZ();
	m_MinZ = IntRect.GetMinZ();
}

CQuadTreeObject::CQuadTreeObject() :
m_pParentQuadTreeNode(NULL),
m_pTreeForward(NULL),
m_pTreeRear(NULL),
m_pSearchForward(NULL),
m_pSearchRear(NULL),
m_uMaskY(0)
{
}

CQuadTreeObject::~CQuadTreeObject()
{
}

void CQuadTreeObject::PrepareForRender()
{
}

void CQuadTreeObject::AttachToQuadTree(CQuadTree* pParent)
{
	DetachFromQuadTree();

	m_pParentQuadTree = pParent;

	pParent->AddObject(this);
}

void CQuadTreeObject::DetachFromQuadTree()
{
	if(m_pParentQuadTreeNode)
	{
		m_pParentQuadTreeNode->RemoveMember(this);
		m_pParentQuadTreeNode = NULL;
	}

	m_pParentQuadTree = NULL;
	m_pTreeForward    = NULL;
	m_pTreeRear       = NULL;
}

CQuadTreeNode::CQuadTreeNode() :
m_pMembers(NULL),
m_pParent(NULL),
m_uLocalMaskY(0),
m_uWorldMaskY(0)
{
	m_pChildren[0] = m_pChildren[1] = m_pChildren[2] = m_pChildren[3] = NULL;
}

CQuadTreeNode::~CQuadTreeNode()
{
}

zerO::UINT32 CQuadTreeNode::AddMember(CQuadTreeObject* pObject, const CQuadTreeRectangle& Rect)
{
	UINT32 uMaskY = GetMaskY( Rect.GetMinY(), Rect.GetMaxY() );

	if(pObject->m_pParentQuadTreeNode != this)
	{
		if( pObject->m_pParentQuadTreeNode )
			pObject->m_pParentQuadTreeNode->RemoveMember(pObject);

		if(m_pMembers)
		{
			pObject->m_pTreeRear    = NULL;
			pObject->m_pTreeForward = m_pMembers;

			m_pMembers->m_pTreeRear = pObject;
		}
		
		m_pMembers = pObject;

		SET_FLAG(m_uWorldMaskY, uMaskY);
		SET_FLAG(m_uLocalMaskY, uMaskY);

		if(m_pParent)
			m_pParent->__DescendantMemberAdded(uMaskY);
	}
	else
		__RebuildLocalMaskY();

	pObject->m_pParentQuadTreeNode = this;
	pObject->m_uMaskY              = uMaskY;

	return uMaskY;
}

void CQuadTreeNode::RemoveMember(CQuadTreeObject* pObject)
{
	DEBUG_ASSERT(pObject, "pObject can not be NULL.");
	DEBUG_ASSERT(pObject->m_pParentQuadTreeNode == this, "Error removing quad tree member");

	CQuadTreeObject* pTemp;

	if( (pTemp = pObject->m_pTreeForward) != NULL )
		pTemp->m_pTreeRear = pObject->m_pTreeRear;

	if( (pTemp = pObject->m_pTreeRear) != NULL )
		pTemp->m_pTreeForward = pObject->m_pTreeForward;

	if(m_pMembers == pObject)
		m_pMembers = pObject->m_pTreeForward;

	pObject->m_pTreeForward = NULL;
	pObject->m_pTreeRear    = NULL;

	if(m_pParent)
		m_pParent->__DescendantMemberRemoved();
}

void CQuadTreeNode::__RebuildLocalMaskY()
{
	m_uLocalMaskY = 0;

	CQuadTreeObject* pObject = m_pMembers;

	while(pObject)
	{
		SET_FLAG(m_uLocalMaskY, pObject->m_uMaskY);

		pObject = pObject->m_pTreeForward;
	}

	__RebuildWorldMaskY();
}

void CQuadTreeNode::__RebuildWorldMaskY()
{
	m_uWorldMaskY = m_uLocalMaskY;

	if (m_pChildren[0])
		SET_FLAG( m_uWorldMaskY, m_pChildren[0]->m_uWorldMaskY);

	if (m_pChildren[1])
		SET_FLAG( m_uWorldMaskY, m_pChildren[1]->m_uWorldMaskY);

	if (m_pChildren[2])
		SET_FLAG( m_uWorldMaskY, m_pChildren[2]->m_uWorldMaskY);

	if (m_pChildren[3])
		SET_FLAG( m_uWorldMaskY, m_pChildren[3]->m_uWorldMaskY);
}

void CQuadTreeNode::__DescendantMemberAdded(zerO::UINT32 uMaskY)
{
	SET_FLAG(m_uWorldMaskY, uMaskY);

	if(m_pParent)
		m_pParent->__DescendantMemberAdded(uMaskY);
}

void CQuadTreeNode::__DescendantMemberRemoved()
{
	__RebuildWorldMaskY();

	if(m_pParent)
		m_pParent->__DescendantMemberRemoved();
}

void CQuadTreeNode::AddMemberToSearchList(
			CQuadTreeObject** ppListHead,
			CQuadTreeObject** ppListTail,
			zerO::UINT32 uMaskY,
			const LPFRUSTUM pFrustum)
{
	static UINT32 s_uTESTFLAG = CRectangle3D::PLANE_FRONT | CRectangle3D::PLANE_INTERSECT;

	CQuadTreeObject* pObject;
	if( TEST_ANY(m_uLocalMaskY, uMaskY) )
	{
		for(pObject = m_pMembers; pObject; pObject = pObject->m_pTreeForward)
		{
			if( TEST_ANY(pObject->m_uMaskY, uMaskY) 
				&& ( !pFrustum || pFrustum->Test(pObject->GetWorldRectangle(), s_uTESTFLAG) ) )
			{
				if(*ppListTail)
				{
					pObject->__AttachToSearchList(NULL, *ppListTail);

					*ppListTail = pObject;
				}
				else
				{
					pObject->m_pSearchForward = NULL;
					pObject->m_pSearchRear    = NULL;

					*ppListTail               = pObject;
					*ppListHead               = pObject;
				}
			}
		}
	}
}

void CQuadTreeNode::AddMemberToSearchList(
	CQuadTreeObject** ppListHead,
	CQuadTreeObject** ppListTail,
	zerO::UINT32 uMaskY,
	const CRectangle3D& WorldRect,
	const LPFRUSTUM pFrustum)
{
	const static UINT32 s_uTESTFLAG = CRectangle3D::PLANE_FRONT | CRectangle3D::PLANE_INTERSECT;

	CQuadTreeObject* pObject;
	if( TEST_ANY(m_uLocalMaskY, uMaskY) )
	{
		for(pObject = m_pMembers; pObject; pObject = pObject->m_pTreeForward)
		{
			if( TEST_ANY(pObject->m_uMaskY, uMaskY) 
				&& WorldRect.TestHit( pObject->GetWorldRectangle() ) 
				&&( !pFrustum || pFrustum->Test(pObject->GetWorldRectangle(),s_uTESTFLAG) ) )
			{
				if(*ppListTail)
				{
					pObject->__AttachToSearchList(NULL, *ppListTail);

					*ppListTail = pObject;
				}
				else
				{
					pObject->m_pSearchForward = NULL;
					pObject->m_pSearchRear    = NULL;

					*ppListTail = pObject;
					*ppListHead = pObject;
				}
			}
		}
	}
}

CQuadTree::CQuadTree(void) :
m_uDepth(0)
{
	memset( m_pLevelNodes, 0, sizeof(m_pLevelNodes) );
}

CQuadTree::~CQuadTree(void)
{
	Destroy();
}

void CQuadTree::Create(const CRectangle3D& Boundary, zerO::UINT uDepth)
{
	DEBUG_ASSERT(m_uDepth, "The quad tree has already been created.");
	DEBUG_ASSERT(uDepth >= MININUM_TREE_DEPTH && uDepth <= MAXINUM_TREE_DEPTH, "Invalid tree depth.");

	m_uDepth = uDepth;

	Boundary.GetMin(m_Offset);

	m_Offset.x = - m_Offset.x;
	m_Offset.y = - m_Offset.y;
	m_Offset.z = - m_Offset.z;

	Boundary.GetSize(m_Scale);

	m_Scale.x = 256.0f / m_Scale.x;
	m_Scale.y = 32.0f  / m_Scale.y;
	m_Scale.z = 256.0f / m_Scale.z;

	UINT x, z, uNodeCount, uLevelDimension, uLevelIndex;

	UINT i;

	for(i = 0; i < uDepth; i ++)
	{
		uNodeCount  = FLAG(i);
		uNodeCount *= uNodeCount;

		DEBUG_NEW(m_pLevelNodes[i], CQuadTreeNode[uNodeCount]);
	}

	for(i = 0; i < uDepth; i ++)
	{
		uLevelDimension = FLAG(i);
		uLevelIndex     = 0;

		for(z = 0; z < uLevelDimension; z ++)
		{
			for(x = 0; x < uLevelDimension; x ++)
			{
				m_pLevelNodes[i][uLevelIndex].Setup(
					__GetNodeFromLevelXZ(i - 1, (x >> 1),    (z >> 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1),    (z << 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1) + 1,(z << 1)    ),
					__GetNodeFromLevelXZ(i + 1, (x << 1),    (z << 1) + 1),
					__GetNodeFromLevelXZ(i + 1, (x << 1) + 1,(z << 1) + 1) );

				uLevelIndex ++;
			}
		}
	}
}

bool CQuadTree::Destroy()
{
	for (UINT i = 0; i < MAXINUM_TREE_DEPTH; i ++)
	{
		DEBUG_DELETE_ARRAY(m_pLevelNodes[i]);

		m_pLevelNodes[i] = NULL;
	}

	m_uDepth = 0;

	return true;
}

CQuadTreeObject* CQuadTree::SearchObject(const CRectangle3D& WorldRectangle, const LPFRUSTUM pFrustum)
{
	CQuadTreeObject* pListHead = NULL;
	CQuadTreeObject* pListTail = NULL;

	CQuadTreeNode* pNode;

	CQuadTreeRectangle ByteRect, LockRect;
	ByteRect.Convert(WorldRectangle, m_Offset, m_Scale);
	
	UINT32 uMaskY = GetMaskY( ByteRect.GetMinY(), ByteRect.GetMaxY() );

	UINT uShift, x, z, uLevel = 0;

	bool bIsSearch = true;

	while(uLevel < m_uDepth && bIsSearch)
	{
		uShift = MAXINUM_TREE_DEPTH - uLevel - MININUM_TREE_DEPTH;

		LockRect.Set(
			ByteRect.GetMinX() >> uShift,
			ByteRect.GetMaxX() >> uShift,
			0,
			0,
			ByteRect.GetMinZ() >> uShift,
			ByteRect.GetMaxZ() >> uShift);

		bIsSearch = false;

		for(z = LockRect.GetMinZ(); z <= LockRect.GetMaxZ(); z ++)
		{
			for(x = LockRect.GetMinX(); x <= LockRect.GetMaxX(); x ++)
			{
				pNode = __GetNodeFromLevelXZ(uLevel, x, z);
				
				if (pNode->GetWorldMaskY() & uMaskY)
				{
					bIsSearch = true;

					if (z == LockRect.GetMinZ() 
						|| z == LockRect.GetMaxZ()
						|| x == LockRect.GetMinX()
						|| x == LockRect.GetMaxX() )
					{
						pNode->AddMemberToSearchList(
							&pListHead, 
							&pListTail, 
							uMaskY, 
							WorldRectangle, 
							pFrustum);
					}
					else
					{
						pNode->AddMemberToSearchList(
							&pListHead, 
							&pListTail, 
							uMaskY, 
							pFrustum);
					}
				}
			}
		}

		uLevel ++;
	}

	return pListHead;
}