#pragma once

#include "debug.h"
#include "BasicString.h"
#include "Effect.h"
#include "Texture.h"
#include "Sprite.h"
#include "RenderMethod.h"
#include "Model.h"

namespace zerO
{
	class CSkinMesh : public CSprite
	{
	public:
		CSkinMesh();
		virtual ~CSkinMesh();

		/*void __GetBoundBox(const LPD3DXMESH pMesh, CRectangle3D& rect3d);*/

	public:
		CModel* GetModel()const;

		void Clone(CSkinMesh& SkinMesh)const;

		bool Create(const PBASICCHAR fileName);
		bool Create(CModel* const pModel);
		bool Destroy();
		bool ApplyForRender();
		void Update();
		void Render(CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag);

		void SetEffectFile(const BASICSTRING& file);

		const BASICSTRING& GetEffectFile() const;
		CEffect* GetEffect() const;
		
		LPD3DXMESH GetMesh()const;

		//// 通过索引设定动作
		//void SetAnimation( UINT index, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		//// 通过名称设定动作
		//void SetAnimationByName( LPSTR pName, DWORD dwControlPlayTime = 0, bool bSmooth = true );
		//// 动作平滑过渡
		//void AnimationChangeSmooth(LPD3DXANIMATIONSET pAnimSet);
		//// 得到当前正在播放的动画的播放次数
		//DWORD GetPlayTime();
		//// 从名称获取动作ID
		//DWORD GetAnimIndex( char sString[] );
		//// 动画是否能播放
		//bool CanPlay(LPSTR pName, DWORD count);

	private:
		void __ApplyForRenderFrame(LPD3DXFRAME pFrame);
		void __ApplyForRenderMeshContainer(LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase);

		VOID __DrawFrame( LPD3DXFRAME pFrame, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );
		VOID __DrawMeshContainer( LPD3DXMESHCONTAINER pMeshContainerBase, LPD3DXFRAME pFrameBase, CRenderQueue::LPRENDERENTRY pEntry, UINT32 uFlag );

		void __Draw(CRenderQueue::LPRENDERENTRY pEntry, zerO::UINT32 uFlag);

		CEffect*					m_pEffect;
		CModel*                     m_pModel;
		BASICSTRING					m_strEffectFile;		// 效果文件

		bool                        m_bIsCreated;
	};

	//---------------------------------------------------------------------------
	// 设置函数
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
	// 获取函数
	//---------------------------------------------------------------------------

	inline CEffect* CSkinMesh::GetEffect() const
	{ 
		return m_pEffect;
	}

	inline CModel* CSkinMesh::GetModel()const
	{
		return m_pModel;
	}

	inline LPD3DXMESH CSkinMesh::GetMesh()const
	{
		return m_pModel->GetMesh();
	}
}