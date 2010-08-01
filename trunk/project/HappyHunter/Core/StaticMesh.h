#pragma once

#include "debug.h"
#include "BasicString.h"
#include "Sprite.h"
#include "RenderMethod.h"
#include "Mesh.h"
//#include "ShadowVolume.h"

namespace zerO
{
	class CStaticMesh : public CSprite
	{
	public:
		CStaticMesh(void);
		~CStaticMesh(void);

	public:
		CMesh& GetMesh();

		bool Create(const PBASICCHAR meshFile);

		virtual bool ApplyForRender();
		virtual void Update();
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		void SetEffectFile(const BASICSTRING& file);

		const BASICSTRING& GetEffectFile() const;
		const CRenderMethod& GetRenderMethod() const;

	private:
		CRenderMethod			m_RenderMethod;		// 渲染方法
		BASICSTRING				m_strEffectFile;	// 效果文件
		CMesh                   m_Mesh;
		//CShadowVolume           m_Shadow;
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

	inline CMesh& CStaticMesh::GetMesh()
	{
		return m_Mesh;
	}

	inline const CRenderMethod& CStaticMesh::GetRenderMethod() const
	{
		return m_RenderMethod;
	}
}
