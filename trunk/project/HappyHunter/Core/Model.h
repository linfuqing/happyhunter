#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// 模型:骨骼基类,对D3DXFRAME和D3DXMESHCONTAINER封装.
	///
	class CModel :
		public CResource
	{
	public:
		CModel(void);
		~CModel(void);
	};
}
