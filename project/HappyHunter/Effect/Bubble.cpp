#include "StdAfx.h"
#include "Bubble.h"

using namespace zerO;

CBubble::CBubble(void) :
m_fSpeed(0.0f),
m_fHight(0.0f),
m_fOffsetHight(0),
m_fFlotage(0.06f),
m_fOffsetRadius(0),
m_uStep(0),
m_Acceleration(0.0f, 0.0f, 0.0f),
m_Source(0.0f, 0.0f, 0.0f),
m_Direction(0.0f, 0.0f, 1.0f)
{
}

CBubble::~CBubble(void)
{
}


///
// ���ݵĳ�ʼ��
///
void InitParicleBubble(CParticleSystem<BUBBLEPARAMETERS>::LPPARTICLE pParticle)
{
	CBubble* pParent = (CBubble*)pParticle->pPARENT;

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
		case CBubble::RANDOM_CUBE :
			pParticle->Vertex.Position.x += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.y  = 0;
			pParticle->Vertex.Position.z += ( (zerO::FLOAT)rand() ) / RAND_MAX * fOffsetRadius - fOffsetRadius * 0.5f;
			break;

		case CBubble::RANDOM_CIRCLE :
			zerO::FLOAT fRandom = (zerO::FLOAT)rand() / RAND_MAX, fSin = sin(fRandom), fCos = cos(fRandom);
			pParticle->Vertex.Position.x += fOffsetRadius * fCos - fOffsetRadius * 0.5f;
			pParticle->Vertex.Position.z += fOffsetRadius * fSin - fOffsetRadius * 0.5f;
			break;
		}
	}
}


////
// ���Ӹ���
////
void UpdateParticleBubble(CParticleSystem<BUBBLEPARAMETERS>::LPPARTICLE pParticle)
{
	CBubble* pParent = (CBubble*)pParticle->pPARENT;

	pParticle->Parameter.Velocity.y += pParent->GetFlotage();

	//pParticle->Parameter.Velocity.x += (zerO::FLOAT)(rand()  % 20 - 10 ) / 100; 

	//pParticle->Parameter.Velocity.z += (zerO::FLOAT)(rand()  % 20 - 10 ) / 100; 

	pParticle->Vertex.Position      += pParticle->Parameter.Velocity;
}

///
// ���������ж�
///
bool IsParticleDestroyBubble(CParticleSystem<BUBBLEPARAMETERS>::LPPARTICLE pParticle)
{
	return pParticle->Vertex.Position.y > 2000 || pParticle->Parameter.bIsFree;
}

zerO::INT g_nBubbleIndex;

///
// ������Ⱦǰ��Ϊ ��Ⱦ���ӵ�β��
// ����:����������Ⱦ����
///
zerO::UINT GetParticleRenderStepBubble(const CParticleSystem<BUBBLEPARAMETERS>::PARTICLE& Particle)
{
	return 1;
}

///
// ������Ⱦ
///
bool SetParticleRenderDataBubble(const CParticleSystem<BUBBLEPARAMETERS>::PARTICLE& Particle, CParticleSystem<BUBBLEPARAMETERS>::PARTICLEVERTEX& Vertex)
{
	Vertex.Position = Particle.Vertex.Position;
	Vertex.Color    = Particle.Vertex.Color;

	return true;
}


///
// ��������
// uMaxNum  ���еĴ�С,ÿ������ܷ�������ӵ�
// uFlush   ÿ����Ⱦ���������
// uDiscard Buffer������
// fSize    ���ӵĴ�С
// ����     �ɹ�����true, ʧ�ܷ���false.
///
bool CBubble::Create(zerO::UINT uMaxNum, zerO::UINT uFlush, zerO::UINT uDiscard, zerO::FLOAT fSize)
{
	D3DCAPS9 Caps;

	DEVICE.GetDeviceCaps(&Caps);

	if(!zerO::CParticleSystem<BUBBLEPARAMETERS>::Create(
		0, uMaxNum, uFlush, uDiscard, fSize, /*2.0f*/Caps.MaxPointSize, 0.0f, 0.0f, 0.0f, 1.0f,
		InitParicleBubble, UpdateParticleBubble, IsParticleDestroyBubble, GetParticleRenderStepBubble, SetParticleRenderDataBubble))
		return false;

	return true;
}

void CBubble::Update()
{
	__BuildAcceleration();

	CParticleSystem<BUBBLEPARAMETERS>::Update();

	m_uNumEmitedPerFrame = 0;
}

void CBubble::Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
{
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, false            );
	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, true         );
	DEVICE.SetRenderState(D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA   );
	DEVICE.SetRenderState(D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA);

	CParticleSystem::Render(pEntry, uFlag);

	DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false        );
	DEVICE.SetRenderState(D3DRS_ZWRITEENABLE    , true         );
}