#ifndef DATAYPE_H
#define DATAYPE_H

///
// 类型定义
///
namespace zerO
{
#ifdef _UNICODE
	typedef wchar_t BASICCHAR, * PBASICCHAR;

#ifndef TEXT
	#define TEXT(a) L ## a
#endif

#else
	typedef char BASICCHAR, * PBASICCHAR;

#ifndef TEXT
	#define TEXT(a) a
#endif

#endif

	typedef char             INT8  ,  * PINT8;
	typedef short            INT16 ,  * PINT16;
	typedef long             INT32 ,  * PINT32;
	typedef __int64          INT64 ,  * PINT64;

	typedef int              INT  ,   * PINT;

	typedef unsigned char    UINT8 ,  * PUINT8;
	typedef unsigned short   UINT16,  * PUINT16;
	typedef unsigned long    UINT32,  * PUINT32;
	typedef unsigned __int64 UINT64,  * PUINT64;

	typedef unsigned int     UINT,    * PUINT;

	typedef float            FLOAT32, * PFLOAT32;
	typedef double           FLOAT64, * PFLOAT64;

	typedef float            FLOAT,   * PFLOAT;
}

#endif