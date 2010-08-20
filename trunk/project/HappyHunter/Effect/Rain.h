//=============================================================================
// 雨效果文件
// 继承粒子系统: "particlesystem.h"
//=============================================================================
#pragma once
#include "particlesystem.h"

namespace zerO
{
	////
	// 雨参数
	// Velocity: 加速度
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
	// 雨
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

		// 雨的速度
		FLOAT GetSpeed() const;

		// 雨受到的重力
		FLOAT GetGravity() const;

		// 偏移半径
		FLOAT GetOffsetRadius() const;

		// 雨的大小
		FLOAT GetSize() const;

		// 雨的偏移类型
		OFFSETTYPE GetOffsetType() const;

		// 雨位置的预设高度
		FLOAT GetHight() const;

		// 高度偏移量
		UINT GetOffsetHight() const;

		// 雨点的最大数量(最大冷却数量),一次最多出现多少雨点
		UINT GetMaxinumNumber() const;

		// 尾巴的密度
		UINT GetStep() const;

		// 从初始位置飞出去时给雨的加速度
		const D3DXVECTOR3& GetAcceleration() const;

		// 雨出现的位置
		const D3DXVECTOR3& GetSource() const;

		// 雨的方向
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
	
		// 设置每帧刷新雨的数量
		//void SetNumEmitePerFrame(UINT uNumEmitedPerFrame);

		//LPPARTICLE FindHitParticle(const CRectangle3D Rect)const;

		
		////
		// 下雨
		////
		void Rain();

		////
		// 构建函数
		// uMaxNum  雨点的数量,每次最多能落下的雨点大小
		// uFlush   每次渲染的最大数量
		// uDiscard BUFFER的容量
		// fSize    粒子的大小
		// 返回     成功返回true,失败返回false.
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