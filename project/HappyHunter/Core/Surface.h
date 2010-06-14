#pragma once
#include "Texture.h"
#include "BitFlag.h"

namespace zerO
{
	class CSurface :
		public CResource
	{
	public:
		typedef UINT16 TEXITUREFLAGTYPE;

		enum CONSTANTS
		{
			MAXINUM_TEXTURE_PER_SURFACE = TOTAL_BITS(TEXITUREFLAGTYPE)
		};

		CSurface(void);
		~CSurface(void);

		void SetMaterial(D3DMATERIAL9& Material);
		void SetTexture(CTexture* pTexture, UINT uIndex);

		CTexture*           GetTexture(UINT uIndex)const;
		const D3DMATERIAL9& GetMaterial()const;
		UINT32              GetTotalTexturesNumber()const;
		TEXITUREFLAGTYPE    GetTextureFlag()const;

	private:
		D3DMATERIAL9     m_Material;
		CTexture*        m_pTextures[MAXINUM_TEXTURE_PER_SURFACE];
		UINT             m_uNumTextures;
		TEXITUREFLAGTYPE m_TextureFlag;
	};

	inline void CSurface::SetMaterial(D3DMATERIAL9& Material)
	{
		memcpy( &m_Material, &Material, sizeof(m_Material) );
	}

	inline void CSurface::SetTexture(CTexture* pTexture, UINT uIndex)
	{
		DEBUG_ASSERT(uIndex < MAXINUM_TEXTURE_PER_SURFACE, "Invalid texture slot");

		if(!m_pTextures[uIndex])
		{
			m_uNumTextures ++;
			SET_BIT(m_TextureFlag, uIndex);
		}

		if(!pTexture)
		{
			m_uNumTextures --;
			CLEAR_BIT(m_TextureFlag, uIndex);
		}

		m_pTextures[uIndex] = pTexture;
	}

	inline CTexture* CSurface::GetTexture(UINT uIndex)const
	{
		DEBUG_ASSERT(uIndex < MAXINUM_TEXTURE_PER_SURFACE, "Invalid texture slot");

		return m_pTextures[uIndex];
	}

	inline const D3DMATERIAL9& CSurface::GetMaterial()const
	{
		return m_Material;
	}

	inline UINT32 CSurface::GetTotalTexturesNumber()const
	{
		return m_uNumTextures;
	}

	inline CSurface::TEXITUREFLAGTYPE CSurface::GetTextureFlag()const
	{
		return m_TextureFlag;
	}
}
