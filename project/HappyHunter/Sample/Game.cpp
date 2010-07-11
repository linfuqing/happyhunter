//--------------------------------------------------------------------------------------
// File: Sample.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"
#include "core.h"
//#include "Mission.h"
#include "StaticMesh.h"

//#define TERRAIN//PARTICLESYSTEM
//#define MISSION
#define STATICMESH

zerO::CGameHost g_Game;

#ifdef TEST
//顶点描述：位置和纹理坐标
const D3DVERTEXELEMENT9 g_VERTEX_DESCRIPTION[] =
{
	{0, 0, D3DDECLTYPE_FLOAT4, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_POSITION, 0},

	{0, 16, D3DDECLTYPE_FLOAT2, D3DDECLMETHOD_DEFAULT, D3DDECLUSAGE_TEXCOORD, 0},
	D3DDECL_END()
};

//测试类
class CTest :
	public zerO::CSceneNode
{
public:
	//顶点格式
	typedef struct
	{
		D3DXVECTOR4 Position;
		D3DXVECTOR2 TextureUV;
	}VERTEX;

	zerO::CEffect& GetEffect()
	{
		return m_Effect;
	}

	//创建
	bool Create()
	{
		//圆柱体构建
		VERTEX Vertices[100];

		for( DWORD i=0; i<50; i++ )
		{
			FLOAT theta = (2*D3DX_PI*i)/(50-1);
			Vertices[2*i+0].Position = D3DXVECTOR4( sinf(theta),-1.0f, cosf(theta), 1.0f );
			Vertices[2*i+0].TextureUV.x = ((FLOAT)i)/(50-1);
			Vertices[2*i+0].TextureUV.y = 1.0f;

			Vertices[2*i+1].Position = D3DXVECTOR4( sinf(theta), 1.0f, cosf(theta), 1.0f );
			Vertices[2*i+1].TextureUV.x = ((FLOAT)i)/(50-1);
			Vertices[2*i+1].TextureUV.y = 0.0f;
		}

		//属性创建及加载
		if( !m_VertexBuffer.Create(50 * 2, sizeof(VERTEX), 0, D3DPOOL_MANAGED, (void*)Vertices) )
			return false;

		if( !m_VertexBuffer.SetVertexDescription(sizeof(g_VERTEX_DESCRIPTION) / sizeof(D3DVERTEXELEMENT9), g_VERTEX_DESCRIPTION) )
			return false;

		if( !m_Effect.Load( TEXT("TerrainShader.fx") ) )
			return false;

		if( !m_Texture.Load( TEXT("terrain_heightmap.png") ) )
			return false;

		//构造世界矩阵
		D3DXMATRIX matWorld;
		D3DXMatrixIdentity( &matWorld );

		//构造观察矩阵
		D3DXMATRIXA16 matView;
		D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5 );
		D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
		D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
		D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

		//构造投影矩阵
		D3DXMATRIXA16 matProj;
		zerO::FLOAT fAspectRatio = (zerO::FLOAT)GAMEHOST.GetDeviceSettings().pp.BackBufferWidth / GAMEHOST.GetDeviceSettings().pp.BackBufferHeight;
		D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 100.0f );

		//为效果设置组合变换矩阵
		D3DXMATRIX mWorldViewProj = matWorld * matView * matProj;

		m_Effect.SetMatrix(zerO::CEffect::WORLD_VIEW_PROJECTION, mWorldViewProj);
		
		//设置剔出模式,为不剔出任何面
		DEVICE.SetRenderState( D3DRS_CULLMODE, D3DCULL_NONE );

		//设置纹理
		m_Effect.GetEffect()->SetTexture("TextureMapping", m_Texture.GetTexture() );

		return true;
	}

	//向队列申请渲染
	bool ApplyForRender()
	{
		UINT uTotalPass = m_Effect.GetTechniqueDesc().Passes, i;

		for (i = 0; i < uTotalPass; i ++)
		{
			//锁定整个队列
			zerO::CRenderQueue::LPRENDERENTRY pRenderEntry = GAMEHOST.GetRenderQueue().LockRenderEntry();
			
			//将信息需求传送到优化队列
			pRenderEntry->hEffect      = m_Effect.GetHandle();
			pRenderEntry->uModelType   = zerO::CRenderQueue::RENDERENTRY::BUFFER;
			pRenderEntry->hModel       = m_VertexBuffer.GetHandle();
			pRenderEntry->uRenderPass  = (zerO::UINT8)i;
			pRenderEntry->pParent      = this;

			//解锁
			GAMEHOST.GetRenderQueue().UnLockRenderEntry(pRenderEntry);
		}

		return true;
	}

	void Render(zerO::CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag)
	{
		//依照更新标志进行更新
		if( TEST_BIT(uFlag, zerO::CRenderQueue::EFFECT) )
			m_Effect.Begin();

		m_Effect.GetEffect()->BeginPass(pEntry->uRenderPass);

		if( TEST_BIT(uFlag, zerO::CRenderQueue::MODEL) )
			m_VertexBuffer.Activate(0, 0, true);

		m_Effect.GetEffect()->CommitChanges();

		//绘制
		DEVICE.DrawPrimitive(D3DPT_TRIANGLESTRIP, 0, 2 * 50 - 2);

		m_Effect.GetEffect()->EndPass();
	}

	private:
		zerO::CVertexBuffer m_VertexBuffer;
		zerO::CEffect m_Effect;
		zerO::CTexture m_Texture;
};

CTest g_Test;
#endif

#ifdef TERRAIN
#define HEIGHT_MAP_FILE TEXT("heightmap.jpg")

zerO::CQuadTree g_QuadTree;
zerO::CTerrain  g_Terrain;
zerO::CTexture  g_HeightMap;
zerO::CSurface  g_Surface;
#endif

#ifdef PARTICLESYSTEM

typedef struct
{
	D3DXVECTOR3 m_vVel;       //当前速度
	D3DXVECTOR3 m_vPos0;      //初始位置
	D3DXVECTOR3 m_vVel0;      //初始速度
	FLOAT       m_fTime0;     //创建时间
}PARTICLEPARAMETERS;

zerO::CParticleSystem<PARTICLEPARAMETERS>* g_pSprayParticles = NULL;

#endif

#ifdef MISSION

zerO::CMission g_Mission;

#endif

#ifdef STATICMESH

zerO::CStaticMesh g_Mesh;

#endif

//--------------------------------------------------------------------------------------
// Rejects any D3D9 devices that aren't acceptable to the app by returning false
//--------------------------------------------------------------------------------------
bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext )
{
    // Typically want to skip back buffer formats that don't support alpha blending
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


//--------------------------------------------------------------------------------------
// Before a device is created, modify the device settings as needed
//--------------------------------------------------------------------------------------
bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}

#ifdef PARTICLESYSTEM

void InitParticle(zerO::CParticleSystem<PARTICLEPARAMETERS>::LPPARTICLE pParticle)
{
	//为新粒子设置初始位置、初始速度、当前位置、当前速度、颜色和创建时间等属性
    float fRand = ((FLOAT)rand()/(FLOAT)RAND_MAX)*2.0f - 1.0f;  //[-1,1]
    pParticle->Parameter.m_vPos0 = D3DXVECTOR3(0,1,0);    //粒子初始位置
    pParticle->Parameter.m_vVel0.x  = 8.0f*fRand;         //粒子初始速度
    pParticle->Parameter.m_vVel0.z  = 10.0f;
	pParticle->Parameter.m_vVel0.y  = 0.0f;

    pParticle->Vertex.Position = pParticle->Parameter.m_vPos0;     
    pParticle->Parameter.m_vVel = pParticle->Parameter.m_vVel0;

    pParticle->Vertex.Color = D3DXCOLOR(1.0f, 1.0f, 1.0f, 1.0f);  //粒子颜色
    pParticle->Parameter.m_fTime0     = TIME;            //粒子创建时间
}

void UpdateParticle(zerO::CParticleSystem<PARTICLEPARAMETERS>::LPPARTICLE pParticle)
{
	//计算粒子的新位置
    float fT = TIME - pParticle->Parameter.m_fTime0;  //当前粒子已存活的时间
    float fGravity = -9.8f;
    pParticle->Vertex.Position    = pParticle->Parameter.m_vVel0 * fT + pParticle->Parameter.m_vPos0;
    pParticle->Vertex.Position.y += (0.5f * fGravity) * (fT * fT);
    pParticle->Parameter.m_vVel.y  = pParticle->Parameter.m_vVel0.y + fGravity * fT; 
}

bool IsParticleDestroy(zerO::CParticleSystem<PARTICLEPARAMETERS>::LPPARTICLE pParticle)
{
	return pParticle->Vertex.Position.y < 0;
}

D3DXVECTOR3 g_vPos;
D3DXVECTOR3 g_vVel;

UINT GetParticleRenderSteps(const zerO::CParticleSystem<PARTICLEPARAMETERS>::PARTICLE& Particle)
{
	//根据粒子的运动速度确定其模糊程度,
    g_vPos = Particle.Vertex.Position;
    g_vVel = Particle.Parameter.m_vVel;

    FLOAT       fLengthSq = D3DXVec3LengthSq(&g_vVel);
    UINT        dwSteps;

    if( fLengthSq < 1.0f )        dwSteps = 2;  //确定粒子的模糊程度
    else if( fLengthSq <  4.00f ) dwSteps = 3;
    else if( fLengthSq <  9.00f ) dwSteps = 4;
    else if( fLengthSq < 12.25f ) dwSteps = 5;
    else if( fLengthSq < 16.00f ) dwSteps = 6;
    else if( fLengthSq < 20.25f ) dwSteps = 7;
    else                          dwSteps = 8;

    g_vVel *= -0.04f / (FLOAT)dwSteps;

	return dwSteps;
}

void SetParticleRenderData(const zerO::CParticleSystem<PARTICLEPARAMETERS>::PARTICLE& Particle, zerO::CParticleSystem<PARTICLEPARAMETERS>::PARTICLEVERTEX& Vertex)
{
	Vertex.Position = g_vPos;                             //点图元的位置
    Vertex.Color    = Particle.Vertex.Color;    //点图元的颜色

	g_vPos          += g_vVel;                            //为实现模糊效果确定该点图元的下一位置
}

#endif

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
	//单件模式：首先需要构建一个GameHost
	zerO::CGameHost::DEVICESETTINGS DeviceSettings;
	memcpy( &DeviceSettings, &DXUTGetDeviceSettings(), sizeof(zerO::CGameHost::DEVICESETTINGS) );

	if( !g_Game.Create(pd3dDevice, DeviceSettings, 0xff) )
		return S_FALSE;

	CAMERA.SetProjection(D3DX_PI / 3.0f, 1.0f, 0.1f, 500.0f);

#ifdef TERRAIN
	D3DXMATRIX Matrix, Rotation;

	D3DXMatrixIdentity(&Matrix);

	D3DXMatrixRotationX(&Rotation, 90);

	Matrix *= Rotation;

	CAMERA.SetTransform(Matrix);

	zerO::CRectangle3D Rect;
	Rect.Set(- 1000.0f, 1000.0f, - 1000.0f, 1000.0f, 0.0f, 500.0f);
	g_QuadTree.Create(Rect, 4);

	g_HeightMap.Load(HEIGHT_MAP_FILE);

	g_Terrain.Create(NULL, &g_HeightMap, Rect, 3);

	g_Terrain.GetRenderMethod().LoadEffect( TEXT("Test.fx") );

	g_Surface.SetTexture(&g_HeightMap, 0);

	g_Terrain.GetRenderMethod().SetSurface(&g_Surface);

	g_Terrain.SetQuadTree(&g_QuadTree);
#endif

#ifdef TEST
	//创建测试类
	if( g_Test.Create() )
		return S_FALSE;
#endif

#ifdef PARTICLESYSTEM
	DEBUG_NEW(g_pSprayParticles, zerO::CParticleSystem<PARTICLEPARAMETERS>);
	g_pSprayParticles->Create(
		1000, 1000, 500, 2000, 0.1f, 100.0f, 0.0f, 0.0f, 0.0f, 1.0f, 
		InitParticle, UpdateParticle, IsParticleDestroy, GetParticleRenderSteps, SetParticleRenderData);

	//设置材料
	D3DMATERIAL9 mtrl;
	 ZeroMemory( &mtrl, sizeof(D3DMATERIAL9) );
     mtrl.Diffuse.r = mtrl.Ambient.r = 1.0f;
     mtrl.Diffuse.g = mtrl.Ambient.g = 1.0f;
     mtrl.Diffuse.b = mtrl.Ambient.b = 1.0f;
     mtrl.Diffuse.a = mtrl.Ambient.a = 1.0f;
     
	 g_pSprayParticles->GetSurface().SetMaterial(mtrl);
	 g_pSprayParticles->GetSurface().LoadTexuture(TEXT("heightmap.jpg"),0);

	 //设置灯光
	 D3DXVECTOR3 vecDir;
     D3DLIGHT9 light;
	 ZeroMemory( &light, sizeof(D3DLIGHT9) );
     light.Type       = D3DLIGHT_DIRECTIONAL;
     light.Diffuse.r  = 1.0f;
     light.Diffuse.g  = 1.0f;
     light.Diffuse.b  = 1.0f;
	 vecDir = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
     D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	 light.Position = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
     light.Range       = 1000.0f;

	 LIGHTMANAGER.SetLight(light, 0);
	 LIGHTMANAGER.SetAmbient(0x00808080);
	 
	 GAMEHOST.SetLightEnable(true);
#endif

#ifdef MISSION
	 g_Mission.GetParams();
#endif

#ifdef STATICMESH
	 g_Mesh.SetMeshFile(TEXT("Brown bear.X"));
	if ( !g_Mesh.Create() )
		return S_FALSE;

	D3DLIGHT9 Light;

	Light.Direction.x = - 1.0f;
	Light.Direction.y = - 1.0f;
	Light.Direction.z = - 1.0f;

	LIGHTMANAGER.SetLight(Light, 0);
#endif

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{

	//自动化重建（未完全）
	if( !GAMEHOST.Restore(*pBackBufferSurfaceDesc) )
		return S_FALSE;

#ifdef TEST
	HRESULT hr;

	//恢复效果对象
	if( g_Test.GetEffect().GetEffect() )
        V_RETURN( g_Test.GetEffect().GetEffect()->OnResetDevice() );

	//构造世界矩阵
	D3DXMATRIX matWorld;
	D3DXMatrixIdentity( &matWorld );

	//构造观察矩阵
	D3DXMATRIXA16 matView;
	D3DXVECTOR3 vEyePt( 0.0f, 3.0f,-5 );
    D3DXVECTOR3 vLookatPt( 0.0f, 0.0f, 0.0f );
    D3DXVECTOR3 vUpVec( 0.0f, 1.0f, 0.0f );
    D3DXMatrixLookAtLH( &matView, &vEyePt, &vLookatPt, &vUpVec );

	//构造投影矩阵
	D3DXMATRIXA16 matProj;
	float fAspectRatio = (float)pBackBufferSurfaceDesc->Width / pBackBufferSurfaceDesc->Height;
    D3DXMatrixPerspectiveFovLH( &matProj, D3DX_PI/4, fAspectRatio, 1.0f, 100.0f );

	//为效果设置组合变换矩阵
	D3DXMATRIX mWorldViewProj = matWorld * matView * matProj;
	g_Test.GetEffect().GetEffect()->SetMatrix( "matWorldViewProj", &mWorldViewProj );
#endif

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	GAMEHOST.Update(fElapsedTime);

	static FLOAT z = 0;

	D3DXMATRIX Matrix, Rotation;

	D3DXMatrixIdentity(&Matrix);

	//Matrix._41 = 1500;
	//Matrix._41 = -z;
	//Matrix._42 = 100;
	Matrix._43 = z;

	/*D3DXMatrixRotationX(&Rotation, D3DX_PI / 2);

	Matrix *= Rotation;*/

	CAMERA.SetTransform(Matrix);

	CAMERA.Update();

	z -= 1;

#ifdef PARTICLESYSTEM
	g_pSprayParticles->SetNumberEmitedPerFrame( (UINT)(1000*ELAPSEDTIME) );
	g_pSprayParticles->Update();
#endif
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

#ifdef TERRAIN
	zerO::CRectangle3D Rect;
	Rect.Set(- 500.0f, 0.0f, - 500.0f, 0.0f, 0.0f, 500.0f);
	zerO::CQuadTreeObject* pObject = g_QuadTree.SearchObject( CAMERA.GetWorldRectangle() );
#endif

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//开始渲染
		g_Game.BeginRender();

#ifdef TERRAIN
		while(pObject)
		{
			pObject->ApplyForRender();
			pObject = pObject->GetNext();
		}

		//g_Terrain.Render();
#endif

#ifdef TEST
		//申请渲染
		g_Test.ApplyForRender();
#endif

#ifdef PARTICLESYSTEM
		g_pSprayParticles->ApplyForRender();
#endif

#ifdef STATICMESH
		g_Mesh.ApplyForRender();
#endif
		//结束渲染
		g_Game.EndRender();

        V( pd3dDevice->EndScene() );
    }
}


//--------------------------------------------------------------------------------------
// Handle messages to the application 
//--------------------------------------------------------------------------------------
LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9ResetDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
	//自动化释放
	DEBUG_ASSERT(GAMEHOST.Disable(), "Disable error.");
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	//自动化销毁
	DEBUG_ASSERT(GAMEHOST.Destroy(), "Destroy error.");

	g_Mesh.Destroy();
}


//--------------------------------------------------------------------------------------
// Initialize everything and go into a render loop
//--------------------------------------------------------------------------------------
INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
    // Enable run-time memory check for debug builds.
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    // Set the callback functions
    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );

    // TODO: Perform any application-level initialization here

    // Initialize DXUT and create the desired Win32 window and Direct3D device for the application
    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"Sample" );
    DXUTCreateDevice( true, 640, 480 );

    // Start the render loop
    DXUTMainLoop();

    // TODO: Perform any application-level cleanup here

    return DXUTGetExitCode();
}


