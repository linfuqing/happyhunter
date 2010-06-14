#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// ¶¥µã»º³å,IDirect3DVertexBuffer9 ·â×°.
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
