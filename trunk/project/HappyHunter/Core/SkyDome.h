#pragma once
#include "Background.h"
#include "VertexBuffer.h"

namespace zerO
{
	class CSkyDome :
		public CBackground
	{
		typedef struct
		{
			FLOAT x, y, z, u, v;
		}VERTEX, * LPVERTEX;
	public:
		CSkyDome(void);
		~CSkyDome(void);

		bool Create(FLOAT fTheta, FLOAT fPhi, FLOAT fRadius);

		void Render();

	private:
		CVertexBuffer m_VertexBuffer;

		UINT m_uNumTriangles;
	};
}
