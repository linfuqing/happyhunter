#pragma once
#include "scenenode.h"
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
		bool Test(const CRectangle3D& Rect, UINT32 uTestFlag);

		void Transform(const D3DXMATRIX& Matrix);

		//从视口投影矩阵构建视锥体
		void ExtractFromMatrix(const D3DXMATRIX& Matrix);

		void Normalize();
	}FRUSTUM, * LPFRUSTUM;

	inline bool FRUSTUM::Test(const CRectangle3D& Rect, UINT32 uTestFlag)
	{
		INT32 nFlag = uTestFlag;

		return TEST_FLAG( nFlag, Rect.Test(Left  ) )
			&& TEST_FLAG( nFlag, Rect.Test(Right ) )
			&& TEST_FLAG( nFlag, Rect.Test(Top   ) )
			&& TEST_FLAG( nFlag, Rect.Test(Bottom) )
			&& TEST_FLAG( nFlag, Rect.Test(Far   ) )
			&& TEST_FLAG( nFlag, Rect.Test(Near  ) );
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
		public CSceneNode
	{
	public:
		CCamera(void);
		~CCamera(void);

		const D3DXMATRIX& GetViewMatrix()const;
		const D3DXMATRIX& GetProjectionMatrix()const;
		const D3DXMATRIX& GetViewProjectionMatrix()const;

		const CRectangle3D& GetSearchRectangle()const;

		//投影矩阵参数.
		void SetProjection(
			FLOAT fFOV, 
			FLOAT fAspect, 
			FLOAT fNearPlane,
			FLOAT fFarPlane);

		void UpdateTransform();
	private:
		D3DXMATRIX m_ProjectionMatrix;
		D3DXMATRIX m_ViewProjectionMatrix;

		FRUSTUM m_Frustum;

		CRectangle3D m_SearchRectangle;

		D3DXVECTOR3	m_FarPlanePoints[8];
	};

	inline const D3DXMATRIX& CCamera::GetViewMatrix()const
	{
		return m_InverseWorldMatrix;
	}

	inline const D3DXMATRIX& CCamera::GetProjectionMatrix()const
	{
		return m_ProjectionMatrix;
	}

	inline const D3DXMATRIX& CCamera::GetViewProjectionMatrix()const
	{
		return m_ViewProjectionMatrix; 
	}

	inline const CRectangle3D& CCamera::GetSearchRectangle()const
	{
		return m_SearchRectangle;
	}
}
