//=============================================================================
// 雪效果文件
// 继承粒子系统: "particlesystem.h"
//=============================================================================
#pragma once
#include "particlesystem.h"

namespace zerO
{
	typedef struct SNOWPARAMETERS
	{
		D3DXVECTOR3  Velocity;
		D3DXVECTOR3* pOldData;
		UINT uOldDataLength;
		UINT uOldDataIndex;
		bool bIsFree;

		SNOWPARAMETERS() :
		pOldData(NULL),
		uOldDataLength(0)
		{
		}

		~SNOWPARAMETERS()
		{
			DEBUG_DELETE_ARRAY(pOldData);
		}
	}SNOWPARAMETERS;

	////
	// 雪
	////
	class CSnow : 
		public CParticleSystem<SNOWPARAMETERS>
	{
	public :
		typedef enum
		{
			RANDOM_CUBE,
			RANDOM_CIRCLE
		}OFFSETTYPE;

		CSnow(void);
		~CSnow(void);

		// 雪的速度
		FLOAT GetSpeed() const;

		// 雪受到的重力
		FLOAT GetGravity() const;

		// 偏移半径
		FLOAT GetOffsetRadius() const;

		// 雪的大小
		FLOAT GetSize() const;

		// 雪的偏移类型
		OFFSETTYPE GetOffsetType() const;

		// 雪位置的预设高度
		FLOAT GetHight() const;

		// 计算摩擦系数
		FLOAT GetFriction() const;

		// 高度偏移量
		UINT GetOffsetHight() const;

		// 雪点的最大数量(最大冷却数量),一次最多出现多少雪点
		UINT GetMaxinumNumber() const;

		// 尾巴的密度
		UINT GetStep() const;

		// 从初始位置飞出去时给雪的加速度
		const D3DXVECTOR3& GetAcceleration() const;

		// 子弹出现的位置
		const D3DXVECTOR3& GetSource() const;

		// 子弹的方向
		const D3DXVECTOR3& GetDirection() const;

		void SetSpeed(FLOAT fSpeed);
		void SetGravity(FLOAT fGravity);
		void SetOffsetRadius(FLOAT fOffsetRadius);
		void SetSize(FLOAT fSize);
		void SetHight(FLOAT fHight);
		void SetOffsetHight(UINT fOffsetHight);
		void SetFriction(FLOAT fFriction);

		void SetOffsetType(OFFSETTYPE Type);

		void SetMaxinumNumber(UINT uMaxNum);
		void SetStep(UINT uStep);

		void SetSource(const D3DXVECTOR3& Source);
		void SetDirection(const D3DXVECTOR3& Direction);

		
		////
		// 下雪
		////
		void Snow();

		////
		// 构建函数
		// uMaxNum  雪点的数量,每次最多能落下的雪点大小
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
		FLOAT m_fFriction;

		OFFSETTYPE m_OffsetType;

		UINT  m_uStep;

		D3DXVECTOR3 m_Acceleration;
		D3DXVECTOR3 m_Source;
		D3DXVECTOR3 m_Direction;

		//bool bIsAccelerationDirty;	 
	};

	inline FLOAT CSnow::GetSpeed() const
	{
		return m_fSpeed;
	}

	inline FLOAT CSnow::GetGravity() const
	{
		return m_fGravity;
	}

	inline FLOAT CSnow::GetOffsetRadius() const
	{
		return m_fOffsetRadius;
	}

	inline FLOAT CSnow::GetSize() const
	{
		return m_fPointSize;
	}

	inline CSnow::OFFSETTYPE CSnow::GetOffsetType() const
	{
		return m_OffsetType;
	}

	inline FLOAT CSnow::GetHight() const
	{
		return m_fHight;
	}

	inline UINT CSnow::GetOffsetHight() const
	{
		return m_fOffsetHight;
	}

	inline FLOAT CSnow::GetFriction() const 
	{
		return m_fFriction;
	}

	inline UINT CSnow::GetMaxinumNumber() const
	{
		return m_uMaxNumParticles;
	}

	inline UINT CSnow::GetStep() const
	{
		return m_uStep;
	}

	inline const D3DXVECTOR3& CSnow::GetAcceleration() const
	{
		return m_Acceleration;
	}

	inline const D3DXVECTOR3& CSnow::GetSource() const
	{
		return m_Source;
	}

	inline const D3DXVECTOR3& CSnow::GetDirection() const
	{
		return m_Direction;
	}

	inline void CSnow::SetSpeed(zerO::FLOAT fSpeed)
	{
		m_fSpeed = fSpeed;
	}

	inline void CSnow::SetGravity(FLOAT fGravity)
	{
		m_fGravity = fGravity;
	}

	inline void CSnow::SetOffsetRadius(FLOAT fOffsetRadius)
	{
		m_fOffsetRadius = fOffsetRadius;
	}

	inline void CSnow::SetSize(FLOAT fSize)
	{
		m_fPointSize = fSize;
	}

	inline void CSnow::SetOffsetType(OFFSETTYPE Type)
	{
		m_OffsetType = Type;
	}

	inline void CSnow::SetHight(FLOAT fHight)
	{
		m_fHight = fHight;
	}

	inline void CSnow::SetOffsetHight(UINT fOffsetHight)
	{
		m_fOffsetHight = fOffsetHight;
	}

	inline void CSnow::SetFriction(FLOAT fFriction)
	{
		m_fFriction = fFriction;
	}

	inline void CSnow::SetMaxinumNumber(UINT uMaxNum)
	{
		m_uMaxNumParticles = uMaxNum;
	}

	inline void CSnow::SetStep(UINT uStep)
	{
		m_uStep = uStep;
	}

	inline void CSnow::SetSource(const D3DXVECTOR3& Source)
	{
		m_Source = Source;
	}

	inline void CSnow::SetDirection(const D3DXVECTOR3& Direction)
	{
		m_Direction = Direction;
	}

	inline void CSnow::Snow()
	{
		m_uNumEmitedPerFrame = 5;
	}

	inline void CSnow::__BuildAcceleration()
	{
		D3DXVec3Normalize(&m_Direction, &m_Direction);

		m_Acceleration = m_Direction;

		m_Acceleration *= m_fSpeed;
	}
}