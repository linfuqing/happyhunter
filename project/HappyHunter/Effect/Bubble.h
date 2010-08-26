//=============================================================================
// 气泡效果文件
// 继承粒子系统: "particlesystem.h"
//=============================================================================
#pragma once
#include "particlesystem.h"

namespace zerO
{
	////
	// 气泡参数
	// Velocity: 加速度
	////
	typedef struct BUBBLEPARAMETERS
	{
		D3DXVECTOR3  Velocity;
		D3DXVECTOR3* pOldData;
		UINT uOldDataLength;
		UINT uOldDataIndex;
		bool bIsFree;

		BUBBLEPARAMETERS() :
		pOldData(NULL),
		uOldDataLength(0)
		{
		}

		~BUBBLEPARAMETERS()
		{
			DEBUG_DELETE_ARRAY(pOldData);
		}
	}BUBBLEPARAMETERS;

	////
	// 气泡
	////
	class CBubble : 
		public CParticleSystem<BUBBLEPARAMETERS>
	{
	public :
		typedef enum
		{
			RANDOM_CUBE,
			RANDOM_CIRCLE
		}OFFSETTYPE;

		CBubble(void);
		~CBubble(void);

		// 气泡的速度
		FLOAT GetSpeed() const;

		// 气泡受到的浮力
		FLOAT GetFlotage() const;

		// 偏移半径
		FLOAT GetOffsetRadius() const;

		// 气泡的大小
		FLOAT GetSize() const;

		// 气泡的偏移类型
		OFFSETTYPE GetOffsetType() const;

		// 气泡位置的预设高度
		FLOAT GetHight() const;

		// 高度偏移量
		UINT GetOffsetHight() const;

		// 气泡点的最大数量(最大冷却数量),一次最多出现多少气泡点
		UINT GetMaxinumNumber() const;

		// 尾巴的密度
		UINT GetStep() const;

		// 从初始位置飞出去时给气泡的加速度
		const D3DXVECTOR3& GetAcceleration() const;

		// 气泡出现的位置
		const D3DXVECTOR3& GetSource() const;

		// 气泡的方向
		const D3DXVECTOR3& GetDirection() const;

		void SetSpeed(FLOAT fSpeed);
		void SetFlotage(FLOAT fFlotage);
		void SetOffsetRadius(FLOAT fOffsetRadius);
		void SetSize(FLOAT fSize);
		void SetHight(FLOAT fHight);
		void SetOffsetHight(UINT fOffsetHight);

		void SetOffsetType(OFFSETTYPE Type);

		void SetMaxinumNumber(UINT uMaxNum);
		void SetStep(UINT uStep);

		void SetSource(const D3DXVECTOR3& Source);
		void SetDirection(const D3DXVECTOR3& Direction);
	
		// 设置每帧刷新气泡的数量
		//void SetNumEmitePerFrame(UINT uNumEmitedPerFrame);

		//LPPARTICLE FindHitParticle(const CRectangle3D Rect)const;

		
		////
		// 气泡
		////
		void Bubble();

		////
		// 构建函数
		// uMaxNum  气泡点的数量,每次最多能落下的气泡点大小
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
		FLOAT m_fFlotage;
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

	inline FLOAT CBubble::GetSpeed() const
	{
		return m_fSpeed;
	}

	inline FLOAT CBubble::GetFlotage() const
	{
		return m_fFlotage;
	}

	inline FLOAT CBubble::GetOffsetRadius() const
	{
		return m_fOffsetRadius;
	}

	inline FLOAT CBubble::GetSize() const
	{
		return m_fPointSize;
	}

	inline CBubble::OFFSETTYPE CBubble::GetOffsetType() const
	{
		return m_OffsetType;
	}

	inline UINT CBubble::GetMaxinumNumber() const
	{
		return m_uMaxNumParticles;
	}

	inline UINT CBubble::GetStep() const
	{
		return m_uStep;
	}

	inline const D3DXVECTOR3& CBubble::GetAcceleration() const
	{
		return m_Acceleration;
	}

	inline const D3DXVECTOR3& CBubble::GetSource() const
	{
		return m_Source;
	}

	inline FLOAT CBubble::GetHight() const
	{
		return m_fHight;
	}

	inline UINT CBubble::GetOffsetHight() const
	{
		return m_fOffsetHight;
	}

	inline const D3DXVECTOR3& CBubble::GetDirection() const
	{
		return m_Direction;
	}

	inline void CBubble::SetSpeed(zerO::FLOAT fSpeed)
	{
		m_fSpeed = fSpeed;
	}

	inline void CBubble::SetFlotage(FLOAT fFlotage)
	{
		m_fFlotage = fFlotage;
	}

	inline void CBubble::SetOffsetRadius(FLOAT fOffsetRadius)
	{
		m_fOffsetRadius = fOffsetRadius;
	}

	inline void CBubble::SetSize(FLOAT fSize)
	{
		m_fPointSize = fSize;
	}

	inline void CBubble::SetHight(FLOAT fHight)
	{
		m_fHight = fHight;
	}

	inline void CBubble::SetOffsetHight(UINT fOffsetHight)
	{
		m_fOffsetHight = fOffsetHight;
	}

	inline void CBubble::SetOffsetType(OFFSETTYPE Type)
	{
		m_OffsetType = Type;
	}

	inline void CBubble::SetMaxinumNumber(UINT uMaxNum)
	{
		m_uMaxNumParticles = uMaxNum;
	}

	inline void CBubble::SetStep(UINT uStep)
	{
		m_uStep = uStep;
	}

	inline void CBubble::SetSource(const D3DXVECTOR3& Source)
	{
		m_Source = Source;
	}

	inline void CBubble::SetDirection(const D3DXVECTOR3& Direction)
	{
		m_Direction = Direction;
	}

	inline void CBubble::Bubble()
	{
		m_uNumEmitedPerFrame = 1;
	}

	inline void CBubble::__BuildAcceleration()
	{
		D3DXVec3Normalize(&m_Direction, &m_Direction);

		m_Acceleration = m_Direction;

		m_Acceleration *= m_fSpeed;
	}
}