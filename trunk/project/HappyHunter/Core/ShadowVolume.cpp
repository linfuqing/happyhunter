//=============================================================================
// Desc: ShadowVolume.cpp
//=============================================================================

#include "stdafx.h"
#include "ShadowVolume.h"
#include "basicutils.h"

using namespace zerO;


CShadowVolume::CShadowVolume(void) :
m_bIsRenderVolume(false)
{
}

CShadowVolume::~CShadowVolume(void)
{
	Destroy();
}

//-----------------------------------------------------------------------------
// Desc: 创建阴影体
//-----------------------------------------------------------------------------
bool CShadowVolume::Create(zerO::UINT uNumVertices, zerO::UINT uNumTriangles, CSceneNode& Parent, zerO::UINT uVertexSize, void* pVertices, void* pIndices)
{
	m_uNumVertices  = uNumVertices;
	m_uNumTriangles = uNumTriangles;

	m_pParent       = &Parent;

	//为阴影体顶点分配存储空间
	DEBUG_NEW(m_pShadowVertices, D3DXVECTOR3[m_uNumTriangles * 3 * 6]);
	DEBUG_NEW(m_pEdges         , EDGE       [m_uNumTriangles * 3    ]);
	DEBUG_NEW(m_pTriangles     , TRIANGLE   [m_uNumTriangles        ]);

	SetMeshData( (PUINT8)pVertices, (PUINT16)pIndices, uVertexSize );

	return true;
}

bool CShadowVolume::Create(ID3DXMesh& Mesh, CSceneNode& Node)
{
	void* pVertices, *pIndices;

	Mesh.LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    Mesh.LockIndexBuffer( 0L, (LPVOID*)&pIndices );

	Mesh.UnlockVertexBuffer();
    Mesh.UnlockIndexBuffer();

	return Create(
		Mesh.GetNumVertices(), 
		Mesh.GetNumFaces(),
		Node,
		Mesh.GetNumBytesPerVertex(),
		pVertices,
		pIndices);
}

void CShadowVolume::SetMeshData(zerO::PUINT8 pVertices, zerO::PUINT16 pIndices, zerO::UINT uVertexSize)
{
	m_pVertices   = pVertices;
	m_pIndices    = pIndices;

	m_uVertexSize = uVertexSize;

	if(!pVertices || !pIndices)
		return;

	zerO::UINT i0, i1, i2, i, j = 0;

	D3DXVECTOR3 v0, v1, v2;

	//遍历网格模型的每个面(三角形), 将网格模型的轮廓边添加到临时边列表中
    for(i = 0; i < m_uNumTriangles; i ++)
    {
		//当前面三个顶点的索引
        i0 = pIndices[j ++] * uVertexSize;
        i1 = pIndices[j ++] * uVertexSize;
        i2 = pIndices[j ++] * uVertexSize;

		v0 = CASE(pVertices[i0], D3DXVECTOR3);
		v1 = CASE(pVertices[i1], D3DXVECTOR3);
		v2 = CASE(pVertices[i2], D3DXVECTOR3);

		//根据顶点索引获取当前面三个顶点的坐标
        m_pTriangles[i].v0 = i0;
        m_pTriangles[i].v1 = i1;
        m_pTriangles[i].v2 = i2;

        D3DXVec3Cross( &m_pTriangles[i].Normal, &(v2 - v1), &(v1 - v0) );
    }
}

void CShadowVolume::SetMeshData(ID3DXMesh& Mesh)
{
	void* pVertices, *pIndices;

	Mesh.LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    Mesh.LockIndexBuffer( 0L, (LPVOID*)&pIndices );

	SetMeshData( (PUINT8)pVertices, (PUINT16)pIndices, Mesh.GetNumBytesPerVertex() );

	Mesh.UnlockVertexBuffer();
    Mesh.UnlockIndexBuffer();
}

//-----------------------------------------------------------------------------
// Desc: 根据网格模型和光源位置构造阴影体
//       首先检查网格模型的每个三角形, 将网格模型的轮廓边保存到一个临时列表中.
//       然后根据轮廓边和光源位置构造阴影体
//-----------------------------------------------------------------------------
void CShadowVolume::Update()
{
	m_uNumEdges = 0;

	D3DXVECTOR3 LightPosition(LIGHTMANAGER.GetLight(0)->Position);

	D3DXMATRIX InverseWorldMatrix;
    D3DXMatrixInverse( &InverseWorldMatrix, NULL, &m_pParent->GetWorldMatrix() );

	D3DXVec3TransformCoord(&LightPosition, &LightPosition, &InverseWorldMatrix);

	D3DXVECTOR3 v1, v2, v3, v4, Extend = LightPosition * 500;

    DWORD i;

	m_uNumShadowVertices = 0;
    //遍历网格模型的每个面(三角形), 将网格模型的轮廓边添加到临时边列表中
    for(i = 0; i < m_uNumTriangles; i ++)
    {
		//如果当前面是背光面, 则将当前面的三条边"添加"到边列表
        if( D3DXVec3Dot(&m_pTriangles[i].Normal, &LightPosition) >= 0.0f )
        {
			__AddEdge(m_pTriangles[i].v0, m_pTriangles[i].v1);
            __AddEdge(m_pTriangles[i].v1, m_pTriangles[i].v2);
            __AddEdge(m_pTriangles[i].v2, m_pTriangles[i].v0);

			v1 = CASE(m_pVertices[m_pTriangles[i].v0], D3DXVECTOR3);
			v2 = CASE(m_pVertices[m_pTriangles[i].v1], D3DXVECTOR3);
			v3 = CASE(m_pVertices[m_pTriangles[i].v2], D3DXVECTOR3);

			/*m_pShadowVertices[m_uNumShadowVertices ++] = v2;
			m_pShadowVertices[m_uNumShadowVertices ++] = v1;
			m_pShadowVertices[m_uNumShadowVertices ++] = v3;

			m_pShadowVertices[m_uNumShadowVertices ++] = v1 + Extend;
			m_pShadowVertices[m_uNumShadowVertices ++] = v2 + Extend;
			m_pShadowVertices[m_uNumShadowVertices ++] = v3 + Extend;*/
        }
    }

	//针对保存的每条轮廓边, 添加一个矩形, 所有的矩形构成阴影体
    for(i = 0; i < m_uNumEdges; i ++)
    {
		//计算矩形的四个的顶点
        v1 = CASE(m_pVertices[m_pEdges[i].v0], D3DXVECTOR3);
        v2 = CASE(m_pVertices[m_pEdges[i].v1], D3DXVECTOR3);
        v3 = v1 - Extend;
        v4 = v2 - Extend;

		//添加矩形
        m_pShadowVertices[m_uNumShadowVertices ++] = v1;
        m_pShadowVertices[m_uNumShadowVertices ++] = v2;
        m_pShadowVertices[m_uNumShadowVertices ++] = v3;

        m_pShadowVertices[m_uNumShadowVertices ++] = v2;
        m_pShadowVertices[m_uNumShadowVertices ++] = v4;
        m_pShadowVertices[m_uNumShadowVertices ++] = v3;
    }
}


//-----------------------------------------------------------------------------
// Desc: 将边添加到列表中
//-----------------------------------------------------------------------------
void CShadowVolume::__AddEdge(zerO::UINT v0, zerO::UINT v1 )
{
	//首先删除内部的边(在边列表中出现两次的边)
    for(UINT i=0; i < m_uNumEdges; i++ )
    {
        if( (m_pEdges[i].v0 == v0 && m_pEdges[i].v1 == v1) ||
            (m_pEdges[i].v0 == v1 && m_pEdges[i].v0 == v0) )
        {
            if(m_uNumEdges > 1)
            {
                m_pEdges[i].v0 = m_pEdges[m_uNumEdges-1].v0;
                m_pEdges[i].v1 = m_pEdges[m_uNumEdges-1].v1;
            }

            m_uNumEdges--;

            return;
        }
    }

	//在边列表中添加一条新边
    m_pEdges[m_uNumEdges].v0 = v0;
    m_pEdges[m_uNumEdges].v1 = v1;

    m_uNumEdges ++;
}

//-----------------------------------------------------------------------------
// Desc: 渲染阴影体, 只更新模板缓冲区, 不更新颜色缓冲区
//-----------------------------------------------------------------------------
void CShadowVolume::Render()
{
	//禁用z缓冲区写操作, 并启用模板缓冲区
    DEVICE.SetRenderState( D3DRS_ZWRITEENABLE,  FALSE );
    DEVICE.SetRenderState( D3DRS_STENCILENABLE, TRUE );

	//使用平面着色模式(不进行插值)
    DEVICE.SetRenderState( D3DRS_SHADEMODE,    D3DSHADE_FLAT );

	//如果渲染阴影体到颜色缓冲区
	if(m_bIsRenderVolume)
	{
		DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		DEVICE.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
		DEVICE.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );

		//为阴影体设置材质
		D3DMATERIAL9 mtrl;
		ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
		mtrl.Diffuse.r = mtrl.Ambient.r = 0.0f;
		mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
		mtrl.Diffuse.b = mtrl.Ambient.b = 0.0f;
		mtrl.Diffuse.a = mtrl.Ambient.a = 0.5f;
		DEVICE.SetMaterial( &mtrl );
	}
	else  //只更新深度模板缓冲区
	{
		DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, TRUE );
		DEVICE.SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_ZERO );
		DEVICE.SetRenderState( D3DRS_DESTBLEND, D3DBLEND_ONE );
	}

	UINT uNumFaces = m_uNumShadowVertices / 3;

	
	DEVICE.SetRenderState( D3DRS_STENCILFUNC,  D3DCMP_ALWAYS );
    

	//渲染阴影体背面
	/*DEVICE.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	DEVICE.SetRenderState( D3DRS_STENCILZFAIL, D3DSTENCILOP_INCR );
	DEVICE.SetTransform( D3DTS_WORLD, &m_pParent->GetWorldMatrix() );
	DEVICE.SetFVF( D3DFVF_XYZ );
    DEVICE.DrawPrimitiveUP( D3DPT_TRIANGLELIST, uNumFaces, m_pShadowVertices, sizeof(D3DXVECTOR3) );*/

	//渲染阴影体前面
	DEVICE.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CCW );
	DEVICE.SetRenderState( /*D3DRS_STENCILZFAIL*/D3DRS_STENCILPASS,  /*D3DSTENCILOP_DECR*/D3DSTENCILOP_INCR );
	DEVICE.SetTransform( D3DTS_WORLD, &m_pParent->GetWorldMatrix() );
	DEVICE.SetFVF( D3DFVF_XYZ );
    DEVICE.DrawPrimitiveUP( D3DPT_TRIANGLELIST, uNumFaces, m_pShadowVertices, sizeof(D3DXVECTOR3) );

	//渲染阴影体背面
	DEVICE.SetRenderState( D3DRS_CULLMODE,   D3DCULL_CW );
	DEVICE.SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_DECR );
	DEVICE.SetTransform( D3DTS_WORLD, &m_pParent->GetWorldMatrix() );
	DEVICE.SetFVF( D3DFVF_XYZ );
    DEVICE.DrawPrimitiveUP( D3DPT_TRIANGLELIST, uNumFaces, m_pShadowVertices, sizeof(D3DXVECTOR3) );

	//恢复渲染状态
	DEVICE.SetRenderState( D3DRS_SHADEMODE, D3DSHADE_GOURAUD );
    DEVICE.SetRenderState( D3DRS_CULLMODE,  D3DCULL_CCW );
    DEVICE.SetRenderState( D3DRS_ZWRITEENABLE,     TRUE );
    DEVICE.SetRenderState( D3DRS_STENCILENABLE,    FALSE );
    DEVICE.SetRenderState( D3DRS_ALPHABLENDENABLE, FALSE );
}


//-----------------------------------------------------------------------------
// Desc: 销毁阴影体
//-----------------------------------------------------------------------------
bool CShadowVolume::Destroy( )
{
	DEBUG_DELETE_ARRAY(m_pShadowVertices);
	DEBUG_DELETE_ARRAY(m_pEdges);
	DEBUG_DELETE_ARRAY(m_pTriangles);

	m_pShadowVertices = NULL;
	m_pEdges          = NULL;
	m_pTriangles      = NULL;

	return true;
}

