#pragma once

#include "Sprite.h"
#include "VertexBuffer.h"
#include "RenderMethod.h"

namespace zerO
{
	class CCrossPlane :
		public CSprite
	{
		typedef struct
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR2 UV;
		}VERTEX, * LPVERTEX;

		typedef struct
		{
			VERTEX Vertices[4];
		}PLANE, * LPPLANE;
	public:
		CCrossPlane(void);
		~CCrossPlane(void);

		CRenderMethod& GetRenderMethod();

		bool Create(FLOAT fWidth, FLOAT fHeight, UINT uNumPlanes, const D3DXVECTOR3* pCenter = NULL);

		void Update();

		bool ApplyForRender();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	private:
		CRenderMethod m_RenderMethod;
		CVertexBuffer m_VertexBuffer;

		FLOAT m_fWidth;
		FLOAT m_fHeight;
		UINT m_uNumPlanes;
	};

	inline CRenderMethod& CCrossPlane::GetRenderMethod()
	{
		return m_RenderMethod;
	}
}
