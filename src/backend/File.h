#pragma once
#include <SDL3/SDL.h>
#include <SDL3/SDL_filesystem.h>

#include <Windows.h>
#include <string>

namespace FS
{
	SDL_EnumerationResult EnumerateDirectory(void* userData, const char* dirName, const char* fname);
	SDL_EnumerationResult GetFiles(void* userData, const char* dirName, const char* fname);
	void EnumerateDirectory(const char* dir);
	void GetFiles(const char* dir, void* data);
	std::wstring FullPathW(LPCWSTR directory, LPCWSTR name);
}