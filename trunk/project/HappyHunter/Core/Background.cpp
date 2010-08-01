#include "StdAfx.h"
#include "Background.h"
#include "Camera.h"

using namespace zerO;

CBackground::CBackground(void)
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

	m_Matrix._41 = Position.x;
	m_Matrix._42 = Position.y;
	m_Matrix._43 = Position.z;
}