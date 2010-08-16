#include "StdAfx.h"
#include "Effect.h"
#include "Camera.h"
#include "safefunction.h"
#include <string>

using namespace zerO;

CEffect::CEffect(void) :
CResource(RESOURCE_EFFECT),
m_pEffect(NULL), 
m_pSurface(NULL),
m_bIsBegin(false),
m_bIsBeginPass(false)
{
}

CEffect::~CEffect(void)
{
	Destroy();
}

void CEffect::Clone(CEffect& Effect)const
{
	Effect.m_pSurface   = m_pSurface;
	Effect.m_pEffect    = m_pEffect;
	Effect.m_hTechnique = m_hTechnique;

	memcpy( &Effect.m_EffectDesc         , &m_EffectDesc         , sizeof(Effect.m_EffectDesc          ) );
	memcpy( &Effect.m_TechniqueDesc      , &m_TechniqueDesc      , sizeof(Effect.m_TechniqueDesc       ) );

	memcpy( Effect.m_MatrixHandles       , m_MatrixHandles       , sizeof(Effect.m_MatrixHandles       ) );
	memcpy( Effect.m_MatrixArrayHandles  , m_MatrixArrayHandles  , sizeof(Effect.m_MatrixArrayHandles  ) );
	memcpy( Effect.m_TextureHandles      , m_TextureHandles      , sizeof(Effect.m_TextureHandles      ) );
	memcpy( Effect.m_TextureMatrixHandles, m_TextureMatrixHandles, sizeof(Effect.m_TextureMatrixHandles) );
	memcpy( Effect.m_ParameterHandles    , m_ParameterHandles    , sizeof(Effect.m_ParameterHandles    ) );

	Effect.m_bIsBegin     = m_bIsBegin;
	Effect.m_bIsBeginPass = m_bIsBeginPass;
}

bool CEffect::Destroy()
{
	DEBUG_RELEASE(m_pEffect);

	m_pEffect = NULL;

	return true;
}

bool CEffect::Disable()
{
	HRESULT hr = m_pEffect->OnLostDevice();

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
}

bool CEffect::Restore()
{
	HRESULT hr = m_pEffect->OnResetDevice();

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
}

bool CEffect::Load(const PBASICCHAR pcFileName)
{
	LPD3DXBUFFER pBufferErrors = NULL;
	HRESULT hr = D3DXCreateEffectFromFile(&DEVICE, pcFileName, NULL, NULL, D3DXSHADER_DEBUG, NULL, &m_pEffect, &pBufferErrors);

	if( FAILED(hr) )
	{
		DEBUG_ERROR( pBufferErrors->GetBufferPointer() );

		DEBUG_RELEASE(pBufferErrors);

		return false;
	}

	DEBUG_RELEASE(pBufferErrors);

	hr = m_pEffect->GetDesc( &m_EffectDesc );

	if( FAILED(hr) )
	{
		DEBUG_ERROR(hr);
		DEBUG_RELEASE(m_pEffect);
        return false;
	}

	hr = m_pEffect->FindNextValidTechnique(NULL, &m_hTechnique);

    if( FAILED(hr) )
	{
		DEBUG_ERROR(hr);
		DEBUG_RELEASE(m_pEffect);
        return false;
	}

	hr = m_pEffect->GetTechniqueDesc(m_hTechnique, &m_TechniqueDesc);

    if( FAILED(hr) )
	{
		DEBUG_ERROR(hr);
		DEBUG_RELEASE(m_pEffect);
        return false;
	}

	hr = m_pEffect->SetTechnique(m_hTechnique);

    if( FAILED(hr) )
	{
		DEBUG_ERROR(hr);
		DEBUG_RELEASE(m_pEffect);
        return false;
	}

	__ParseParameters();

	if(m_pSurface)
		return __ApplySurface();

	return true;
}

void CEffect::__ParseParameters()
{
	memset( m_MatrixHandles,        0, sizeof(m_MatrixHandles) );
	memset( m_MatrixArrayHandles,   0, sizeof(m_MatrixArrayHandles) );
	memset( m_TextureHandles,       0, sizeof(m_TextureHandles) );
	memset( m_TextureMatrixHandles, 0, sizeof(m_TextureMatrixHandles) );
	memset( m_ParameterHandles,     0, sizeof(m_ParameterHandles) );

	static CHAR s_Numerals[] = "0123456789";

	D3DXHANDLE                                        hParameter;
	D3DXPARAMETER_DESC                                ParameterDesc;
	std::basic_string< CHAR, std::char_traits<CHAR> > Semantic;
	UINT32                                            uNumberPosition, uIndex;

	for(UINT32 i = 0; i < m_EffectDesc.Parameters; i ++)
	{
		hParameter = m_pEffect->GetParameter(NULL, i);
		m_pEffect->GetParameterDesc(hParameter, &ParameterDesc);

		if(ParameterDesc.Semantic != NULL)
		{
			if(ParameterDesc.Class == D3DXPC_MATRIX_ROWS || ParameterDesc.Class == D3DXPC_MATRIX_COLUMNS)
			{
				if(STRCMP(ParameterDesc.Semantic, "WORLDMATRIXARRAY") == 0)
					m_MatrixArrayHandles[WORLD_MATRIX_ARRAY] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "WORLDVIEWPROJECTION") == 0)
					m_MatrixHandles[WORLD_VIEW_PROJECTION] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "VIEWPROJECTION") == 0)
					m_MatrixHandles[VIEW_PROJECTION] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "WORLDVIEW") == 0)
					m_MatrixHandles[WORLD_VIEW] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "WORLD") == 0)
					m_MatrixHandles[WORLD] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "VIEW") == 0)
					m_MatrixHandles[VIEW] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "PROJECTION") == 0)
					m_MatrixHandles[PROJECTION] = hParameter;
				else if(STRNICMP(ParameterDesc.Semantic, "TEXTURE", 3) == 0)
				{
					Semantic.assign(ParameterDesc.Semantic);
					uNumberPosition = Semantic.find_first_of(s_Numerals);

					if(uNumberPosition != std::basic_string<CHAR, std::char_traits<CHAR> >::npos)
					{
						uIndex = atoi(&ParameterDesc.Semantic[uNumberPosition]);
						if(uIndex < MAXINUM_TEXTURE_HANDLES)
							m_TextureMatrixHandles[uIndex] = hParameter;
					}
				}
			}
			else if(ParameterDesc.Class == D3DXPC_VECTOR)
			{
				if( STRCMP(ParameterDesc.Semantic, "MATERIALAMBIENT") == 0)
					m_ParameterHandles[AMBIENT_MATERIAL_COLOR] = hParameter;
				else if( STRCMP(ParameterDesc.Semantic, "MATERIALDIFFUSE") == 0 )
					m_ParameterHandles[DIFFUSE_MATERIAL_COLOR] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "MATERIALEMISSIVE") == 0)
					m_ParameterHandles[EMISSIVE_MATERIAL_COLOR] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "MATERIALSPECULAR") == 0)
					m_ParameterHandles[SPECULAR_MATERIAL_COLOR] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "LIGHTAMBIENT") == 0)
					m_ParameterHandles[AMBIENT_LIGHT_COLOR] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "POSITIONOFFSET") == 0)
					m_ParameterHandles[POSITION_OFFSET] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "UVOFFSET") == 0)
					m_ParameterHandles[UV_OFFSET] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "EYEPOSITION") == 0 || STRCMP(ParameterDesc.Semantic, "CAMERAPOSITION") == 0)
					m_ParameterHandles[EYE_POSITION] = hParameter;
			}
			else if(ParameterDesc.Class == D3DXPC_SCALAR)
			{
				if( STRCMP(ParameterDesc.Semantic, "MATERIALPOWER") == 0 )
					m_ParameterHandles[SPECULAR_MATERIAL_POWER] = hParameter;
				else if( STRCMP(ParameterDesc.Semantic, "BONEINFLUENCESNUMBER") == 0 )
					m_ParameterHandles[BONE_INFLUENCES_NUMBER] = hParameter;
			}
			else if(ParameterDesc.Class == D3DXPC_OBJECT)
			{
				if(ParameterDesc.Type == D3DXPT_TEXTURE
				|| ParameterDesc.Type == D3DXPT_TEXTURE2D
				|| ParameterDesc.Type == D3DXPT_TEXTURE3D
				|| ParameterDesc.Type == D3DXPT_TEXTURECUBE)
				{
					Semantic.assign(ParameterDesc.Semantic);
					uNumberPosition = Semantic.find_first_of(s_Numerals);

					if(uNumberPosition != std::basic_string<CHAR, std::char_traits<CHAR> >::npos)
					{
						uIndex = atoi(&ParameterDesc.Semantic[uNumberPosition]);
						if(uIndex < MAXINUM_TEXTURE_HANDLES)
							m_TextureHandles[uIndex] = hParameter;
					}
				}
			}
		}
	}
}

bool CEffect::BeginPass(zerO::UINT uPass)
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	if(m_bIsBeginPass)
		EndPass();

	m_bIsBeginPass = true;

	HRESULT hr = m_pEffect->BeginPass(uPass);

    if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
}

bool CEffect::EndPass()
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	m_bIsBeginPass = false;

	HRESULT hr = m_pEffect->EndPass();

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
        return false;
	}

	return true;
}

bool CEffect::Begin()
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	if(m_bIsBegin)
		End();

	HRESULT hr = m_pEffect->Begin(NULL, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE);

    if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
        return false;
	}

	SetMatrix(VIEW, CAMERA.GetViewMatrix() );
	SetMatrix(PROJECTION, CAMERA.GetProjectionMatrix() );
	SetMatrix(VIEW_PROJECTION, CAMERA.GetViewProjectionMatrix() );

	D3DXVECTOR4 RGBA;
	LIGHTMANAGER.GetAmbientRGBA(RGBA);

	SetParameter(AMBIENT_LIGHT_COLOR, &RGBA);
	SetParameter( EYE_POSITION, &CAMERA.GetWorldPosition() );

	m_bIsBegin = true;

	return true;
}

bool CEffect::End()
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	if(!m_bIsBeginPass)
		EndPass();

	m_bIsBegin = false;

	HRESULT hr = m_pEffect->End();

    if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	DEVICE.SetVertexShader(NULL);
	DEVICE.SetPixelShader(NULL);

	return true;
}

bool CEffect::Active(zerO::UINT uPass)
{
	return Begin() && BeginPass(uPass);
}

bool CEffect::SetSurface(CSurface* const pSurface)
{
	m_pSurface = pSurface;
	if(m_pEffect)
		return __ApplySurface();

	return true;
}

bool CEffect::__ApplySurface()
{
	const D3DMATERIAL9& MATERIAL = m_pSurface->GetMaterial();
	SetParameter(AMBIENT_MATERIAL_COLOR, &MATERIAL.Ambient);
	SetParameter(DIFFUSE_MATERIAL_COLOR, &MATERIAL.Diffuse);
	SetParameter(EMISSIVE_MATERIAL_COLOR, &MATERIAL.Emissive);
	SetParameter(SPECULAR_MATERIAL_COLOR, &MATERIAL.Specular);
	SetParameter(SPECULAR_MATERIAL_POWER, &MATERIAL.Power);

	for(UINT32 i = 0; i< MAXINUM_TEXTURE_HANDLES; i++)
	{
		if( TEST_BIT(m_pSurface->GetTextureFlag(), i) )
			SetTexture( i, *m_pSurface->GetTexture(i) );
	}

	return true;
}