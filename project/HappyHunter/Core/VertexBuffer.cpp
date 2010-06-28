#include "StdAfx.h"
#include "VertexBuffer.h"

using namespace zerO;

CVertexBuffer::CVertexBuffer(void) :
m_pBuffer(NULL),
m_pVertexDeclaration(NULL),
m_uVertexElementCount(0),
uStride(0),
m_uMemberCount(0),
m_uStride(0),
m_uByteSize(0),
m_dwUsage(0),
m_Pool(D3DPOOL_FORCE_DWORD)
{
}

CVertexBuffer::~CVertexBuffer(void)
{
}

bool CVertexBuffer::Create(zerO::UINT uCount, zerO::UINT uStride, DWORD dwUsage, D3DPOOL Pool, void* pData)//(UINT uCount, UINT uStride, UINT16 uFlag, void* pData)
{
	DEBUG_ASSERT(m_pBuffer == NULL, "Buffer already allocated");
	/*DEBUG_ASSERT(m_puBackupCopy == NULL, "Backup buffer already allocated");*/

	/*m_uUsageFlags  = 0;*/

	m_uStride      = uStride;
	m_uMemberCount = uCount;
	/*m_uTypeFlags   = uFlag;
	m_uStateFlags  = 0;*/
	m_uByteSize    = uStride * uCount;

	m_dwUsage      = dwUsage;
	m_Pool         = Pool;

	HRESULT hr = DEVICE.CreateVertexBuffer(m_uByteSize, dwUsage, 0, Pool, &m_pBuffer,NULL);

	if( FAILED(hr) )
	{
		m_pBuffer = NULL;
		DEBUG_WARNING(hr);
		return false;
	}

	if(pData)
	{
		void* pVertices;

		hr = m_pBuffer->Lock(0, m_uByteSize, &pVertices, D3DLOCK_DISCARD);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);
			return false;
		}

		memcpy(pVertices, pData, m_uByteSize);

		hr = m_pBuffer->Unlock();

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);
			return false;
		}
	}

	return true;

	//if(TEST_BIT(m_uTypeFlags, SOFTWARE) || !HARDWARE_VERTEX_SHADERS_ALLOWED)
	//{
	//	SET_BIT(m_uTypeFlags, SOFTWARE);
	//	m_uUsageFlags |= D3DUSAGE_SOFTWAREPROCESSING;
	//}

	//if( TEST_BIT(m_uTypeFlags, DYNAMIC_OVERWRITE) )
	//{
	//	DEBUG_ASSERT(!TEST_BIT(uFlag, DYNAMIC_UPDATE), "DYNAMIC_OVERWRITE and DYNAMIC_UPDATE are mutually exclusive");
	//	m_uUsageFlags |= D3DUSAGE_DYNAMIC;
	//}

	//if( TEST_BIT(m_uTypeFlags, SOFTWARE) )
	//{
	//	CLEAR_BIT(m_uTypeFlags, RAM_BACKUP);

	//	SET_BIT(m_uTypeFlags, READABLE);
	//}
	//else
	//{
	//	if(TEST_BIT(m_uTypeFlags,DYNAMIC_UPDATE) 
	//		|| TEST_BIT(m_uTypeFlags,READABLE)
	//		|| TEST_BIT(m_uTypeFlags,RAM_BACKUP) )
	//	{
	//		SET_BIT(m_uTypeFlags.RAM_BACKUP);

	//		SET_BIT(m_uTypeFlags, READABLE);
	//		SET_BITm_uTypeFlags, DYNAMIC_UPDATE);
	//	}
	//}

	//if(TEST_BIT(m_uTypeFlags,DYNAMIC_UPDATE))
	//{
	//	DEBUG_ASSERT(!TEST_BIT(m_uTypeFlags,DYNAMIC_OVERWRITE), "DYNAMIC_OVERWRITE and DYNAMIC_UPDATE are mutually exclusive");
	//	m_uUsageFlags |= D3DUSAGE_DYNAMIC;
	//}

	//if(TEST_BIT(m_uTypeFlags,RAM_BACKUP)
	//	|| !TEST_BIT(m_uTypeFlags,READABLE) )
	//	m_uUsageFlags |= D3DUSAGE_WRITEONLY;

	//if( TEST_BIT(m_uTypeFlags,RAM_BACKUP) )
	//{
	//	SET_BIT(m_uTypeFlags, READABLE);

	//	//m_uBackupCopy;
	//}

	//SET_BIT( m_uStateFlags, (VOLATILE, (m_uUsageFlags & D3DUSAGE_DYNAMIC) ? true : false) );

	//return false;
}

bool CVertexBuffer::SetVertexDescription(zerO::UINT uElementCount, const D3DVERTEXELEMENT9* pElementList)
{
	DEBUG_ASSERT(uElementCount < MAXINUM_ELEMENTS, "too many vertex elements");

	for (UINT i = 0; i < uElementCount; i ++)
	{
		m_VertexElement[i] = pElementList[i];
	}

	m_uVertexElementCount = uElementCount;

	DEBUG_RELEASE(m_pVertexDeclaration);

	HRESULT hr = DEVICE.CreateVertexDeclaration(pElementList, &m_pVertexDeclaration );

	if( FAILED(hr) )
	{
		DEBUG_WARNING(hr);
		return false;
	}

	return true;
}

void CVertexBuffer::Activate(zerO::UINT uStream, zerO::UINT uIndex, bool bIsSetDeclaration)const
{
	DEBUG_ASSERT(m_pBuffer, "Attempting to activate an invalid buffer.");

	HRESULT hr = DEVICE.SetStreamSource(uStream, m_pBuffer, uIndex * m_uStride, m_uStride);

	if( FAILED(hr) )
		DEBUG_ERROR(hr);

	if(uStream == 0 && bIsSetDeclaration && m_pVertexDeclaration)
	{
		hr = DEVICE.SetVertexDeclaration(m_pVertexDeclaration);

		DEBUG_ASSERT(SUCCEEDED(hr), hr);
	}
}
