#pragma once

#include "SceneNode.h"
#include "VertexBuffer.h"
#include "RenderMethod.h"

namespace zerO
{
	class CBillboarding :
		public CSceneNode
	{
		typedef struct
		{
			D3DXVECTOR3 Position;
			D3DXVECTOR2 UV;
		}VERTEX;
	public:
		typedef enum
		{
			UNLOCK = 0,
			LOCK_XYZ
		}TYPE;
		CBillboarding(void);
		~CBillboarding(void);

		CRenderMethod& GetRenderMethod();

		void SetupAnimation(UINT uFramesPerAnimation, UINT uLength);

		void Play(UINT uIndex = 0);
		void Stop();
		void Pause();

		bool Create(FLOAT fWidth, FLOAT fHeight);

		void Update();

		bool ApplyForRender();

		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

	private:
		FLOAT         m_fHalfWidth;
		FLOAT         m_fHalfHeight;
		FLOAT         m_fOffsetU;
		FLOAT         m_fOffsetV;

		CRectangle2D  m_CurrentUV;
		CRectangle2D  m_MaxUV;

		bool          m_bIsPlay;

		TYPE          m_Type;

		VERTEX        m_RenderData[4];
		CVertexBuffer m_VertexBuffer;
		CRenderMethod m_RenderMethod;
	};

	inline CRenderMethod& CBillboarding::GetRenderMethod()
	{
		return m_RenderMethod;
	}

	inline void CBillboarding::SetupAnimation(zerO::UINT uFramesPerAnimation, zerO::UINT uLength)
	{
		m_fOffsetU = 1.0f / uFramesPerAnimation;
		m_fOffsetV = 1.0f / uLength;

		m_MaxUV.Set(0.0f, 1.0f, 0.0f, m_fOffsetV);
	}

	inline void CBillboarding::Play(UINT uIndex)
	{
		m_bIsPlay = true;

		if(uIndex)
		{
			m_MaxUV.Set(0.0f, 1.0f, m_fOffsetV * (uIndex - 1), m_fOffsetV * uIndex);
			m_CurrentUV.Set(0.0f, m_fOffsetU, 0.0f, m_fOffsetV); 
		}
	}

	inline void CBillboarding::Stop()
	{
		m_bIsPlay = false;

		m_CurrentUV.Set(0.0f, m_fOffsetU, 0.0f, m_fOffsetV);
	}

	inline void CBillboarding::Pause()
	{
		m_bIsPlay = true;
	}
}
