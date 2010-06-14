#pragma once
#include <vector>
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

		void SetStageSize(UINT uSize);

		CEffect* GetEffect(UINT uStage)const;

		UINT GetStageSize()const;

		void LoadEffect(const PBASICCHAR pcFileName, UINT uStage);

	private:
		std::vector<CEffect*> m_EffectList;
	};

	inline void CRenderMethod::SetEffect(CEffect* pEffect, UINT uStage)
	{
		DEBUG_ASSERT(uStage < m_EffectList.max_size(), "Stage error!");

		m_EffectList.insert(m_EffectList.begin() + uStage, pEffect);
	}

	inline void CRenderMethod::SetEffect(CEffect* pEffect)
	{
		m_EffectList.push_back(pEffect);
	}

	inline void CRenderMethod::SetStageSize(UINT uSize)
	{
		m_EffectList.resize(uSize);
	}

	inline CEffect* CRenderMethod::GetEffect(UINT uStage)const
	{
		return m_EffectList.at(uStage);
	}

	inline UINT CRenderMethod::GetStageSize()const
	{
		return m_EffectList.max_size();
	}
}