#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// ���㻺��,IDirect3DVertexBuffer9 ��װ.
	///
	class CIndexBuffer :
		public CResource
	{
	public:
		CIndexBuffer(void);
		~CIndexBuffer(void);

	private:
		LPDIRECT3DINDEXBUFFER9 m_pBuffer;
	};
}
