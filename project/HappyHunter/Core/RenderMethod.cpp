#include "StdAfx.h"
#include "RenderMethod.h"


using namespace zerO;

CRenderMethod::CRenderMethod(void) :
m_uActiveEffect(0),
m_uActiveSurface(0)
{
}

CRenderMethod::~CRenderMethod(void)
{
	Destroy();
}

bool CRenderMethod::LoadEffect(const PBASICCHAR pcFileName)
{
	CEffect* pEffect;
	
	DEBUG_NEW(pEffect, CEffect);

	if( pEffect->Load(pcFileName) )
	{
		m_EffectDestroyList.push_back(pEffect);

		m_uActiveEffect = m_EffectList.size();

		m_EffectList.push_back(pEffect);

		return true;
	}
	else
		DEBUG_DELETE(pEffect);

	return false;
}

bool CRenderMethod::LoadEffect(const PBASICCHAR pcFileName, zerO::UINT uStage)
{
	DEBUG_ASSERT(uStage < m_EffectList.size(), "Stage error!");

	CEffect* pEffect;
	
	DEBUG_NEW(pEffect, CEffect);

	if( pEffect->Load(pcFileName) )
	{
		m_EffectDestroyList.push_back(pEffect);

		m_EffectList[uStage] = pEffect;

		m_uActiveEffect = uStage;

		return true;
	}
	else
		DEBUG_DELETE(pEffect);

	return false;
}

bool CRenderMethod::Destroy()
{
	for(std::list<CEffect*>::iterator i = m_EffectDestroyList.begin(); i != m_EffectDestroyList.end(); i ++)
		DEBUG_DELETE(*i);

	m_EffectDestroyList.clear();

	m_EffectList.clear();

	m_SurfaceList.clear();

	return true;
}

void CRenderMethod::DestroySurface()
{
	for(std::vector<CSurface*>::iterator i = m_SurfaceList.begin(); i != m_SurfaceList.end(); i ++)
		DEBUG_DELETE(*i);

	m_SurfaceList.clear();
}