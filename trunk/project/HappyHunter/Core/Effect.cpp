#include "StdAfx.h"
#include "Effect.h"
#include "BasicString.h"
#include "Camera.h"

using namespace zerO;

CEffect::CEffect(void) :
CResource(RESOURCE_EFFECT),
m_pEffect(NULL), 
m_pSurface(NULL)
{
}

CEffect::~CEffect(void)
{
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
	memset( m_TextureHandles,       0, sizeof(m_TextureHandles) );
	memset( m_TextureMatrixHandles, 0, sizeof(m_TextureMatrixHandles) );
	memset( m_ParameterHandles,     0, sizeof(m_ParameterHandles) );

	static BASICCHAR s_Numerals[] = TEXT("0123456789");

	D3DXHANDLE         hParameter;
	D3DXPARAMETER_DESC ParameterDesc;
	CBasicString       Semantic;
	UINT32             uNumberPosition, uIndex;

	for(UINT32 i = 0; i < m_EffectDesc.Parameters; i ++)
	{
		hParameter = m_pEffect->GetParameter(NULL, i);
		m_pEffect->GetParameterDesc(hParameter, &ParameterDesc);

		if(ParameterDesc.Semantic != NULL)
		{
			if(ParameterDesc.Class == D3DXPC_MATRIX_ROWS || ParameterDesc.Class == D3DXPC_MATRIX_COLUMNS)
			{
				if( STRCMP(ParameterDesc.Semantic, "WORLDVIEWPROJECTION") == 0)
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
					Semantic = (PBASICCHAR)ParameterDesc.Semantic;
					uNumberPosition = Semantic.find_first_of(s_Numerals);

					if(uNumberPosition != CBasicString::npos)
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
				else if(STRCMP(ParameterDesc.Semantic, "POSITION") == 0)
					m_ParameterHandles[POSITION] = hParameter;
				else if(STRCMP(ParameterDesc.Semantic, "UV") == 0 || STRCMP(ParameterDesc.Semantic, "TextureUV") == 0)
					m_ParameterHandles[UV] = hParameter;
			}
			else if(ParameterDesc.Class == D3DXPC_SCALAR)
			{
				if( STRCMP(ParameterDesc.Semantic, "MATERIALPOWER") == 0 )
					m_ParameterHandles[SPECULAR_MATERIAL_POWER] = hParameter;
			}
			else if(ParameterDesc.Class == D3DXPC_OBJECT)
			{
				if(ParameterDesc.Type == D3DXPT_TEXTURE
				|| ParameterDesc.Type == D3DXPT_TEXTURE2D
				|| ParameterDesc.Type == D3DXPT_TEXTURE3D
				|| ParameterDesc.Type == D3DXPT_TEXTURECUBE)
				{
					Semantic        = (PBASICCHAR)ParameterDesc.Semantic;
					uNumberPosition = Semantic.find_first_of(s_Numerals);

					if(uNumberPosition != CBasicString::npos)
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


bool CEffect::Begin()
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	HRESULT hr = m_pEffect->Begin(NULL, D3DXFX_DONOTSAVESTATE|D3DXFX_DONOTSAVESHADERSTATE);

    if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
        return false;
	}

	SetMatrix(VIEW, CAMERA.GetViewMatrix() );
	SetMatrix(PROJECTION, CAMERA.GetProjectionMatrix() );
	SetMatrix(VIEW_PROJECTION, CAMERA.GetViewProjectionMatrix() );

	return true;
}

bool CEffect::End()
{
	DEBUG_ASSERT(m_pEffect, "This Effect is not valid");

	HRESULT hr = m_pEffect->End();

    if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);

		return false;
	}

	return true;
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
			if( !SetTexture( i, *m_pSurface->GetTexture(i) ) )
				return false;
	}

	return true;
}