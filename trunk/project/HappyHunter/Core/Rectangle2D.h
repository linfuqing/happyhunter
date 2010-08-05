#pragma once

#include "basicmath.h"

namespace zerO
{
	///
	//2D¾ØÐÎÄ£°å
	///
	template<typename T>
	class CBasicRectangle2D
	{
	public:
		CBasicRectangle2D(void) {}
		CBasicRectangle2D(const CBasicRectangle2D<T>& Rect);

		T GetSizeX()const;
		T GetSizeY()const;

		T GetMaxX()const;
		T GetMinX()const;

		T GetMaxY()const;
		T GetMinY()const;

		T& GetMaxX();
		T& GetMinX();

		T& GetMaxY();
		T& GetMinY();

		void Set(const T& MinX, const T& MaxX, const T& MinY, const T& MaxY);
		virtual bool IsValuable()const;

		CBasicRectangle2D<T>& operator  =(const CBasicRectangle2D<T>& Rect);
		CBasicRectangle2D<T>& operator +=(const CBasicRectangle2D<T>& Rect);
		CBasicRectangle2D<T>& operator *=(const CBasicRectangle2D<T>& Rect);
	protected:
		T m_MaxX;
		T m_MinX;
		T m_MaxY;
		T m_MinY;
	};

	template<typename T>
	inline CBasicRectangle2D<T>::CBasicRectangle2D(const CBasicRectangle2D<T>& Rect)
	{
		m_MaxX = Rect.m_MaxX;
		m_MinX = Rect.m_MinX;
		m_MaxY = Rect.m_MaxY;
		m_MinY = Rect.m_MinY;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetSizeX()const
	{
		return m_MaxX - m_MinX;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetSizeY()const
	{
		return m_MaxY - m_MinY;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetMaxX()const
	{
		return m_MaxX;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetMinX()const
	{
		return m_MinX;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetMaxY()const
	{
		return m_MaxY;
	}

	template<typename T>
	inline T CBasicRectangle2D<T>::GetMinY()const
	{
		return m_MinY;
	}

	template<typename T>
	inline T& CBasicRectangle2D<T>::GetMaxX()
	{
		return m_MaxX;
	}

	template<typename T>
	inline T& CBasicRectangle2D<T>::GetMinX()
	{
		return m_MinX;
	}

	template<typename T>
	inline T& CBasicRectangle2D<T>::GetMaxY()
	{
		return m_MaxY;
	}

	template<typename T>
	inline T& CBasicRectangle2D<T>::GetMinY()
	{
		return m_MinY;
	}

	template<typename T>
	inline void CBasicRectangle2D<T>::Set(const T& MinX, const T& MaxX, const T& MinY, const T& MaxY)
	{
		m_MinX = MinX;
		m_MaxX = MaxX;

		m_MinY = MinY;
		m_MaxY = MaxY;
	}

	template<typename T>
	inline CBasicRectangle2D<T>& CBasicRectangle2D<T>::operator =(const CBasicRectangle2D<T>& Rect)
	{
		m_MaxX = Rect.m_MaxX;
		m_MinX = Rect.m_MinX;
		m_MaxY = Rect.m_MaxY;
		m_MinY = Rect.m_MinY;

		return *this;
	}

	template<typename T>
	inline CBasicRectangle2D<T>& CBasicRectangle2D<T>::operator +=(const CBasicRectangle2D<T>& Rect)
	{
		m_MaxX += Rect.m_MaxX;
		m_MinX += Rect.m_MinX;
		m_MaxY += Rect.m_MaxY;
		m_MinY += Rect.m_MinY;

		return *this;
	}

	template<typename T>
	inline CBasicRectangle2D<T>& CBasicRectangle2D<T>::operator *=(const CBasicRectangle2D<T>& Rect)
	{
		m_MaxX *= Rect.m_MaxX;
		m_MinX *= Rect.m_MinX;
		m_MaxY *= Rect.m_MaxY;
		m_MinY *= Rect.m_MinY;

		return *this;
	}

	///
	//2D¾ØÐÎ
	///
	template<typename T>
	inline bool CBasicRectangle2D<T>::IsValuable()const
	{
		return m_MaxX > m_MinX && m_MaxY > m_MinX;
	}

	class CRectangle2D : 
		public CBasicRectangle2D<FLOAT>
	{
	public:
		CRectangle2D(void);
		~CRectangle2D(void);
	};
}
