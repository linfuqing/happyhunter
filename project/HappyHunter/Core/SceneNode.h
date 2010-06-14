#pragma once

#include <list>
#include "RenderQueue.h"

namespace zerO
{
	///
	// ��Դ�����ڵ�,���ṹ.����ϵ����.
	///
	class CSceneNode
	{
	public:
		CSceneNode(void);
		~CSceneNode(void);

		// �����Ӽ�.
		void AddChild(CSceneNode* pChild);

		//������Ⱦ�鲢���뵽��Ⱦ������,����Ⱦ�����Զ����ñ�����Ⱦ����.
		bool ApplyForRender();

		//������Ⱦ��,���Ӽ�����
		virtual void Build(CRenderQueue::LPRENDERENTRY pEntry);

		//��Ⱦ����,���Ӽ�����
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