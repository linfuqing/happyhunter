#pragma once

#include "SceneNode.h"
#include "VertexBuffer.h"
#include "Surface.h"

namespace zerO
{
	class CBillboarding :
		public CSceneNode
	{
		typedef struct
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR2 UV;
		}VERTEX;
	public:
		CBillboarding(void);
		~CBillboarding(void);

		CSurface& GetSurface();

		bool Create(FLOAT fWidth, FLOAT fHeight);

		void Update();

		bool ApplyForRender();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	private:
		FLOAT         m_fHalfWidth;
		FLOAT         m_fHalfHeight;
		CSurface      m_Surface;
		CVertexBuffer m_VertexBuffer;
	};
}
