#include "StdAfx.h"
#include "LightManager.h"
#include "GameHost.h"

using namespace zerO;

CLightManager::CLightManager(void) :
m_Ambient(0),
m_uFlag(0),
m_uNumLights(0)
{
	memset( m_Light, 0, sizeof(m_Light) );
}

CLightManager::~CLightManager(void)
{
}

void CLightManager::Activate()
{
	DEVICE.SetRenderState(D3DRS_LIGHTING, TRUE);

	if(m_uNumLights)
	{
		for(UINT i = 0; i < MAXINUM_LIGHTS_NUMBER; i ++)
		{
			if( TEST_BIT(m_uFlag, i) )
			{
				DEVICE.SetLight(i, &m_Light[i]);
				DEVICE.LightEnable(i, TRUE);
			}
		}
	}

	DEVICE.SetRenderState(D3DRS_AMBIENT, m_Ambient);
}