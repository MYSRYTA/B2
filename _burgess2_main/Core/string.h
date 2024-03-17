//******************************************************************************
//! @file	string.h
//! @brief	 文字列ヘルパー関数
//! @date 2019/3/8
//******************************************************************************
/**
*	@defgroup Base
*	@{
*/
#pragma once
#include <string>
#include <vector>

	/**
	*	@defgroup Data Type Alias
	*	@{
	*/
using String = std::string;
	/** @} */

	/**
	*	@defgroup String
	*	@{
	*/

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		文字列を指定文字で分割する
//------------------------------------------------------------------------------------------------------------------------------------------------
inline std::vector<String> StringSplit(const String& s, char delim) {
	std::vector<String> elems;
	String item;
	for (char ch : s) {
		if (ch == delim) {
			if (!item.empty())
				elems.push_back(item);
			item.clear();
		}
		else {
			item += ch;
		}
	}
	if (!item.empty()) {
		elems.push_back(item);
	}
	return elems;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		文字列を指定文字で分割する
//------------------------------------------------------------------------------------------------------------------------------------------------
inline std::vector<String> StringSplit(const String& str, const String& separator) {
	std::vector<String> result;

	String tstr = str + separator;
	long l = tstr.length(), sl = separator.length();
	String::size_type pos = 0, prev = 0;
	for (; pos < l && (pos = tstr.find(separator, pos)) != String::npos; prev = (pos += sl)) {
		String item = tstr.substr(prev, pos - prev);
		result.push_back(item);
	}
	return result;
}

//------------------------------------------------------------------------------------------------------------------------------------------------
//!		特定の文字列が含まれている場合はtrueを返す
//------------------------------------------------------------------------------------------------------------------------------------------------
inline bool StringFind(const String& fStr, const String& sStr) {
	for (int i = 0; i < fStr.length(); i++) {
		if (fStr.substr(i, sStr.length()) == sStr) {
			return true;
		}
	}
	return false;
}

template <typename T>
String GetFloorString ( T val );

#include "string.inl"

	/** @} */
/** @} */
//******************************************************************************
//! End of File
//******************************************************************************