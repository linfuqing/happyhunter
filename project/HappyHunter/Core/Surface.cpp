#include "StdAfx.h"
#include "Surface.h"
#include "GameHost.h"

using namespace zerO;

CSurface::CSurface(void) :
CResource(RESOURCE_SURFACE),
m_uNumTextures(0),
m_TextureFlag(0)
{
	memset( &m_Material, 0, sizeof(m_Material) );
	memset( m_pTextures, 0, sizeof(m_pTextures) );
}

CSurface::~CSurface(void)
{
}

void CSurface::Activate()
{
     DEVICE.SetMaterial(&m_Material);

	 if(m_uNumTextures)
		 for(UINT i = 0; i < MAXINUM_TEXTURE_PER_SURFACE; i ++)
			if( TEST_BIT(m_TextureFlag, i) )
				DEVICE.SetTexture( i, m_pTextures[i]->GetTexture() );
}