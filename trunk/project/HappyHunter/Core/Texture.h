#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// IDirect3DTexture9·â×°,´ø¾ØÕó.
	///
	class CTexture :
		public CResource
	{
	public:
		CTexture(void);
		~CTexture(void);

		bool Create(
				UINT      uWidth     = D3DX_DEFAULT, 
				UINT      uHeight    = D3DX_DEFAULT, 
				UINT      uMipLevels = D3DX_DEFAULT, 
				UINT      uUsage     = 0, 
				D3DFORMAT Format     = D3DFMT_UNKNOWN, 
				D3DPOOL   Pool       = D3DPOOL_MANAGED);

		bool Destroy();

		bool Load(const PBASICCHAR pcFileName = NULL);

		void SetMatrix(D3DXMATRIX* pMatrix);

			LPDIRECT3DTEXTURE9 GetTexture()const;
			D3DXMATRIX*        GetMatrix()const;
			UINT32             GetWidth()const;
			UINT32             GetHeight()const;
			UINT32             GetMipLevels()const;
			UINT32             GetUsage()const;
			D3DFORMAT          GetFormat()const;
			D3DPOOL            GetPool()const;
		private:
			void __Initialization(
				UINT      uWidth     = D3DX_DEFAULT, 
				UINT      uHeight    = D3DX_DEFAULT, 
				UINT      uMipLevels = D3DX_DEFAULT, 
				UINT      uUsage     = 0, 
				D3DFORMAT Format     = D3DFMT_UNKNOWN, 
				D3DPOOL   Pool       = D3DPOOL_MANAGED);

			LPDIRECT3DTEXTURE9 m_pTexture;
			D3DXMATRIX*        m_pMatrix;
			UINT               m_uWidth;
			UINT               m_uHeight;
			UINT               m_uMipLevels;
			UINT               m_uUsage;
			D3DFORMAT          m_Format;
			D3DPOOL            m_Pool;
	};

	inline void CTexture::SetMatrix(D3DXMATRIX* pMatrix)
	{
		m_pMatrix = pMatrix;
	}

	inline D3DXMATRIX* CTexture::GetMatrix()const
	{
		return m_pMatrix;
	}

	inline LPDIRECT3DTEXTURE9 CTexture::GetTexture() const
	{
		return m_pTexture;
	}

	inline UINT32 CTexture::GetWidth() const
	{
		return m_uWidth;
	}

	inline UINT32 CTexture::GetHeight() const
	{
		return m_uHeight;
	}

	inline UINT32 CTexture::GetMipLevels() const
	{
		return m_uMipLevels;
	}

	inline UINT32 CTexture::GetUsage() const
	{
		return m_uUsage;
	}

	inline D3DFORMAT CTexture::GetFormat() const
	{
		return m_Format;
	}

	inline D3DPOOL CTexture::GetPool() const
	{
		return m_Pool;
	}

	inline void CTexture::__Initialization(
		UINT      uWidth, 
		UINT      uHeight, 
		UINT      uMipLevels, 
		UINT      uUsage, 
		D3DFORMAT Format, 
		D3DPOOL   Pool)
	{
		m_uWidth     = uWidth;
		m_uHeight    = uHeight;
		m_uMipLevels = uMipLevels;
		m_uUsage     = uUsage;
		m_Format     = Format;
		m_Pool       = Pool;
	}
}