#pragma once
#include "Sprite.h"
#include "BitFlag.h"

namespace zerO
{
	///
	//视锥体
	///
	typedef struct FRUSTUM
	{
		union
		{
			struct
			{
				D3DXPLANE Left;
				D3DXPLANE Right;
				D3DXPLANE Top;
				D3DXPLANE Bottom;
				D3DXPLANE Far;
				D3DXPLANE Near;
			};

			struct
			{
				D3DXPLANE Plane[6];
			};
		};

		//根据CRectangle3D::TESTPLANESTATE值测试矩形位置
		bool Test(const CRectangle3D& Rect, UINT32 uTestFlag)const;

		bool TestHit(const CRectangle3D& Rect)const;

		void Transform(const D3DXMATRIX& Matrix);

		//从视口投影矩阵构建视锥体
		void ExtractFromMatrix(const D3DXMATRIX& Matrix);

		void Normalize();
	}FRUSTUM, * LPFRUSTUM;

	inline bool FRUSTUM::Test(const CRectangle3D& Rect, UINT32 uTestFlag)const
	{
		INT32 nFlag = uTestFlag;

		return TEST_FLAG( nFlag, Rect.Test(Left  ) )
			&& TEST_FLAG( nFlag, Rect.Test(Right ) )
			&& TEST_FLAG( nFlag, Rect.Test(Top   ) )
			&& TEST_FLAG( nFlag, Rect.Test(Bottom) )
			&& TEST_FLAG( nFlag, Rect.Test(Far   ) )
			&& TEST_FLAG( nFlag, Rect.Test(Near  ) );
	}

	inline bool FRUSTUM::TestHit(const CRectangle3D& Rect)const
	{
		static UINT32 s_uTestFlag = CRectangle3D::PLANE_FRONT | CRectangle3D::PLANE_INTERSECT;
		return Test(Rect, s_uTestFlag);
	}

	inline void FRUSTUM::Transform(const D3DXMATRIX& Matrix)
	{
		D3DXPlaneTransformArray(Plane, sizeof(D3DXPLANE), Plane, sizeof(D3DXPLANE), &Matrix, 6);
	}

	inline void FRUSTUM::Normalize()
	{
		D3DXPlaneNormalize(&Left  , &Left  );
		D3DXPlaneNormalize(&Right , &Right );
		D3DXPlaneNormalize(&Top   , &Top   );
		D3DXPlaneNormalize(&Bottom, &Bottom);
		D3DXPlaneNormalize(&Far   , &Far   );
		D3DXPlaneNormalize(&Near  , &Near  );
	}

	///
	// 相机类
	///
	class CCamera :
		public CSprite
	{
	public:
		CCamera(void);
		~CCamera(void);

		const D3DXMATRIX& GetViewMatrix()const;
		const D3DXMATRIX& GetProjectionMatrix()const;
		const D3DXMATRIX& GetViewProjectionMatrix()const;

		const D3DXVECTOR3& GetWorldPosition()const;

		const FRUSTUM& GetFrustum()const;

		FLOAT GetFOV()const;
		FLOAT GetAspect()const;
		FLOAT GetNearPlane()const;
		FLOAT GetFarPlane()const;

		//投影矩阵参数.
		void SetProjection(
			FLOAT fFOV, 
			FLOAT fAspect, 
			FLOAT fNearPlane,
			FLOAT fFarPlane);

		void UpdateTransform();
	private:
		D3DXMATRIX m_ViewMatrix;
		D3DXMATRIX m_ProjectionMatrix;
		D3DXMATRIX m_InverseProjectionMatrix;
		D3DXMATRIX m_ViewProjectionMatrix;
		D3DXMATRIX m_InverseViewProjectionMatrix;

		D3DXVECTOR3	m_WorldPosition;

		FRUSTUM m_Frustum;

		D3DXVECTOR3	m_FarPlanePoints[8];

		FLOAT m_fFOV;
		FLOAT m_fAspect;
		FLOAT m_fNearPlane;
		FLOAT m_fFarPlane;
	};

	inline const D3DXMATRIX& CCamera::GetViewMatrix()const
	{
		return m_ViewMatrix;
	}

	inline const D3DXMATRIX& CCamera::GetProjectionMatrix()const
	{
		return m_ProjectionMatrix;
	}

	inline const D3DXMATRIX& CCamera::GetViewProjectionMatrix()const
	{
		return m_ViewProjectionMatrix; 
	}

	inline const FRUSTUM& CCamera::GetFrustum()const
	{
		return m_Frustum;
	}

	inline const D3DXVECTOR3& CCamera::GetWorldPosition()const
	{
		return m_WorldPosition;
	}

	inline FLOAT CCamera::GetFOV()const
	{
		return m_fFOV;
	}

	inline FLOAT CCamera::GetAspect()const
	{
		return m_fAspect;
	}

	inline FLOAT CCamera::GetNearPlane()const
	{
		return m_fNearPlane;
	}

	inline FLOAT CCamera::GetFarPlane()const
	{
		return m_fFarPlane;
	}
}
