#include "StdAfx.h"
#include "Sprite.h"

using namespace zerO;

CSprite::CSprite(void) :
m_Position(0.0f, 0.0f, 0.0f),
m_Rotation(0.0f, 0.0f, 0.0f),
m_Scale(0.0f, 0.0f, 0.0f),
m_uDirtyFlag(0)
{
}

CSprite::~CSprite(void)
{
}

void CSprite::Clone(CSprite& Sprite)const
{
	CSceneNode::Clone(Sprite);

	Sprite.m_Position   = m_Position;
	Sprite.m_Rotation   = m_Rotation;
	Sprite.m_Scale      = m_Scale;
	Sprite.m_uDirtyFlag = m_uDirtyFlag;
}

void CSprite::SetDirection(const D3DXVECTOR3& Direction)
{
	D3DXVECTOR3 Rotation(0.0f, 0.0f, m_Rotation.z);

	if( Direction.x == 0 && Direction.z == 0 && Direction.y == 0 )
	{
		Rotation.x = 0;
		Rotation.y = 0;
	}
	else if( Direction.x == 0 && Direction.z == 0  )
	{
		Rotation.x = - asin( Direction.y / D3DXVec3Length(&Direction) );
		Rotation.y =   0;
	}
	else if( Direction.z < 0 )
	{
		Rotation.x = - asin( Direction.y / D3DXVec3Length(&Direction) );
		Rotation.y =   D3DX_PI - asin( Direction.x / sqrt(Direction.x * Direction.x + Direction.z * Direction.z) );
	}
	else
	{
		Rotation.x = - asin( Direction.y / D3DXVec3Length(&Direction) );
		Rotation.y =   asin( Direction.x / sqrt(Direction.x * Direction.x + Direction.z * Direction.z) );
	}

	SetRotation(Rotation);
}

void CSprite::Update()
{
	if(m_uDirtyFlag)
	{
		D3DXMATRIX Matrix;

		D3DXMatrixIdentity(&m_LocalMatrix);

		if( TEST_BIT(m_uDirtyFlag, ROTATION) )
		{
			D3DXQuaternionRotationYawPitchRoll(&m_Quaternion, m_Rotation.y, m_Rotation.x, m_Rotation.z);

			D3DXMatrixRotationQuaternion(&Matrix, &m_Quaternion);

			m_LocalMatrix *= Matrix;
		}

		if( TEST_BIT(m_uDirtyFlag, SCALE) )
		{
			D3DXMatrixScaling(&Matrix, m_Scale.x, m_Scale.y, m_Scale.z);

			m_LocalMatrix *= Matrix;
		}

		if( TEST_BIT(m_uDirtyFlag, POSITION) )
		{
			D3DXMatrixTranslation(&Matrix, m_Position.x, m_Position.y, m_Position.z);

			m_LocalMatrix *= Matrix;
		}

		m_bIsTransformDirty = true;
	}

	CSceneNode::Update();
}

void CSprite::Right(zerO::FLOAT fNumSteps)
{
	D3DXVECTOR3 Right(m_LocalMatrix._11, m_LocalMatrix._12, m_LocalMatrix._13);

	if(m_LocalMatrix._14)
		Right /= m_LocalMatrix._14;

	D3DXVec3Normalize(&Right, &Right);

	Right *= fNumSteps;

	m_Position += Right;

	SET_BIT(m_uDirtyFlag, POSITION);
}

void CSprite::Up(zerO::FLOAT fNumSteps)
{
	D3DXVECTOR3 Up(m_LocalMatrix._21, m_LocalMatrix._22, m_LocalMatrix._23);

	if(m_LocalMatrix._24)
		Up /= m_LocalMatrix._24;

	D3DXVec3Normalize(&Up, &Up);

	Up *= fNumSteps;

	m_Position += Up;

	SET_BIT(m_uDirtyFlag, POSITION);
}

void CSprite::Forward(zerO::FLOAT fNumSteps)
{
	D3DXVECTOR3 Forward(m_LocalMatrix._31, m_LocalMatrix._32, m_LocalMatrix._33);

	if(m_LocalMatrix._34)
		Forward /= m_LocalMatrix._34;

	D3DXVec3Normalize(&Forward, &Forward);

	Forward *= fNumSteps;

	m_Position += Forward;

	SET_BIT(m_uDirtyFlag, POSITION);
}