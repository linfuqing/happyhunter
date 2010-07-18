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

	private:
		HRESULT __GenerateDeclMesh(LPD3DXMESH& pMesh);
		void __GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d);

	public:
		bool Create(const PBASICCHAR meshFile);
		bool Destroy();
		virtual bool ApplyForRender();
		virtual void Update();
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		void SetEffectFile(const BASICSTRING& file);

		const BASICSTRING& GetEffectFile() const;
		const CRenderMethod& GetRenderMethod() const;

	private:
		LPD3DXMESH              m_pMesh;			// 原网格模型
		LPD3DXBUFFER            m_pAdjacencyBuffer; // 网格模型面邻接信息
		DWORD                   m_dwNumMaterials;   // 材质数量	
		CRenderMethod			m_RenderMethod;		// 渲染方法
		BASICSTRING				m_strEffectFile;	// 效果文件

		struct BoxVertex
		{
			D3DXVECTOR3 p;

			enum FVF
			{
				FVF_Flags = D3DFVF_XYZ
			};
		};
	};

	//---------------------------------------------------------------------------
	// 设置函数
	//---------------------------------------------------------------------------

	inline void CStaticMesh::SetEffectFile(const BASICSTRING& file)
	{
		m_strEffectFile = file;
	}
		
	inline const BASICSTRING& CStaticMesh::GetEffectFile() const
	{
		return m_strEffectFile;
	}

	//---------------------------------------------------------------------------
	// 获取函数
	//---------------------------------------------------------------------------

	inline const CRenderMethod& CStaticMesh::GetRenderMethod() const
	{
		return m_RenderMethod;
	}
}
