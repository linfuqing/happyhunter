#pragma once

#include "debug.h"
#include "datatype.h"
#include <vector>

namespace zerO
{

#define GAMEHOST zerO::CGameHost::GetInstance()
#define DEVICE   GAMEHOST.GetDevice()
#define CAMERA   GAMEHOST.GetCamera()
#define TIME     GAMEHOST.GetElapsedTime()

	typedef enum
	{
		RESOURCE_EFFECT = 0,
		RESOURCE_VERTEX_BUFFER,
		RESOURCE_INDEX_BUFFER,
		RESOURCE_TEXTURE,
		RESOURCE_SURFACE,
		RESOURCE_MODEL,

		TOTAL_RESOURCE_TYPES
	}RESOURCETYPE;

	typedef UINT16 RESOURCEHANDLE;

	class CResource;
	class CRenderQueue;
	class CCamera;

	///
	// 主渲染框架类，单件模式，包含资源池，帧渲染调用，
	// 其中资源池采用链表与静态数组结合，以便使用索引句柄进行快速查找与插入。
	// 各种情况继承CGameHost然后重写Create方法.
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

		FLOAT GetElapsedTime()const;

		CCamera& GetCamera();

		const DEVICESETTINGS& GetDeviceSettings()const;

		RESOURCEHANDLE AddResource(CResource* const pResource, RESOURCETYPE Type);
		CResource* GetResource(RESOURCEHANDLE Handle, RESOURCETYPE Type);
		void RemoveResource(const CResource* pResource, RESOURCETYPE Type);

		virtual bool Destroy(); 
		virtual bool Disable(); 
		virtual bool Restore(const D3DSURFACE_DESC& BackBufferSurfaceDesc); 

		virtual bool Create(LPDIRECT3DDEVICE9 pDevice, const DEVICESETTINGS& DeviceSettings, UINT uMaxQueue);
		virtual bool Update(FLOAT fElapsedTime);
		virtual bool BeginRender();
		virtual bool EndRender();
	private:
		//LPDIRECT3D9 m_pDirect;
		LPDIRECT3DDEVICE9 m_pDevice;
		DEVICESETTINGS m_DeviceSettings;
		D3DSURFACE_DESC m_DeviceSurfaceDest;
		FLOAT m_fElapsedTime;

		static CGameHost* sm_pInstance;

		std::vector<CResource*> m_ResourceList[TOTAL_RESOURCE_TYPES];

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

	inline FLOAT CGameHost::GetElapsedTime()const
	{
		return m_fElapsedTime;
	}
}
