#include "StdAfx.h"
#include "SceneNode.h"

using namespace zerO;

CSceneNode::CSceneNode(void) :
m_pParent(NULL), 
m_bIsTransformDirty(false), 
m_Position(0.0f, 0.0f, 0.0f),
m_Right(1.0f, 0.0f, 0.0f),
m_Up(0.0f, 1.0f, 0.0f),
m_Forward(0.0f, 0.0f, 1.0f)
{
	D3DXMatrixIdentity(&m_LocalMatrix);
	D3DXMatrixIdentity(&m_WorldMatrix);
	D3DXMatrixIdentity(&m_InverseWorldMatrix);
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
	m_Children.push_back(pChild);

	pChild->m_pParent = this;

	pChild->_SetTransformDirty();
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

	D3DXMatrixInverse(&m_InverseWorldMatrix, NULL, &m_WorldMatrix);

	m_WorldRect = m_LocalRect;

	m_WorldRect.Transform(m_WorldMatrix);

	m_Right.x    = m_WorldMatrix._11 / m_WorldMatrix._14;
	m_Right.x    = m_WorldMatrix._12 / m_WorldMatrix._14;
	m_Right.x    = m_WorldMatrix._13 / m_WorldMatrix._14;

	m_Up.x       = m_WorldMatrix._21 / m_WorldMatrix._24;
	m_Up.x       = m_WorldMatrix._22 / m_WorldMatrix._24;
	m_Up.x       = m_WorldMatrix._23 / m_WorldMatrix._24;

	m_Forward.x  = m_WorldMatrix._31 / m_WorldMatrix._34;
	m_Forward.x  = m_WorldMatrix._32 / m_WorldMatrix._34;
	m_Forward.x  = m_WorldMatrix._33 / m_WorldMatrix._34;

	m_Position.x = m_WorldMatrix._41 / m_WorldMatrix._44;
	m_Position.x = m_WorldMatrix._42 / m_WorldMatrix._44;
	m_Position.x = m_WorldMatrix._43 / m_WorldMatrix._44;
}

void CSceneNode::Update()
{
	if(m_bIsTransformDirty)
		UpdateTransform();
}

bool CSceneNode::ApplyForRender()
{
	return true;
}

void CSceneNode::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
}