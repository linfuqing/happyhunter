#include "StdAfx.h"
#include "IndexBuffer.h"
#include "BitFlag.h"

using namespace zerO;

CIndexBuffer::CIndexBuffer(void) :
CResource(RESOURCE_INDEX_BUFFER),
m_pBuffer(NULL),
m_Type(D3DPT_FORCE_DWORD),

m_uMemberCount(0),
m_uStride(0),
m_uByteSize(0),
m_dwUsage(0),

m_Pool(D3DPOOL_FORCE_DWORD),

m_Format(D3DFMT_FORCE_DWORD)
{
}

CIndexBuffer::~CIndexBuffer(void)
{
}

bool CIndexBuffer::Create(D3DPRIMITIVETYPE Type, zerO::UINT uCount, DWORD dwUsage, D3DPOOL Pool, void* pData)
{
	DEBUG_ASSERT(m_pBuffer == NULL, "Buffer already allocated");

	if(uCount < 0xFFFF)
	{
		m_uStride  = /*TOTAL_BITS(UINT16)*/2;
		m_Format   = D3DFMT_INDEX16;
	}
	else
	{
		m_uStride  = /*TOTAL_BITS(UINT32)*/4;
		m_Format   = D3DFMT_INDEX32;
	}

	m_uMemberCount = uCount;
	m_uByteSize    = m_uStride * uCount;

	m_dwUsage      = dwUsage;
	m_Pool         = Pool;

	m_Type         = Type;

	HRESULT hr;

	hr = DEVICE.CreateIndexBuffer(m_uByteSize, dwUsage, m_Format, Pool, &m_pBuffer, NULL);

	if( FAILED(hr) )
	{
		m_pBuffer = NULL;
		DEBUG_WARNING(hr);
		return false;
	}

	if(pData)
	{
		void* pIndices;

		hr = m_pBuffer->Lock(0, m_uByteSize, &pIndices, D3DLOCK_DISCARD);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);
			return false;
		}

		memcpy(pIndices, pData, m_uByteSize);

		hr = m_pBuffer->Unlock();

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);
			return false;
		}
	}

	return true;
}

bool CIndexBuffer::CreateSingleStripGrid(
	zerO::UINT uVerticesX,
	zerO::UINT uVerticesY,
	zerO::UINT uStepX,
	zerO::UINT uStepY,
	zerO::UINT uStride,
	DWORD dwUsage, 
	D3DPOOL Pool)
{
	if(uVerticesX < 2 || uVerticesY < 2)
	{
		DEBUG_WARNING("Vertices must be more than 2.");
		return false;
	}

	UINT uTotalStrips         = uVerticesY - 1,
		uTotalIndexesPerStrip = uVerticesX << 1,
		uTotalIndexes         = (uTotalIndexesPerStrip * uTotalStrips) + (uTotalStrips << 1) - 2,
		uStartVertex          = 0, 
		uLineStep             = uStepY * uStride,
		uVertex, i, j;

	PUINT16 puIndexValues, puIndex;

	DEBUG_NEW(puIndexValues, UINT16[uTotalIndexes]);

	puIndex = puIndexValues;

	for(j = 0; j <  uTotalStrips; j ++)
	{
		uVertex = uStartVertex;

		for(i = 0;i < uVerticesX; i ++)
		{
			*(puIndex ++) = uVertex;
			*(puIndex ++) = uVertex + uLineStep;

			uVertex      += uStepX;
		}

		uStartVertex     += uLineStep;

		if(j < uTotalStrips - 1)
		{
			*(puIndex ++) = (uVertex - uStepX) + uLineStep;
			*(puIndex ++) = uStartVertex;
		}
	}

	bool bResult = Create(D3DPT_TRIANGLESTRIP, uTotalIndexes, dwUsage, Pool, puIndexValues);

	DEBUG_DELETE_ARRAY(puIndexValues);

	return bResult;
}