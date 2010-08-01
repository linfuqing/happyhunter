#include "StdAfx.h"
#include "SceneNode.h"

using namespace zerO;

CSceneNode::CSceneNode(void) :
m_pParent(NULL), 
m_bIsTransformDirty(false)
{
	D3DXMatrixIdentity(&m_LocalMatrix);
	D3DXMatrixIdentity(&m_WorldMatrix);

	if( GAMEHOST.GetScene() )
		GAMEHOST.GetScene()->AddChild(this);
}

CSceneNode::~CSceneNode(void)
{
	Destroy();
}

bool CSceneNode::Destroy()
{
	m_Children.clear();

	return true;
}


void CSceneNode::AddChild(CSceneNode* pChild)
{
	if(pChild == this)
		return;

	if(pChild->m_pParent)
		pChild->m_pParent->RemoveChild(pChild);

	m_Children.push_back(pChild);

	pChild->m_pParent = this;

	pChild->_SetTransformDirty();
}

void CSceneNode::RemoveChild(CSceneNode* pChild)
{
	m_Children.remove(pChild);

	pChild->m_pParent = NULL;

	pChild->_SetTransformDirty();
}

void CSceneNode::UpdateChildren()
{
	for(std::list<CSceneNode*>::const_iterator i = m_Children.begin(); i != m_Children.end(); i ++)
	{
		(*i)->Update();
		(*i)->UpdateChildren();
	}
}

void CSceneNode::_SetTransformDirty()
{
	m_bIsTransformDirty = true;

	for(std::list<CSceneNode*>::const_iterator i =  m_Children.begin(); i != m_Children.end(); i ++)
		(*i)->_SetTransformDirty();
}

void CSceneNode::UpdateTransform()
{
	m_bIsTransformDirty = false;

	if(m_pParent)
		D3DXMatrixMultiply(&m_WorldMatrix, &m_LocalMatrix, &m_pParent->m_WorldMatrix);
	else
		m_WorldMatrix = m_LocalMatrix;

	m_WorldRect = m_LocalRect;

	m_WorldRect.Transform(m_WorldMatrix);
}

void CSceneNode::Update()
{
	if(m_bIsTransformDirty)
		UpdateTransform();
}

void CSceneNode::Clone(CSceneNode& Node)const
{
	for(std::list<CSceneNode*>::const_iterator i =  m_Children.begin(); i != m_Children.end(); i ++)
		Node.m_Children.push_back(*i);

	Node.m_LocalMatrix = m_LocalMatrix;
	Node.m_WorldMatrix = m_WorldMatrix;
	Node.m_LocalRect   = m_LocalRect;
	Node.m_WorldRect   = m_WorldRect;

	Node.m_bIsTransformDirty = m_bIsTransformDirty;
}

bool CSceneNode::ApplyForRender()
{
	return true;
}

void CSceneNode::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
}