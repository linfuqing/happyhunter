#pragma once
#include "Resource.h"
#include "Sprite.h"
#include "Surface.h"
//#include "ShadowVolume.h"

namespace zerO
{
	//-----------------------------------------------------------------------------
	// Desc: 继承自DXDXFRAME结构的结构
	//-----------------------------------------------------------------------------
	typedef struct MODELFRAME: public D3DXFRAME
	{
		D3DXMATRIXA16 CombinedTransformationMatrix;
		UINT          uIndex;
	}MODELFRAME, * LPMODELFRAME;

	//-----------------------------------------------------------------------------
	// Desc: 继承自D3DXMESHCONTAINER结构的结构
	//-----------------------------------------------------------------------------
	typedef struct MODELCONTAINER: public D3DXMESHCONTAINER
	{
		UINT                 uIndex;
		LPMODELFRAME         pParent;
		PUINT8               puMeshBuffer;
		D3DXVECTOR3*         pTangentBuffer;
		D3DXVECTOR3*         pTangentInfo;
		CSurface*            pSurfaces; 
		LPD3DXMESH           pOrigMesh;             //原网格
		LPD3DXATTRIBUTERANGE pAttributeTable;
		DWORD                NumAttributeGroups;    //骨骼数量
		DWORD                NumInfl;               //每个顶点最多可以影响多少骨骼
		LPD3DXBUFFER         pBoneCombinationBuf;   //骨骼结合表
		D3DXMATRIX**         ppBoneMatrixPtrs;      //存放骨骼的组合变换矩阵
		D3DXMATRIX*          pBoneOffsetMatrices;   //存放骨骼的初始变换矩阵
		DWORD                NumPaletteEntries;     //有多少骨骼可以使用(上限)
		bool                 UseSoftwareVP;         //标识是否使用软件顶点处理
	}MODELCONTAINER, * LPMODELCONTAINER;

	//-----------------------------------------------------------------------------
	// Desc: 该类用来从.X文件加载框架层次和网格模型数据
	//-----------------------------------------------------------------------------
	class CAllocateHierarchy: public ID3DXAllocateHierarchy
	{
	private:
		HRESULT __AllocateName( LPCSTR Name, LPSTR *pNewName );
		HRESULT __GenerateSkinnedMesh(MODELCONTAINER *pMeshContainer);
		HRESULT __GenerateDeclMesh(MODELCONTAINER *pMeshContainer);

	public:
		typedef enum
		{
			HARDWARE,
			SOFTWARE
		}TYPE;

		UINT GetFramesNumber()const;
		UINT GetContainersNumber()const;
		TYPE GetType()const;

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

		CAllocateHierarchy(void);
		~CAllocateHierarchy(void);

	private:
		BASICCHAR					m_strFilePath[MAX_PATH];
		D3DXMATRIXA16*              m_pBoneMatrices;
		UINT                        m_NumBoneMatricesMax;
		UINT                        m_uNumContainers;
		TYPE                        m_Type;
		LPD3DXMESH                  m_pMesh;

		friend class CModel;
	};

	inline UINT CAllocateHierarchy::GetContainersNumber()const
	{
		return m_uNumContainers;
	}

	inline CAllocateHierarchy::TYPE CAllocateHierarchy::GetType()const
	{
		return m_Type;
	}

	///
	// 模型:骨骼基类,对D3DXFRAME和D3DXMESHCONTAINER封装.
	///
	class CModel :
		public CResource
	{
	public:
		CModel(void);
		~CModel(void);

		LPD3DXFRAME GetFrameRoot()const;

		LPMODELFRAME GetModelFrame(UINT uIndex)const;
		LPMODELCONTAINER GetModelContainer(UINT uIndex)const;

		LPD3DXMATRIX GetBoneMatrices()const;

		LPD3DXMESH GetMesh()const;

		CAllocateHierarchy& GetAllocateHierarchy();

		bool Destroy(); 
		bool Disable(); 
		bool Restore();

		bool Load(const PBASICCHAR pcFileName = NULL);

		void Update(FLOAT fElapsedAppTime);
		void Update(const LPD3DXMATRIX pMatrix);

		// 通过索引设定动作
		void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		// 通过名称设定动作
		void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		// 动作平滑过渡
		void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet);
		// 得到当前正在播放的动画的播放次数
		UINT GetPlayTime();
		// 从名称获取动作ID
		UINT GetAnimIndex( char sString[] );
		// 动画是否能播放
		bool CanPlay(LPSTR pName, DWORD count);
	private:
		bool __SetupFrame( LPD3DXFRAME pFrame );
		bool __SetupMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase );

		void __UpdateFrameMatrices(LPD3DXFRAME pFrameBase, const LPD3DXMATRIX pParentMatrix);

		void __ReleaseFrame(LPD3DXFRAME pFrame);

		CAllocateHierarchy			m_Alloc;
		LPD3DXFRAME					m_pFrameRoot;
		LPD3DXANIMATIONCONTROLLER	m_pAnimController;
		bool						m_bPlayAnim;
		
		UINT						m_uCurrentTrack;		// 当前动作
		FLOAT						m_fTimeCurrent;			// 当前时间	
		UINT						m_uPlayTime;			// 当前动作播放次数	
		FLOAT						m_fFrameTime;			// 当前动画播放时间	
		FLOAT64						m_lfTotalFrameTime;		// 当前动画总播放时间
		UINT						m_uControlPlayTime;	// 当前动作设定播放次数(0为重复播放)　
		char						m_szASName[64];			// 动作名称
		std::string					m_strNowAnimSetName;	// 当前动作名称

		LPMODELCONTAINER*           m_ppContainers;
		UINT                        m_uNumContainers;
	};

	inline LPD3DXFRAME CModel::GetFrameRoot()const
	{
		return m_pFrameRoot;
	}

	inline LPD3DXMATRIX CModel::GetBoneMatrices()const
	{
		return m_Alloc.m_pBoneMatrices;
	}

	inline LPD3DXMESH CModel::GetMesh()const
	{
		return m_Alloc.m_pMesh;
	}

	inline LPMODELCONTAINER CModel::GetModelContainer(UINT uIndex)const
	{
		if( m_ppContainers && uIndex < m_Alloc.GetContainersNumber() )
			return m_ppContainers[uIndex];

		return NULL;
	}

	inline CAllocateHierarchy& CModel::GetAllocateHierarchy()
	{
		return m_Alloc;
	}
}
