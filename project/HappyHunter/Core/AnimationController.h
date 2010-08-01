#pragma once

#include "Resource.h"

namespace zerO
{
	class CAnimationController :
		public CResource
	{
	public:
		CAnimationController(void);
		~CAnimationController(void);

		bool Create(const LPD3DXANIMATIONCONTROLLER pAnimationController);
		bool Destroy(); 
		bool Disable(); 
		bool Restore();

		void Update();

		LPD3DXANIMATIONCONTROLLER GetAnimationController()const;

		// 通过索引设定动作
		void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true, FLOAT fSmoothTranstionTime = 0.25f );
		// 通过名称设定动作
		void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true, FLOAT fSmoothTranstionTime = 0.25f );
		// 动作平滑过渡
		void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet, FLOAT fSmoothTranstionTime = 0.25f);
		// 得到当前正在播放的动画的播放次数
		UINT GetPlayTime();
		// 从名称获取动作ID
		UINT GetAnimIndex( char sString[] );
		// 动画是否能播放
		bool CanPlay(LPSTR pName, DWORD count);

	private:
		LPD3DXANIMATIONCONTROLLER	m_pAnimController;
		bool						m_bPlayAnim;
		
		UINT						m_uCurrentTrack;		// 当前动作
		FLOAT						m_fTimeCurrent;			// 当前时间	
		UINT						m_uPlayTime;			// 当前动作播放次数	
		FLOAT						m_fFrameTime;			// 当前动画播放时间	
		FLOAT64						m_lfTotalFrameTime;		// 当前动画总播放时间
		UINT						m_uControlPlayTime;		// 当前动作设定播放次数(0为重复播放)　
		char						m_szASName[64];			// 动作名称
		std::string					m_strNowAnimSetName;	// 当前动作名称
	};

	inline LPD3DXANIMATIONCONTROLLER CAnimationController::GetAnimationController()const
	{
		return m_pAnimController;
	}
}
