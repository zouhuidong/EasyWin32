/**
 * @file	HiString.h
 * @brief	HiEasyX 字符串包装
 * @author	Margoo
 * @modify	huidong
*/

#pragma once

#include <string>
#include <tchar.h>
#include <windows.h>

namespace HiEasyX
{
#ifdef UNICODE

	///< HiEasyX 内部使用字符串类型，用于适配不同的字符集设置
	///< 由于使用了 std，因此该类型不会暴露在二进制接口中
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