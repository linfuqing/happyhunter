#pragma once
#include "Resource.h"
#include "BitFlag.h"

namespace zerO
{
#define HARDWARE_VERTEX_SHADERS_ALLOWED \
	( TEST_FLAG(GAMEHOST.GetDeviceSettings().BehaviorFlags, D3DCREATE_HARDWARE_VERTEXPROCESSING) || TEST_FLAG(GAMEHOST.GetDeviceSettings().BehaviorFlags, D3DCREATE_MIXED_VERTEXPROCESSING) )

	///
	// IDirect3DVertexBuffer9 ��װ.
	///
	class CVertexBuffer :
		public CResource
	{
		/*typedef enum
		{
			DYNAMIC_OVERWRITE,
			DYNAMIC_UPDATE,
			READABLE,
			RAM_BACKUP,
			SOFTWARE,	
		}FLAGBIT;

		typedef enum
		{
			LOCKED_READ,
			LOCKED_WRITE,
			DATA_INITIALIZED,
			VOLATILE
		}STATEBIT;*/

		typedef enum
		{
			MAXINUM_ELEMENTS = 16//TOTAL_BITS(UINT16)
		}CONSTANTS;
	public:
		CVertexBuffer(void);
		~CVertexBuffer(void);

		//bool Create(UINT uCount, UINT uStride, UINT16 uFlag, void* pData);
		bool Create(UINT uCount, UINT uStride, DWORD dwUsage, D3DPOOL Pool, void* pData);

		bool Lock(DWORD dwFlags, void** ppData);
		bool Unlock();

		void Activate(zerO::UINT uStream, UINT uIndex, bool bIsSetDeclaration)const;

		bool SetVertexDescription(UINT uElementCount, const D3DVERTEXELEMENT9* pElementList);

		LPDIRECT3DVERTEXBUFFER9 GetBuffer()const;
	private:
		LPDIRECT3DVERTEXBUFFER9	 m_pBuffer;
		UINT uStride;
		/*PUINT8 m_puBackupCopy;
		UINT16	m_uTypeFlags;
		UINT16	m_uStateFlags;*/
		UINT m_uMemberCount;
		UINT m_uStride;
		UINT m_uByteSize;
		/*UINT m_uLockOffset;
		UINT m_uLockSize;*/
		DWORD m_dwUsage;

		D3DPOOL m_Pool;

		UINT m_uVertexElementCount;
		D3DVERTEXELEMENT9 m_VertexElement[MAXINUM_ELEMENTS];
		LPDIRECT3DVERTEXDECLARATION9 m_pVertexDeclaration;
	};

	inline LPDIRECT3DVERTEXBUFFER9 CVertexBuffer::GetBuffer()const
	{
		return m_pBuffer;
	}

	inline bool CVertexBuffer::Lock(DWORD dwFlags, void** ppData)
	{
		HRESULT hr = m_pBuffer->Lock(0, m_uByteSize, ppData, dwFlags);

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		return true;
	}

	inline bool CVertexBuffer::Unlock()
	{
		HRESULT hr = m_pBuffer->Unlock();

		if( FAILED(hr) )
		{
			DEBUG_WARNING(hr);

			return false;
		}

		return true;
	}
}