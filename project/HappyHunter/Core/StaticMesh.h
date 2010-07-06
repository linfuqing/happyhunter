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
		LPD3DXMESH              m_pMesh;			//ԭ����ģ��
		LPD3DXPMESH             m_pPMesh;			//���ϸ������ģ��
		LPD3DXBUFFER            m_pAdjacencyBuffer; //����ģ�����ڽ���Ϣ
		DWORD                   m_dwNumMaterials;   //��������	
		CRenderMethod			m_RenderMethod;
	};

	inline CRenderMethod& CStaticMesh::GetRenderMethod()
	{
		return m_RenderMethod;
	}
}
