#include "StdAfx.h"
#include "AnimationController.h"

using namespace zerO;
CAnimationController::CAnimationController(void) :
CResource(RESOURCE_ANIMATIONCONTROLLER),
m_bPlayAnim(true),
m_pAnimController(NULL),
m_uCurrentTrack(0),
m_fTimeCurrent(0.0f),
m_uPlayTime(0),
m_fFrameTime(0.0f),
m_lfTotalFrameTime(0.0),
m_uControlPlayTime(0)
{
}

CAnimationController::~CAnimationController(void)
{
	Destroy();
}

bool CAnimationController::Create(const LPD3DXANIMATIONCONTROLLER pAnimationController)
{
	HRESULT hr = pAnimationController->CloneAnimationController(
		pAnimationController->GetMaxNumAnimationOutputs(),
		pAnimationController->GetMaxNumAnimationSets(),
		pAnimationController->GetMaxNumTracks(),
		pAnimationController->GetMaxNumEvents(),
		&m_pAnimController);

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
}

bool CAnimationController::Destroy()
{
    DEBUG_RELEASE(m_pAnimController);
	m_pAnimController = NULL;

	return true;
}

bool CAnimationController::Restore()
{
	if (m_pAnimController == NULL)
		return false;

	m_pAnimController->ResetTime();
	m_pAnimController->AdvanceTime( m_fTimeCurrent, NULL );

	// Initialize current track
	if( m_szASName[0] != '\0' )
	{
		DWORD dwActiveSet = GetAnimIndex( m_szASName );
		LPD3DXANIMATIONSET pAS = NULL;
		m_pAnimController->GetAnimationSet( dwActiveSet, &pAS );
		m_pAnimController->SetTrackAnimationSet( m_uCurrentTrack, pAS );
		DEBUG_RELEASE( pAS );
	}

	m_pAnimController->SetTrackEnable( m_uCurrentTrack, TRUE );
	m_pAnimController->SetTrackWeight( m_uCurrentTrack, 1.0f );
	m_pAnimController->SetTrackSpeed( m_uCurrentTrack, 1.0f );

	return true;
}

bool CAnimationController::Disable()
{
	if (m_pAnimController == NULL)
		return false;

	LPD3DXANIMATIONSET pAS = NULL;
	m_pAnimController->GetTrackAnimationSet(m_uCurrentTrack, &pAS);
	if( pAS->GetName() )
		strcpy( m_szASName, pAS->GetName() );
	DEBUG_RELEASE(pAS);

	return true;
}


void CAnimationController::Update()
{
	FLOAT fElapsedAppTime = ELAPSEDTIME;

	if( 0.0f == fElapsedAppTime ) 
        return;
	
	m_fTimeCurrent += fElapsedAppTime;

	if (m_bPlayAnim && m_pAnimController != NULL)
	{
		if(m_uControlPlayTime == 0 || m_uPlayTime < m_uControlPlayTime)
			m_pAnimController->AdvanceTime( fElapsedAppTime, NULL );

		// 计算动画播放次数
		m_fFrameTime += fElapsedAppTime;
		if(m_fFrameTime >= m_lfTotalFrameTime)
		{
			m_fFrameTime = 0.0f;
			m_uPlayTime++;
		}
	}
}

void CAnimationController::SetAnimation( zerO::UINT index, DWORD dwControlPlayTime, bool bSmooth, zerO::FLOAT fSmoothTranstionTime )
{
	if( index >= m_pAnimController->GetNumAnimationSets() )
		return;

	LPD3DXANIMATIONSET pAnimSet = NULL;
	m_pAnimController->GetAnimationSet( index, &pAnimSet );

	if(pAnimSet)
	{
		if(!strcmp(m_strNowAnimSetName.c_str(), pAnimSet->GetName()))
			return;

		m_strNowAnimSetName = pAnimSet->GetName();
	}

	m_uPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_uControlPlayTime = dwControlPlayTime;

	if( pAnimSet )
	{
		m_lfTotalFrameTime = pAnimSet->GetPeriod();
		if(bSmooth)
			AnimationChangeSmooth(pAnimSet, fSmoothTranstionTime);
		else
		{
			DWORD dwNewTrack = 0;
			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
		}
		pAnimSet->Release();
	}
}

void CAnimationController::SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime, bool bSmooth, zerO::FLOAT fSmoothTranstionTime )
{
	if(m_pAnimController == NULL)
		return;
	
	if(!strcmp(m_strNowAnimSetName.c_str(), pName))
		return;
	
	m_strNowAnimSetName = pName;

	m_uPlayTime = 0;
	m_fFrameTime = 0.0f;
	m_uControlPlayTime = dwControlPlayTime;

	LPD3DXANIMATIONSET pAnimSet = NULL;
	m_pAnimController->SetTrackPosition(0, 0.0);
	m_pAnimController->GetAnimationSetByName( pName, &pAnimSet );
	if( pAnimSet )
	{
		m_lfTotalFrameTime = pAnimSet->GetPeriod();
		if(bSmooth)
			AnimationChangeSmooth(pAnimSet, fSmoothTranstionTime);
		else
		{
			DWORD dwNewTrack = 0;
			m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );
			m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
		}
		pAnimSet->Release();
	}
}

void CAnimationController::AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet, zerO::FLOAT fSmoothTranstionTime)
{
	DWORD dwNewTrack = ( m_uCurrentTrack == 0 ? 1 : 0 );

	m_pAnimController->SetTrackAnimationSet( dwNewTrack, pAnimSet );

	m_pAnimController->UnkeyAllTrackEvents( m_uCurrentTrack );
	m_pAnimController->UnkeyAllTrackEvents( dwNewTrack );

	m_pAnimController->KeyTrackEnable( m_uCurrentTrack, FALSE, m_fTimeCurrent + fSmoothTranstionTime );
	m_pAnimController->KeyTrackSpeed( m_uCurrentTrack, 0.0f, m_fTimeCurrent, fSmoothTranstionTime, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( m_uCurrentTrack, 0.0f, m_fTimeCurrent, fSmoothTranstionTime, D3DXTRANSITION_LINEAR );
	m_pAnimController->SetTrackEnable( dwNewTrack, TRUE );
	m_pAnimController->KeyTrackSpeed( dwNewTrack, 1.0f, m_fTimeCurrent, fSmoothTranstionTime, D3DXTRANSITION_LINEAR );
	m_pAnimController->KeyTrackWeight( dwNewTrack, 1.0f, m_fTimeCurrent, fSmoothTranstionTime, D3DXTRANSITION_LINEAR );

	m_uCurrentTrack = dwNewTrack;
}

zerO::UINT CAnimationController::GetPlayTime() 
{ 
	// 如果没有动画控制器，则默认播放了一次
	if(m_pAnimController == NULL)
		m_uPlayTime = 1;

	return m_uPlayTime; 
}

zerO::UINT CAnimationController::GetAnimIndex( char sString[] )
{
	HRESULT hr;
	LPD3DXANIMATIONSET pAS;
	zerO::UINT dwRet = 0xffffffff;

	for( zerO::UINT i = 0; i < m_pAnimController->GetNumAnimationSets(); ++ i )
	{
		hr = m_pAnimController->GetAnimationSet( i, & pAS );
		if( FAILED( hr ) )
			continue;

		if( pAS->GetName() &&
			!strncmp( pAS->GetName(), sString, min( strlen( pAS->GetName() ), strlen( sString ) ) ) )
		{
			dwRet = i;
			pAS->Release();
			break;
		}

		pAS->Release();
	}

	return dwRet;
}

bool CAnimationController::CanPlay(LPSTR pName, DWORD count)
{
	if (strcmp(m_strNowAnimSetName.c_str(), pName) == 0 
		|| GetPlayTime() >= count)
		return true;

	return false;
}