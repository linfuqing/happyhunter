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

		void Clone(CCrossPlane& CrossPlane)const;

		bool Create(FLOAT fWidth, FLOAT fHeight, UINT uNumPlanes, const D3DXVECTOR3* pCenter = NULL);

		bool Destroy();

		void Update();

		bool ApplyForRender();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	private:
		CRenderMethod m_RenderMethod;
		CVertexBuffer* m_pVertexBuffer;

		FLOAT m_fWidth;
		FLOAT m_fHeight;
		UINT m_uNumPlanes;

		bool m_bIsCreated;
	};

	inline CRenderMethod& CCrossPlane::GetRenderMethod()
	{
		return m_RenderMethod;
	}
}
