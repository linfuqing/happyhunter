#pragma once

#include <list>
#include "RenderQueue.h"
#include "Rectangle3D.h"

namespace zerO
{
	///
	// 资源场景节点,树结构.坐标系基类,所有坐标系实体都继承此类.
	///
	class CSceneNode
	{
	public:
		CSceneNode(void);
		~CSceneNode(void);

		virtual bool Destroy();

		CSceneNode* GetParent()const;

		//本地矩阵
		const D3DXMATRIX& GetLocalMatrix()const;
		//世界矩阵
		const D3DXMATRIX& GetWorldMatrix()const;
		//本地边界盒
		const CRectangle3D& GetLocalRectangle()const;
		//世界边界盒
		const CRectangle3D& GetWorldRectangle()const;

		void Clone(CSceneNode& Node)const;

		// 加入子集.
		void AddChild(CSceneNode* pChild);
		void RemoveChild(CSceneNode* pChild);

		void UpdateChildren();

		//构建渲染块并加入到渲染队列中,由渲染队列自动调用本身渲染机制.
		virtual bool ApplyForRender();

		virtual void UpdateTransform();

		//渲染之外的所有变换更新,进行CPU计算.
		virtual void Update();

		//渲染机制,供子集重载
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	protected:
		void _SetTransformDirty();

		bool m_bIsTransformDirty;

		D3DXMATRIX m_LocalMatrix;
		D3DXMATRIX m_WorldMatrix;

		CSceneNode* m_pParent;

		CRectangle3D m_LocalRect;
		CRectangle3D m_WorldRect;
	private:
		std::list<CSceneNode*> m_Children;
	};

	inline CSceneNode* CSceneNode::GetParent()const
	{
		return m_pParent;
	}

	inline const D3DXMATRIX& CSceneNode::GetLocalMatrix()const
	{
		return m_LocalMatrix;
	}

	inline const D3DXMATRIX& CSceneNode::GetWorldMatrix()const
	{
		return m_WorldMatrix;
	}

	inline const CRectangle3D& CSceneNode::GetLocalRectangle()const
	{
		return m_LocalRect;
	}

	inline const CRectangle3D& CSceneNode::GetWorldRectangle()const
	{
		return m_WorldRect;
	}
}