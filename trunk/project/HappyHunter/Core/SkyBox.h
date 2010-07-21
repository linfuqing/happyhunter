#pragma once

//#include "SceneNode.h"
#include "Background.h"
#include "VertexBuffer.h"
#include "RenderMethod.h"

namespace zerO
{
	class CSkyBox :
		public CBackground//CSceneNode
	{
		typedef struct
		{
			FLOAT x, y, z, u, v;
		}VERTEX;

		typedef struct
		{
			VERTEX Vertices[4];
		}FACE;

		typedef struct
		{
			FACE Top, Bottom, Left, Right, Front, Back;
		}BOX;
	public:
		CSkyBox(void);
		~CSkyBox(void);

		CRenderMethod& GetRenderMethod();

		void SetCloudSpeed(FLOAT x, FLOAT y);

		bool Create(FLOAT fSize);

		void Update();

		void Render();
	private:
		CRenderMethod  m_RenderMethod;
		CVertexBuffer m_VertexBuffer;
		FLOAT         m_fHalfSize;
		FLOAT         m_fCloudSpeedX;
		FLOAT         m_fCloudSpeedY;
	};

	inline CRenderMethod& CSkyBox::GetRenderMethod()
	{
		return m_RenderMethod;
	}

	inline void CSkyBox::SetCloudSpeed(FLOAT x, FLOAT y)
	{
		m_fCloudSpeedX = x;
		m_fCloudSpeedY = y;
	}
}
