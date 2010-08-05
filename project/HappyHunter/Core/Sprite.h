#pragma once

#include "SceneNode.h"

namespace zerO
{
	class CSprite :
		public CSceneNode
	{
		typedef enum
		{
			NONE     = 0,
			ROTATION,
			POSITION,
			SCALE

		}DIRTYFLAG;
	public:
		CSprite(void);
		~CSprite(void);

		void SetTransform(const D3DXMATRIX& Matrix);

		void SetPosition(const D3DXVECTOR3& Position);
		void SetScale(const D3DXVECTOR3& Scale);
		void SetRotation(const D3DXVECTOR3& Rotation);

		void SetDirection(const D3DXVECTOR3& Direction);
		void SetSceneDirection(const D3DXVECTOR3& Direction);

		const D3DXVECTOR3 GetPosition()const;
		const D3DXVECTOR3 GetScale()const;
		const D3DXVECTOR3 GetRotation()const;

		void Clone(CSprite& Sprite)const;

		void Right(FLOAT fNumSteps);
		void Up(FLOAT fNumSteps);
		void Forward(FLOAT fNumSteps);

		void Update();

	protected:
		D3DXVECTOR3 m_Position;
		D3DXVECTOR3 m_Rotation;
		D3DXVECTOR3 m_Scale;

		D3DXQUATERNION m_Quaternion;

		UINT32 m_uDirtyFlag;
	};

	inline void CSprite::SetTransform(const D3DXMATRIX& Matrix)
	{
		m_bIsTransformDirty = true;

		m_LocalMatrix = Matrix;

		D3DXMatrixDecompose(&m_Scale, &m_Quaternion, &m_Position, &Matrix);
			
		FLOAT fTest = m_Quaternion.x * m_Quaternion.y + m_Quaternion.z * m_Quaternion.w;

		if(fTest >= 0.5f) 
		{
			m_Rotation.x = 2 * atan2(m_Quaternion.x, m_Quaternion.w);
			m_Rotation.y = D3DX_PI / 2;
			m_Rotation.z = 0.0f;

			return;
		}
		if(fTest <= -0.5f)
		{
			m_Rotation.x = - 2 * atan2(m_Quaternion.x, m_Quaternion.w);
			m_Rotation.y = - D3DX_PI / 2;
			m_Rotation.z = 0.0f;

			return;
		}
	    
	    FLOAT sqx = m_Quaternion.x * m_Quaternion.x;
	    FLOAT sqy = m_Quaternion.y * m_Quaternion.y;
	    FLOAT sqz = m_Quaternion.z * m_Quaternion.z;
	    
	    m_Rotation.x = atan2(2 * m_Quaternion.y * m_Quaternion.w - 2 * m_Quaternion.x * m_Quaternion.z , 1 - 2 * sqy - 2 * sqz);
		m_Rotation.y = asin( 2 * fTest);
		m_Rotation.z = atan2(2 * m_Quaternion.x * m_Quaternion.w - 2 * m_Quaternion.y * m_Quaternion.z , 1 - 2 * sqx - 2 * sqz);
	}

	inline void CSprite::SetPosition(const D3DXVECTOR3& Position)
	{
		m_Position = Position;

		SET_BIT(m_uDirtyFlag, POSITION);
	}

	inline void CSprite::SetScale(const D3DXVECTOR3& Scale)
	{
		m_Scale = Scale;
		
		SET_BIT(m_uDirtyFlag, SCALE);
	}

	inline void CSprite::SetRotation(const D3DXVECTOR3& Rotation)
	{
		m_Rotation = Rotation;

		SET_BIT(m_uDirtyFlag, ROTATION);
	}

	inline const D3DXVECTOR3 CSprite::GetPosition()const
	{
		return m_Position;
	}

	inline const D3DXVECTOR3 CSprite::GetScale()const
	{
		return m_Scale;
	}

	inline const D3DXVECTOR3 CSprite::GetRotation()const
	{
		return m_Rotation;
	}
}
