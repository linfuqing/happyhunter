#include "StdAfx.h"
#include "Rectangle3D.h"

using namespace zerO;

CRectangle3D::CRectangle3D(void)
{
	m_MaxX = m_MinX = m_MaxY = m_MinY = m_MaxZ = m_MinZ = 0.0f;
}

CRectangle3D::~CRectangle3D(void)
{
}

CRectangle3D::TESTPLANESTATE CRectangle3D::Test(const D3DXPLANE& Plane)const
{
	D3DXVECTOR3 Max, Min;

	if(Plane.a > 0.0f)
	{
		Max.x = m_MaxX;
		Min.x = m_MinX;
	}
	else
	{
		Max.x = m_MinX;
		Min.x = m_MaxX;
	}

	if(Plane.b > 0.0f)
	{
		Max.y = m_MaxY;
		Min.y = m_MinY;
	}
	else
	{
		Max.y = m_MinY;
		Min.y = m_MaxY;
	}

	if(Plane.c > 0.0f)
	{
		Max.z = m_MaxZ;
		Min.z = m_MinZ;
	}
	else
	{
		Max.z = m_MinZ;
		Min.z = m_MaxZ;
	}

	zerO::FLOAT fMax = D3DXPlaneDotCoord(&Plane, &Max), fMin = D3DXPlaneDotCoord(&Plane, &Min);

	if(fMin == 0.0f || fMax * fMin < 0.0f)
		return PLANE_INTERSECT;
	else if(fMin > 0.0f)
		return PLANE_FRONT;

	return PLANE_BACK;
}

void CRectangle3D::Transform(const D3DXMATRIX& Matrix)
{
	D3DXVECTOR3 Min(m_MinX, m_MinY, m_MinZ),
		LegX(m_MaxX, m_MinY, m_MinZ),
		LegY(m_MinX, m_MaxY, m_MinZ),
		LegZ(m_MinX, m_MinY, m_MaxZ);

	D3DXVec3TransformCoord(&Min, &Min, &Matrix);
	D3DXVec3TransformCoord(&LegX, &LegX, &Matrix);
	D3DXVec3TransformCoord(&LegY, &LegY, &Matrix);
	D3DXVec3TransformCoord(&LegZ, &LegZ, &Matrix);

	LegX -= Min;
	LegY -= Min;
	LegZ -= Min;

	D3DXVECTOR3 Max(Min + LegX + LegY + LegZ);

	m_MaxX = Max.x;
	m_MaxY = Max.y;
	m_MaxZ = Max.z;

	m_MinX = Min.x;
	m_MinY = Min.y;
	m_MinZ = Min.z;

	Union(Min + LegX);
	Union(Min + LegY);
	Union(Min + LegZ);
	Union(Min + LegX + LegY);
	Union(Min + LegY + LegZ);
	Union(Min + LegZ + LegX);
}
