#pragma once

#include "debug.h"
#include "BasicString.h"
#include "Effect.h"
#include "Texture.h"
#include "SceneNode.h"
#include "RenderMethod.h"

namespace zerO
{
	class CStaticMesh : public CSceneNode
	{
	public:
		CStaticMesh(void);
		~CStaticMesh(void);

	public:
		bool Create();
		virtual bool ApplyForRender();
		virtual void Update();
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		void SetMeshFile(const BASICSTRING& file);
		void SetEffectFile(const BASICSTRING& file);

		const BASICSTRING& GetMeshFile() const;
		const BASICSTRING& GetEffectFile() const;
		const CRenderMethod& GetRenderMethod() const;

	private:
		LPD3DXMESH              m_pMesh;			// 原网格模型
		LPD3DXPMESH             m_pPMesh;			// 层次细节网格模型
		LPD3DXBUFFER            m_pAdjacencyBuffer; // 网格模型面邻接信息
		DWORD                   m_dwNumMaterials;   // 材质数量	
		CRenderMethod			m_RenderMethod;		// 渲染方法
		BASICSTRING				m_strMeshFile;		// 模型文件
		BASICSTRING				m_strEffectFile;	// shader文件
	};

	//---------------------------------------------------------------------------
	// 设置函数
	//---------------------------------------------------------------------------

	inline void CStaticMesh::SetMeshFile(const BASICSTRING &file)
	{
		m_strMeshFile = file;
	}

	inline void CStaticMesh::SetEffectFile(const BASICSTRING &file)
	{
		m_strEffectFile = file;
	}

	//---------------------------------------------------------------------------
	// 获取函数
	//---------------------------------------------------------------------------

	inline const BASICSTRING& CStaticMesh::GetMeshFile() const
	{
		return m_strMeshFile;
	}

	inline const BASICSTRING& CStaticMesh::GetEffectFile() const
	{
		return m_strEffectFile;
	}

	inline const CRenderMethod& CStaticMesh::GetRenderMethod() const
	{
		return m_RenderMethod;
	}
}
