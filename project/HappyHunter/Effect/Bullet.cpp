#include "DXUT.h"
#include "Game.h"
#include "Bullet.h"
#include <mmsystem.h>

using namespace zerO;

CBullet::CBullet(void) :
m_fSpeed(0.0f),
m_fGravity(0.0f),
m_fOffsetRadius(0.0f),
m_fAngle(0.0f),

m_uStep(0),
m_uLength(0),

m_Acceleration(0.0f, 0.0f, 0.0f),
m_Source(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 1.0f),

bIsAccelerationDirty(false),

m_OffsetType(RANDOM_NONE)
{
}

CBullet::~CBullet(void)
{
}

///
// ���ӳ�ʼ��
///
void InitParticle(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	CBullet* pParent = (CBullet*)pParticle->pPARENT;

	pParticle->Parameter.bIsFree = false;

	pParticle->Parameter.Velocity = pParent->GetAcceleration();

	pParticle->Vertex.Color       = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pParticle->Vertex.Position    = pParent->GetSource();

	zerO::FLOAT fOffsetRadius     = pParent->GetOffsetRadius();
	zerO::FLOAT fAngle			  = pParent->GetAngle();

	if(fOffsetRadius)
	{
		switch( pParent->GetOffsetType() )
		{
		case CBullet::RANDOM_NONE:
			break;

		case CBullet::RANDOM_CUBE:
			pParticle->Vertex.Position.x += RangedRand(0.0f, RAND_MAX) * fOffsetRadius - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y += RangedRand(0.0f, RAND_MAX) * fOffsetRadius - fOffsetRadius * 0.5f;
			break;

		case CBullet::RANDOM_CIRCLE:
			if ((fAngle += USER_PI / 3.0f) > USER_PI * 2.0f)
				fAngle = 0.0f;
			pParent->SetAngle(fAngle);
			zerO::FLOAT fSin = sin(fAngle), fCos = cos(fAngle);
			pParticle->Vertex.Position.x += fOffsetRadius * fCos - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y += fOffsetRadius * fSin;
			break;
		}
	}

	pParticle->Parameter.Rectangle.Extract( pParticle->Vertex.Position, pParent->GetSize() );
}

///
// ���Ӹ���
///
void UpdateParticle(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	CBullet* pParent = (CBullet*)pParticle->pPARENT;

	pParticle->Parameter.Velocity.y -= pParent->GetGravity();

	pParticle->Vertex.Position      += pParticle->Parameter.Velocity;

	pParticle->Parameter.Rectangle.Extract( pParticle->Vertex.Position, pParent->GetSize() );
}

///
// ���������ж�
///
bool IsParticleDestroy(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	return !pParticle->Parameter.Rectangle.TestHit( CAMERA.GetWorldRectangle() ) || pParticle->Parameter.bIsFree;//pParticle->Vertex.Position.y < - 100.0f;
}

D3DXVECTOR3 g_Position;
D3DXVECTOR3 g_Velocity;

///
// ������Ⱦǰ��Ϊ
// ����:����������Ⱦ����
///
zerO::UINT GetParticleRenderSteps(const CParticleSystem<BULLETPARAMETERS>::PARTICLE& Particle)
{
	CBullet* pParent = (CBullet*)Particle.pPARENT;

	g_Position = Particle.Vertex.Position;
	g_Velocity = Particle.Parameter.Velocity / (zerO::FLOAT)pParent->GetStep() * (pParent->GetLength() + 1.0f);

	return pParent->GetStep();
}

///
// ������Ⱦ
///
bool SetParticleRenderData(const CParticleSystem<BULLETPARAMETERS>::PARTICLE& Particle, CParticleSystem<BULLETPARAMETERS>::PARTICLEVERTEX& Vertex)
{
	Vertex.Position = g_Position;
	Vertex.Color    = Particle.Vertex.Color;

	g_Position     -= g_Velocity;

	return true;
}

CParticleSystem<BULLETPARAMETERS>::LPPARTICLE CBullet::FindHitParticle(const CRectangle3D Rect)const
{
	PARTICLENODE* pParticle = m_pParticles;

	while(pParticle)
	{
		if( pParticle->Particle.Parameter.Rectangle.TestHit(Rect) )
			return &pParticle->Particle;

		pParticle = pParticle->pNext;
	}

	return NULL;
}

bool CBullet::FreeHitParticle(const CRectangle3D Rect)const
{
	bool bResult = false;

	PARTICLENODE* pParticle = m_pParticles;

	while(pParticle)
	{
		if( pParticle->Particle.Parameter.Rectangle.TestHit(Rect) )
		{
			pParticle->Particle.Parameter.bIsFree = true;

			bResult = true;
		}

		pParticle = pParticle->pNext;
	}

	return bResult;
}

///
// ��������
// uMaxNum  ���еĴ�С,ÿ������ܷ�������ӵ�
// uFlush   ÿ����Ⱦ���������
// uDiscard Buffer������
// fSize    ���ӵĴ�С
// ����     �ɹ�����true, ʧ�ܷ���false.
///
bool CBullet::Create(zerO::UINT uMaxNum, zerO::UINT uFlush, zerO::UINT uDiscard, zerO::FLOAT fSize)
{
	D3DCAPS9 Caps;

	DEVICE.GetDeviceCaps(&Caps);

	if( !zerO::CParticleSystem<BULLETPARAMETERS>::Create(
		0, uMaxNum, uFlush, uDiscard, fSize, Caps.MaxPointSize, 0.0f, 0.0f, 0.0f, 1.0f, 
		InitParticle, UpdateParticle, IsParticleDestroy, GetParticleRenderSteps, SetParticleRenderData) )
		return false;

	 return true;
}

void CBullet::Update()
{
	if(bIsAccelerationDirty)
	{
		__BuildAcceleration();

		bIsAccelerationDirty = false;
	}

	CParticleSystem<BULLETPARAMETERS>::Update();

	m_uNumEmitedPerFrame = 0;
}

void CBullet::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, false            );
	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, true         );
	DEVICE.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA   );
	DEVICE.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	CParticleSystem::Render(pEntry, uFlag);

	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false        );
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE    , true         );
}