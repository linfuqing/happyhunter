#pragma once
#include "Resource.h"

namespace zerO
{
	///
	// ¶¥µã»º³å,IDirect3DVertexBuffer9 ·â×°.
	///
	class CIndexBuffer :
		public CResource
	{
	public:
		CIndexBuffer(void);
		~CIndexBuffer(void);

		bool Create(D3DPRIMITIVETYPE Type, UINT uCount, DWORD dwUsage, D3DPOOL Pool, void* pData);
		bool Destroy(); 
		bool Disable(); 
		bool Restore(); 

		bool CreateSingleStripGrid(
		UINT uVerticesX,
		UINT uVerticesY,
		UINT uStepX,
		UINT uStepY,
		UINT uStride,
		DWORD dwUsage, 
		D3DPOOL Pool);

		bool Lock(DWORD dwFlags, void** ppData);
		bool Unlock();

		void Activate()const;

		LPDIRECT3DINDEXBUFFER9 GetBuffer()const;
		D3DPRIMITIVETYPE GetType()const;

		UINT GetPrimitiveCount(UINT uNumMember)const;
		UINT GetPrimitiveCount()const;
	private:
		LPDIRECT3DINDEXBUFFER9 m_pBuffer;
		D3DPRIMITIVETYPE m_Type;
		PUINT8 m_puData;

		void* m_pLockData;
		UINT m_uLockOffset;
		UINT m_uLockSize;

		UINT m_uMemberCount;
		UINT m_uStride;
		UINT m_uByteSize;
		DWORD m_dwUsage;

		D3DPOOL m_Pool;

		D3DFORMAT m_Format;
	};

	inline LPDIRECT3DINDEXBUFFER9 CIndexBuffer::GetBuffer()const
	{
		return m_pBuffer;
	}

	inline D3DPRIMITIVETYPE CIndexBuffer::GetType()const
	{
		return m_Type;
	}

	inline UINT CIndexBuffer::GetPrimitiveCount(UINT uNumMember)const
	{
		return (m_Type == D3DPT_TRIANGLESTRIP || m_Type == D3DPT_TRIANGLEFAN) ? (uNumMember - 2) : (uNumMember / 3);
	}

	inline UINT CIndexBuffer::GetPrimitiveCount()const
	{
		return GetPrimitiveCount(m_uMemberCount);
	}

	inline bool CIndexBuffer::Lock(DWORD dwFlags, void** ppData)
	{
		if(!m_pBuffer)
			return false;

		HRESULT hr = m_pBuffer->Lock(0, m_uByteSize, ppData, dwFlags);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		m_uLockOffset = 0;
		m_uLockSize   = m_uByteSize;

		m_pLockData   = *ppData;

		return true;
	}

	inline bool CIndexBuffer::Unlock()
	{
		if(!m_pBuffer)
			return false;

		HRESULT hr = m_pBuffer->Unlock();

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		if(m_puData && m_Pool == D3DPOOL_DEFAULT)
			memcpy(m_puData + m_uLockOffset, m_pLockData, m_uLockSize);

		m_uLockOffset = 0;
		m_uLockSize   = 0;
		m_pLockData   = NULL;

		return true;
	}

	inline void CIndexBuffer::Activate()const
	{
		DEBUG_WARNING_ASSERT(m_pBuffer, "Attempting to activate an invalid buffer");

		HRESULT hr = DEVICE.SetIndices(m_pBuffer);

		DEBUG_ASSERT(SUCCEEDED(hr), hr);
	}
}
