#include "fi/app/fileIO.h"

#include <cstdio>
#include <filesystem>

namespace {
	FILE *openFile(const char *path, const char *mode)
	{
#if defined(_WIN32)
		FILE *file = nullptr;
		return fopen_s(&file, path, mode) == 0 ? file : nullptr;
#else
		return fopen(path, mode);
#endif
	}
}

namespace FI {
	std::vector<std::string> filesInDirectory( std::string dirPath, std::string ext )
	{
		std::vector<std::string> list;
		std::error_code ec;
		for (const auto &entry : std::filesystem::directory_iterator(
				dirPath, std::filesystem::directory_options::skip_permission_denied, ec))
		{
			if (!entry.is_regular_file(ec)) continue;
			const std::string name = entry.path().filename().string();
			if (!name.empty() && name.front() == '.') continue;
			if (ext==".*" || ext=="*" || ext=="*.*" || name.find(ext) != std::string::npos)
				list.push_back(name);
		}
		return list;
	}

	size_t writeFile(std::string path, const std::vector<uint8_t> &buffer)
	{
		FILE *f = openFile(path.c_str(), "wb");
		if(!f) return 0;

		fwrite(buffer.data(), buffer.size(), 1, f);
		fclose(f);
		
		return buffer.size();
	}

	size_t readFile(std::string path, std::vector<uint8_t> &buffer)
	{
		FILE *f = openFile(path.c_str(), "rb");
		if(!f) return 0;

		fseek(f, 0, SEEK_END);
		long size = ftell(f);
		fseek(f, 0, SEEK_SET);
		
		if(!size)
		{
			fclose(f);
			return 0;
		}

		buffer.resize(size);
		fread(buffer.data(), size, 1, f);
		fclose(f);

		return size;
	}
	
} // NS FI
