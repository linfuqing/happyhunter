#include "StdAfx.h"
#include "Texture.h"

using namespace zerO;

CTexture::CTexture(void) :
CResource(RESOURCE_TEXTURE),
m_pTexture(NULL),
m_pMatrix(NULL)
{
}

CTexture::~CTexture(void)
{
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
		HRESULT hr = D3DXCreateTexture(
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

	return false;
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