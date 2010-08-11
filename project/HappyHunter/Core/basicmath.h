#ifndef BASICMATH_H
#define BASICMATH_H

#include "datatype.h"
#include "Debug.h"

///
// 基本的数学函数
///
namespace zerO
{
#define MAX(x, y) ( (x) > (y) ? (x) : (y) )
#define MIN(x, y) ( (x) < (y) ? (x) : (y) )
#define ROUND(x) (INT)( (x) > 0.0f ? ( (x) + 0.5f ) : ( (x) < 0.0f ? (x) : 0.0f ) )
#define FLOOR(x) (INT)( (x) > 0.0f ? (x) : ( (x) < 0.0f ? ( (x) - 0.5f ) : 0.0f ) )
#define CLAMP(Value, Min, Max) ( (Value) > (Max) ? (Max) : ( (Value) < (Min) ? (Min) : (Value) ) )
#define ABS(x) ( (x) < 0 ? - (x) : (x) )
#define ABS_SUBTRACT(x, y) ( (x) < (y) ? (y) - (x) : (x) - (y) )
#define EQUAL(Source, Target, Error) (ABS_SUBTRACT( (Source),(Target) ) < Error)
}

#endif