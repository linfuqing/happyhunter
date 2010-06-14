#include "StdAfx.h"
#include "BasicString.h"

using namespace zerO;


#define TEXT_BUFFERSIZE 0xfff
void __cdecl FormatText(PBASICCHAR pBuffer, zerO::UINT uCount, const PBASICCHAR pText, ...)
{
	va_list	pArgList;
	va_start(pArgList, pText);
	VSPRINTF(pBuffer, uCount, pText, pArgList);
	va_end(pArgList);
}

void __cdecl CBasicString::Format(const PBASICCHAR pText, ...)
{
	va_list	pArglist;
	va_start(pArglist, pText);

	Format(pText, pArglist);
	va_end(pArglist);
}

void __cdecl CBasicString::Format(const PBASICCHAR pText, va_list pArgList)
{
	BASICCHAR pBuffer[TEXT_BUFFERSIZE];

	VSPRINTF(pBuffer, TEXT_BUFFERSIZE, pText, pArgList);

	(*this) = pBuffer; 
}
