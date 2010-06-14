#pragma once

#include "GameHost.h"

namespace zerO
{
	///
	//资源基类,工厂模式,子类为D3D资源,构架立刻加入到资源列表里,GameHost自动调整.
	///
	class CResource
	{
	public:

		CResource(void)
		{
			m_Handle = GAMEHOST.AddResource(this);
		}
		~CResource(void)
		{
			GAMEHOST.RemoveResource(this);
		}

		inline RESOURCEHANDLE GetHandle()const
		{
			return m_Handle;
		}

		//virtual bool Destroy() = 0; 
		//// destroy the resource 
		//virtual bool Disable() = 0; 
		//// purge the resource from volatile memory 
		//virtual bool Restore() = 0; 
		//// restore the resource to volatile memory 
		virtual bool Load(const PBASICCHAR pcFileName = NULL) = 0; 
		// load the resource from a file (or NULL to use the resource name) 
		//virtual bool Save(const PBASICCHAR pcFileName = NULL) = 0; 
		//// save the resource to a file (or NULL to use the resource name)

	private:
		RESOURCEHANDLE m_Handle;
	};
}
