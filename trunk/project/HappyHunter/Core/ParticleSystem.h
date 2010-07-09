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

		typedef struct
		{
			PARTICLEPARAMETERS Parameter;
			PARTICLEVERTEX     Vertex;
		}PARTICLE, * LPPARTICLE;

		//Update
		typedef void (*HANDLEPARTICLE)(LPPARTICLE);
		typedef bool (*CHECKPARTICLE)(LPPARTICLE);

		//Render
		typedef UINT (*GETPARTICLEDATA)(const PARTICLE&);
		typedef void (*SETPARTICLEDATA)(const PARTICLE&, PARTICLEVERTEX&);

		CParticleSystem(void);

		~CParticleSystem(void);

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

		bool Restore();

		void Update();

		bool ApplyForRender();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag);

	private:
		typedef struct PARTICLENODE
		{
			PARTICLE      Particle;
			PARTICLENODE* pNext;
		}PARTICLENODE, * LPPARTICLENODE;

		UINT m_uNumEmitedPerFrame;

		UINT m_uNumParticles;      //��ǰ���������ӵ�����
		UINT m_uMaxNumParticles;   //���������ӵ��������

		FLOAT m_fPointSize;
		FLOAT m_fPointSizeMax;
		FLOAT m_fPointSizeMin;
		FLOAT m_fPointScaleA;
		FLOAT m_fPointScaleB;
		FLOAT m_fPointScaleC;

		LPPARTICLENODE m_pParticles;    //��ǰ��������
		LPPARTICLENODE m_pParticlesFree;

		CVertexBuffer m_VertexBuffer;

		LPDIRECT3DTEXTURE9 m_pTexture;   //��������

		UINT  m_uBase;       //ÿ����䶥�㻺����ʱ����ʼλ��:m_uBase += m_uFlush;     
		UINT  m_uFlush;      //һ����䶥�㻺��������������
		UINT  m_uDiscard;    //���㻺�����ܹ����ɵ������������

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
	inline CParticleSystem<T>::CParticleSystem() :
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
	m_pParticlesFree(NULL)
	{
		m_pTexture = NULL;
	}

	template<typename T>
	inline CParticleSystem<T>::~CParticleSystem(void)
	{
		while( m_pParticles )
		{
			PARTICLENODE* pSpark = m_pParticles;
			m_pParticles = pSpark->pNext;
			delete pSpark;
		}

		while( m_pParticlesFree )
		{
			PARTICLENODE *pSpark = m_pParticlesFree;
			m_pParticlesFree = pSpark->pNext;
			delete pSpark;
		}
	}

	template<typename T>
	inline bool CParticleSystem<T>::Create(
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
	inline void CParticleSystem<T>::Update()
	{
		CSceneNode::Update();

		DEVICE.SetTransform( D3DTS_WORLD,      &m_WorldMatrix); 
		DEVICE.SetTransform( D3DTS_VIEW,       &CAMERA.GetViewMatrix() );
		DEVICE.SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );

		LPPARTICLENODE pParticle, *ppParticle;

		//�����Ѵ������ӵ�����ֵ
		ppParticle = &m_pParticles;   //��������
		while( *ppParticle )
		{
			pParticle = *ppParticle;  //ȡ����ǰ����

			//��������
			m_pfnUpdate(&pParticle->Particle);

			//ɾ����������
			if( m_pfnIsDestroy(&pParticle->Particle) )
			{
				*ppParticle = pParticle->pNext;
				pParticle->pNext = m_pParticlesFree;
				m_pParticlesFree = pParticle;
				m_uNumParticles--;
			}
			else  //׼��������һ������
				ppParticle = &pParticle->pNext;
		}

		//���������
		UINT uEmited = 0;
		while( m_uNumParticles < m_uMaxNumParticles && uEmited <= m_uNumEmitedPerFrame)
		{
			if( m_pParticlesFree )
			{
				pParticle = m_pParticlesFree;
				m_pParticlesFree = pParticle->pNext;
			}
			else
				DEBUG_NEW(pParticle, PARTICLENODE);

			pParticle->pNext = m_pParticles;
			m_pParticles = pParticle;
			m_uNumParticles++;
			uEmited++;

			m_pfnInit(&pParticle->Particle);
		}
	}

	template<typename T>
	inline bool CParticleSystem<T>::ApplyForRender()
	{
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
	inline void CParticleSystem<T>::Render(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
	{
		HRESULT hr;

		//DEVICE.SetRenderState( D3DRS_ZWRITEENABLE, false );     //������Ȼ���������
		//DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, true );  //����Alpha ���
		//DEVICE.SetRenderState( D3DRS_SRCBLEND, D3DBLEND_ONE );  //ע��Alpha��Ϸ�ʽ
		//DEVICE.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );

		DEVICE.SetRenderState( D3DRS_POINTSPRITEENABLE, TRUE );     //ʹ�õ㾫��
		DEVICE.SetRenderState( D3DRS_POINTSCALEENABLE,  TRUE );

		if( TEST_BIT(uFlag, CRenderQueue::MODEL) )
		{
			//Ϊʹ�õ㾫�����������Ⱦ״̬
			DEVICE.SetRenderState( D3DRS_POINTSIZE,     FtoDW(m_fPointSize) );  //���Ӵ�С
			DEVICE.SetRenderState( D3DRS_POINTSIZE_MAX, FtoDW(m_fPointSizeMax) );
			DEVICE.SetRenderState( D3DRS_POINTSIZE_MIN, FtoDW(m_fPointSizeMin) ); //�����С�ߴ�
			DEVICE.SetRenderState( D3DRS_POINTSCALE_A,  FtoDW(m_fPointScaleA) );
			DEVICE.SetRenderState( D3DRS_POINTSCALE_B,  FtoDW(m_fPointScaleB) );
			DEVICE.SetRenderState( D3DRS_POINTSCALE_C,  FtoDW(m_fPointScaleC) );

			m_VertexBuffer.Activate(0, 0, false);
		}

		if( TEST_BIT(uFlag, CRenderQueue::SURFACE) )
			m_Surface.Activate();

		LPPARTICLENODE   pParticle = m_pParticles;
		LPPARTICLEVERTEX pVertices;
		UINT             uNumParticlesToRender = 0, uParticleVertexSize = sizeof(PARTICLEVERTEX);

		m_uBase += m_uFlush;

		if(m_uBase >= m_uDiscard)
			m_uBase = 0;

		hr = m_VertexBuffer.GetBuffer()->Lock(
			m_uBase * uParticleVertexSize,
			m_uFlush * uParticleVertexSize,
			(void**) &pVertices, 
			m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

		DEBUG_ASSERT(SUCCEEDED(hr), hr);

		//��Ⱦ����
		while( pParticle )
		{
			UINT uSteps = m_pfnGetSteps(pParticle->Particle);

			//ͨ�������ڲ�ͬλ����Ⱦ�����ʵ��ģ��Ч��
			for( DWORD i = 0; i < uSteps; i++ )
			{
				m_pfnSetRenderData(pParticle->Particle, *pVertices);

				pVertices ++;

				if( ++ uNumParticlesToRender == m_uFlush )  //������ָ�������ݿ�
				{
				   hr = m_VertexBuffer.GetBuffer()->Unlock();

				   DEBUG_ASSERT(SUCCEEDED(hr), hr);

				   hr = DEVICE.DrawPrimitive(D3DPT_POINTLIST, m_uBase, uNumParticlesToRender);

				   DEBUG_ASSERT(SUCCEEDED(hr), hr);

					m_uBase += m_uFlush;
					if(m_uBase >= m_uDiscard)
						m_uBase = 0;

					hr = m_VertexBuffer.GetBuffer()->Lock(
						m_uBase * uParticleVertexSize, 
						m_uFlush * uParticleVertexSize, 
						(void**)&pVertices, 
						m_uBase ? D3DLOCK_NOOVERWRITE : D3DLOCK_DISCARD);

					DEBUG_ASSERT(SUCCEEDED(hr), hr);

					uNumParticlesToRender = 0;  //����Ҫ��Ⱦ�Ķ�������������
				} //end if
			}  //end for 
			pParticle = pParticle->pNext;
		} //end while

		// Unlock the vertex buffer
		hr = m_VertexBuffer.GetBuffer()->Unlock();

		DEBUG_ASSERT(SUCCEEDED(hr), hr);

		//��Ⱦʣ�಻��һ�������
		if(uNumParticlesToRender)
		{
			hr = DEVICE.DrawPrimitive( D3DPT_POINTLIST, m_uBase, uNumParticlesToRender);

			DEBUG_ASSERT(SUCCEEDED(hr), hr);
		}

		//�ָ���Ⱦ״̬
		DEVICE.SetRenderState(D3DRS_POINTSPRITEENABLE, FALSE);
		DEVICE.SetRenderState(D3DRS_POINTSCALEENABLE,  FALSE);

		DEVICE.SetRenderState(D3DRS_ZWRITEENABLE, true);
		DEVICE.SetRenderState(D3DRS_ALPHABLENDENABLE, false);
	}
}
