#pragma once

#include "resource.h"
#include <map>
#include <vector>
#include <string>
#include <regex>
using namespace std;

//���ַ���ת��ͨ�ַ���
char *wtoc(wchar_t *wText)
{
	DWORD dwNum = WideCharToMultiByte(CP_ACP, NULL, wText, -1, NULL, 0, NULL, FALSE);
	char *psText = NULL;
	psText = new char[dwNum];
	if (!psText)
	{
		delete[]psText;
		psText = NULL;
	}
	WideCharToMultiByte(CP_ACP, NULL, wText, -1, psText, dwNum, NULL, FALSE);
	return psText;
}

//��ͨ�ַ���ת���ַ���
wchar_t *ctow(char *sText)
{
	DWORD dwNum = MultiByteToWideChar(CP_ACP, 0, sText, -1, NULL, 0);

	wchar_t *pwText = NULL;
	pwText = new wchar_t[dwNum];
	if (!pwText)
	{
		delete[]pwText;
		pwText = NULL;
	}
	unsigned nLen = MultiByteToWideChar(CP_ACP, 0, sText, -1, pwText, dwNum + 10);
	if (nLen >= 0)
	{
		pwText[nLen] = 0;
	}
	return pwText;
}


// std::wstring�汾
std::vector<std::wstring> ws_split(const std::wstring& in, const std::wstring& delim) {
	if (in.length() < delim.length()) {
		return std::vector<std::wstring>();
	}
	std::wregex re{ delim };
	return std::vector<std::wstring> {
		std::wsregex_token_iterator(in.begin(), in.end(), re, -1),
			std::wsregex_token_iterator()
	};
}
// c string�汾
std::vector<std::string> c_split(const char* in, const char* delim) {
	std::regex re{ delim };
	return std::vector<std::string> {
		std::cregex_token_iterator(in, in + strlen(in), re, -1),
			std::cregex_token_iterator()
	};
}
// ֧��wchar_t���ַ����İ汾
std::vector<std::wstring> wc_split(const wchar_t* in, const wchar_t* delim) {
	std::wregex re{ delim };
	return std::vector<std::wstring> {
		std::wcregex_token_iterator(in, in + wcslen(in), re, -1),
			std::wcregex_token_iterator()
	};
}
// �����s_split��ws_split����ͳһ��ģ����ʵ��
template<typename E,
	typename TR = std::char_traits<E>,
	typename AL = std::allocator<E>,
	typename _str_type = std::basic_string<E, TR, AL>>
	std::vector<_str_type> bs_split(const std::basic_string<E, TR, AL>& in, const std::basic_string<E, TR, AL>& delim) {
	std::basic_regex<E> re{ delim };
	return std::vector<_str_type> {
		std::regex_token_iterator<typename _str_type::const_iterator>(in.begin(), in.end(), re, -1),
			std::regex_token_iterator<typename _str_type::const_iterator>()
	};
}