#pragma once
#include "Texture.h"
#include "BitFlag.h"

namespace zerO
{
	///
	//表面,带有一个材质,16个纹理对象
	///
	class CSurface :
		public CResource
	{
	public:
		typedef UINT16 TEXITUREFLAGTYPE;

		enum CONSTANTS
		{
			MAXINUM_TEXTURE_PER_SURFACE = 16//TOTAL_BITS(TEXITUREFLAGTYPE)
		};

		CSurface(void);
		~CSurface(void);

		bool Destroy();

		void SetMaterial(const D3DMATERIAL9& Material);
		void SetTexture(CTexture* pTexture, UINT uIndex);

		CTexture*           GetTexture(UINT uIndex)const;
		const D3DMATERIAL9& GetMaterial()const;
		UINT32              GetTotalTexturesNumber()const;
		TEXITUREFLAGTYPE    GetTextureFlag()const;

		bool LoadTexuture(const PBASICCHAR pcFileName, UINT uIndex);

		void Activate();

	private:
		D3DMATERIAL9     m_Material;
		CTexture*        m_pTextures[MAXINUM_TEXTURE_PER_SURFACE];
		UINT             m_uNumTextures;
		TEXITUREFLAGTYPE m_TextureFlag;
		TEXITUREFLAGTYPE m_DestroyFlag;
	};

	inline void CSurface::SetMaterial(const D3DMATERIAL9& Material)
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
		else if( TEST_BIT(m_DestroyFlag, uIndex) )
		{
			DEBUG_DELETE(m_pTextures[uIndex]);

			m_pTextures[uIndex] = NULL;

			CLEAR_BIT(m_DestroyFlag, uIndex);
		}

		if(!pTexture)
		{
			m_uNumTextures --;
			CLEAR_BIT(m_TextureFlag, uIndex);
		}

		m_pTextures[uIndex] = pTexture;
	}

	inline bool CSurface::LoadTexuture(const PBASICCHAR pcFileName, UINT uIndex)
	{
		CTexture* pTexture;

		DEBUG_NEW(pTexture, CTexture);

		if( pTexture->Load(pcFileName) )
		{
			SetTexture(pTexture, uIndex);

			SET_BIT(m_DestroyFlag, uIndex);

			return true;
		}
		else
			DEBUG_DELETE(pTexture);

		return false;
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
