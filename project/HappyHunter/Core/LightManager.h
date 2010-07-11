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

		void SetLight(const D3DLIGHT9& Light, UINT uIndex);
		void SetAmbient(ARPGCOLOR Color);

		void Activate();
	private:
		ARPGCOLOR m_Ambient;
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

	inline void CLightManager::SetLight(const D3DLIGHT9& Light, UINT uIndex)
	{
		DEBUG_ASSERT(uIndex < MAXINUM_LIGHTS_NUMBER, "uIndex must be less than MAXINUM_LIGHTS_NUMBER.");

		memcpy(&m_Light[uIndex], &Light, sizeof(D3DLIGHT9) );
		SET_BIT(m_uFlag, uIndex);

		m_uNumLights ++;
	}

	inline void CLightManager::SetAmbient(ARPGCOLOR Color)
	{
		m_Ambient = Color;
	}
}
