//=============================================================================
// Desc: ShadowVolume.h
//=============================================================================

#pragma once

#include "SceneNode.h"
#include "Shadow.h"

namespace zerO
{
	class CShadowVolume :
		public CShadow
	{
		typedef struct
		{
			UINT         v0;
			UINT         v1;
			UINT         v2;

			D3DXVECTOR3  Normal;
		}TRIANGLE, * LPTRIANGLE;

		typedef struct
		{
			UINT v0;
			UINT v1;
		}EDGE, * LPEDGE;
	public:
		CShadowVolume(void);
		~CShadowVolume(void);

		void SetMeshData(PUINT8 pVertices, PUINT16 pIndices, UINT uVertexSize);

		bool Create(ID3DXMesh& Mesh, CSceneNode& Noe);
		bool Create(UINT uNumVertices, UINT uNumTriangles, CSceneNode& Parent, UINT uVertexSize, void* pVertices = NULL, void* pIndices = NULL);
		void Update();
		void Render();
		bool Destroy();

	private:
		void __AddEdge(UINT v0, UINT v1);

		PUINT8       m_pVertices;
		PUINT16      m_pIndices;

		LPTRIANGLE   m_pTriangles;
		LPEDGE       m_pEdges;
		D3DXVECTOR3* m_pShadowVertices;  //������Ⱦ��Ӱ��Ķ�������

		UINT         m_uVertexSize;
		UINT         m_uIndexSize;

		UINT         m_uNumVertices;
		UINT         m_uNumTriangles;
		UINT         m_uNumEdges;
		UINT         m_uNumShadowVertices;

		CSceneNode*  m_pParent;
		bool         m_bIsRenderVolume;
	};
}