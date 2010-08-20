#include "StdAfx.h"
#include "Snow.h"

using namespace zerO;

CSnow::CSnow(void) :
m_fSpeed(0.1f),
m_fGravity(0.06f),
m_fOffsetRadius(0),
m_uStep(0),
m_Acceleration(0.0f, 0.0f, 0.0f),
m_Source(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 1.0f)
{
}

CSnow::~CSnow(void)
{
}


///
// 雪的初始化
///
void InitParicleSnow(CParticleSystem<SNOWPARAMETERS>::LPPARTICLE pParticle)
{
	CSnow* pParent = (CSnow*)pParticle->pPARENT;

	pParticle->Parameter.bIsFree = false;

	pParticle->Parameter.Velocity = pParent->GetAcceleration();

	pParticle->Vertex.Color       = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pParticle->Vertex.Position    = pParent->GetSource();

	zerO::FLOAT fOffsetRadius     = pParent->GetOffsetRadius();

	if(fOffsetRadius)
	{
		switch(pParent->GetOffsetType())
		{
		case CSnow::RANDOM_CUBE :
			pParticle->Vertex.Position.x += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius;
			pParticle->Vertex.Position.z += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			break;

		case CSnow::RANDOM_CIRCLE :
			zerO::FLOAT fRandom = (zerO::FLOAT)rand() / RAND_MAX, fSin = sin(fRandom), fCos = cos(fRandom);
			pParticle->Vertex.Position.x += fOffsetRadius * fCos - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y += fOffsetRadius * fSin - fOffsetRadius * 0.5f;
			break;
		}
	}
}


////
// 粒子更新
////
void UpdateParticleSnow(CParticleSystem<SNOWPARAMETERS>::LPPARTICLE pParticle)
{
	CSnow* pParent = (CSnow*)pParticle->pPARENT;

	pParticle->Parameter.Velocity.y -= pParent->GetGravity();

	pParticle->Vertex.Position      += pParticle->Parameter.Velocity;
}

///
// 粒子销毁判断
///
bool IsParticleDestroySnow(CParticleSystem<SNOWPARAMETERS>::LPPARTICLE pParticle)
{
	return pParticle->Vertex.Position.y < 0 || pParticle->Parameter.bIsFree;
}

zerO::INT g_nSnowIndex;


zerO::UINT GetParticleRenderStepSnow(const CParticleSystem<SNOWPARAMETERS>::PARTICLE& Particle)
{
	return 1;
}

bool SetParticleRenderDataSnow(const CParticleSystem<SNOWPARAMETERS>::PARTICLE& Particle, CParticleSystem<SNOWPARAMETERS>::PARTICLEVERTEX& Vertex)
{
	Vertex.Position = Particle.Vertex.Position;
	Vertex.Color    = Particle.Vertex.Color;

	return true;
}



bool CSnow::Create(zerO::UINT uMaxNum, zerO::UINT uFlush, zerO::UINT uDiscard, zerO::FLOAT fSize)
{
	D3DCAPS9 Caps;

	DEVICE.GetDeviceCaps(&Caps);

	if(!zerO::CParticleSystem<SNOWPARAMETERS>::Create(
		0, uMaxNum, uFlush, uDiscard, fSize, /*4.0f*/Caps.MaxPointSize, 0.0f, 0.0f, 0.0f, 1.0f,
		InitParicleSnow, UpdateParticleSnow, IsParticleDestroySnow, GetParticleRenderStepSnow, SetParticleRenderDataSnow))
		return false;

	return true;
}

void CSnow::Update()
{
	__BuildAcceleration();

	CParticleSystem<SNOWPARAMETERS>::Update();

	m_uNumEmitedPerFrame = 0;
}

void CSnow::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, false            );
	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, true         );
	DEVICE.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA   );
	DEVICE.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	CParticleSystem::Render(pEntry, uFlag);

	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false        );
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE    , true         );
}