#pragma once

#include "debug.h"
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
		bool ApplyForRender();
		void Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag);
		CRenderMethod& GetRenderMethod();

	private:
		LPD3DXMESH              m_pMesh;			//原网格模型
		LPD3DXPMESH             m_pPMesh;			//层次细节网格模型
		LPD3DXBUFFER            m_pAdjacencyBuffer; //网格模型面邻接信息
		DWORD                   m_dwNumMaterials;   //材质数量	
		CRenderMethod			m_RenderMethod;
	};

	inline CRenderMethod& CStaticMesh::GetRenderMethod()
	{
		return m_RenderMethod;
	}
}
