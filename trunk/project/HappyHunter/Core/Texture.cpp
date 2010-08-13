#include "StdAfx.h"
#include "Texture.h"

using namespace zerO;

CTexture::CTexture(void) :
CResource(RESOURCE_TEXTURE),
m_pTexture(NULL),
m_pMatrix(NULL),
m_pfnReset(NULL)
{
	__Initialization();
}

CTexture::~CTexture(void)
{
	Destroy();
}

bool CTexture::Disable()
{
	if(m_Pool == D3DPOOL_DEFAULT)
	{
		DEBUG_RELEASE(m_pTexture);
		m_pTexture = NULL;
	}

	return true;
}

bool CTexture::Restore()
{
	if(m_Pool == D3DPOOL_DEFAULT)
	{
		DEBUG_RELEASE(m_pTexture);

		if(m_pfnReset)
			m_pfnReset(
				m_uWidth,
				m_uHeight,
				m_Format);

		HRESULT	hr = D3DXCreateTexture(
			&DEVICE,
			m_uWidth,
			m_uHeight,
			m_uMipLevels,
			m_uUsage,
			m_Format,
			m_Pool,
			&m_pTexture);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);
			return false;
		}
	}

	return true;
}

bool CTexture::Create(
					  zerO::UINT uWidth, 
					  zerO::UINT uHeight, 
					  zerO::UINT uMipLevels, 
					  zerO::UINT uUsage, 
					  D3DFORMAT  Format, 
					  D3DPOOL    Pool)
{
	DEBUG_RELEASE(m_pTexture);
	//Destroy();

	HRESULT hr = D3DXCheckTextureRequirements(          					
		&DEVICE,
		(UINT*)&uWidth,
		(UINT*)&uHeight,
		(UINT*)&uMipLevels,
		uUsage,
		&Format,
		Pool);

	if( SUCCEEDED(hr) )
	{
		if(Pool == D3DPOOL_DEFAULT)
		{
			__Initialization(uWidth, uHeight, uMipLevels, uUsage, Format, Pool);
			return true;
		}

		hr = D3DXCreateTexture(
			&DEVICE,
			uWidth,
			uHeight,
			uMipLevels,
			uUsage,
			Format,
			Pool,
			&m_pTexture
		);

		if( SUCCEEDED(hr) )
		{
			__Initialization(uWidth, uHeight, uMipLevels, uUsage, Format, Pool);
			return true;
		}
	}

	DEBUG_WARNING(hr);

	return false;
}

bool CTexture::Create(RESET pfnReset, zerO::UINT32 uUsage)
{
	m_pfnReset = pfnReset;

	UINT uWidth, uHeight;
	D3DFORMAT Format; 

	pfnReset(uWidth, uHeight, Format);

	return Create(uWidth, uHeight, 1, uUsage, Format, D3DPOOL_DEFAULT);
}

bool CTexture::Destroy()
{
	DEBUG_RELEASE(m_pTexture);

	m_pTexture = NULL;
	m_pfnReset = NULL;

	return true;
}

bool CTexture::Load(const PBASICCHAR pcFileName)
{
	HRESULT hr = D3DXCreateTextureFromFileEx(
		&DEVICE, 
		pcFileName, 
		D3DX_DEFAULT, 
		D3DX_DEFAULT,
		1, 
		0, 
		D3DFMT_UNKNOWN, 
		D3DPOOL_MANAGED, 
		D3DX_DEFAULT, 
		D3DX_DEFAULT, 
		0, 
		NULL, 
		NULL, 
		&m_pTexture);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	D3DSURFACE_DESC Desc;
	hr = m_pTexture->GetLevelDesc(0, &Desc);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	__Initialization(Desc.Width, Desc.Height, 1, 0, Desc.Format, Desc.Pool);

	return true;
}