#pragma once

#include <vector>
#include "Texture.h"

namespace zerO
{
	class CTexturePrinter :
		public CResource
	{
		typedef struct TEXTURE
		{
			CTexture Texture;
			LPDIRECT3DSURFACE9 pSurface;

			TEXTURE(CTexture::RESET pfnReset) :
			pSurface(NULL)
			{
				Texture.Create(pfnReset, D3DUSAGE_RENDERTARGET);
			}
		}TEXTURE, * LPTEXTURE;
	public:
		CTexturePrinter(void);
		~CTexturePrinter(void);

		CTexture* GetTexture(UINT uIndex)const;

		void AddTexture(CTexture::RESET pfnReset);
		void AddTexture();

		bool Disable();
		bool Restore();

		void Begin();
		void End();

		void Activate(UINT uTextureIndex, UINT32 uClearFlag = 0, ARGBCOLOR Color = 0);

		bool Destroy();

	private:
		std::vector<LPTEXTURE> m_TextureList;

		/// 保存原有的渲染表面
		LPDIRECT3DSURFACE9 m_pSavedRenderSurface;
		/// 保存原有的深度表面
		LPDIRECT3DSURFACE9 m_pSavedDepthStencilSurface;

		LPDIRECT3DSURFACE9 m_pDepthStencilSurface;
	};

	inline CTexture* CTexturePrinter::GetTexture(UINT uIndex)const
	{
		if( uIndex < m_TextureList.size() )
			return &m_TextureList[uIndex]->Texture;

		return NULL;
	}
}