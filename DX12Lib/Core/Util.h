#pragma once

#include <cstdio>

namespace Util
{
	inline void Log(const char* msg)
	{
		printf("%s\n", msg);
	}

	inline void Log(const wchar_t* msg)
	{
		wprintf(L"%ws\n", msg);
	}

	inline void Logf(const char* format, ...)
	{
		char buffer[256];
		va_list ap;
		va_start(ap, format);
		vsprintf_s(buffer, 256, format, ap);
		va_end(ap);
		Log(buffer);
	}

	inline void Logf(const wchar_t* format, ...)
	{
		wchar_t buffer[256];
		va_list ap;
		va_start(ap, format);
		vswprintf(buffer, 256, format, ap);
		va_end(ap);
		Log(buffer);
	}
}