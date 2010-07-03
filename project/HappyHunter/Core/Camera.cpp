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
	D3DXMatrixIdentity(&m_ProjectionMatrix);
	D3DXMatrixIdentity(&m_ViewProjectionMatrix);
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

	m_bIsTransformDirty = true;
}

void CCamera::UpdateTransform()
{
	CSceneNode::UpdateTransform();

	D3DXMatrixMultiply(&m_ViewProjectionMatrix, &m_WorldMatrix, &m_ProjectionMatrix);

	/*m_ViewProjectionMatrix._11 = 0.0f;
	m_ViewProjectionMatrix._12 = 0.0f;
	m_ViewProjectionMatrix._13 = 1.0f;
	m_ViewProjectionMatrix._14 = 1.0f;

	m_ViewProjectionMatrix._21 = 1.0f;
	m_ViewProjectionMatrix._22 = 0.0f;
	m_ViewProjectionMatrix._23 = 0.0f;
	m_ViewProjectionMatrix._24 = 0.0f;

	m_ViewProjectionMatrix._31 = 0.0f;
	m_ViewProjectionMatrix._32 = 0.0f;
	m_ViewProjectionMatrix._33 = 1.7f;
	m_ViewProjectionMatrix._34 = 0.0f;

	m_ViewProjectionMatrix._41 = 0.0f;
	m_ViewProjectionMatrix._42 = -439.0f;
	m_ViewProjectionMatrix._43 = -5.0f;
	m_ViewProjectionMatrix._44 = 0.0f;*/

	m_Frustum.ExtractFromMatrix(m_ViewProjectionMatrix);

	static D3DXVECTOR3 p0(-1.0f,1.0f,1.0f);
	static D3DXVECTOR3 p1(-1.0f,-1.0f,1.0f);
	static D3DXVECTOR3 p2(1.0f,-1.0f,1.0f);
	static D3DXVECTOR3 p3(1.0f,1.0f,1.0f);
	static D3DXVECTOR3 p4(-1.0f,1.0f,0.0f);
	static D3DXVECTOR3 p5(-1.0f,-1.0f,0.0f);
	static D3DXVECTOR3 p6(1.0f,-1.0f,0.0f);
	static D3DXVECTOR3 p7(1.0f,1.0f,0.0f);

	D3DXVec3TransformCoord(&m_FarPlanePoints[0], &p0, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[1], &p1, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[2], &p2, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[3], &p3, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[4], &p4, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[5], &p5, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[6], &p6, &m_ViewProjectionMatrix);
	D3DXVec3TransformCoord(&m_FarPlanePoints[7], &p7, &m_ViewProjectionMatrix);

	m_SearchRectangle.Set(
		m_WorldMatrix._41, m_WorldMatrix._41, 
		m_WorldMatrix._42, m_WorldMatrix._42, 
		m_WorldMatrix._43, m_WorldMatrix._43);

	m_SearchRectangle.Union(m_FarPlanePoints[0]);
	m_SearchRectangle.Union(m_FarPlanePoints[1]);
	m_SearchRectangle.Union(m_FarPlanePoints[2]);
	m_SearchRectangle.Union(m_FarPlanePoints[3]);
	m_SearchRectangle.Union(m_FarPlanePoints[4]);
	m_SearchRectangle.Union(m_FarPlanePoints[5]);
	m_SearchRectangle.Union(m_FarPlanePoints[6]);
	m_SearchRectangle.Union(m_FarPlanePoints[7]);
}
