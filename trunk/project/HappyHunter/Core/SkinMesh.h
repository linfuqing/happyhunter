#pragma once

#include "debug.h"
#include "Effect.h"
#include "Texture.h"
#include "SceneNode.h"
#include "RenderMethod.h"

namespace zerO
{
	//-----------------------------------------------------------------------------
	// Desc: 继承自DXDXFRAME结构的结构
	//-----------------------------------------------------------------------------
	struct D3DXFRAME_DERIVED: public D3DXFRAME
	{
		D3DXMATRIXA16 CombinedTransformationMatrix;
	};

	//-----------------------------------------------------------------------------
	// Desc: 继承自D3DXMESHCONTAINER结构的结构
	//-----------------------------------------------------------------------------
	struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*  ppTextures;            //纹理数组
		LPD3DXMESH           pOrigMesh;             //原网格
		DWORD                NumAttributeGroups;    //骨骼数量
		DWORD                NumInfl;               //每个顶点最多可以影响多少骨骼
		LPD3DXBUFFER         pBoneCombinationBuf;   //骨骼结合表
		D3DXMATRIX**         ppBoneMatrixPtrs;      //存放骨骼的组合变换矩阵
		D3DXMATRIX*          pBoneOffsetMatrices;   //存放骨骼的初始变换矩阵
		DWORD                NumPaletteEntries;     //有多少骨骼可以使用(上限)
		bool                 UseSoftwareVP;         //标识是否使用软件顶点处理
	};


	//-----------------------------------------------------------------------------
	// Desc: 该类用来从.X文件加载框架层次和网格模型数据
	//-----------------------------------------------------------------------------
	class CAllocateHierarchy: public ID3DXAllocateHierarchy
	{
	private:
		HRESULT __GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
		HRESULT __AllocateName( LPCSTR Name, LPSTR *pNewName );

	public:
		STDMETHOD(CreateFrame)(THIS_ LPCSTR Name, LPD3DXFRAME *ppNewFrame);
		STDMETHOD(CreateMeshContainer)( THIS_ LPCSTR              Name, 
			CONST D3DXMESHDATA*       pMeshData,
			CONST D3DXMATERIAL*       pMaterials, 
			CONST D3DXEFFECTINSTANCE* pEffectInstances, 
			DWORD                     NumMaterials, 
			CONST DWORD *             pAdjacency, 
			LPD3DXSKININFO pSkinInfo, 
			LPD3DXMESHCONTAINER *ppNewMeshContainer);    
		STDMETHOD(DestroyFrame)(THIS_ LPD3DXFRAME pFrameToFree);
		STDMETHOD(DestroyMeshContainer)(THIS_ LPD3DXMESHCONTAINER pMeshContainerBase);

		CAllocateHierarchy() : m_pBoneMatrices(NULL), m_NumBoneMatricesMax(0) {}
		~CAllocateHierarchy();

		D3DXMATRIXA16*              m_pBoneMatrices;
		UINT                        m_NumBoneMatricesMax;
	};

#define ANIM_TRANSITION_TIME 0.25		//平滑变换时间

	class CSkinMesh : public CSceneNode
	{
	public:
		CSkinMesh();
		virtual ~CSkinMesh();

	private:
		HRESULT __SetupBoneMatrixPointers( LPD3DXFRAME pFrame );
		HRESULT __SetupBoneMatrixPointersOnMesh( LPD3DXMESHCONTAINER pMeshContainerBase );

		HRESULT __LoadFromXFile(WCHAR* strFileName);
		VOID    __UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix ); 

		VOID __DrawFrame( LPD3DXFRAME pFrame, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );
		VOID __DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );
	public:
		HRESULT OnCreate(BASICCHAR * strEffectName, BASICCHAR *strFileName);
		HRESULT OnDestory();
		HRESULT OnReset();
		HRESULT OnLost();
		void Update(float fElapsedAppTime);
		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);
		bool ApplyForRender();
		CEffect& GetEffect() { return m_Effect; }
		VOID SetMatView(const D3DXMATRIXA16& view) { m_matView = view; }
		/** 通过索引设定动作
		@param
		index 目标动作索引
		@param
		dwControlPlayTime 动作播放时间
		@param
		bSmooth 是否平滑过渡
		*/
		void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		/** 通过名称设定动作
		@param
		pName 目标动作名称
		@param
		dwControlPlayTime 动作播放时间
		@param
		bSmooth 是否平滑过渡
		*/
		void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		/** 动作平滑过渡
		*/
		void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet);
		/** 得到当前正在播放的动画的播放次数
		*/
		DWORD GetPlayTime();
		/** 从名称获取动作ID
		@param
		sString 名称
		*/
		DWORD GetAnimIndex( char sString[] );
		bool CanPlay(LPSTR pName, DWORD count);

	private:
		CAllocateHierarchy*         m_pAlloc;
		LPD3DXFRAME					m_pFrameRoot;
		//ID3DXEffect*				m_pEffect;
		CEffect						m_Effect;
		D3DXMATRIXA16				m_matView;
		LPD3DXANIMATIONCONTROLLER	m_pAnimController;
		BOOL						m_bPlayAnim;

		/// 当前动作
		DWORD						m_dwCurrentTrack;
		/// 当前时间 
		float						m_fTimeCurrent;
		/// 当前动作播放次数
		DWORD						m_dwPlayTime;
		/// 当前动画播放时间
		float						m_fFrameTime;
		/// 当前动画总播放时间
		double						m_lfTotalFrameTime;
		/// 当前动作设定播放次数(0为重复播放)
		DWORD						m_dwControlPlayTime;
		/// 动作名称　
		char						m_szASName[64];
		/// 当前动作名称
		std::string					m_strNowAnimSetName;
	};
}