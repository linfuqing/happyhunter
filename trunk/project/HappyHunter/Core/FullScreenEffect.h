#pragma once

#include "Texture.h"
#include "VertexBuffer.h"
#include "TexturePrinter.h"

namespace zerO
{
	class CFullScreenEffect :
		public CResource
	{
	public:
		CFullScreenEffect(void);
		~CFullScreenEffect(void);

		bool Restore();

		virtual void Render(CTexturePrinter& TexturePrinter);
	protected:

		typedef struct
		{
			D3DXVECTOR4 Position;
			D3DXVECTOR2 UV;
		}VERTEX, * LPVERTEX;

		VERTEX m_Vertices[4];
	};
}
