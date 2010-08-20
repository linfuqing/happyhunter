//=============================================================================
// ��Ч���ļ�
// �̳�����ϵͳ: "particlesystem.h"
//=============================================================================
#pragma once
#include "particlesystem.h"

namespace zerO
{
	////
	// �����
	// Velocity: ���ٶ�
	////
	typedef struct RAINPARAMETERS
	{
		D3DXVECTOR3  Velocity;
		D3DXVECTOR3* pOldData;
		UINT uOldDataLength;
		UINT uOldDataIndex;
		bool bIsFree;

		RAINPARAMETERS() :
		pOldData(NULL),
		uOldDataLength(0)
		{
		}

		~RAINPARAMETERS()
		{
			DEBUG_DELETE_ARRAY(pOldData);
		}
	}RAINPARAMETERS;

	////
	// ��
	////
	class CRain : 
		public CParticleSystem<RAINPARAMETERS>
	{
	public :
		typedef enum
		{
			RANDOM_CUBE,
			RANDOM_CIRCLE
		}OFFSETTYPE;

		CRain(void);
		~CRain(void);

		// ����ٶ�
		FLOAT GetSpeed() const;

		// ���ܵ�������
		FLOAT GetGravity() const;

		// ƫ�ư뾶
		FLOAT GetOffsetRadius() const;

		// ��Ĵ�С
		FLOAT GetSize() const;

		// ���ƫ������
		OFFSETTYPE GetOffsetType() const;

		// ��λ�õ�Ԥ��߶�
		FLOAT GetHight() const;

		// �߶�ƫ����
		UINT GetOffsetHight() const;

		// �����������(�����ȴ����),һ�������ֶ������
		UINT GetMaxinumNumber() const;

		// β�͵��ܶ�
		UINT GetStep() const;

		// �ӳ�ʼλ�÷ɳ�ȥʱ����ļ��ٶ�
		const D3DXVECTOR3& GetAcceleration() const;

		// ����ֵ�λ��
		const D3DXVECTOR3& GetSource() const;

		// ��ķ���
		const D3DXVECTOR3& GetDirection() const;

		void SetSpeed(FLOAT fSpeed);
		void SetGravity(FLOAT fGravity);
		void SetOffsetRadius(FLOAT fOffsetRadius);
		void SetSize(FLOAT fSize);
		void SetHight(FLOAT fHight);
		void SetOffsetHight(UINT fOffsetHight);

		void SetOffsetType(OFFSETTYPE Type);

		void SetMaxinumNumber(UINT uMaxNum);
		void SetStep(UINT uStep);

		void SetSource(const D3DXVECTOR3& Source);
		void SetDirection(const D3DXVECTOR3& Direction);
	
		// ����ÿ֡ˢ���������
		//void SetNumEmitePerFrame(UINT uNumEmitedPerFrame);

		//LPPARTICLE FindHitParticle(const CRectangle3D Rect)const;

		
		////
		// ����
		////
		void Rain();

		////
		// ��������
		// uMaxNum  ��������,ÿ����������µ�����С
		// uFlush   ÿ����Ⱦ���������
		// uDiscard BUFFER������
		// fSize    ���ӵĴ�С
		// ����     �ɹ�����true,ʧ�ܷ���false.
		////
		bool Create(UINT uMaxNum, UINT uFlush, UINT uDiscard, FLOAT fSize);

		void Update();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);
	private:
		void __BuildAcceleration();

		FLOAT m_fSpeed;
		FLOAT m_fGravity;
		FLOAT m_fOffsetRadius;
		FLOAT m_fHight;
		UINT  m_fOffsetHight;

		OFFSETTYPE m_OffsetType;

		UINT  m_uStep;

		D3DXVECTOR3 m_Acceleration;
		D3DXVECTOR3 m_Source;
		D3DXVECTOR3 m_Direction;

		//bool bIsAccelerationDirty;	 
	};

	inline FLOAT CRain::GetSpeed() const
	{
		return m_fSpeed;
	}

	inline FLOAT CRain::GetGravity() const
	{
		return m_fGravity;
	}

	inline FLOAT CRain::GetOffsetRadius() const
	{
		return m_fOffsetRadius;
	}

	inline FLOAT CRain::GetSize() const
	{
		return m_fPointSize;
	}

	inline CRain::OFFSETTYPE CRain::GetOffsetType() const
	{
		return m_OffsetType;
	}

	inline UINT CRain::GetMaxinumNumber() const
	{
		return m_uMaxNumParticles;
	}

	inline UINT CRain::GetStep() const
	{
		return m_uStep;
	}

	inline const D3DXVECTOR3& CRain::GetAcceleration() const
	{
		return m_Acceleration;
	}

	inline const D3DXVECTOR3& CRain::GetSource() const
	{
		return m_Source;
	}

	inline FLOAT CRain::GetHight() const
	{
		return m_fHight;
	}

	inline UINT CRain::GetOffsetHight() const
	{
		return m_fOffsetHight;
	}

	inline const D3DXVECTOR3& CRain::GetDirection() const
	{
		return m_Direction;
	}

	inline void CRain::SetSpeed(zerO::FLOAT fSpeed)
	{
		m_fSpeed = fSpeed;
	}

	inline void CRain::SetGravity(FLOAT fGravity)
	{
		m_fGravity = fGravity;
	}

	inline void CRain::SetOffsetRadius(FLOAT fOffsetRadius)
	{
		m_fOffsetRadius = fOffsetRadius;
	}

	inline void CRain::SetSize(FLOAT fSize)
	{
		m_fPointSize = fSize;
	}

	inline void CRain::SetHight(FLOAT fHight)
	{
		m_fHight = fHight;
	}

	inline void CRain::SetOffsetHight(UINT fOffsetHight)
	{
		m_fOffsetHight = fOffsetHight;
	}

	inline void CRain::SetOffsetType(OFFSETTYPE Type)
	{
		m_OffsetType = Type;
	}

	inline void CRain::SetMaxinumNumber(UINT uMaxNum)
	{
		m_uMaxNumParticles = uMaxNum;
	}

	inline void CRain::SetStep(UINT uStep)
	{
		m_uStep = uStep;
	}

	inline void CRain::SetSource(const D3DXVECTOR3& Source)
	{
		m_Source = Source;
	}

	inline void CRain::SetDirection(const D3DXVECTOR3& Direction)
	{
		m_Direction = Direction;
	}

	inline void CRain::Rain()
	{
		m_uNumEmitedPerFrame = 20;
	}

	inline void CRain::__BuildAcceleration()
	{
		D3DXVec3Normalize(&m_Direction, &m_Direction);

		m_Acceleration = m_Direction;

		m_Acceleration *= m_fSpeed;
	}
}