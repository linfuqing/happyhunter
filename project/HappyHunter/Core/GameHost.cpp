#include "StdAfx.h"
#include "GameHost.h"

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

bool CGameHost::Create()
{
	return true;
}