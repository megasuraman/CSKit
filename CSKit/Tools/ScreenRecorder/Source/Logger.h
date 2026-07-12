#pragma once

#include "pch.h"

#include <fstream>

// ログ出力をコンソールだけでなくファイルにも出力するクラス
class Logger
{
public:
	static void Init(const std::string& filename)
	{
		m_file.open(filename, std::ios::out);
	}

	template<typename T>
	Logger& operator<<(const T& msg)
	{
		if constexpr (std::is_convertible_v<T, std::wstring>)
		{
			return (*this) << std::wstring(msg);
		}
		else
		{
			std::cout << msg;
			if (m_file.is_open())
			{
				m_file << msg;
				m_file.flush();
			}
			return *this;
		}
	}

	// std::endl などのマニピュレータ用
	Logger& operator<<(std::ostream& (*manip)(std::ostream&))
	{
		manip(std::cout);
		if (m_file.is_open())
		{
			manip(m_file);
			m_file.flush();
		}
		return *this;
	}

	// ワイド文字列用
	Logger& operator<<(const std::wstring& msg)
	{
		std::wcout << msg;
		if (m_file.is_open())
		{
			std::string narrow = WideToUtf8(msg);
			m_file << narrow;
			m_file.flush();
		}
		return *this;
	}

	static Logger& Instance()
	{
		static Logger instance;
		return instance;
	}

private:
	Logger() = default;
	static std::ofstream m_file;

	static std::string WideToUtf8(const std::wstring& wstr)
	{
		if (wstr.empty()) return std::string();
		int size_needed = WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), NULL, 0, NULL, NULL);
		std::string strTo(size_needed, 0);
		WideCharToMultiByte(CP_UTF8, 0, &wstr[0], (int)wstr.size(), &strTo[0], size_needed, NULL, NULL);
		return strTo;
	}
};