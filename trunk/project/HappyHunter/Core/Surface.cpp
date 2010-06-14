#include "StdAfx.h"
#include "Surface.h"

using namespace zerO;

CSurface::CSurface(void) :
m_uNumTextures(0),
m_TextureFlag(0)
{
	memset( &m_Material, 0, sizeof(m_Material) );
	memset( m_pTextures, 0, sizeof(m_pTextures) );
}

CSurface::~CSurface(void)
{
}