#pragma once

#include "color.h"
#include "BitFlag.h"
#include "debug.h"

namespace zerO
{
	class CLightManager
	{
	public:
		typedef enum
		{
			MAXINUM_LIGHTS_NUMBER = 8
		}CONSTANTS;

		CLightManager(void);
		~CLightManager(void);

		const D3DLIGHT9* GetLight(UINT uIndex)const;
		ARGBCOLOR GetAmbient()const;
		void GetAmbientRGBA(D3DXVECTOR4& Color)const;

		void SetLight(const D3DLIGHT9& Light, UINT uIndex);
		void SetAmbient(ARGBCOLOR Color);

		void Activate();
	private:
		ARGBCOLOR m_Ambient;
		D3DLIGHT9 m_Light[MAXINUM_LIGHTS_NUMBER];
		UINT8     m_uFlag;
		UINT      m_uNumLights;
	};

	inline const D3DLIGHT9* CLightManager::GetLight(UINT uIndex)const
	{
		DEBUG_ASSERT(uIndex < MAXINUM_LIGHTS_NUMBER, "uIndex must be less than MAXINUM_LIGHTS_NUMBER.");

		if( TEST_BIT(m_uFlag, uIndex) )
			return &m_Light[uIndex];

		return NULL;
	}

	inline ARGBCOLOR CLightManager::GetAmbient()const
	{
		return m_Ambient;
	}

	inline void CLightManager::GetAmbientRGBA(D3DXVECTOR4& Color)const
	{
		Color.x = ( (m_Ambient >> 16) & 0xff ) / 255.0f;
		Color.y = ( (m_Ambient >>  8) & 0xff ) / 255.0f;
		Color.z = ( (m_Ambient      ) & 0xff ) / 255.0f;
		Color.w = ( (m_Ambient >> 24) & 0xff ) / 255.0f;
	}

	inline void CLightManager::SetLight(const D3DLIGHT9& Light, UINT uIndex)
	{
		DEBUG_ASSERT(uIndex < MAXINUM_LIGHTS_NUMBER, "uIndex must be less than MAXINUM_LIGHTS_NUMBER.");

		memcpy(&m_Light[uIndex], &Light, sizeof(D3DLIGHT9) );
		SET_BIT(m_uFlag, uIndex);

		m_uNumLights ++;
	}

	inline void CLightManager::SetAmbient(ARGBCOLOR Color)
	{
		m_Ambient = Color;
	}
}
