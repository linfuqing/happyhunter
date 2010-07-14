#pragma once
#include "particlesystem.h"

namespace zerO
{
	///
	// �ӵ�����
	// Velocity: ���ٶ�
	// Rectangle: ��Χ��
	///
	typedef struct
	{
		D3DXVECTOR3 Velocity;
		CRectangle3D Rectangle;
	}BULLETPARAMETERS;


	///
	//�ӵ���
	///
	class CBullet : 
		public CParticleSystem<BULLETPARAMETERS>
	{
	public:
		CBullet(void);
		~CBullet(void);

		//�ӵ����ٶ�
		FLOAT GetSpeed()const;

		//�ӵ��ܵ�������
		FLOAT GetGravity()const;

		//ƫ�ư뾶,�������,�������ð뾶�ڽ�������ĳ�ʼλ�õ��趨(���ӵ����ڲ�ͬλ�ó���)
		FLOAT GetOffsetRadius()const;

		//�ӵ��Ĵ�С
		FLOAT GetSize()const;

		//���е�����(�����ȴ����),���һ���Է�������ӵ�
		UINT  GetMaxinumNumber()const;

		//β�͵��ܶ�
		UINT  GetStep()const;

		//β�͵Ķ���,����Խ��,β��Խ��.
		UINT  GetLength()const;

		//�ӳ�ʼλ�÷ɳ�ȥʱ���ӵ��ļ��ٶ�
		const D3DXVECTOR3& GetAcceleration()const;

		//�ӵ�����λ��
		const D3DXVECTOR3& GetSource()const;

		//�ӵ�Ŀ��λ��
		const D3DXVECTOR3& GetTarget()const;

		void SetSpeed(FLOAT fSpeed);
		void SetGravity(FLOAT fGravity);
		void SetOffsetRadius(FLOAT fOffsetRadius);
		void SetSize(FLOAT fSize);

		void SetMaxinumNumber(UINT uMaxNum);
		void SetStep(UINT uStep);
		void SetLength(UINT uLength);

		void SetSource(const D3DXVECTOR3& Source);
		void SetTarget(const D3DXVECTOR3& Target);

		///
		// ���
		///
		void Shoot();

		///
		// ��������
		// uMaxNum  ���еĴ�С,ÿ������ܷ�������ӵ�
		// uFlush   ÿ����Ⱦ���������
		// uDiscard Buffer������
		// fSize    ���ӵĴ�С
		// ����     �ɹ�����true, ʧ�ܷ���false.
		///
		bool Create(UINT uMaxNum, UINT uFlush, UINT uDiscard, FLOAT fSize);

		void Update();
	private:
		void __BuildAcceleration();

		FLOAT m_fSpeed;
		FLOAT m_fGravity;
		FLOAT m_fOffsetRadius;

		UINT  m_uStep;
		UINT  m_uLength;

		D3DXVECTOR3 m_Acceleration;
		D3DXVECTOR3 m_Source;
		D3DXVECTOR3 m_Target;

		bool bIsAccelerationDirty;
	};

	inline FLOAT CBullet::GetSpeed()const
	{
		return m_fSpeed;
	}

	inline FLOAT CBullet::GetGravity()const
	{
		return m_fGravity;
	}

	inline FLOAT CBullet::GetOffsetRadius()const
	{
		return m_fOffsetRadius;
	}

	inline FLOAT CBullet::GetSize()const
	{
		return m_fPointSize;
	}

	inline UINT CBullet::GetMaxinumNumber()const
	{
		return m_uMaxNumParticles;
	}

	inline UINT CBullet::GetStep()const
	{
		return m_uStep;
	}

	inline UINT CBullet::GetLength()const
	{
		return m_uLength;
	}

	inline const D3DXVECTOR3& CBullet::GetAcceleration()const
	{
		return m_Acceleration;
	}

	inline const D3DXVECTOR3& CBullet::GetSource()const
	{
		return m_Source;
	}

	inline const D3DXVECTOR3& CBullet::GetTarget()const
	{
		return m_Target;
	}

	inline void CBullet::SetSpeed(FLOAT fSpeed)
	{
		m_fSpeed = fSpeed;

		bIsAccelerationDirty = true;
	}

	inline void CBullet::SetGravity(FLOAT fGravity)
	{
		m_fGravity = fGravity;
	}

	inline void CBullet::SetOffsetRadius(FLOAT fOffsetRadius)
	{
		m_fOffsetRadius = fOffsetRadius;
	}

	inline void CBullet::SetSize(FLOAT fSize)
	{
		m_fPointSize = fSize;
	}

	inline void CBullet::SetMaxinumNumber(UINT uMaxNum)
	{
		m_uMaxNumParticles = uMaxNum;
	}

	inline void CBullet::SetStep(UINT uStep)
	{
		m_uStep = uStep;
	}

	inline void CBullet::SetLength(UINT uLength)
	{
		m_uLength = uLength;
	}

	inline void CBullet::SetSource(const D3DXVECTOR3& Source)
	{
		m_Source = Source;

		bIsAccelerationDirty = true;
	}

	inline void CBullet::SetTarget(const D3DXVECTOR3& Target)
	{
		m_Target = Target;

		bIsAccelerationDirty = true;
	}

	inline void CBullet::Shoot()
	{
		m_uNumEmitedPerFrame = 1;
	}

	inline void CBullet::__BuildAcceleration()
	{
		m_Acceleration = m_Target - m_Source;

		D3DXVec3Normalize(&m_Acceleration, &m_Acceleration);

		m_Acceleration *= m_fSpeed;
	}
}
