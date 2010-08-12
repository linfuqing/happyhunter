#pragma once

#include "color.h"

namespace zerO
{
	class CFogManager
	{
		typedef enum
		{
			NONE   = D3DFOG_NONE,
			EXP    = D3DFOG_EXP,
			EXP2   = D3DFOG_EXP2,
			LINEAR = D3DFOG_LINEAR,
		}FOGMODEL;
	public:
		typedef enum
		{
			VERTEX = D3DRS_FOGVERTEXMODE,
			TABLE  = D3DRS_FOGTABLEMODE
		}FOGTYPE;

		CFogManager(void);
		~CFogManager(void);

		void SetType(FOGTYPE Type);
		void SetColor(ARGBCOLOR Color);
		void SetLinearFog(FLOAT fStart, FLOAT fEnd);
		void SetExpFog(FLOAT fDensity);
		void SetExp2Fog(FLOAT fDensity);
		void SetRangefogEnable(bool bValue);

		void Activate();
	private:
		FOGTYPE m_Type;
		FOGMODEL m_Model;
		COLOR   m_Color;
		FLOAT   m_fStart;
		FLOAT   m_fEnd;
		FLOAT   m_fDensity;
		bool    m_bRangefogEnable;
	};

	inline void CFogManager::SetType(FOGTYPE Type)
	{
		m_Type = Type;
	}

	inline void CFogManager::SetColor(ARGBCOLOR Color)
	{
		m_Color = Color;
	}

	inline void CFogManager::SetLinearFog(FLOAT fStart, FLOAT fEnd)
	{
		m_Model  = LINEAR;

		m_fStart = fStart;
		m_fEnd   = fEnd;
	}

	inline void CFogManager::SetExpFog(FLOAT fDensity)
	{
		m_Model    = EXP;
		m_fDensity = fDensity;
	}

	inline void CFogManager::SetExp2Fog(FLOAT fDensity)
	{
		m_Model    = EXP2;
		m_fDensity = fDensity;
	}

	inline void CFogManager::SetRangefogEnable(bool bValue)
	{
		m_bRangefogEnable = bValue;
	}
}