#pragma once

#include "Gamehost.h"

namespace zerO
{
	class CShadow
	{
	public:
		CShadow(void)
		{
			GAMEHOST.AddShadow(this);
		}

		~CShadow(void)
		{
			GAMEHOST.RemoveShadow(this);
		}

		virtual void Update() = 0;
		virtual void Render() = 0;
		virtual bool Destroy() = 0;
	};
}
