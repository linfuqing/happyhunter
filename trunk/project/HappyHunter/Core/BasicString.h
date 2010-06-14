#pragma once

#include <stdarg.h> 
#include <tchar.h> 
#include <string>
#include "datatype.h"
#include "safefunction.h"


namespace zerO
{
	///
	// 格式化文本,vsprintf封装.
	///
	void __cdecl FormatText(PBASICCHAR pBuffer, UINT16 uCount, const PBASICCHAR pText, ...);

	typedef std::basic_string<BASICCHAR, std::char_traits<BASICCHAR> > BASICSTRING;

	///
	// 基本字符串类.
	///
	class CBasicString : public BASICSTRING
	{
	public:
		CBasicString(void) {}
		CBasicString(const CBasicString& String);
		CBasicString(const BASICSTRING & String);
		CBasicString(const PBASICCHAR    pChar, ...);
		CBasicString(      BASICCHAR     Char  );
		~CBasicString(void) {}

		CBasicString& operator=(const CBasicString& String);
		CBasicString& operator=(const BASICSTRING & String);
		CBasicString& operator=(const PBASICCHAR    pChar );
		CBasicString& operator=(      BASICCHAR     Char  );

		CBasicString& operator+=(const CBasicString& String);
		CBasicString& operator+=(const BASICSTRING & String);
		CBasicString& operator+=(const PBASICCHAR    pChar );
		CBasicString& operator+=(      BASICCHAR     Char  );

		bool operator==(const CBasicString& String);
		bool operator==(const PBASICCHAR    pChar );
		bool operator==(const BASICSTRING & String);
		bool operator==(      BASICCHAR     Char  );

		bool operator!=(const CBasicString& String);
		bool operator!=(const PBASICCHAR    pChar ); 
		bool operator!=(const BASICSTRING & String);
		bool operator!=(      BASICCHAR     Char  );

		bool operator<(const CBasicString& String);
		bool operator<(const PBASICCHAR    pChar ); 
		bool operator<(const BASICSTRING & String);
		bool operator<(      BASICCHAR     Char  );

		bool operator<=(const CBasicString& String);
		bool operator<=(const PBASICCHAR    pChar ); 
		bool operator<=(const BASICSTRING & String);
		bool operator<=(      BASICCHAR     Char  );

		bool operator>(const CBasicString& String);
		bool operator>(const PBASICCHAR    pChar ); 
		bool operator>(const BASICSTRING & String);
		bool operator>(      BASICCHAR     Char  );

		bool operator>=(const CBasicString& String);
		bool operator>=(const PBASICCHAR    pChar ); 
		bool operator>=(const BASICSTRING & String);
		bool operator>=(      BASICCHAR     Char  );

		operator const PBASICCHAR()const;

		void __cdecl Format(const PBASICCHAR pText, ...);
		void __cdecl Format(const PBASICCHAR pText, va_list pArgList);
	};

#define NULL_STRING TEXT("")

	inline CBasicString::CBasicString(const CBasicString& String)
	{
		assign( (const BASICSTRING) String ); 
	}

	inline CBasicString::CBasicString(const BASICSTRING & String)
	{
		assign(String);
	}

	inline CBasicString::CBasicString(const PBASICCHAR    pChar, ...)
	{
		va_list pArglist;
		va_start(pArglist, pChar);

		if (*pArglist != 0)
		{
			Format(pChar, pArglist);
		}
		else
		{
			assign(pChar);
		}

		va_end(pArglist);
	}

	inline CBasicString::CBasicString(      BASICCHAR     Char  )
	{
		assign(1,Char);
	}

	inline CBasicString& CBasicString::operator=(const CBasicString& String)
	{
		assign( (const BASICSTRING) String ); 
		return (*this);
	}

	inline CBasicString& CBasicString::operator=(const BASICSTRING & String)
	{
		assign(String); 
		return (*this);
	}

	inline CBasicString& CBasicString::operator=(const PBASICCHAR    pChar)
	{
		assign(pChar);
		return (*this);
	}

	inline CBasicString& CBasicString::operator=(      BASICCHAR     Char)
	{
		assign(1, Char);
		return (*this);
	}

	inline CBasicString& CBasicString::operator+=(const CBasicString& String)
	{
		append( (const BASICSTRING) String ); 
		return (*this);
	}

	inline CBasicString& CBasicString::operator+=(const BASICSTRING & String)
	{
		append(String); 
		return (*this);
	}

	inline CBasicString& CBasicString::operator+=(const PBASICCHAR    pChar)
	{
		append(pChar);
		return (*this);
	}

	inline CBasicString& CBasicString::operator+=(      BASICCHAR     Char)
	{
		append(1, Char);
		return (*this);
	}

	inline bool CBasicString::operator==(const CBasicString& String)
	{
		return _tcscmp( c_str(), String.c_str() ) == 0;
	}

	inline bool CBasicString::operator==(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) == 0;
	}

	inline bool CBasicString::operator==(const BASICSTRING & String)
	{
		return _tcscmp( c_str(), String.c_str() ) == 0;
	}

	inline bool CBasicString::operator==(      BASICCHAR     Char)
	{
		return size() == 1 && c_str()[0] == Char;
	}

	inline bool CBasicString::operator!=(const CBasicString& String)
	{
		return _tcscmp( c_str(), String.c_str() ) != 0;
	}

	inline bool CBasicString::operator!=(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) != 0;
	}

	inline bool CBasicString::operator!=(const BASICSTRING & String)
	{
		return _tcscmp( c_str(), String.c_str() ) != 0;
	}

	inline bool CBasicString::operator!=(      BASICCHAR     Char  )
	{
		return size() != 1 || c_str()[0] != Char;
	}

	inline bool CBasicString::operator<(const CBasicString& String)
	{
		return _tcscmp( c_str(), String.c_str() ) < 0;
	}

	inline bool CBasicString::operator<(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) < 0;
	}

	inline bool CBasicString::operator<(const BASICSTRING & String)
	{
		return _tcscmp(c_str(), String.c_str()) < 0;
	}

	inline bool CBasicString::operator<(      BASICCHAR     Char  )
	{
		return size() == 1 && c_str()[0] < Char;
	}

	inline bool CBasicString::operator<=(const CBasicString& String)
	{
		return _tcscmp( c_str(), String.c_str() ) <= 0;
	}

	inline bool CBasicString::operator<=(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) <= 0;
	}

	inline bool CBasicString::operator<=(const BASICSTRING & String)
	{
		return _tcscmp( c_str(), String.c_str() ) <= 0;
	}

	inline bool CBasicString::operator<=(      BASICCHAR     Char  )
	{
		return size() == 1 && c_str()[0] <= Char;
	}

	inline bool CBasicString::operator>(const CBasicString& String)
	{
		return _tcscmp(c_str(), String.c_str()) > 0;
	}

	inline bool CBasicString::operator>(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) > 0;
	}

	inline bool CBasicString::operator>(const BASICSTRING & String)
	{
		return _tcscmp( c_str(), String.c_str() ) > 0;
	}

	inline bool CBasicString::operator>(      BASICCHAR     Char  )
	{
		return size() > 1 || (size() == 1 && c_str()[0] > Char);
	}

	inline bool CBasicString::operator>=(const CBasicString& String)
	{
		return _tcscmp(c_str(), String.c_str()) >= 0;
	}

	inline bool CBasicString::operator>=(const PBASICCHAR    pChar )
	{
		return _tcscmp(c_str(), pChar) >= 0;
	}

	inline bool CBasicString::operator>=(const BASICSTRING & String)
	{
		return _tcscmp( c_str(), String.c_str() ) >= 0;
	}

	inline bool CBasicString::operator>=(      BASICCHAR     Char  )
	{
		return size() > 1 || (size() == 1 && c_str()[0] >= Char);
	}

	inline CBasicString::operator const PBASICCHAR()const
	{
		return (PBASICCHAR)c_str();
	}
}

