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
		HRESULT hr = m_pBuffer->Lock(0, m_uByteSize, ppData, dwFlags);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		return true;
	}

	inline bool CIndexBuffer::Unlock()
	{
		HRESULT hr = m_pBuffer->Unlock();

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		return true;
	}

	inline void CIndexBuffer::Activate()const
	{
		DEBUG_ASSERT(m_pBuffer, "Attempting to activate an invalid buffer");

		HRESULT hr = DEVICE.SetIndices(m_pBuffer);

		DEBUG_ASSERT(SUCCEEDED(hr), hr);
	}
}
