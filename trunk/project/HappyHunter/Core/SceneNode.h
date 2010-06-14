#pragma once

#include <list>
#include "RenderQueue.h"

namespace zerO
{
	///
	// 资源场景节点,树结构.坐标系基类.
	///
	class CSceneNode
	{
	public:
		CSceneNode(void);
		~CSceneNode(void);

		// 加入子集.
		void AddChild(CSceneNode* pChild);

		//构建渲染块并加入到渲染队列中,由渲染队列自动调用本身渲染机制.
		bool ApplyForRender();

		//构建渲染块,供子集重载
		virtual void Build(CRenderQueue::LPRENDERENTRY pEntry);

		//渲染机制,供子集重载
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);
	private:
		D3DXMATRIX m_LocalMatrix;
		D3DXMATRIX m_WorldMatrix;
		D3DXMATRIX m_InverseWorldMatrix;

		D3DXVECTOR3	m_Position;
		D3DXVECTOR3	m_Right;
		D3DXVECTOR3	m_Up;
		D3DXVECTOR3	m_Forward;

		CSceneNode* m_pParent;
		std::list<CSceneNode*> m_Children;
	};
}