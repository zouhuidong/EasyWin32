/**
 * @file	HiString.h
 * @brief	HiEasyX ×Ö·û´®°ü×°
 * @author	Margoo
 * @modify	huidong
*/

#pragma once

#include <string>
#include <windows.h>

namespace HiEasyX
{
#ifdef UNICODE

	using HXString = std::wstring;
	using HXChar = wchar_t;
#define HXStr(Text) L##Text
#define t_vsnprintf _vsnwprintf
#define t_vsnprintf_s _vsnwprintf_s

	template<class Type>
	HXString ToHXString(Type value) {
		return std::to_wstring(value);
	}

#else

	using HXString = std::string;
	using HXChar = char;
#define HXStr(Text) (##Text)
#define t_vsnprintf _vsnprintf
#define t_vsnprintf_s _vsnprintf_s

	template<class Type>
	HXString ToHXString(Type value) {
		return std::to_string(value);
	}

#endif
}