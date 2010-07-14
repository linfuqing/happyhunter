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
		void __GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d);

	public:
		bool Create(const PBASICCHAR meshFile);
		bool Destroy();
		virtual bool ApplyForRender();
		virtual void Update();
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		const CRenderMethod& GetRenderMethod() const;

	private:
		LPD3DXMESH              m_pMesh;			// ԭ����ģ��
		LPD3DXBUFFER            m_pAdjacencyBuffer; // ����ģ�����ڽ���Ϣ
		DWORD                   m_dwNumMaterials;   // ��������	
		CRenderMethod			m_RenderMethod;		// ��Ⱦ����

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
	// ��ȡ����
	//---------------------------------------------------------------------------

	inline const CRenderMethod& CStaticMesh::GetRenderMethod() const
	{
		return m_RenderMethod;
	}
}
