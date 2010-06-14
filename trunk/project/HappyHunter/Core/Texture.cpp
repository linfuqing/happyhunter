#include "StdAfx.h"
#include "Texture.h"

using namespace zerO;

CTexture::CTexture(void) :
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

	HRESULT hr = 
	D3DXCheckTextureRequirements(          
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
	return SUCCEEDED( D3DXCreateTextureFromFile(&DEVICE, pcFileName, &m_pTexture) );
}