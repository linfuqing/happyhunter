#pragma once

#include "debug.h"
#include "datatype.h"
#include <vector>

namespace zerO
{

#define GAMEHOST CGameHost::GetInstance()
#define DEVICE   GAMEHOST.GetDevice()
#define CAMERA   GAMEHOST.GetCamera()

	typedef UINT16 RESOURCEHANDLE;

	class CResource;
	class CRenderQueue;
	class CCamera;

	///
	// ����Ⱦ����࣬����ģʽ��������Դ�أ�֡��Ⱦ���ã�
	// ������Դ�ز��������뾲̬�����ϣ��Ա�ʹ������������п��ٲ�������롣
	// ��������̳�CGameHostȻ����дCreate����.
	///
	class CGameHost
	{
	public:
		typedef struct
		{
			UINT AdapterOrdinal;
			D3DDEVTYPE DeviceType;
			D3DFORMAT AdapterFormat;
			DWORD BehaviorFlags;
			D3DPRESENT_PARAMETERS pp;
		}DEVICESETTINGS, * LPDEVICESETTINGS;

		CGameHost(void);
		~CGameHost(void);

		IDirect3DDevice9& GetDevice();
		CRenderQueue& GetRenderQueue();
		static CGameHost& GetInstance();

		CCamera& GetCamera();

		const DEVICESETTINGS& GetDeviceSettings()const;

		RESOURCEHANDLE AddResource(CResource* const pResource);
		CResource* GetResource(RESOURCEHANDLE Handle);
		void RemoveResource(const CResource* pResource);

		void Destroy(); 
		void Disable(); 
		void Restore(); 

		virtual bool Create(LPDIRECT3DDEVICE9 pDevice, const DEVICESETTINGS& DeviceSettings, UINT uMaxQueue);
		virtual bool BeginRender();
		virtual bool EndRender();
	private:
		//LPDIRECT3D9 m_pDirect;
		LPDIRECT3DDEVICE9 m_pDevice;
		DEVICESETTINGS m_DeviceSettings;

		static CGameHost* sm_pInstance;

		std::vector<CResource*> m_ResourceList;

		CRenderQueue* m_pRenderQueue;

		CCamera* m_pCamera;
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

	inline CCamera& CGameHost::GetCamera()
	{
		return *m_pCamera;
	}

	inline const CGameHost::DEVICESETTINGS& CGameHost::GetDeviceSettings()const
	{
		return m_DeviceSettings;
	}
}
