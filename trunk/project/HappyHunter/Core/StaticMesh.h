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
		LPD3DXMESH              m_pMesh;			// ԭ����ģ��
		LPD3DXPMESH             m_pPMesh;			// ���ϸ������ģ��
		LPD3DXBUFFER            m_pAdjacencyBuffer; // ����ģ�����ڽ���Ϣ
		DWORD                   m_dwNumMaterials;   // ��������	
		CRenderMethod			m_RenderMethod;		// ��Ⱦ����
		BASICSTRING				m_strMeshFile;		// ģ���ļ�
		BASICSTRING				m_strEffectFile;	// shader�ļ�
	};

	//---------------------------------------------------------------------------
	// ���ú���
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
	// ��ȡ����
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
