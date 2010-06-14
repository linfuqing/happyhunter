#ifndef BASICUTILS_H
#define BASICUTILS_H

#include "datatype.h"

///
// 附带工具
///
namespace zerO
{
	///
	// 快速排序
	///
	template<typename T>
	void Sort(T Array[], UINT uLength, bool IsWrap(const T& Source, const T& Target) )
	{
		UINT i, j, uStage = 1;
		T Temp;

		while ((uStage * 3 + 1) < uLength) 
			uStage = 3 * uStage + 1;

		while(uStage > 0) 
		{
			for (i = uStage - 1; i < uLength; i ++) 
			{
				Temp = Array[i];

				for(j = i; (j >= uStage) && IsWrap(Temp, Array[j - uStage]); j -= uStage) 
					Array[j] = Array[j - uStage];
	            
				Array[j] = Temp;
			}

			uStage = uStage / 3;
		}
	}
}

#endif