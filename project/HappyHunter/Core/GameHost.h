#pragma once

#include "debug.h"
#include "datatype.h"
#include <vector>

namespace zerO
{
	///
	//GameHost����.
	///
#define GAMEHOST CGameHost::GetInstance()

	///
	//D3D�豸
	///
#define DEVICE   GAMEHOST.GetDevice()

	typedef UINT16 RESOURCEHANDLE;

	class CResource;
	class CRenderQueue;

	///
	// ����Ⱦ����࣬����ģʽ��������Դ�أ�֡��Ⱦ���ã�
	// ������Դ�ز��������뾲̬�����ϣ��Ա�ʹ������������п��ٲ�������롣
	// ��������̳�CGameHostȻ����дCreate����.
	///
	class CGameHost
	{
	public:
		CGameHost(void);
		~CGameHost(void);

		IDirect3DDevice9& GetDevice();
		CRenderQueue& GetRenderQueue();
		static CGameHost& GetInstance();

		RESOURCEHANDLE AddResource(CResource* const pResource);
		CResource* GetResource(RESOURCEHANDLE Handle);
		void RemoveResource(const CResource* pResource);

		virtual bool Create();
	private:
		LPDIRECT3DDEVICE9 m_pDevice;

		static CGameHost* sm_pInstance;

		std::vector<CResource*> m_ResourceList;

		CRenderQueue* m_pRenderQueue;
	};

	inline IDirect3DDevice9& CGameHost::GetDevice()
	{
		DEBUG_ASSERT(m_pDevice, "The game host has not yet been created.");

		return *m_pDevice;
	}

	inline CRenderQueue& CGameHost::GetRenderQueue()
	{
		return *m_pRenderQueue;
	}

	inline CGameHost& CGameHost::GetInstance()
	{
		DEBUG_ASSERT(sm_pInstance, "The game host has not yet been defined.");

		return *sm_pInstance;
	}
}
