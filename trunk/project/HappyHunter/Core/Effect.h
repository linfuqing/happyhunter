#pragma once
#include "Resource.h"
#include "Surface.h"

namespace zerO
{
	///
	// 效果文件,ID3DXEffect封装,可通过解析效果文件注释关键字自动获取各种句柄.
	///
	class CEffect :
		public CResource
	{
	public:

		//通过注释自动获取的枚举句柄
		enum MATRIXHANDLE
		{
			WORLD = 0,
			VIEW,
			PROJECTION,
			
			WORLD_VIEW,
			VIEW_PROJECTION,
			WORLD_VIEW_PROJECTION,

			MAXINUM_MATRIX_HANDLES
		};
		
		//通过注释自动获取枚举变量
		enum PARAMETERHANDLE
		{
			AMBIENT_MATERIAL_COLOR = 0,
			DIFFUSE_MATERIAL_COLOR,
			EMISSIVE_MATERIAL_COLOR,
			SPECULAR_MATERIAL_COLOR,
			SPECULAR_MATERIAL_POWER,

			POSITION,
			UV,

			MAXINUM_PARAMETER_HANDLES
		};
		
		//纹理最大句柄数
		enum TEXTUREHANDLE
		{
			MAXINUM_TEXTURE_HANDLES = CSurface::MAXINUM_TEXTURE_PER_SURFACE
		};

		CEffect(void);
		~CEffect();

		bool Destroy(); 
		bool Disable(); 
		bool Restore(); 

		bool Load(const PBASICCHAR pcFileName = NULL);

		//检查枚举是否可用.
		bool IsMatrixUsed(MATRIXHANDLE Index);
		bool IsParameterUsed(PARAMETERHANDLE Index);
		bool IsTextureUsed(UINT32 uIndex);

		//各种设置
		bool SetParameter(PARAMETERHANDLE Index, const void* pData, INT uSize= D3DX_DEFAULT);
		bool SetMatrix(MATRIXHANDLE Index, const D3DXMATRIX& Matrix);
		bool SetTexture(UINT uIndex, const CTexture& Texture);
		bool SetTextureMatrix(UINT uIndex, const D3DXMATRIX& Matrix);

		bool SetSurface(CSurface* const pSurface);

		bool Begin();
		bool End();

		LPD3DXEFFECT GetEffect()const;
		CSurface* GetSurface()const;

		const D3DXEFFECT_DESC& GetEffectDesc()const;
		const D3DXTECHNIQUE_DESC& GetTechniqueDesc()const;


	private:
		void __ParseParameters();
		bool __ApplySurface();

		CSurface*                 m_pSurface;
		LPD3DXEFFECT              m_pEffect;
		D3DXEFFECT_DESC           m_EffectDesc;
		D3DXHANDLE                m_hTechnique;
		D3DXTECHNIQUE_DESC        m_TechniqueDesc;
		D3DXHANDLE                m_MatrixHandles[MAXINUM_MATRIX_HANDLES];
		D3DXHANDLE                m_TextureHandles[MAXINUM_TEXTURE_HANDLES];
		D3DXHANDLE                m_TextureMatrixHandles[MAXINUM_TEXTURE_HANDLES];
		D3DXHANDLE                m_ParameterHandles[MAXINUM_PARAMETER_HANDLES];
	};

	inline bool CEffect::IsMatrixUsed(MATRIXHANDLE Index)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		return m_MatrixHandles[Index] != NULL;
	}

	inline bool CEffect::IsParameterUsed(PARAMETERHANDLE Index)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		return m_ParameterHandles[Index] != NULL;
	}

	inline bool CEffect::IsTextureUsed(UINT32 uIndex)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		return m_TextureHandles[uIndex] != NULL;
	}

	inline bool CEffect::SetParameter(PARAMETERHANDLE Index, const void* pData, INT uSize)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		if(m_ParameterHandles[Index] != NULL)
		{
			bool bResult = SUCCEEDED( m_pEffect->SetValue(m_ParameterHandles[Index], pData, uSize) );
			DEBUG_ASSERT(bResult, "effect file error!");
			return bResult;
		}

		return false;
	}

	inline bool CEffect::SetMatrix(MATRIXHANDLE Index, const D3DXMATRIX& Matrix)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		if(m_MatrixHandles[Index] != NULL)
		{
			bool bResult = SUCCEEDED( m_pEffect->SetMatrix(m_MatrixHandles[Index], &Matrix) );
			DEBUG_ASSERT(bResult, "effect file error!");
			return bResult;
		}

		return false;
	}

	inline bool CEffect::SetTexture(UINT uIndex, const CTexture& Texture)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		DEBUG_ASSERT(uIndex < MAXINUM_TEXTURE_HANDLES, "Index error!");

		if(m_TextureHandles[uIndex] != NULL)
		{
			bool bResult = SUCCEEDED( m_pEffect->SetTexture( m_TextureHandles[uIndex], Texture.GetTexture() ) );
			DEBUG_ASSERT(bResult, "Effect file error!");
						
			if(bResult && m_TextureMatrixHandles[uIndex] != NULL && Texture.GetMatrix() )
			{
				bResult = SUCCEEDED( m_pEffect->SetMatrix( m_TextureMatrixHandles[uIndex], Texture.GetMatrix() ) );
				DEBUG_ASSERT(bResult, "Effect file error!");
				return bResult;
			}
		}

		return false;
	}

	inline bool CEffect::SetTextureMatrix(UINT uIndex, const D3DXMATRIX& Matrix)
	{
		DEBUG_ASSERT(m_pEffect, "Effect is null!");
		DEBUG_ASSERT(uIndex < MAXINUM_TEXTURE_HANDLES, "Index error!");

		if(m_TextureMatrixHandles[uIndex] != NULL)
		{
			bool bResult = SUCCEEDED( m_pEffect->SetMatrix(m_TextureMatrixHandles[uIndex], &Matrix) );
			DEBUG_ASSERT(bResult, "effect file error!");
			return bResult;
		}

		return false;
	}

	inline LPD3DXEFFECT CEffect::GetEffect() const
	{
		return m_pEffect;
	}

	inline CSurface* CEffect::GetSurface()const
	{
		return m_pSurface;
	}

	inline const D3DXEFFECT_DESC& CEffect::GetEffectDesc()const
	{
		return m_EffectDesc;
	}

	inline const D3DXTECHNIQUE_DESC& CEffect::GetTechniqueDesc()const
	{
		return m_TechniqueDesc;
	}
}
