#pragma once

#include "GameHost.h"

namespace zerO
{
	class CSceneNode;

	///
	// 渲染队列，提供整个渲染流水线排序及优化.
	// 所有CSenceNode先锁定渲染队列获取一个渲染块,等队列满时候按照写入渲染块进行最优化排序渲染.
	///
	class CRenderQueue
	{
	public:

		typedef enum
		{
			//效果文件
			EFFECT = 0,
			//效果Pass
			EFFECT_PASS,
			//效果变量
			EFFECT_PARAM,
			//细节层次
			EFFECT_LOD,
			MODEL,
			MODEL_PARAMA,
			MODEL_PARAMB,
			SURFACE,

			TOTAL_ACTIVATE_FLAGS
		}ACTIVATEFLAG;

		///
		// 渲染块结构体.
		///
		typedef struct RENDERENTRY
		{
			//内存集中化
#pragma pack(1)
			union
			{
				struct
				{
					UINT32 uSortValueA;
					UINT32 uSortValueB;
					UINT32 uSortValueC;
				};

				struct
				{
					// uSortValueA
					UINT8 uDetailLevel: 4;	

					//枚举TYPE中的模型类型.
					UINT8 uModelType  : 2;		
					UINT8 uBoneCount  : 2;

					UINT8 uRenderPass;			
					RESOURCEHANDLE hEffect;	

					// uSortValueB

					//第二个顶点缓冲或模型框架
					UINT16 uModelParamA;	

					//第一个顶点缓冲或模型句柄
					RESOURCEHANDLE hModel;		

					// uSortValueC
					RESOURCEHANDLE hSurface;	

					//索引缓冲或模型子集
					UINT16 uModelParamB;		
				};
			};

			CSceneNode*	pParent;
			UINT32	uUserData;
#pragma pack()
			enum TYPE
			{
				BUFFER = 0,	
				MODEL,		
			};

			void Clear()
			{
				uSortValueA = 0;
				uSortValueB = 0;
				uSortValueC = 0;
			}
		}RENDERENTRY, * LPRENDERENTRY;

		CRenderQueue(UINT uMaxRenderEntries);
		~CRenderQueue(void);

		//锁定渲染队列从渲染块池获取一个渲染块.
		LPRENDERENTRY LockRenderEntry();

		//把锁定的渲染块加入渲染列表并解锁,
		void UnLockRenderEntry(LPRENDERENTRY pEntry);

		//清空渲染列表.
		void Reset();

		//渲染渲染列表中的渲染块.
		void Render();

		//更新列表
		void Update(float fElapsedTime);

	private:
		UINT m_uActiveEntries;
		UINT m_uMaxRenderEntries;

		bool m_bIsLockEntry;

		LPRENDERENTRY  m_pEntryPool;
		LPRENDERENTRY* m_ppEntryList;
	};

	inline void CRenderQueue::Reset()
	{
		m_uActiveEntries = 0;
	}
}
