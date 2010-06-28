#pragma once

#include "Rectangle2D.h"

namespace zerO
{
	///
	// 矩形模板类,所有类型的矩形接口.
	///
	template<typename T>
	class CBasicRectangle3D :
		public CBasicRectangle2D<T>
	{
	public:
		CBasicRectangle3D() {}
		CBasicRectangle3D(const CBasicRectangle3D<T>& Rect);

		T GetSizeZ()const;

		T GetMaxZ()const;
		T GetMinZ()const;

		T& GetMaxZ();
		T& GetMinZ();

		void Set(const T& MinX, const T& MaxX, const T& MinY, const T& MaxY, const T& MinZ, const T& MaxZ);


		CBasicRectangle3D<T>& operator  =(const CBasicRectangle3D<T>& Rect);
		CBasicRectangle3D<T>& operator +=(const CBasicRectangle3D<T>& Rect);
		CBasicRectangle3D<T>& operator *=(const CBasicRectangle3D<T>& Rect);

		//判定矩形是否有价值的,即最大值大于最小值返回true,否则false.
		bool IsValuable()const;

	protected:
		T m_MaxZ;
		T m_MinZ;
	};

	template<typename T>
	inline CBasicRectangle3D<T>::CBasicRectangle3D(const CBasicRectangle3D<T>& Rect)
	{
		m_MaxX = Rect.m_MaxX;
		m_MinX = Rect.m_MinX;
		m_MaxY = Rect.m_MaxY;
		m_MinY = Rect.m_MinY;
		m_MaxZ = Rect.m_MaxZ;
		m_MinZ = Rect.m_MinZ;
	}

	template<typename T>
	inline T CBasicRectangle3D<T>::GetSizeZ()const
	{
		return m_MaxZ - m_MaxZ;
	}

	template<typename T>
	inline T CBasicRectangle3D<T>::GetMaxZ()const
	{
		return m_MaxZ;
	}

	template<typename T>
	inline T CBasicRectangle3D<T>::GetMinZ()const
	{
		return m_MinZ;
	}

	template<typename T>
	inline T& CBasicRectangle3D<T>::GetMaxZ()
	{
		return m_MaxZ;
	}

	template<typename T>
	inline T& CBasicRectangle3D<T>::GetMinZ()
	{
		return m_MinZ;
	}

	template<typename T>
	inline void CBasicRectangle3D<T>::Set(const T& MinX, const T& MaxX, const T& MinY, const T& MaxY, const T& MinZ, const T& MaxZ)
	{
		m_MinX = MinX;
		m_MaxX = MaxX;

		m_MinY = MinY;
		m_MaxY = MaxY;

		m_MinZ = MinZ;
		m_MaxZ = MaxZ;
	}

	template<typename T>
	inline CBasicRectangle3D<T>& CBasicRectangle3D<T>::operator =(const CBasicRectangle3D<T>& Rect)
	{
		m_MaxX = Rect.m_MaxX;
		m_MinX = Rect.m_MinX;
		m_MaxY = Rect.m_MaxY;
		m_MinY = Rect.m_MinY;
		m_MaxZ = Rect.m_MaxZ;
		m_MinZ = Rect.m_MinZ;

		return *this;
	}

	template<typename T>
	inline CBasicRectangle3D<T>& CBasicRectangle3D<T>::operator *=(const CBasicRectangle3D<T>& Rect)
	{
		m_MaxX *= Rect.m_MaxX;
		m_MinX *= Rect.m_MinX;
		m_MaxY *= Rect.m_MaxY;
		m_MinY *= Rect.m_MinY;
		m_MaxZ *= Rect.m_MaxZ;
		m_MinZ *= Rect.m_MinZ;

		return *this;
	}

	template<typename T>
	inline bool CBasicRectangle3D<T>::IsValuable()const
	{
		return m_MaxX > m_MinX && m_MaxY > m_MinX && m_MaxZ > m_MinZ;
	}

	///
	// 矩形,FLOAT型
	///
	class CRectangle3D :
		public CBasicRectangle3D<FLOAT>
	{
	public:
		typedef enum
		{
			PLANE_NONE      = 0,

			PLANE_FRONT     = 0x01,
			PLANE_BACK      = 0x02,
			PLANE_INTERSECT = 0x04
		}TESTPLANESTATE;
		CRectangle3D(void);
		~CRectangle3D(void);

		void GetSize(D3DXVECTOR3& Size)const;
		void GetMin(D3DXVECTOR3& Min)const;
		void GetMax(D3DXVECTOR3& Max)const;

		//确认矩形包含一个点,否则扩展矩形知道它包含这个点为止.
		void Union(const D3DXVECTOR3 Point);
		void Transform(const D3DXMATRIX& Matrix);

		//测试矩形在平面的什么位置
		//返回:
		//PLANE_FRONT     在平面前面
		//PLANE_BACK      在平面背面
		//PLANE_INTERSECT 与平面相交
		TESTPLANESTATE Test(const D3DXPLANE& Plane)const;

		//检测两个矩形是否碰撞
		bool TestHit(const CRectangle3D& Rect)const;

		CRectangle3D& operator +=(const D3DXVECTOR3& v);
		CRectangle3D& operator *=(const D3DXVECTOR3& v);
	};

	inline void CRectangle3D::GetSize(D3DXVECTOR3& Size)const
	{
		Size.x = GetSizeX();
		Size.y = GetSizeY();
		Size.z = GetSizeZ();
	}

	inline void CRectangle3D::GetMin(D3DXVECTOR3& Min)const
	{
		Min.x = m_MinX;
		Min.y = m_MinY;
		Min.z = m_MinZ;
	}

	inline void CRectangle3D::GetMax(D3DXVECTOR3& Max)const
	{
		Max.x = m_MaxX;
		Max.y = m_MaxY;
		Max.z = m_MaxZ;
	}

	inline void CRectangle3D::Union(const D3DXVECTOR3 Point)
	{
		m_MaxX = MAX(Point.x, m_MaxX);
		m_MaxY = MAX(Point.y, m_MaxY);
		m_MaxZ = MAX(Point.z, m_MaxZ);

		m_MinX = MAX(Point.x, m_MinX);
		m_MinY = MAX(Point.y, m_MinY);
		m_MinZ = MAX(Point.z, m_MinZ);
	}

	inline bool CRectangle3D::TestHit(const CRectangle3D& Rect)const
	{
		CRectangle3D Result;

		Result.m_MaxX = MIN(Rect.m_MaxX, m_MaxX);
		Result.m_MaxY = MIN(Rect.m_MaxY, m_MaxY);
		Result.m_MaxZ = MIN(Rect.m_MaxZ, m_MaxZ);

		Result.m_MinX = MAX(Rect.m_MinX, m_MinX);
		Result.m_MinY = MAX(Rect.m_MinY, m_MinY);
		Result.m_MinZ = MAX(Rect.m_MinZ, m_MinZ);

		return Result.IsValuable();
	}

	inline CRectangle3D& CRectangle3D::operator +=(const D3DXVECTOR3& v)
	{
		m_MaxX += v.x;
		m_MinX += v.x;
		m_MaxY += v.y;
		m_MinY += v.y;
		m_MaxZ += v.z;
		m_MinZ += v.z;

		return *this;
	}

	inline CRectangle3D& CRectangle3D::operator *=(const D3DXVECTOR3& v)
	{
		m_MaxX *= v.x;
		m_MinX *= v.x;
		m_MaxY *= v.y;
		m_MinY *= v.y;
		m_MaxZ *= v.z;
		m_MinZ *= v.z;

		return *this;
	}
}
