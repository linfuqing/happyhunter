#include "StdAfx.h"
#include "GameHost.h"
#include "Resource.h"
#include "Camera.h"
#include "RenderQueue.h"
#include "Background.h"

using namespace zerO;

CGameHost* CGameHost::sm_pInstance = NULL;

CGameHost::CGameHost(void) :
m_pDevice(NULL),
m_pScene(NULL),
m_pBackground(NULL),
m_bLightEnable(false),
m_fTime(0)
{
	DEBUG_ASSERT(!sm_pInstance, "Only one instance of CGameHost is permitted.");

	sm_pInstance = this;
}

CGameHost::~CGameHost(void)
{
	sm_pInstance = NULL;

	Destroy();
}

RESOURCEHANDLE CGameHost::AddResource(CResource* const pResource, RESOURCETYPE Type)
{
	DEBUG_ASSERT(pResource, "pResource can not be NULL.");

	m_ResourceList[Type].push_back(pResource);

	return m_ResourceList[Type].size();
}

CResource* CGameHost::GetResource(RESOURCEHANDLE Handle, RESOURCETYPE Type)
{
	return m_ResourceList[Type][Handle - 1];
}

void CGameHost::RemoveResource(const CResource* pResource, RESOURCETYPE Type)
{
	DEBUG_ASSERT(pResource, "pResource can not be NULL.");

	for(std::vector<CResource*>::iterator i = m_ResourceList[Type].begin(); i != m_ResourceList[Type].end();)
	{
		if(*i == pResource)
			i = m_ResourceList[Type].erase(i);
		else
			i ++;
	}
}

bool CGameHost::Destroy()
{
	UINT uTotalResourceTypes = TOTAL_RESOURCE_TYPES;

	UINT uIndex;
	std::vector<CResource*>::iterator Iteractor;
	for(uIndex = 0; uIndex < uTotalResourceTypes; uIndex ++)
		for(Iteractor = m_ResourceList[uIndex].begin(); Iteractor != m_ResourceList[uIndex].end(); Iteractor ++)
			if( !(*Iteractor)->Destroy() )
				return false;

	DEBUG_DELETE(m_pRenderQueue);
	DEBUG_DELETE(m_pCamera);
	DEBUG_DELETE(m_pScene);

	m_pRenderQueue = NULL;
	m_pCamera      = NULL;
	m_pScene       = NULL;

	for(INT i = 0; i < TOTAL_RESOURCE_TYPES; i ++)
		m_ResourceList[i].clear();

	return true;
}

bool CGameHost::Disable()
{
	UINT uTotalResourceTypes = TOTAL_RESOURCE_TYPES;

	UINT uIndex;
	std::vector<CResource*>::iterator Iteractor;
	for(uIndex = 0; uIndex < uTotalResourceTypes; uIndex ++)
		for(Iteractor = m_ResourceList[uIndex].begin(); Iteractor != m_ResourceList[uIndex].end(); Iteractor ++)
			if( !(*Iteractor)->Disable() )
				return false;

	return true;
}

bool CGameHost::Restore(const D3DSURFACE_DESC& BackBufferSurfaceDesc)
{
	memcpy( &m_DeviceSurfaceDest, &BackBufferSurfaceDesc, sizeof(D3DSURFACE_DESC) );

	UINT uTotalResourceTypes = TOTAL_RESOURCE_TYPES;

	UINT uIndex;
	std::vector<CResource*>::iterator Iteractor;
	for(uIndex = 0; uIndex < uTotalResourceTypes; uIndex ++)
		for(Iteractor = m_ResourceList[uIndex].begin(); Iteractor != m_ResourceList[uIndex].end(); Iteractor ++)
			if( !(*Iteractor)->Restore() )
				return false;

	return true;
}

bool CGameHost::Create(LPDIRECT3DDEVICE9 pDevice, const DEVICESETTINGS& DeviceSettings, zerO::UINT uMaxQueue)
{
	if(pDevice == NULL || uMaxQueue == 0)
		return false;

	m_pDevice = pDevice;

	memcpy( &m_DeviceSettings, &DeviceSettings, sizeof(DeviceSettings) );
	DEBUG_NEW( m_pRenderQueue, CRenderQueue(uMaxQueue) );
	DEBUG_NEW( m_pScene, CSceneNode );
	DEBUG_NEW(m_pCamera, CCamera);

	return true;
}

bool CGameHost::Update(zerO::FLOAT fElapsedTime)
{
	m_fElapsedTime = fElapsedTime;

	m_fTime += fElapsedTime;

	m_pCamera->Update();

	if(m_bLightEnable)
		m_LightManager.Activate();

	if(m_pBackground)
		m_pBackground->Update();

	return true;
}

bool CGameHost::BeginRender()
{
	if(m_pBackground)
	{
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, FALSE);
		m_pBackground->Render();
		m_pDevice->SetRenderState(D3DRS_ZWRITEENABLE, TRUE);
	}

	return true;
}

bool CGameHost::EndRender()
{
	m_pRenderQueue->Render();

	return true;
}