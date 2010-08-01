#pragma once

#include "Resource.h"
#include "Surface.h"
#include "Rectangle3D.h"

namespace zerO
{
	class CMesh :
		public CResource
	{
		typedef enum
		{
			TEXTURE = 0,
			NORMAL
		}MAPTYPE;
	public:
		CMesh(void);
		~CMesh(void);

		UINT GetSurfacesNumber()const;
		CSurface* GetSurfaces()const;

		LPD3DXMESH GetMesh()const;

		const CRectangle3D& GetRectangle()const;

		void SetNormalMap(CTexture* pTexture);

		bool Load(const PBASICCHAR pcFileName = NULL);
		bool Destroy();

	private:
		bool __GenerateDeclMesh(LPD3DXMESH& pMesh);
		bool __GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& Rect3d);

		LPD3DXMESH   m_pMesh;
		UINT         m_uNumSurfaces;
		CSurface*    m_pSurfaces;
		CRectangle3D m_Rectangle;
	};

	inline UINT CMesh::GetSurfacesNumber()const
	{
		return m_uNumSurfaces;
	}

	inline CSurface* CMesh::GetSurfaces()const
	{
		return m_pSurfaces;
	}

	inline LPD3DXMESH CMesh::GetMesh()const
	{
		return m_pMesh;
	}

	inline const CRectangle3D& CMesh::GetRectangle()const
	{
		return m_Rectangle;
	}
}
