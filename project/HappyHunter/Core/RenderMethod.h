#pragma once
#include <vector>
#include <list>
#include "Effect.h"

namespace zerO
{
	///
	// 渲染方式,效果文件的集合.
	///
	class CRenderMethod
	{
	public:
		CRenderMethod(void);
		~CRenderMethod(void);

		void SetEffect(CEffect* pEffect, UINT uStage);
		void SetEffect(CEffect* pEffect);

		void SetSurface(CSurface* pSurface, UINT uStage);
		void SetSurface(CSurface* pSurface);

		CEffect* GetEffect()const;
		CEffect* GetEffect(UINT uStage)const;

		CSurface* GetSurface()const;
		CSurface* GetSurface(UINT uStage)const;

		bool LoadEffect(const PBASICCHAR pcFileName);
		bool LoadEffect(const PBASICCHAR pcFileName, UINT uStage);

		bool Destroy();
	private:
		std::vector<CEffect*> m_EffectList;
		std::vector<CSurface*> m_SurfaceList;

		std::list<CEffect*> m_EffectDestroyList;

		UINT m_uActiveEffect;
		UINT m_uActiveSurface;
	};

	inline void CRenderMethod::SetEffect(CEffect* pEffect, UINT uStage)
	{
		DEBUG_ASSERT(uStage < m_EffectList.size(), "Stage error!");

		m_EffectList.insert(m_EffectList.begin() + uStage, pEffect);

		m_uActiveEffect = uStage;
	}

	inline void CRenderMethod::SetEffect(CEffect* pEffect)
	{
		m_uActiveEffect = m_EffectList.size();

		m_EffectList.push_back(pEffect);
	}

	inline void CRenderMethod::SetSurface(CSurface* pSurface, UINT uStage)
	{
		DEBUG_ASSERT(uStage < m_SurfaceList.size(), "Stage error!");

		m_SurfaceList.insert(m_SurfaceList.begin() + uStage, pSurface);
	}

	inline void CRenderMethod::SetSurface(CSurface* pSurface)
	{
		m_uActiveSurface = m_SurfaceList.size();

		m_SurfaceList.push_back(pSurface);
	}

	inline CEffect* CRenderMethod::GetEffect()const
	{
		return m_EffectList.empty() ? NULL : m_EffectList.at(m_uActiveEffect);
	}

	inline CEffect* CRenderMethod::GetEffect(UINT uStage)const
	{
		return m_EffectList.empty() ? NULL : m_EffectList.at(uStage);
	}

	inline CSurface* CRenderMethod::GetSurface()const
	{
		return m_SurfaceList.empty() ? NULL : m_SurfaceList.at(m_uActiveSurface);
	}

	inline CSurface* CRenderMethod::GetSurface(UINT uStage)const
	{
		return m_SurfaceList.empty() ? NULL : m_SurfaceList.at(uStage);
	}
}