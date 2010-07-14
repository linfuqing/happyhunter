#pragma once
#include "scenenode.h"
#include "BitFlag.h"

namespace zerO
{
	///
	//��׶��
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

		//����CRectangle3D::TESTPLANESTATEֵ���Ծ���λ��
		bool Test(const CRectangle3D& Rect, UINT32 uTestFlag)const;

		void Transform(const D3DXMATRIX& Matrix);

		//���ӿ�ͶӰ���󹹽���׶��
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
	// �����
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

		const FRUSTUM& GetFrustum()const;

		//ͶӰ�������.
		void SetProjection(
			FLOAT fFOV, 
			FLOAT fAspect, 
			FLOAT fNearPlane,
			FLOAT fFarPlane);

		void UpdateTransform();
	private:
		D3DXMATRIX m_ProjectionMatrix;
		D3DXMATRIX m_InverseProjectionMatrix;
		D3DXMATRIX m_ViewProjectionMatrix;
		D3DXMATRIX m_InverseViewProjectionMatrix;

		FRUSTUM m_Frustum;

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

	inline const FRUSTUM& CCamera::GetFrustum()const
	{
		return m_Frustum;
	}
}
