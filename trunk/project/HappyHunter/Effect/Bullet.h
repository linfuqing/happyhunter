#pragma once
#include "particlesystem.h"

namespace zerO
{
	///
	// 子弹参数
	// Velocity: 加速度
	// Rectangle: 包围盒
	///
	typedef struct
	{
		D3DXVECTOR3 Velocity;
		CRectangle3D Rectangle;
		bool bIsFree;
	}BULLETPARAMETERS;


	///
	//子弹类
	///
	class CBullet : 
		public CParticleSystem<BULLETPARAMETERS>
	{
	public:
		typedef enum
		{
			RANDOM_CUBE,
			RANDOM_CIRCLE
		}OFFSETTYPE;

		CBullet(void);
		~CBullet(void);

		//子弹的速度
		FLOAT GetSpeed()const;

		//子弹受到的重力
		FLOAT GetGravity()const;

		//偏移半径,如果设置,将在设置半径内进行随机的初始位置的设定(即子弹会在不同位置出现)
		FLOAT GetOffsetRadius()const;

		//子弹的大小
		FLOAT GetSize()const;

		OFFSETTYPE GetOffsetType()const;

		//弹夹的容量(最大冷却数量),最大一次性发射多少子弹
		UINT  GetMaxinumNumber()const;

		//尾巴的密度
		UINT  GetStep()const;

		//尾巴的段数,段数越高,尾巴越长.
		UINT  GetLength()const;

		//从初始位置飞出去时给子弹的加速度
		const D3DXVECTOR3& GetAcceleration()const;

		//子弹出现位置
		const D3DXVECTOR3& GetSource()const;

		//子弹方向
		const D3DXVECTOR3& GetDirection()const;

		void SetSpeed(FLOAT fSpeed);
		void SetGravity(FLOAT fGravity);
		void SetOffsetRadius(FLOAT fOffsetRadius);
		void SetSize(FLOAT fSize);

		void SetOffsetType(OFFSETTYPE Type);

		void SetMaxinumNumber(UINT uMaxNum);
		void SetStep(UINT uStep);
		void SetLength(UINT uLength);

		void SetSource(const D3DXVECTOR3& Source);
		void SetDirection(const D3DXVECTOR3& Direction);

		LPPARTICLE FindHitParticle(const CRectangle3D Rect)const;

		bool FreeHitParticle(const CRectangle3D Rect)const;

		///
		// 射击
		///
		void Shoot();

		///
		// 构建函数
		// uMaxNum  弹夹的大小,每次最大能发射多少子弹
		// uFlush   每次渲染的最大数量
		// uDiscard Buffer的容量
		// fSize    粒子的大小
		// 返回     成功返回true, 失败返回false.
		///
		bool Create(UINT uMaxNum, UINT uFlush, UINT uDiscard, FLOAT fSize);

		void Update();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);
	private:
		void __BuildAcceleration();

		FLOAT m_fSpeed;
		FLOAT m_fGravity;
		FLOAT m_fOffsetRadius;

		OFFSETTYPE m_OffsetType;

		UINT  m_uStep;
		UINT  m_uLength;

		D3DXVECTOR3 m_Acceleration;
		D3DXVECTOR3 m_Source;
		D3DXVECTOR3 m_Direction;

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

	inline CBullet::OFFSETTYPE CBullet::GetOffsetType()const
	{
		return m_OffsetType;
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

	inline const D3DXVECTOR3& CBullet::GetDirection()const
	{
		return m_Direction;
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

	inline void CBullet::SetOffsetType(OFFSETTYPE Type)
	{
		m_OffsetType = Type;
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
	}

	inline void CBullet::SetDirection(const D3DXVECTOR3& Direction)
	{
		m_Direction = Direction;

		bIsAccelerationDirty = true;
	}

	inline void CBullet::Shoot()
	{
		m_uNumEmitedPerFrame = 1;
	}

	inline void CBullet::__BuildAcceleration()
	{
		D3DXVec3Normalize(&m_Direction, &m_Direction);

		m_Acceleration = m_Direction;

		m_Acceleration *= m_fSpeed;
	}
}
