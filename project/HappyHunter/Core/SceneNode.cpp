#include "StdAfx.h"
#include "SceneNode.h"

using namespace zerO;

CSceneNode::CSceneNode(void)
{
}

CSceneNode::~CSceneNode(void)
{
}


void CSceneNode::AddChild(CSceneNode* pChild)
{
	m_Children.push_back(pChild);
}

bool CSceneNode::ApplyForRender()
{
	CRenderQueue::LPRENDERENTRY pEntry;
	if( pEntry = GAMEHOST.GetRenderQueue().LockRenderEntry() )
	{
		Build(pEntry);

		return true;
	}

	return false;
}

void CSceneNode::Build(CRenderQueue::LPRENDERENTRY pEntry)
{
}

void CSceneNode::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
}