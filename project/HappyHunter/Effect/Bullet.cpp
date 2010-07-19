#include "StdAfx.h"
#include "Bullet.h"

using namespace zerO;

CBullet::CBullet(void) :
m_fSpeed(0.0f),
m_fGravity(0.0f),
m_fOffsetRadius(0),

m_uStep(0),
m_uLength(0),

m_Acceleration(0.0f, 0.0f, 0.0f),
m_Source(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 1.0f),

bIsAccelerationDirty(false)
{
}

CBullet::~CBullet(void)
{
}

///
// 粒子初始化
///
void InitParticle(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	CBullet* pParent = (CBullet*)pParticle->pPARENT;

	pParticle->Parameter.bIsFree = false;

	pParticle->Parameter.Velocity = pParent->GetAcceleration();

	pParticle->Vertex.Color       = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pParticle->Vertex.Position    = pParent->GetSource();

	zerO::FLOAT fOffsetRadius      = pParent->GetOffsetRadius();

	if(fOffsetRadius)
	{
		pParticle->Vertex.Position.x += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
		pParticle->Vertex.Position.y += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
		pParticle->Vertex.Position.z += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
	}

	pParticle->Parameter.Rectangle.Extract( pParticle->Vertex.Position, pParent->GetSize() );
}

///
// 粒子更新
///
void UpdateParticle(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	CBullet* pParent = (CBullet*)pParticle->pPARENT;

	pParticle->Parameter.Velocity.y -= pParent->GetGravity();

	pParticle->Vertex.Position      += pParticle->Parameter.Velocity;

	pParticle->Parameter.Rectangle.Extract( pParticle->Vertex.Position, pParent->GetSize() );
}

///
// 粒子销毁判断
///
bool IsParticleDestroy(CParticleSystem<BULLETPARAMETERS>::LPPARTICLE pParticle)
{
	return !pParticle->Parameter.Rectangle.TestHit( CAMERA.GetWorldRectangle() ) || pParticle->Parameter.bIsFree;//pParticle->Vertex.Position.y < - 100.0f;
}

D3DXVECTOR3 g_Position;
D3DXVECTOR3 g_Velocity;

///
// 粒子渲染前行为
// 返回:单个粒子渲染次数
///
zerO::UINT GetParticleRenderSteps(const CParticleSystem<BULLETPARAMETERS>::PARTICLE& Particle)
{
	CBullet* pParent = (CBullet*)Particle.pPARENT;

	g_Position = Particle.Vertex.Position;
	g_Velocity = Particle.Parameter.Velocity / (zerO::FLOAT)pParent->GetStep() * (pParent->GetLength() + 1.0f);

	return pParent->GetStep();
}

///
// 粒子渲染
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

///
// 构建函数
// uMaxNum  弹夹的大小,每次最大能发射多少子弹
// uFlush   每次渲染的最大数量
// uDiscard Buffer的容量
// fSize    粒子的大小
// 返回     成功返回true, 失败返回false.
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