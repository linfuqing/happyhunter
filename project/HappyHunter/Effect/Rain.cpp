#include "StdAfx.h"
#include "Rain.h"

using namespace zerO;

CRain::CRain(void) :
m_fSpeed(0.0f),
m_fHight(0.0f),
m_fOffsetHight(0),
m_fGravity(0.06f),
m_fOffsetRadius(0),
m_uStep(0),
m_Acceleration(0.0f, 0.0f, 0.0f),
m_Source(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 1.0f)
{
}

CRain::~CRain(void)
{
}


///
// 雨的初始化
///
void InitParicleRain(CParticleSystem<RAINPARAMETERS>::LPPARTICLE pParticle)
{
	CRain* pParent = (CRain*)pParticle->pPARENT;

	pParticle->Parameter.bIsFree = false;

	pParticle->Parameter.Velocity = pParent->GetAcceleration();

	pParticle->Vertex.Color       = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);

	pParticle->Vertex.Position    = pParent->GetSource();

	zerO::FLOAT fHight			  = pParent->GetHight();

	zerO::UINT fOffsetHight      = pParent->GetOffsetHight();

	zerO::FLOAT fOffsetRadius     = pParent->GetOffsetRadius();

	if(fOffsetRadius)
	{
		switch(pParent->GetOffsetType())
		{
		case CRain::RANDOM_CUBE :
			pParticle->Vertex.Position.x += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y += ( rand() % fOffsetHight) + fHight;
			pParticle->Vertex.Position.z += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			break;

		case CRain::RANDOM_CIRCLE :
			zerO::FLOAT fRandom = (zerO::FLOAT)rand() / RAND_MAX, fSin = sin(fRandom), fCos = cos(fRandom);
			pParticle->Vertex.Position.x += fOffsetRadius * fCos - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.z += fOffsetRadius * fSin - fOffsetRadius * 0.5f;
			break;
		}
	}
}


////
// 粒子更新
////
void UpdateParticleRain(CParticleSystem<RAINPARAMETERS>::LPPARTICLE pParticle)
{
	CRain* pParent = (CRain*)pParticle->pPARENT;

	pParticle->Parameter.Velocity.y -= pParent->GetGravity();

	pParticle->Vertex.Position      += pParticle->Parameter.Velocity;
}

///
// 粒子销毁判断
///
bool IsParticleDestroyRain(CParticleSystem<RAINPARAMETERS>::LPPARTICLE pParticle)
{
	return pParticle->Vertex.Position.y < 0 || pParticle->Parameter.bIsFree;
}

zerO::INT g_nRainIndex;

///
// 粒子渲染前行为 渲染粒子的尾巴
// 返回:单个粒子渲染次数
///
zerO::UINT GetParticleRenderStepRain(const CParticleSystem<RAINPARAMETERS>::PARTICLE& Particle)
{
	return 1;
}

///
// 粒子渲染
///
bool SetParticleRenderDataRain(const CParticleSystem<RAINPARAMETERS>::PARTICLE& Particle, CParticleSystem<RAINPARAMETERS>::PARTICLEVERTEX& Vertex)
{
	Vertex.Position = Particle.Vertex.Position;
	Vertex.Color    = Particle.Vertex.Color;

	return true;
}


///
// 构建函数
// uMaxNum  弹夹的大小,每次最大能发射多少子弹
// uFlush   每次渲染的最大数量
// uDiscard Buffer的容量
// fSize    粒子的大小
// 返回     成功返回true, 失败返回false.
///
bool CRain::Create(zerO::UINT uMaxNum, zerO::UINT uFlush, zerO::UINT uDiscard, zerO::FLOAT fSize)
{
	D3DCAPS9 Caps;

	DEVICE.GetDeviceCaps(&Caps);

	if(!zerO::CParticleSystem<RAINPARAMETERS>::Create(
		0, uMaxNum, uFlush, uDiscard, fSize, /*2.0f*/Caps.MaxPointSize, 0.0f, 0.0f, 0.0f, 1.0f,
		InitParicleRain, UpdateParticleRain, IsParticleDestroyRain, GetParticleRenderStepRain, SetParticleRenderDataRain))
		return false;

	return true;
}

void CRain::Update()
{
	__BuildAcceleration();

	CParticleSystem<RAINPARAMETERS>::Update();

	m_uNumEmitedPerFrame = 0;
}

void CRain::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, false            );
	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, true         );
	DEVICE.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA   );
	DEVICE.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	CParticleSystem::Render(pEntry, uFlag);

	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false        );
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE    , true         );
}