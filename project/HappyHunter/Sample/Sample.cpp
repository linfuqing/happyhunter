//--------------------------------------------------------------------------------------
// File: Sample.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"
#include "core.h"

#define TERRAIN//PARTICLESYSTEM

zerO::CGameHost g_Game;

#define HEIGHT_MAP_FILE TEXT("最终高度图.jpg")

zerO::CQuadTree g_QuadTree;
zerO::CTexture  g_HeightMap;
zerO::CTexture  g_Texture;
zerO::CTexture  g_Detail;
zerO::CSurface  g_TerrainSurface;
zerO::CTerrain g_Terrain;

zerO::CSkyBox g_SkyBox;
zerO::CSurface g_SkyBoxSurface;

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

	CAMERA.SetProjection(D3DX_PI / 4.0f, 1.0f, 0.5f, 3000.0f);

	zerO::CRectangle3D Rect;
	Rect.Set(- 3000.0f, 3000.0f, 0.0f, 500.0f, - 3000.0f, 3000.0f);
	g_QuadTree.Create(Rect, 4);

	g_HeightMap.Load(HEIGHT_MAP_FILE);

	g_Terrain.Create(NULL, &g_HeightMap, Rect, 6);

	g_Terrain.GetRenderMethod().LoadEffect( TEXT("Test.fx") );


	g_Texture.Load( TEXT("最终纹理.dds") );

	g_Detail.Load( TEXT("dirt_grass.jpg") );

	g_TerrainSurface.SetTexture(&g_Texture, 0);
	g_TerrainSurface.SetTexture(&g_Detail, 1);

	g_Terrain.GetRenderMethod().SetSurface(&g_TerrainSurface);

	g_Terrain.SetQuadTree(&g_QuadTree);


	g_SkyBox.Create(10.0f);

	g_SkyBox.SetCloudSpeed(0.01f, 0.01f);

	D3DMATERIAL9 Matrial;
	memset( &Matrial, 0, sizeof(D3DMATERIAL9) );
	Matrial.Diffuse.r = Matrial.Ambient.r = 1.0f;
	Matrial.Diffuse.g = Matrial.Ambient.g = 1.0f;
	Matrial.Diffuse.b = Matrial.Ambient.b = 1.0f;
	Matrial.Diffuse.a = Matrial.Ambient.a = 1.0f;

	g_SkyBoxSurface.SetMaterial(Matrial);
	//g_SkyBoxSurface.LoadTexture(TEXT("blue_0005.png"), 0);
	////g_Surface.LoadTexture(TEXT("blue_0005.png"), 1);
	//g_SkyBoxSurface.LoadTexture(TEXT("blue_0001.png"), 2);
	//g_SkyBoxSurface.LoadTexture(TEXT("blue_0003.png"), 3);
	//g_SkyBoxSurface.LoadTexture(TEXT("blue_0002.png"), 4);
	//g_SkyBoxSurface.LoadTexture(TEXT("blue_0004.png"), 5);
	//g_SkyBoxSurface.LoadTexture(TEXT("Cloud1.png"), 6);

	/*g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_UP.jpg"), 0);
	g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_DN.jpg"), 1);
	g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_LF.jpg"), 2);
	g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_RT.jpg"), 3);
	g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_FR.jpg"), 4);
	g_SkyBoxSurface.LoadTexture(TEXT("night/afx_darksky_BK.jpg"), 5);*/
	//g_SkyBoxSurface.LoadTexture(TEXT("night/Cloud.tga"), 6);

	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_UP.dds"), 0);
	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_DN.dds"), 1);
	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_LF.dds"), 2);
	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_RT.dds"), 3);
	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_FR.dds"), 5);
	g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_BK.dds"), 4);
	//g_SkyBoxSurface.LoadTexture(TEXT("Cloud1.png"), 6);

	g_SkyBox.GetRenderMethod().SetSurface(&g_SkyBoxSurface);
	g_SkyBox.GetRenderMethod().LoadEffect( TEXT("EffectTexture.fx") );

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

	// GAMEHOST.SetLightEnable(true);

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Create any D3D9 resources that won't live through a device reset (D3DPOOL_DEFAULT) 
// or that are tied to the back buffer size 
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{

	//自动化重建
	if( !GAMEHOST.Restore(*pBackBufferSurfaceDesc) )
		return S_FALSE;

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	GAMEHOST.Update(fElapsedTime);

	static FLOAT x = 0, y = 0, z = 0, RotationY = 0, RotationX = 0;

	D3DXMATRIX Matrix, Rotation, Translation;

	D3DXMatrixIdentity(&Matrix);

	D3DXMatrixRotationYawPitchRoll(&Rotation, RotationY / 180 * D3DX_PI, RotationX / 180 * D3DX_PI, 0.0f);

	Matrix *= Rotation;

	D3DXMatrixTranslation( &Translation, x, g_Terrain.GetHeight(CAMERA.GetPosition().x, CAMERA.GetPosition().z) + 100.0f, z );

	Matrix *= Translation;

	CAMERA.SetTransform(Matrix);

	/*if( DXUTIsKeyDown(VK_UP) )
		y += 10.0f;
	
	if( DXUTIsKeyDown(VK_DOWN) )
		y -= 10.0f;*/

	if( DXUTIsKeyDown(VK_LEFT) )
		x -= 10.0f;

	if( DXUTIsKeyDown(VK_RIGHT) )
		x += 10.0f;

	if( DXUTIsKeyDown('W') )
		z += 10.0f;

	if( DXUTIsKeyDown('S') )
		z -= 10.0f;

	if( DXUTIsKeyDown('A') )
		RotationY -= 1.0f;

	if( DXUTIsKeyDown('D') )
		RotationY += 1.0f;

	if( DXUTIsKeyDown('Q') )
		RotationX -= 1.0f;

	if( DXUTIsKeyDown('E') )
		RotationX += 1.0f;
}


//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

	zerO::CRectangle3D Rect;
	Rect.Set(- 500.0f, 0.0f, - 500.0f, 0.0f, 0.0f, 500.0f);
	zerO::CQuadTreeObject* pObject = g_QuadTree.SearchObject( CAMERA.GetWorldRectangle() ), * pCurrentObject;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//开始渲染
		g_Game.BeginRender();

		pCurrentObject = pObject;

		while(pCurrentObject)
		{
			pCurrentObject->ApplyForRender();
			pCurrentObject = pCurrentObject->GetNext();
		}

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


