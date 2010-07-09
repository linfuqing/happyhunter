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
}

bool CRenderMethod::LoadEffect(const PBASICCHAR pcFileName)
{
	CEffect* pEffect;
	
	DEBUG_NEW(pEffect, CEffect);

	if( pEffect->Load(pcFileName) )
	{
		m_uActiveEffect = m_EffectList.size();

		m_EffectList.push_back(pEffect);

		return true;
	}

	return false;
}

bool CRenderMethod::LoadEffect(const PBASICCHAR pcFileName, zerO::UINT uStage)
{
	DEBUG_ASSERT(uStage < m_EffectList.max_size(), "Stage error!");

	CEffect* pEffect;
	
	DEBUG_NEW(pEffect, CEffect);

	if( pEffect->Load(pcFileName) )
	{
		m_EffectList.insert(m_EffectList.begin() + uStage, pEffect);

		m_uActiveEffect = uStage;

		return true;
	}

	return false;
}

void CRenderMethod::Destory()
{
	if ( m_EffectList.empty() )
		return;

	for ( std::vector<CEffect*>::iterator i = m_EffectList.begin(); i != m_EffectList.end(); i ++ )
	{
		DEBUG_DELETE( *i );
		i = m_EffectList.erase( i );
	}
	m_EffectList.clear();

	if ( m_SurfaceList.empty() )
		return;

	for ( std::vector<CSurface*>::iterator i = m_SurfaceList.begin(); i != m_SurfaceList.end(); i ++ )
	{
		DEBUG_DELETE( *i );
		i = m_SurfaceList.erase( i );
	}
	m_SurfaceList.clear();
}