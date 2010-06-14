#pragma once

#include "datatype.h"

///
//比特工具
///
namespace zerO
{

	///
	//比特工具宏
	///
#define FLAG(x)                          ( 1 << (x) )
#define FLAG_VALUE(Bit, IsBit)           ( (IsBit) << (Bit) )
#define MASK(x)                          ( ( 1 << (x + 1) ) - 1 )
#define TEST_BIT(Value, Bit)             ( ( (Value) & FLAG(Bit) ) !=0 )
#define TEST_FLAG(Value, FLAG)           ( ( (Value) & (FLAG) ) == (FLAG) )
#define TEST_ANY(Value, Set)             ( ( (Value) & (Set) ) != 0 )
#define SET_BIT(Value, Bit)              ( (Value) |= FLAG( (Bit) ) )
#define SET_BIT_VALUE(Value, Bit, IsSet) ( (Value) |= FLAG_VALUE(Bit, IsSet) )
#define CLEAR_BIT(Value, Bit)            ( (Value) &= ~FLAG((Bit) ) )
#define SET_FLAG(Value, FLAG)            ( (Value) |= (FLAG) )
#define CLEAR_FLAG(Value, FLAG)          ( (Value) &= ~(FLAG) )
#define TOTAL_BITS(Type)                 (sizeof(Type) << 3)

	typedef UINT8 BIT;

	///
	//比特标志类
	///
	template<typename T>
	class CBitFlag
	{
	public:
		inline CBitFlag(void) : 
		m_Value(0)
		{
		}

		inline CBitFlag(T Value) : 
		m_Value(Value)
		{
		}

		inline CBitFlag(const CBitFlag& Src) : 
		m_Value(Src.m_Value)
		{
		}

		inline CBitFlag& operator=(T Value)
		{
			m_Value = Value;
			return *this;
		}

		inline CBitFlag& operator=(CBitFlag& Src)
		{
			m_Value = Src.m_Value;
			return *this;
		}

		inline operator T()const
		{
			return m_Value;
		}

		inline bool operator==(T Value)const
		{
			return m_Value == Value;
		}

		inline bool operator==(CBitFlag& Src)const
		{
			return m_Value == Src.m_Value;
		}

		inline void Set(T Value)
		{
			m_Value = Value;
		}

		inline void Clear()
		{
			m_Value = 0;
		}

		inline void SetFlag(T Flag)
		{
			m_Value |= Flag;
		}

		inline void ClearFlag(T Flag)
		{
			m_Value &= ~Flag;
		}

		inline void SetBit(BIT Bit)
		{
			m_Value |= 1 << BIT;
		}

		inline void ClearBit(BIT Bit)
		{
			m_Value &= ~(1 << BIT);
		}

		inline bool IsEmpty()
		{
			return m_Value == static_cast<T>(0);
		}

		inline bool TestBit(BIT Bit)
		{
			return m_Value & (1 << Bit);
		}

		inline bool TestFlag(T Flag)
		{
			return (m_Value & Flag) == Flag;
		}

		inline bool TestAny(T Value)
		{
			return m_Value & Value;
		}

		inline UINT8 GetTotalBits()const
		{
			return sizeof(T) << 3;
		}

		inline UINT8 GetTotalSet()const
		{
			UINT8 uCount = 0;

			T Value = m_Value;
			
			for (UINT8 i = sizeof(T) << 3; i > 0; i --)
			{
				uCount += (Value & 1);
				Value >>= 1;
			}

			return uCount;
		}

	private:
		T m_Value;
	};

	typedef CBitFlag<UINT8 > UINT8FLAG;
	typedef CBitFlag<UINT16> UINT16FLAG;
	typedef CBitFlag<UINT32> UINT32FLAG;
}