#ifndef BASICUTILS_H
#define BASICUTILS_H

#include "datatype.h"
#include "debug.h"

///
// 附带工具
///
namespace zerO
{
	///
	// 快速排序
	///
	template<typename T>
	void Sort(T Output[], const T Input[], UINT uLength, bool IsWrap(const T& Source, const T& Target) )
	{
		UINT i, j, uStage = 1;
		T Temp;

		if(Output != Input)
			memcpy( Output, Input, sizeof(Output) );

		while ((uStage * 3 + 1) < uLength) 
			uStage = 3 * uStage + 1;

		while(uStage > 0) 
		{
			for (i = uStage - 1; i < uLength; i ++) 
			{
				Temp = Output[i];

				for(j = i; (j >= uStage) && IsWrap(Temp, Output[j - uStage]); j -= uStage) 
					Output[j] = Output[j - uStage];
	            
				Output[j] = Temp;
			}

			uStage = uStage / 3;
		}
	}

	inline INT HighestBitSet(UINT8 uInput)
	{
		DEBUG_ASSERT(uInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dl, uInput 
		_asm and edx, 0xff
		_asm bsr eax, edx 
		_asm mov nResult, eax

		return nResult;
	}

	inline INT HighestBitSet(INT8 nInput)
	{
		DEBUG_ASSERT(nInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dl, nInput
		_asm and edx, 0xff
		_asm bsr eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT HighestBitSet(UINT16 uInput)
	{
		DEBUG_ASSERT(uInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dx, uInput
		_asm and edx, 0xffff
		_asm bsr eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT HighestBitSet(INT16 nInput)
	{
		DEBUG_ASSERT(nInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dx, nInput
		_asm and edx, 0xffff
		_asm bsr eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT LowestBitSet(UINT8 uInput)
	{
		DEBUG_ASSERT(uInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dl, uInput
		_asm and edx, 0xff
		_asm bsf eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT LowestBitSet(INT8 nInput)
	{
		DEBUG_ASSERT(nInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dl, nInput
		_asm and edx, 0xff 
		_asm bsf eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT LowestBitSet(UINT16 uInput)
	{
		DEBUG_ASSERT(uInput, "Zero is invalid input!");

		register INT nResult;

		_asm mov dx, uInput
		_asm and edx, 0xffff
		_asm bsf eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	inline INT LowestBitSet(INT16 nInput)
	{
		register INT nResult;

		_asm mov dx, nInput
		_asm and edx, 0xffff
		_asm bsf eax, edx
		_asm mov nResult, eax

		return nResult;
	}

	//-----------------------------------------------------------------------------
	// Desc: 从绝对路径中提取文件名
	//-----------------------------------------------------------------------------
	inline void RemovePathFromFileName(LPSTR fullPath, LPWSTR fileName)
	{
		//先将fullPath的类型变换为LPWSTR
		WCHAR wszBuf[MAX_PATH];
		MultiByteToWideChar( CP_ACP, 0, fullPath, -1, wszBuf, MAX_PATH );
		wszBuf[MAX_PATH-1] = L'\0';

		WCHAR* wszFullPath = wszBuf;

		//从绝对路径中提取文件名
		LPWSTR pch=wcsrchr(wszFullPath,'\\');
		if (pch)
			lstrcpy(fileName, ++pch);
		else
			lstrcpy(fileName, wszFullPath);
	}
}

#endif