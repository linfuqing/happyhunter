#include "StdAfx.h"
#include "RenderMethod.h"


using namespace zerO;

CRenderMethod::CRenderMethod(void)
{
}

CRenderMethod::~CRenderMethod(void)
{
}

void CRenderMethod::LoadEffect(const PBASICCHAR pcFileName, zerO::UINT uStage)
{
	DEBUG_ASSERT(uStage < m_EffectList.max_size(), "Stage error!");

	CEffect* pEffect = new CEffect;

	pEffect->Load(pcFileName);

	m_EffectList.insert(m_EffectList.begin() + uStage, pEffect);
}