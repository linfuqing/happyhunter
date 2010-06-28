#include "StdAfx.h"
#include "GameHost.h"
#include "Resource.h"
#include "Camera.h"
#include "RenderQueue.h"

using namespace zerO;

CGameHost* CGameHost::sm_pInstance = NULL;

CGameHost::CGameHost(void) :
m_pDevice(NULL)
{
	DEBUG_ASSERT(!sm_pInstance, "Only one instance of CGameHost is permitted.");

	sm_pInstance = this;
}

CGameHost::~CGameHost(void)
{
	sm_pInstance = NULL;
}

RESOURCEHANDLE CGameHost::AddResource(CResource* const pResource)
{
	DEBUG_ASSERT(pResource, "pResource can not be NULL.");
	m_ResourceList.push_back(pResource);

	return m_ResourceList.size();
}

CResource* CGameHost::GetResource(RESOURCEHANDLE Handle)
{
	return m_ResourceList[Handle - 1];
}

void CGameHost::RemoveResource(const CResource* pResource)
{
	DEBUG_ASSERT(pResource, "pResource can not be NULL.");

	for(std::vector<CResource*>::iterator i = m_ResourceList.begin(); i != m_ResourceList.end(); i ++)
		if(*i == pResource)
			i = m_ResourceList.erase(i);
}

void CGameHost::Destroy()
{
	for(std::vector<CResource*>::iterator i = m_ResourceList.begin(); i != m_ResourceList.end(); i ++)
		(*i)->Destroy();
}

void CGameHost::Disable()
{
	for(std::vector<CResource*>::iterator i = m_ResourceList.begin(); i != m_ResourceList.end(); i ++)
		(*i)->Disable();
}

void CGameHost::Restore()
{
	for(std::vector<CResource*>::iterator i = m_ResourceList.begin(); i != m_ResourceList.end(); i ++)
		(*i)->Restore();
}

bool CGameHost::Create(LPDIRECT3DDEVICE9 pDevice, const DEVICESETTINGS& DeviceSettings, zerO::UINT uMaxQueue)
{
	if(pDevice == NULL || uMaxQueue == 0)
		return false;

	m_pDevice = pDevice;

	memcpy( &m_DeviceSettings, &DeviceSettings, sizeof(DeviceSettings) );
	DEBUG_NEW( m_pRenderQueue, CRenderQueue(uMaxQueue) );
	DEBUG_NEW(m_pCamera, CCamera);

	return true;
}

bool CGameHost::BeginRender()
{
	return true;
}

bool CGameHost::EndRender()
{
	m_pRenderQueue->Render();

	return true;
}