#pragma once

#include "debug.h"
#include "BasicString.h"
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
		LPDIRECT3DTEXTURE9*	 ppBumpMaps;            //��͹��������
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
		HRESULT __AllocateName( LPCSTR Name, LPSTR *pNewName );
		HRESULT __GenerateSkinnedMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);
		HRESULT __GenerateDeclMesh(D3DXMESHCONTAINER_DERIVED *pMeshContainer);

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

		CAllocateHierarchy(void);
		~CAllocateHierarchy(void);

	private:
		BASICCHAR					m_strFilePath[MAX_PATH];
		D3DXMATRIXA16*              m_pBoneMatrices;
		UINT                        m_NumBoneMatricesMax;

		friend class CSkinMesh;
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

		HRESULT __LoadFromXFile( const PBASICCHAR strFileName );
	
		VOID __UpdateFrameMatrices( LPD3DXFRAME pFrameBase, LPD3DXMATRIX pParentMatrix ); 
		VOID __DrawFrame( LPD3DXFRAME pFrame, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );
		VOID __DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );

		void __GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d);

	public:
		bool Create(const PBASICCHAR fileName);
		virtual bool Destroy();
		virtual bool ApplyForRender();
		virtual void Update();
		virtual void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		HRESULT Restore();
		HRESULT Lost();

		void SetEffectFile(const BASICSTRING& file);

		const BASICSTRING& GetEffectFile() const;
		const CEffect& GetEffect() const;
		const LPD3DXMESH GetMesh() const;
		
		// ͨ�������趨����
		void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		// ͨ�������趨����
		void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		// ����ƽ������
		void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet);
		// �õ���ǰ���ڲ��ŵĶ����Ĳ��Ŵ���
		DWORD GetPlayTime();
		// �����ƻ�ȡ����ID
		DWORD GetAnimIndex( char sString[] );
		// �����Ƿ��ܲ���
		bool CanPlay(LPSTR pName, DWORD count);

	private:
		CAllocateHierarchy			m_Alloc;
		LPD3DXFRAME					m_pFrameRoot;
		CEffect						m_Effect;
		D3DXMATRIXA16				m_matView;
		LPD3DXANIMATIONCONTROLLER	m_pAnimController;
		BOOL						m_bPlayAnim;
		
		DWORD						m_dwCurrentTrack;		// ��ǰ����
		FLOAT						m_fTimeCurrent;			// ��ǰʱ��	
		DWORD						m_dwPlayTime;			// ��ǰ�������Ŵ���	
		FLOAT						m_fFrameTime;			// ��ǰ��������ʱ��	
		DOUBLE						m_lfTotalFrameTime;		// ��ǰ�����ܲ���ʱ��
		DWORD						m_dwControlPlayTime;	// ��ǰ�����趨���Ŵ���(0Ϊ�ظ�����)��
		char						m_szASName[64];			// ��������
		std::string					m_strNowAnimSetName;	// ��ǰ��������

		BASICSTRING					m_strEffectFile;		// Ч���ļ�
	
		struct BoxVertex
		{
			D3DXVECTOR3 p;

			enum FVF
			{
				FVF_Flags = D3DFVF_XYZ
			};
		};
	};

	//---------------------------------------------------------------------------
	// ���ú���
	//---------------------------------------------------------------------------

	inline void CSkinMesh::SetEffectFile(const BASICSTRING& file)
	{
		m_strEffectFile = file;
	}
		
	inline const BASICSTRING& CSkinMesh::GetEffectFile() const
	{
		return m_strEffectFile;
	}

	//---------------------------------------------------------------------------
	// ��ȡ����
	//---------------------------------------------------------------------------

	inline const CEffect& CSkinMesh::GetEffect() const
	{ 
		return m_Effect;
	}
}