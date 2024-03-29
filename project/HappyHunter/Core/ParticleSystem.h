#pragma once

#include "Surface.h"
#include "VertexBuffer.h"
#include "SceneNode.h"

namespace zerO
{
	template<typename T>
	class CParticleSystem :
		public CSceneNode
	{
	public:
		typedef T PARTICLEPARAMETERS, * LPPARTICLEPARAMETERS;

		typedef struct
		{
			D3DXVECTOR3 Position;
			D3DCOLOR    Color;
		}PARTICLEVERTEX, * LPPARTICLEVERTEX;

		typedef struct PARTICLE
		{
			PARTICLEPARAMETERS Parameter;
			PARTICLEVERTEX     Vertex;
			CParticleSystem* const pPARENT;

			PARTICLE(CParticleSystem* const pParent) :
			pPARENT(pParent)
			{
			}
		}PARTICLE, * LPPARTICLE;

		//Update
		typedef void (*HANDLEPARTICLE)(LPPARTICLE);
		typedef bool (*CHECKPARTICLE)(LPPARTICLE);

		//Render
		typedef UINT (*GETPARTICLEDATA)(const PARTICLE&);
		typedef bool (*SETPARTICLEDATA)(const PARTICLE&, PARTICLEVERTEX&);

		CParticleSystem(void);

		~CParticleSystem(void);

		bool Destroy();

		CSurface& GetSurface();

		void SetNumberEmitedPerFrame(UINT uValue);

		bool Create(
			UINT uNumEmitedPerFrame,
			UINT uMaxNumParticles,
			UINT uFlush, 
			UINT uDiscard, 
			FLOAT fPointSize,
			FLOAT fPointSizeMax,
			FLOAT fPointSizeMin,
			FLOAT fPointScaleA,
			FLOAT fPointScaleB,
			FLOAT fPointScaleC,
			HANDLEPARTICLE pfnInit, 
			HANDLEPARTICLE pfnUpdate, 
			CHECKPARTICLE pfnIsDestroy,
			GETPARTICLEDATA pfnGetSteps,
			SETPARTICLEDATA pfnSetRenderData);

		void Update();

		bool ApplyForRender();

		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag);

	protected:
		typedef struct PARTICLENODE
		{
			PARTICLE      Particle;
			PARTICLENODE* pNext;
			PARTICLENODE* pRear;

			PARTICLENODE(CParticleSystem* const pParent) :
			Particle(pParent),
				pNext(NULL),
				pRear(NULL)
			{
			}
		}PARTICLENODE, * LPPARTICLENODE;

		UINT m_uNumEmitedPerFrame;

		UINT m_uNumParticles;      //当前链表中粒子的数量
		UINT m_uMaxNumParticles;   //链表中粒子的最大数量

		FLOAT m_fPointSize;
		FLOAT m_fPointSizeMax;
		FLOAT m_fPointSizeMin;
		FLOAT m_fPointScaleA;
		FLOAT m_fPointScaleB;
		FLOAT m_fPointScaleC;

		LPPARTICLENODE m_pParticles;    //当前粒子链表
		LPPARTICLENODE m_pTail;
		LPPARTICLENODE m_pParticlesFree;

		CVertexBuffer m_VertexBuffer;

		UINT  m_uBase;       //每次填充顶点缓冲区时的起始位置:m_uBase += m_uFlush;     
		UINT  m_uFlush;      //一次填充顶点缓冲区的粒子数量
		UINT  m_uDiscard;    //顶点缓冲区能够容纳的最大粒子数量

		CSurface m_Surface;

		HANDLEPARTICLE m_pfnInit;
		HANDLEPARTICLE m_pfnUpdate;
		CHECKPARTICLE  m_pfnIsDestroy;

		GETPARTICLEDATA m_pfnGetSteps;
		SETPARTICLEDATA m_pfnSetRenderData;
	};

	template<typename T>
	inline CSurface& CParticleSystem<T>::GetSurface()
	{
		return m_Surface;
	}

	template<typename T>
	inline void CParticleSystem<T>::SetNumberEmitedPerFrame(UINT uValue)
	{
		m_uNumEmitedPerFrame = uValue;
	}

	
	template<typename T>
	CParticleSystem<T>::CParticleSystem() :
	m_uNumEmitedPerFrame(0),
	m_uNumParticles(0),
	m_uMaxNumParticles(0),
	m_uBase(0),
	m_uFlush(0),
	m_fPointSize(0),
	m_fPointSizeMax(0),
	m_fPointSizeMin(0),
	m_fPointScaleA(0),
	m_fPointScaleB(0),
	m_fPointScaleC(0),
	m_uDiscard(0),
	m_pfnInit(NULL),
	m_pfnUpdate(NULL),
	m_pfnIsDestroy(NULL),
	m_pfnGetSteps(NULL),
	m_pfnSetRenderData(NULL),
	m_pParticles(NULL),
	m_pTail(NULL),
	m_pParticlesFree(NULL)
	{
	}

	template<typename T>
	CParticleSystem<T>::~CParticleSystem(void)
	{
		Destroy();
	}

	template<typename T>
	bool CParticleSystem<T>::Create(
								 zerO::UINT uNumEmitedPerFrame,
								 zerO::UINT uMaxNumParticles,
								 zerO::UINT uFlush, 
								 zerO::UINT uDiscard, 
								 zerO::FLOAT fPointSize,
								 zerO::FLOAT fPointSizeMax,
								 zerO::FLOAT fPointSizeMin,
								 zerO::FLOAT fPointScaleA,
								 zerO::FLOAT fPointScaleB,
								 zerO::FLOAT fPointScaleC,
								 HANDLEPARTICLE pfnInit, 
								 HANDLEPARTICLE pfnUpdate, 
								 CHECKPARTICLE pfnIsDestroy,
								 GETPARTICLEDATA pfnGetSteps,
								 SETPARTICLEDATA pfnSetRenderData)
	{
		m_uNumEmitedPerFrame = uNumEmitedPerFrame;
		m_uMaxNumParticles   = uMaxNumParticles;
		m_uBase              = uDiscard;
		m_uFlush             = uFlush;
		m_uDiscard           = uDiscard;
		m_fPointSize         = fPointSize;
		m_fPointSizeMax      = fPointSizeMax;
		m_fPointSizeMin      = fPointSizeMin;
		m_fPointScaleA       = fPointScaleA;
		m_fPointScaleB       = fPointScaleB;
		m_fPointScaleC       = fPointScaleC;
		m_pfnInit            = pfnInit;
		m_pfnUpdate          = pfnUpdate;
		m_pfnIsDestroy       = pfnIsDestroy;
		m_pfnGetSteps        = pfnGetSteps;
		m_pfnSetRenderData   = pfnSetRenderData;

		return m_VertexBuffer.Create(
			m_uDiscard, 
			sizeof(PARTICLEVERTEX), 
			D3DUSAGE_DYNAMIC | D3DUSAGE_WRITEONLY | D3DUSAGE_POINTS, 
			D3DPOOL_DEFAULT, 
			NULL, 
			D3DFVF_XYZ | D3DFVF_DIFFUSE);
	}

	template<typename T>
	bool CParticleSystem<T>::Destroy()
	{
		while(m_pParticles)
		{
			PARTICLENODE* pParticle = m_pParticles;
			m_pParticles = pParticle->pNext;
			DEBUG_DELETE(pParticle);
		}

		while(m_pParticlesFree)
		{
			PARTICLENODE *pParticle = m_pParticlesFree;
			m_pParticlesFree = pParticle->pNext;
			DEBUG_DELETE(pParticle);
		}

		return true;
	}

	template<typename T>
	void CParticleSystem<T>::Update()
	{
		CSceneNode::Update();

		LPPARTICLENODE pParticle, *ppParticle;

		//更新已存在粒子的属性值
		ppParticle = &m_pParticles;   //粒子链表
		while( *ppParticle )
		{
			pParticle = *ppParticle;  //取出当前粒子

			//更新粒子
			m_pfnUpdate(&pParticle->Particle);

			//删除死亡粒子
			if( m_pfnIsDestroy(&pParticle->Particle) )
			{
				if(pParticle->pRear)
					pParticle->pRear->pNext = pParticle->pNext;

				if(pParticle->pNext)
					pParticle->pNext->pRear = pParticle->pRear;

				if(pParticle == m_pParticles)
					m_pParticles = pParticle->pNext;

				if(pParticle == m_pTail)
					m_pTail = pParticle->pRear;

				if(pParticle == m_pTail)
					m_pTail = pParticle->pRear;

				*ppParticle = pParticle->pNext;
				pParticle->pNext = m_pParticlesFree;
				pParticle->pRear = NULL;

				if(m_pParticlesFree)
					m_pParticlesFree->pRear = pParticle;

				m_pParticlesFree = pParticle;
				m_uNumParticles--;
			}
			else  //准备处理下一个粒子
				ppParticle = &pParticle->pNext;
		}

		//添加新粒子
		UINT uEmited = 0;
		while( m_uNumParticles < m_uMaxNumParticles && uEmited < m_uNumEmitedPerFrame)
		{
			if( m_pParticlesFree )
			{
				pParticle        = m_pParticlesFree;
				m_pParticlesFree = pParticle->pNext;
				pParticle->pNext = NULL;
			}
			else
			{
				DEBUG_NEW( pParticle, PARTICLENODE(this) );
			}

			if(!m_pParticles)
				m_pParticles = pParticle;

			pParticle->pRear = m_pTail;
			
			if(m_pTail)
				m_pTail->pNext = pParticle;

			m_pTail = pParticle;

			/*pParticle->pNext = m_pParticles;
			m_pParticles = pParticle;*/
			m_uNumParticles++;
			uEmited++;

			m_pfnInit(&pParticle->Particle);
		}
	}

	template<typename T>
	bool CParticleSystem<T>::ApplyForRender()
	{
		if( !(m_pfnInit && m_pfnUpdate && m_pfnIsDestroy && m_pfnGetSteps && m_pfnSetRenderData) )
		{
			DEBUG_WARNING("Particle System had be created.");

			return false;
		}

		CRenderQueue::LPRENDERENTRY pEntry = RENDERQUEUE.LockRenderEntry();
		pEntry->uModelType = CRenderQueue::RENDERENTRY::PARTICLE_TYPE;
		pEntry->hModel     = m_VertexBuffer.GetHandle();
		pEntry->hSurface   = m_Surface.GetHandle();
		pEntry->pParent    = this;
		RENDERQUEUE.UnLockRenderEntry(pEntry);

		return true;
	}

	inline DWORD FtoDW( zerO::FLOAT f ) { return *((DWORD*)&f); }

	template<typename T>
	void CParticleSystem<T>::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
	{
		DEBUG_ASSERT(m_pfnInit && m_pfnUpdate && m_pfnIsDestroy && m_pfnGetSteps && m_pfnSetRenderData, "Particle System had be created.");

		HRESULT hr;
		DEVICE.SetVertexShader(NULL);
		DEVICE.SetPixelShader(NULL);

		//DEVICE.SetRenderState( D3DRS_ZWRITEENABLE, false );     //禁用深度缓冲区操作
		//DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, true );  //启用Alpha 混合
		//DEVICE.SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );  //注意Alpha混合方式
		//DEVICE.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		DEVICE.SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );     //使用点精灵
		DEVICE.SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
		{
			//为使用点精灵设置相关渲染状态
			DEVICE.SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_fPointSize) );  //粒子大小
			DEVICE.SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(m_fPointSizeMax) );
			DEVICE.SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(m_fPointSizeMin) ); //点的最小尺寸
			DEVICE.SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(m_fPointScaleA) );
			DEVICE.SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(m_fPointScaleB) );
			DEVICE.SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(m_fPointScaleC) );

			DEVICE.SetTransform( D3DTS_WORLD,      &m_WorldMatrix); 
			DEVICE.SetTransform( D3DTS_VIEW,       &CAMERA.GetViewMatrix() );
			DEVICE.SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );

			m_VertexBuffer.Activate(0, 0, true);
		}

		if( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			m_Surface.Activate();

		LPPARTICLENODE   pParticle = m_pParticles;
		LPPARTICLEVERTEX pVertices;
		UINT             uNumParticlesToRender = 0;//, uParticleVertexSize = sizeof(PARTICLEVERTEX);

		m_uBase += m_uFlush;

		if(m_uBase >= m_uDiscard)
			m_uBase = 0;

		/*hr = m_VertexBuffer.GetBuffer()->Lock(
			m_uBase * uParticleVertexSize,
			m_uFlush * uParticleVertexSize,
			(void**) &pVertices, 
			m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DEBUG_ASSERT(SUCCEEDED(hr), hr);*/

		if( !m_VertexBuffer.Lock(m_uBase, m_uFlush, m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD, (void**)&pVertices) )
			return;

		//渲染粒子
		while( pParticle )
		{
			UINT uSteps = m_pfnGetSteps(pParticle->Particle);

			//通过对其在不同位置渲染多次来实现模糊效果
			for( UINT i = 0; i < uSteps; i++ )
			{
				if( !m_pfnSetRenderData(pParticle->Particle, *pVertices) )
					continue;

				pVertices ++;

				if( ++ uNumParticlesToRender == m_uFlush )  //填充完毕指定的数据块
				{
				   /*hr = m_VertexBuffer.GetBuffer()->Unlock();

				   DEBUG_ASSERT(SUCCEEDED(hr), hr);*/

					if( !m_VertexBuffer.Unlock() )
						return;

				   hr = DEVICE.DrawPrimitive(D3DPT_POINTLIST, m_uBase, uNumParticlesToRender);

				   DEBUG_ASSERT(SUCCEEDED(hr), hr);

					m_uBase += m_uFlush;
					if(m_uBase >= m_uDiscard)
						m_uBase = 0;

					/*hr = m_VertexBuffer.GetBuffer()->Lock(
						m_uBase * uParticleVertexSize, 
						m_uFlush * uParticleVertexSize, 
						(void**)&pVertices, 
						m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					DEBUG_ASSERT(SUCCEEDED(hr), hr);*/

					if( !m_VertexBuffer.Lock(m_uBase, m_uFlush, m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD, (void**)&pVertices) )
						return;

					uNumParticlesToRender = 0;  //将需要渲染的顶点数重新置零
				} //end if
			}  //end for 
			pParticle = pParticle->pNext;
		} //end while

		// Unlock the vertex buffer
		/*hr = m_VertexBuffer.GetBuffer()->Unlock();*/

		if( !m_VertexBuffer.Unlock() )
			return;

		/*DEBUG_ASSERT(SUCCEEDED(hr), hr);*/

		//渲染剩余不足一块的粒子
		if(uNumParticlesToRender)
		{
			hr = DEVICE.DrawPrimitive(D3DPT_POINTLIST, m_uBase, uNumParticlesToRender);

			DEBUG_ASSERT(SUCCEEDED(hr), hr);
		}

		//恢复渲染状态
		DEVICE.SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		DEVICE.SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);

		DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, true);
		DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
}
