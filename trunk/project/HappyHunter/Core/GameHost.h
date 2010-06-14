#pragma once

#include "debug.h"
#include "datatype.h"
#include <vector>

namespace zerO
{
	///
	//GameHost本身.
	///
#define GAMEHOST CGameHost::GetInstance()

	///
	//D3D设备
	///
#define DEVICE   GAMEHOST.GetDevice()

	typedef UINT16 RESOURCEHANDLE;

	class CResource;
	class CRenderQueue;

	///
	// 主渲染框架类，单件模式，包含资源池，帧渲染调用，
	// 其中资源池采用链表与静态数组结合，以便使用索引句柄进行快速查找与插入。
	// 各种情况继承CGameHost然后重写Create方法.
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
