#ifndef SAFEFUCTION_H
#define SAFEFUCTION_H

namespace zerO
{
#ifdef _UNICODE
#define STRCPY(pDest, pSource) wcscpy_s(pDest, pSource) 
#define STRTOK(pStr, pDelim, ppContext) wcstok_s(pStr, pDelim, ppContext)
#define VSPRINTF(pBuffer, uCount, pText, pArgList) vswprintf_s(pBuffer, uCount, pText, pArgList)
#else
#define STRCPY(pDest, pSource) strcpy_s(pDest, pSource) 
#define STRTOK(pStr, pDelim, ppContext) strtok_s(pStr, pDelim, ppContext)
#define VSPRINTF(pBuffer, uCount, pText, pArgList) vsprintf_s(pBuffer, uCount, pText, pArgList)
#endif

#define STRCMP(pStr1, pStr2)              _strcmpi(pStr1, pStr2)
#define STRNICMP(pStr1, pStr2, uMaxCount) _strnicmp(pStr1, pStr2, uMaxCount)
}

#endif