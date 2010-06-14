#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// IDirect3DVertexBuffer9 ·â×°.
	///
	class CVertexBuffer :
		public CResource
	{
	public:
		CVertexBuffer(void);
		~CVertexBuffer(void);

	private:
		LPDIRECT3DVERTEXBUFFER9	 m_pBuffer;
	};
}
