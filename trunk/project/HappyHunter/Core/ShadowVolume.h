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

		void SetTransform(const D3DXMATRIX Matrix);

		void SetMeshData(PUINT8 pVertices, PUINT16 pIndices, UINT uVertexSize);
		void SetMeshData(ID3DXMesh& Mesh);

		bool Create(ID3DXMesh& Mesh, CSceneNode& Node);
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
		D3DXVECTOR3* m_pShadowVertices;  //用于渲染阴影体的顶点数据

		UINT         m_uVertexSize;
		UINT         m_uIndexSize;

		UINT         m_uNumVertices;
		UINT         m_uNumTriangles;
		UINT         m_uNumEdges;
		UINT         m_uNumShadowVertices;

		D3DXMATRIX   m_Matrix;

		CSceneNode*  m_pParent;
		bool         m_bIsRenderVolume;
	};

	inline void CShadowVolume::SetTransform(const D3DXMATRIX Matrix)
	{
		m_Matrix = Matrix;
	}
}