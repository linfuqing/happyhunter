#pragma once

#include "debug.h"
#include "datatype.h"
#include "LightManager.h"
#include <vector>
#include <list>

namespace zerO
{

#define GAMEHOST     zerO::CGameHost::GetInstance()
#define DEVICE       GAMEHOST.GetDevice()
#define CAMERA       GAMEHOST.GetCamera()
#define RENDERQUEUE  GAMEHOST.GetRenderQueue()
#define ELAPSEDTIME  GAMEHOST.GetElapsedTime()
#define TIME         GAMEHOST.GetTime()
#define LIGHTMANAGER GAMEHOST.GetLightManager()

	typedef enum
	{
		RESOURCE_VERTEX_BUFFER = 0,
		RESOURCE_INDEX_BUFFER,
		RESOURCE_TEXTURE,
		RESOURCE_SURFACE,
		RESOURCE_EFFECT,
		RESOURCE_MESH,
		RESOURCE_MODEL,
		RESOURCE_ANIMATIONCONTROLLER,

		TOTAL_RESOURCE_TYPES
	}RESOURCETYPE;

	typedef UINT16 RESOURCEHANDLE;

	class CResource;
	class CRenderQueue;
	class CSceneNode;
	class CCamera;
	class CBackground;
	class CShadow;
	class CVertexBuffer;

	///
	// 主渲染框架类，单件模式，包含资源池，帧渲染调用，
	// 其中资源池采用链表与静态数组结合，以便使用索引句柄进行快速查找与插入。
	// 各种情况继承CGameHost然后重写Create方法.
	///
	class CGameHost
	{
		typedef struct
		{
			D3DXVECTOR4 Position;
			D3DCOLOR    Color;
		}VERTEX, * LPVERTEX;
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
		FLOAT64 GetTime()const;

		CCamera& GetCamera();

		CSceneNode* GetScene()const;

		CBackground* GetBackground()const;

		CLightManager& GetLightManager();

		const DEVICESETTINGS& GetDeviceSettings()const;

		void SetLightEnable(bool bValue);

		void SetBackground(CBackground* pBackground);

		RESOURCEHANDLE AddResource(CResource* const pResource, RESOURCETYPE Type);
		CResource* GetResource(RESOURCEHANDLE Handle, RESOURCETYPE Type);
		void RemoveResource(const CResource* pResource, RESOURCETYPE Type);

		void AddShadow(CShadow* const pShadow);
		void RemoveShadow(CShadow* const pShadow);

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
		FLOAT64 m_fTime;

		static CGameHost* sm_pInstance;

		std::vector<CResource*> m_ResourceList[TOTAL_RESOURCE_TYPES];
		std::list<CShadow*>     m_ShadowList;

		CRenderQueue* m_pRenderQueue;

		CSceneNode* m_pScene;

		CCamera* m_pCamera;

		CBackground* m_pBackground;

		CLightManager m_LightManager;

		CVertexBuffer* m_pVertexBuffer;

		bool m_bLightEnable;
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

	inline CSceneNode* CGameHost::GetScene()const
	{
		return m_pScene;
	}

	inline CBackground* CGameHost::GetBackground()const
	{
		return m_pBackground;
	}

	inline const CGameHost::DEVICESETTINGS& CGameHost::GetDeviceSettings()const
	{
		return m_DeviceSettings;
	}

	inline FLOAT CGameHost::GetElapsedTime()const
	{
		return m_fElapsedTime;
	}

	inline FLOAT64 CGameHost::GetTime()const
	{
		return m_fTime;
	}

	inline CLightManager& CGameHost::GetLightManager()
	{
		return m_LightManager;
	}

	inline void CGameHost::SetBackground(CBackground* pBackground)
	{
		m_pBackground = pBackground;
	}

	inline void CGameHost::SetLightEnable(bool bValue)
	{
		m_bLightEnable = bValue;
	}
}
