#include "StdAfx.h"
#include "Background.h"
#include "Camera.h"

using namespace zerO;

CBackground::CBackground(void) :
m_bFogEnable(false)
{
	GAMEHOST.SetBackground(this);
	D3DXMatrixIdentity(&m_Matrix);
}

CBackground::~CBackground(void)
{
}


void CBackground::Update()
{
	D3DXVECTOR3 Position = CAMERA.GetWorldPosition();

	FLOAT fScale = CAMERA.GetFarPlane();

	m_Matrix._11 = fScale;
	m_Matrix._22 = fScale;
	m_Matrix._33 = fScale;
	
	m_Matrix._41 = Position.x;
	m_Matrix._42 = Position.y;
	m_Matrix._43 = Position.z;
}