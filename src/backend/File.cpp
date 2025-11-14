#include "File.h"
#include <vector>
#include <string>

SDL_EnumerationResult FS::EnumerateDirectory(void* userData, const char* dirName, const char* fname)
{
	SDL_Log("dirname: %s | fname: %s", dirName, fname);
	return SDL_ENUM_CONTINUE;
}

SDL_EnumerationResult FS::GetFiles(void* userData, const char* dirName, const char* fname)
{
	SDL_Log("adding dirname: %s | fname: %s", dirName, fname);

	std::vector<std::string>& vec = *reinterpret_cast<std::vector<std::string>*>(userData);
	vec.emplace_back(fname);

	return SDL_ENUM_CONTINUE;
}

void FS::EnumerateDirectory(const char* dir)
{
	if (!SDL_EnumerateDirectory(dir, EnumerateDirectory, NULL))
	{
		SDL_Log("Some issue with the callback.");
	}
	else
	{
		SDL_Log("All files enumerated.");
	}
}

void FS::GetFiles(const char* dir, void* data)
{
	if (!SDL_EnumerateDirectory(dir, GetFiles, data))
	{
		SDL_Log("Some issue with the callback.");
	}
	else
	{
		SDL_Log("All files enumerated.");
	}
}
