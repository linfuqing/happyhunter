#include "StdAfx.h"
#include "FogManager.h"

#include "GameHost.h"
#include "basicutils.h"

using namespace zerO;
CFogManager::CFogManager(void) :
m_Type(TABLE),
m_Model(NONE),
m_Color(0x0),
m_fStart(0.0f),
m_fEnd(0.0f),
m_fDensity(0.0f),
m_bRangefogEnable(false)
{
}

CFogManager::~CFogManager(void)
{
}

void CFogManager::Activate()
{
	D3DCAPS9 Caps;
	DEVICE.GetDeviceCaps(&Caps);

	DEVICE.SetRenderState(D3DRS_FOGENABLE, TRUE);

	DEVICE.SetRenderState( D3DRS_RANGEFOGENABLE, m_bRangefogEnable && m_Type == VERTEX && TEST_FLAG(Caps.RasterCaps, D3DPRASTERCAPS_FOGRANGE) );

	DEVICE.SetRenderState( (D3DRENDERSTATETYPE)m_Type, m_Model );

	DEVICE.SetRenderState(D3DRS_FOGCOLOR, m_Color);

	DEVICE.SetRenderState( D3DRS_FOGSTART  , CASE(m_fStart  , DWORD) );
	DEVICE.SetRenderState( D3DRS_FOGEND    , CASE(m_fEnd    , DWORD) );
	DEVICE.SetRenderState( D3DRS_FOGDENSITY, CASE(m_fDensity, DWORD) );
}
