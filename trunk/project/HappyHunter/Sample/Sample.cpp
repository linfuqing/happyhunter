//--------------------------------------------------------------------------------------
// File: Sample.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"
#include "core.h"

#include "SDKmisc.h"

#define TERRAIN//PARTICLESYSTEM


ID3DXFont*                 g_pFont = NULL;          //ID3DXFont字体对象
ID3DXSprite*               g_pTextSprite = NULL;    //ID3DXSprite文本精灵对象

zerO::CGameHost g_Game;

#define HEIGHT_MAP_FILE TEXT("summer6.jpg")

zerO::CQuadTree g_QuadTree;
zerO::CTexture  g_HeightMap;
zerO::CTexture  g_Texture;
zerO::CTexture  g_Detail;
zerO::CSurface  g_TerrainSurface;
zerO::CTerrain g_Terrain;

zerO::CStaticMesh g_Mesh;
zerO::CStaticMesh g_Copy;
//zerO::CSkinMesh   g_SkinMesh;
//zerO::CSkinMesh   g_CopyMesh;

zerO::CTerrainSystem g_TerrainSystem;

zerO::CSkyBox g_SkyBox;
zerO::CSurface g_SkyBoxSurface;

//zerO::CShadowVolume        g_ShadowVolume2;
//zerO::CShadowVolume*       g_pShadowVolume;
//LPDIRECT3DVERTEXBUFFER9    g_pBigSquareVB;
//zerO::CSkyDome g_SkyDome;

//阴影举行顶点结构和顶点格式
struct SHADOWVERTEX
{
    D3DXVECTOR4 p;
    D3DCOLOR    color;

    static const DWORD FVF = D3DFVF_XYZRHW | D3DFVF_DIFFUSE;
};

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

	//为当前设备设置深度模板缓冲区格式
	pDeviceSettings->d3d9.pp.AutoDepthStencilFormat = D3DFMT_D24S8;

    return true;
}

//--------------------------------------------------------------------------------------
// Create any D3D9 resources that will live through a device reset (D3DPOOL_MANAGED)
// and aren't tied to the back buffer size
//--------------------------------------------------------------------------------------
HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
	HRESULT hr;

	//创建字体
    V_RETURN( D3DXCreateFont( pd3dDevice, 15, 0, FW_BOLD, 1, FALSE, DEFAULT_CHARSET, 
                         OUT_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_DONTCARE, 
                         L"Arial", &g_pFont ) );

	//单件模式：首先需要构建一个GameHost
	zerO::CGameHost::DEVICESETTINGS DeviceSettings;
	memcpy( &DeviceSettings, &DXUTGetDeviceSettings().d3d9, sizeof(zerO::CGameHost::DEVICESETTINGS) );

	if( !g_Game.Create(pd3dDevice, DeviceSettings, 0xff) )
		return S_FALSE;

	CAMERA.SetProjection(D3DX_PI / 4.0f, (zerO::FLOAT)DeviceSettings.pp.BackBufferWidth / DeviceSettings.pp.BackBufferHeight, 0.5f, 3000.0f);

	/*g_SkinMesh.SetEffectFile( TEXT("HLSLSkinSoftware.fx") );
	if ( !g_SkinMesh.Create(TEXT("Aardvark.X")) )
		return S_FALSE;

	g_CopyMesh.SetEffectFile( TEXT("HLSLSkinSoftware.fx") );
	if ( !g_CopyMesh.Create( TEXT("机关枪.x") ) )
		return S_FALSE;*/

	//float start = 1000.0f;
	//float end   = 2000.0f;
	//pd3dDevice->SetRenderState(D3DRS_FOGENABLE, TRUE);
	//pd3dDevice->SetRenderState(D3DRS_FOGVERTEXMODE, D3DFOG_LINEAR);
	////pd3dDevice->SetRenderState(D3DRS_FOGDENSITY, *(DWORD*)&start);
	//pd3dDevice->SetRenderState(D3DRS_FOGSTART, CASE(start, DWORD));
	//pd3dDevice->SetRenderState(D3DRS_FOGEND,  CASE(end, DWORD));
	//pd3dDevice->SetRenderState(D3DRS_FOGCOLOR, 0x7f00ff00);
	//g_SkinMesh.Clone(g_CopyMesh);

	FOGMANAGER.SetLinearFog(100.0f, 1500.0f);
	GAMEHOST.SetFogEnable(true);

	g_Mesh.SetEffectFile( TEXT("ItemEffect.fx") );

	g_Mesh.Create( TEXT("Brown bear.x") );

	g_Mesh.Clone(g_Copy);

	//创建阴影体
	/*g_pShadowVolume = new zerO::CShadowVolume();

	g_pShadowVolume->Create(*g_SkinMesh.GetMesh(), g_SkinMesh);*/
	//g_ShadowVolume2.Create(*g_CopyMesh.GetMesh(), g_CopyMesh);

	//void* pVertices, *pIndices;

	////锁定网格模型顶点缓冲区和索引缓冲区
	//g_SkinMesh./*GetMesh().*/GetMesh()->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
 //   g_SkinMesh./*GetMesh().*/GetMesh()->LockIndexBuffer( 0L, (LPVOID*)&pIndices );
	///*g_pShadowVolume->Create( g_Mesh.GetMesh().GetMesh(), g_Mesh);*/
	//g_pShadowVolume->Create(
	//	g_SkinMesh./*GetMesh().*/GetMesh()->GetNumVertices(), 
	//	g_SkinMesh./*GetMesh().*/GetMesh()->GetNumFaces(),
	//	g_SkinMesh,
	//	g_SkinMesh./*GetMesh().*/GetMesh()->GetNumBytesPerVertex(),
	//	pVertices,
	//	pIndices);

	//g_SkinMesh./*GetMesh().*/GetMesh()->UnlockVertexBuffer();
 //   g_SkinMesh./*GetMesh().*/GetMesh()->UnlockIndexBuffer();

	g_HeightMap.Load(HEIGHT_MAP_FILE);

	zerO::CRectangle3D Rect;
	Rect.Set(- 2560.0f, 2560.0f, 0.0f, 640.0f, - 2560.0f, 2560.0f);

	g_TerrainSystem.Create(&g_HeightMap, Rect, 6, 4/*, zerO::CTerrainSystem::ROAM*/);
	/*g_QuadTree.Create(Rect, 4);

	g_Terrain.Create(NULL, &g_HeightMap, Rect, 6);

	g_Terrain.GetRenderMethod().LoadEffect( TEXT("Test.fx") );*/


	g_Texture.Load( TEXT("summer6_TX.dds") );

	g_Detail.Load( TEXT("dirt_grass.jpg") );

	g_TerrainSurface.SetTexture(&g_Texture, 0);
	g_TerrainSurface.SetTexture(&g_Detail, 1);

	g_TerrainSystem.GetTerrain()->GetRenderMethod().SetSurface(&g_TerrainSurface);
	g_TerrainSystem.GetTerrain()->GetRenderMethod().LoadEffect( TEXT("Test.fx") );

	//( (zerO::CRoamTerrain*)g_TerrainSystem.GetTerrain() )->SetTessellationParameters(10.33f, 0.3f);

	//g_SkyDome.Create(5.0f, 5.0f, 10.0f);

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

	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_UP.dds"), 0);
	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_DN.dds"), 1);
	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_LF.dds"), 2);
	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_RT.dds"), 3);
	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_FR.dds"), 5);
	//g_SkyBoxSurface.LoadTexture(TEXT("noon/cloudy_noon_BK.dds"), 4);
	////g_SkyBoxSurface.LoadTexture(TEXT("Cloud1.png"), 6);

	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0005.jpg"), 0);
	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0006.jpg"), 1);
	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0004.jpg"), 2);
	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0002.jpg"), 3);
	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0003.jpg"), 5);
	g_SkyBoxSurface.LoadTexture(TEXT("sky_blue/blue_0001.jpg"), 4);
	//g_SkyBoxSurface.LoadTexture(TEXT("Cloud1.png"), 6);

	g_SkyBox.GetRenderMethod().SetSurface(&g_SkyBoxSurface);
	g_SkyBox.GetRenderMethod().LoadEffect( TEXT("EffectTexture.fx") );

		// //设置灯光
	 //D3DXVECTOR3 vecDir;
  //   D3DLIGHT9 light;
	 //ZeroMemory( &light, sizeof(D3DLIGHT9) );
  //   light.Type       = D3DLIGHT_DIRECTIONAL;
  //   light.Diffuse.r  = 1.0f;
  //   light.Diffuse.g  = 1.0f;
  //   light.Diffuse.b  = 1.0f;
	 //vecDir = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
  //   D3DXVec3Normalize( (D3DXVECTOR3*)&light.Direction, &vecDir );
	 //light.Position = D3DXVECTOR3(-1.0f, -1.0f, 2.0f);
  //   light.Range       = 1000.0f;

	 //LIGHTMANAGER.SetLight(light, 0);
	 LIGHTMANAGER.SetAmbient(0xffffffff);

	GAMEHOST.SetLightEnable(true);

	//创建矩形顶点缓冲区
    /*V_RETURN( pd3dDevice->CreateVertexBuffer( 4*sizeof(SHADOWVERTEX),
                                       D3DUSAGE_WRITEONLY, SHADOWVERTEX::FVF,
                                       D3DPOOL_MANAGED, &g_pBigSquareVB, NULL ));*/

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

	////填充矩形顶点缓冲区
	//SHADOWVERTEX* v;
 //   float sx = (float)pBackBufferSurfaceDesc->Width;
 //   float sy = (float)pBackBufferSurfaceDesc->Height;
 //   g_pBigSquareVB->Lock( 0, 0, (void**)&v, 0 );
 //   v[0].p = D3DXVECTOR4(  0, sy, 0.0f, 1.0f );
 //   v[1].p = D3DXVECTOR4(  0,  0, 0.0f, 1.0f );
 //   v[2].p = D3DXVECTOR4( sx, sy, 0.0f, 1.0f );
 //   v[3].p = D3DXVECTOR4( sx,  0, 0.0f, 1.0f );
 //   v[0].color = 0x7f000000;
 //   v[1].color = 0x7f000000;
 //   v[2].color = 0x7f000000;
 //   v[3].color = 0x7f000000;
 //   g_pBigSquareVB->Unlock();

	//pd3dDevice->SetRenderState( D3DRS_AMBIENT, 0xffffffff );

	HRESULT hr;

		//恢复字体
    if( g_pFont )
        V_RETURN( g_pFont->OnResetDevice() );
   
	//创建ID3DXSprite接口对象
    V_RETURN( D3DXCreateSprite( pd3dDevice, &g_pTextSprite ) );

    return S_OK;
}


//--------------------------------------------------------------------------------------
// Handle updates to the scene.  This is called regardless of which D3D API is used
//--------------------------------------------------------------------------------------
void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
	//设置灯光, 实时更新光源位置
	D3DLIGHT9 light;
    ZeroMemory( &light, sizeof(D3DLIGHT9) );
    light.Type       = D3DLIGHT_POINT;  
    light.Diffuse.r  = 1.0f;
    light.Diffuse.g  = 1.0f;
    light.Diffuse.b  = 1.0f;

	//light.Direction.x = - 1.0f;
	//light.Direction.y =  -1.0f;
	//light.Direction.z = 1.0f;

	static float h = 10000;
	float lx,ly,lz;           //光源位置
	lx = 1000.0f * sinf(timeGetTime()/360.0f);
	ly = 10000.0f;
	lz = 1000.0f * cosf(timeGetTime()/360.0f);
	light.Position= D3DXVECTOR3(lx,ly,lz);
	light.Direction = D3DXVECTOR3(lx, 1000, lz);

    light.Range        = 100.0f;
	light.Attenuation0 = 0.9f;
    light.Attenuation1 = 0.0f;

	LIGHTMANAGER.SetLight(light, 0);

	GAMEHOST.Update(fElapsedTime);

	g_TerrainSystem.Update();

	//根据光源位置更新阴影体
	//g_pShadowVolume->Update();

	g_Mesh.Update();
	g_Copy.Update();
	/*g_SkinMesh.Update();
	g_CopyMesh.Update();*/

	//g_SkinMesh.SetDirection(D3DXVECTOR3(0, 0, -1.0f));
	//g_CopyMesh.SetSceneDirection(D3DXVECTOR3(-0.588091, -0.030936, 0.808203));

	/*void* pVertices, *pIndices;
	g_CopyMesh.GetMesh()->LockVertexBuffer( 0L, (LPVOID*)&pVertices );
    g_CopyMesh.GetMesh()->LockIndexBuffer( 0L, (LPVOID*)&pIndices );

	g_CopyMesh.GetMesh()->UnlockVertexBuffer();
    g_CopyMesh.GetMesh()->UnlockIndexBuffer();*/

	//g_ShadowVolume2.SetMeshData((zerO::PUINT8)pVertices, (zerO::PUINT16)pIndices, g_CopyMesh.GetMesh()->GetNumBytesPerVertex() );

	static FLOAT x = 0, y = 0, z = 0, RotationY = 0, RotationX = 0;

	/*D3DXMATRIX Matrix, Rotation, Translation;

	D3DXMatrixIdentity(&Matrix);

	D3DXMatrixRotationYawPitchRoll(&Rotation, RotationY / 180 * D3DX_PI, RotationX / 180 * D3DX_PI, 0.0f);

	Matrix *= Rotation;

	D3DXMatrixTranslation( &Translation, x, g_Terrain.GetHeight(CAMERA.GetWorldPosition().x, CAMERA.GetWorldPosition().z) + 100.0f, z );

	Matrix *= Translation;

	CAMERA.SetTransform(Matrix);*/

	g_Mesh.SetPosition( 
		D3DXVECTOR3(
		g_Mesh.GetPosition().x, 
		g_TerrainSystem.GetTerrain()->GetHeight(g_Mesh.GetPosition().x, g_Mesh.GetPosition().z) + 100.0f, 
		g_Mesh.GetPosition().z)  );

	g_Copy.SetPosition( 
		D3DXVECTOR3(
		100, 
		g_TerrainSystem.GetTerrain()->GetHeight(g_Mesh.GetPosition().x, g_Mesh.GetPosition().z) + 100.0f, 
		g_Mesh.GetPosition().z)  );

	/*g_SkinMesh.SetPosition( 
		D3DXVECTOR3(
		g_SkinMesh.GetPosition().x, 
		g_TerrainSystem.GetTerrain()->GetHeight(g_SkinMesh.GetPosition().x, g_SkinMesh.GetPosition().z), 
		g_SkinMesh.GetPosition().z)  );

	g_CopyMesh.SetPosition( 
		D3DXVECTOR3(
		100, 
		g_TerrainSystem.GetTerrain()->GetHeight(g_SkinMesh.GetPosition().x, g_SkinMesh.GetPosition().z), 
		g_SkinMesh.GetPosition().z)  );*/

	CAMERA.SetRotation( 
		D3DXVECTOR3(RotationX / 180 * D3DX_PI, RotationY / 180 * D3DX_PI, 0.0f) );
	CAMERA.SetPosition( 
		D3DXVECTOR3(CAMERA.GetPosition().x, 
		g_TerrainSystem.GetTerrain()->GetHeight(CAMERA.GetPosition().x, CAMERA.GetPosition().z) + 100.0f, 
		CAMERA.GetPosition().z) );

	if( DXUTIsKeyDown(VK_UP) )
		RotationX += 1.0f;
	
	if( DXUTIsKeyDown(VK_DOWN) )
		RotationX -= 1.0f;

	if( DXUTIsKeyDown(VK_LEFT) )
		CAMERA.Right (- 10.0f);

	if( DXUTIsKeyDown(VK_RIGHT) )
		CAMERA.Right (  10.0f);

	if( DXUTIsKeyDown('W') )
	{
		/*z += 10.0f;*/
		CAMERA.Forward(10.0f);
	}

	if( DXUTIsKeyDown('S') )
		CAMERA.Forward(-10.0f);

	if( DXUTIsKeyDown('A') )
		RotationY -= 1.0f;

	if( DXUTIsKeyDown('D') )
		RotationY += 1.0f;
}

////-----------------------------------------------------------------------------
//// Desc: 渲染阴影
////-----------------------------------------------------------------------------
//HRESULT DrawShadow(IDirect3DDevice9* pd3dDevice)
//{
//	//关闭深度测试, 启用Alpha混合
//    pd3dDevice->SetRenderState( D3DRS_ZENABLE,          false );
//    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, true );
//    pd3dDevice->SetRenderState( D3DRS_SRCBLEND,  D3DBLEND_SRCALPHA );
//    pd3dDevice->SetRenderState( D3DRS_DESTBLEND, D3DBLEND_INVSRCALPHA );
//
//    //设置模板相关渲染状态
//	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    true );
//    pd3dDevice->SetRenderState( D3DRS_STENCILREF,  0x1 );
//    pd3dDevice->SetRenderState( D3DRS_STENCILFUNC, D3DCMP_LESSEQUAL );
//    pd3dDevice->SetRenderState( D3DRS_STENCILPASS, D3DSTENCILOP_KEEP );
//
//    //渲染一个灰色矩形, 只有通过模板测试的像素才会被渲染到颜色缓冲区,表示阴影
//    pd3dDevice->SetFVF( SHADOWVERTEX::FVF );
//    pd3dDevice->SetStreamSource( 0, g_pBigSquareVB, 0, sizeof(SHADOWVERTEX) );
//    pd3dDevice->DrawPrimitive( D3DPT_TRIANGLESTRIP, 0, 2 );
//
//    //恢复渲染状态
//    pd3dDevice->SetRenderState( D3DRS_ZENABLE,          true );
//    pd3dDevice->SetRenderState( D3DRS_ALPHABLENDENABLE, false );
//	pd3dDevice->SetRenderState( D3DRS_STENCILENABLE,    false );
//
//    return S_OK;
//}

//-----------------------------------------------------------------------------
// Desc: 渲染文本
//-----------------------------------------------------------------------------
void RenderText()
{
    CDXUTTextHelper txtHelper( g_pFont, g_pTextSprite, 15 );

    //显示当前Direct3D设备状态和渲染帧速率
    txtHelper.Begin();
    txtHelper.SetInsertionPos( 5, 5 );
    txtHelper.SetForegroundColor( D3DXCOLOR( 1.0f, 1.0f, 0.0f, 1.0f ) );
    txtHelper.DrawTextLine( DXUTGetFrameStats(true) );
    txtHelper.DrawTextLine( DXUTGetDeviceStats() );

    txtHelper.End();
}

//--------------------------------------------------------------------------------------
// Render the scene using the D3D9 device
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER | D3DCLEAR_STENCIL, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
		//开始渲染
		g_Game.BeginRender();

		g_TerrainSystem.Render();

		g_Mesh.ApplyForRender();
		g_Copy.ApplyForRender();
		/*g_SkinMesh.ApplyForRender();
		g_CopyMesh.ApplyForRender();*/

		//结束渲染
		g_Game.EndRender();

		/*pd3dDevice->SetVertexShader(NULL);
		pd3dDevice->SetPixelShader(NULL);*/

		//pd3dDevice->SetTransform( D3DTS_VIEW, &CAMERA.GetViewMatrix() );

		////设置投影矩阵
		//pd3dDevice->SetTransform( D3DTS_PROJECTION, &CAMERA.GetProjectionMatrix() );

		//渲染阴影
		//g_pShadowVolume->Render(false);

		//DrawShadow(pd3dDevice);

		//渲染文本和控件
        DXUT_BeginPerfEvent( DXUT_PERFEVENTCOLOR, L"HUD / Stats" ); 
        RenderText();
        DXUT_EndPerfEvent();

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

	if( g_pFont )
        g_pFont->OnLostDevice();
    SAFE_RELEASE( g_pTextSprite );
}


//--------------------------------------------------------------------------------------
// Release D3D9 resources created in the OnD3D9CreateDevice callback 
//--------------------------------------------------------------------------------------
void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
	//自动化销毁
	DEBUG_ASSERT(GAMEHOST.Destroy(), "Destroy error.");

	/*if(g_pBigSquareVB)
		g_pBigSquareVB->Release();*/

	/*g_pShadowVolume->Destroy();*/
	/*if(g_pShadowVolume )
		delete g_pShadowVolume ;*/

	SAFE_RELEASE( g_pFont );
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


