#include "StdAfx.h"
#include "Camera.h"

using namespace zerO;

void FRUSTUM::ExtractFromMatrix(const D3DXMATRIX& Matrix)
{
	Left.a   = Matrix._14 + Matrix._11;
	Left.b   = Matrix._24 + Matrix._21;
	Left.c   = Matrix._34 + Matrix._31;
	Left.d   = Matrix._44 + Matrix._41;

	Right.a  = Matrix._14 - Matrix._11;
	Right.b  = Matrix._24 - Matrix._21;
	Right.c  = Matrix._34 - Matrix._31;
	Right.d  = Matrix._44 - Matrix._41;

	Top.a    = Matrix._14 - Matrix._12;
	Top.b    = Matrix._24 - Matrix._22;
	Top.c    = Matrix._34 - Matrix._32;
	Top.d    = Matrix._44 - Matrix._42;

	Bottom.a = Matrix._14 + Matrix._12;
	Bottom.b = Matrix._24 + Matrix._22;
	Bottom.c = Matrix._34 + Matrix._32;
	Bottom.d = Matrix._44 + Matrix._42;

	Far.a    = Matrix._14 - Matrix._13;
	Far.b    = Matrix._24 - Matrix._23;
	Far.c    = Matrix._34 - Matrix._33;
	Far.d    = Matrix._44 - Matrix._43;

	Near.a   = Matrix._13;
	Near.b   = Matrix._23;
	Near.c   = Matrix._33;
	Near.d   = Matrix._43;
}

CCamera::CCamera(void)
{
}

CCamera::~CCamera(void)
{
}

void CCamera::SetProjection(
							zerO::FLOAT fFOV, 
							zerO::FLOAT fAspect, 
							zerO::FLOAT fNearPlane,
							zerO::FLOAT fFarPlane)
{
	D3DXMatrixPerspectiveFovLH(&m_ProjectionMatrix, fFOV, fAspect, fNearPlane, fFarPlane);
}

void CCamera::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	D3DXMatrixMultiply(&m_ViewProjectionMatrix, &m_InverseWorldMatrix, &m_ProjectionMatrix);

	m_Frustum.ExtractFromMatrix(m_ViewProjectionMatrix);
}
