#pragma once

#include "debug.h"
#include "Effect.h"
#include "Texture.h"
#include "SceneNode.h"
#include "RenderMethod.h"

namespace zerO
{
	//-----------------------------------------------------------------------------
	// Desc: �̳���DXDXFRAME�ṹ�Ľṹ
	//-----------------------------------------------------------------------------
	struct D3DXFRAME_DERIVED: public D3DXFRAME
	{
		D3DXMATRIXA16 CombinedTransformationMatrix;
	};

	//-----------------------------------------------------------------------------
	// Desc: �̳���D3DXMESHCONTAINER�ṹ�Ľṹ
	//-----------------------------------------------------------------------------
	struct D3DXMESHCONTAINER_DERIVED: public D3DXMESHCONTAINER
	{
		LPDIRECT3DTEXTURE9*  ppTextures;            //��������
		LPD3DXMESH           pOrigMesh;             //ԭ����
		DWORD                NumAttributeGroups;    //��������
		DWORD                NumInfl;               //ÿ������������Ӱ����ٹ���
		LPD3DXBUFFER         pBoneCombinationBuf;   //������ϱ�
		D3DXMATRIX**         ppBoneMatrixPtrs;      //��Ź�������ϱ任����
		D3DXMATRIX*          pBoneOffsetMatrices;   //��Ź����ĳ�ʼ�任����
		DWORD                NumPaletteEntries;     //�ж��ٹ�������ʹ��(����)
		bool                 UseSoftwareVP;         //��ʶ�Ƿ�ʹ��������㴦��
	};


	//-----------------------------------------------------------------------------
	// Desc: ����������.X�ļ����ؿ�ܲ�κ�����ģ������
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

#define ANIM_TRANSITION_TIME 0.25		//ƽ���任ʱ��

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
		void Update();
		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);
		bool ApplyForRender();
		CEffect& GetEffect() { return m_Effect; }
		VOID SetMatView(const D3DXMATRIXA16& view) { m_matView = view; }
		/** ͨ�������趨����
		@param
		index Ŀ�궯������
		@param
		dwControlPlayTime ��������ʱ��
		@param
		bSmooth �Ƿ�ƽ������
		*/
		void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		/** ͨ�������趨����
		@param
		pName Ŀ�궯������
		@param
		dwControlPlayTime ��������ʱ��
		@param
		bSmooth �Ƿ�ƽ������
		*/
		void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		/** ����ƽ������
		*/
		void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet);
		/** �õ���ǰ���ڲ��ŵĶ����Ĳ��Ŵ���
		*/
		DWORD GetPlayTime();
		/** �����ƻ�ȡ����ID
		@param
		sString ����
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

		/// ��ǰ����
		DWORD						m_dwCurrentTrack;
		/// ��ǰʱ�� 
		float						m_fTimeCurrent;
		/// ��ǰ�������Ŵ���
		DWORD						m_dwPlayTime;
		/// ��ǰ��������ʱ��
		float						m_fFrameTime;
		/// ��ǰ�����ܲ���ʱ��
		double						m_lfTotalFrameTime;
		/// ��ǰ�����趨���Ŵ���(0Ϊ�ظ�����)
		DWORD						m_dwControlPlayTime;
		/// �������ơ�
		char						m_szASName[64];
		/// ��ǰ��������
		std::string					m_strNowAnimSetName;
	};
}