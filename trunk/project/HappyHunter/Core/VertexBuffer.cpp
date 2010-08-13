#include "StdAfx.h"
#include "VertexBuffer.h"

using namespace zerO;

CVertexBuffer::CVertexBuffer(void) :
CResource(RESOURCE_VERTEX_BUFFER),
m_pBuffer(NULL),
m_pVertexDeclaration(NULL),
m_puData(NULL),
m_pLockData(NULL),

m_uLockOffset(0),
m_uLockSize(0),
m_uVertexElementCount(0),
uStride(0),
m_uMemberCount(0),
m_uStride(0),
m_uByteSize(0),
m_dwUsage(0),
m_dwFVF(0),
m_Pool(D3DPOOL_FORCE_DWORD)
{
}

CVertexBuffer::~CVertexBuffer(void)
{
	Destroy();
}

bool CVertexBuffer::Create(zerO::UINT uCount, zerO::UINT uStride, DWORD dwUsage, D3DPOOL Pool, void* pData, DWORD dwFVF)//(UINT uCount, UINT uStride, UINT16 uFlag, void* pData)
{
	Destroy();

	m_uStride      = uStride;
	m_uMemberCount = uCount;
	m_uByteSize    = uStride * uCount;

	m_dwUsage      = dwUsage;
	m_dwFVF        = dwFVF;

	m_Pool         = Pool;

	if(Pool == D3DPOOL_DEFAULT)
	{
		DEBUG_DELETE(m_puData);

		DEBUG_NEW(m_puData, UINT8[m_uByteSize]);

		if(pData)
			memcpy(m_puData, pData, m_uByteSize);
		else
			memset(m_puData, 0, m_uByteSize);
	}

	HRESULT hr = DEVICE.CreateVertexBuffer(m_uByteSize, dwUsage, dwFVF, Pool, &m_pBuffer, NULL);

	if( FAILED(hr) )
	{
		m_pBuffer = NULL;
		DEBUG_WARNING(hr);
		return false;
	}

	if(pData)
	{
		void* pVertices;

		if( !Lock(0, &pVertices) )
			return false;

		memcpy(pVertices, pData, m_uByteSize);

		if( !Unlock() )
			return false;
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


bool CVertexBuffer::Destroy()
{
	DEBUG_DELETE_ARRAY(m_puData);
	DEBUG_RELEASE(m_pBuffer);
	DEBUG_RELEASE(m_pVertexDeclaration);

	m_pBuffer            = NULL;
	m_pVertexDeclaration = NULL;
	m_puData             = NULL;

	return true;
}

bool CVertexBuffer::Disable()
{
	if(m_Pool == D3DPOOL_DEFAULT)
	{
		DEBUG_RELEASE(m_pBuffer);
		m_pBuffer = NULL;
	}

	return true;
}

bool CVertexBuffer::Restore()
{
	if(m_Pool == D3DPOOL_DEFAULT)
	{
		DEBUG_RELEASE(m_pBuffer);

		HRESULT hr = DEVICE.CreateVertexBuffer(m_uByteSize, m_dwUsage, m_dwFVF, m_Pool, &m_pBuffer, NULL);

		if( FAILED(hr) )
		{
			m_pBuffer = NULL;
			DEBUG_WARNING(hr);
			return false;
		}

		if(m_puData)
		{
			void* pVertices;

			if( !Lock(0, &pVertices) )
				return false;

			memcpy(pVertices, m_puData, m_uByteSize);

			if( !Unlock() )
				return false;
		}
	}

	return true;
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

	DEBUG_ASSERT(SUCCEEDED(hr), hr);

	if(bIsSetDeclaration)
	{
		if(m_dwFVF)
			hr = DEVICE.SetFVF(m_dwFVF);
		else if(m_pVertexDeclaration)
			hr = DEVICE.SetVertexDeclaration(m_pVertexDeclaration);
		else
			hr = S_FALSE;

		DEBUG_ASSERT(SUCCEEDED(hr), hr);
	}
}
