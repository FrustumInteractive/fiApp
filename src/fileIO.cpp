#include "fi/app/fileIO.h"

#include <dirent.h>
#include <string.h>
#include <stdlib.h>

namespace FI {

#ifdef _WIN32
	
	std::vector<std::string> filesInDirectory( std::string dirPath, std::string ext )
	{
		struct _finddata_t dirFile;
		long hFile;

		if (( hFile = _findfirst( dirPath.c_str(), &dirFile )) != -1 )
		{
			do
			{
				if ( !strcmp( dirFile.name, "."   )) continue;
				if ( !strcmp( dirFile.name, ".."  )) continue;
				if ( gIgnoreHidden )
				{
					if ( dirFile.attrib & _A_HIDDEN ) continue;
					if ( dirFile.name[0] == '.' ) continue;
				}

				if(ext==".*" || ext=="*" || ext=="*.*")
				{
					list.push_back(dirFile.name);
				}
				else
				{
					if ( strstr( dirFile.name, ext.c_str() ))
					{
						list.push_back(dirFile.name);
					}
				}

			} while ( _findnext( hFile, &dirFile ) == 0 );
			_findclose( hFile );
		}
	}
#else

	std::vector<std::string> filesInDirectory( std::string dirPath, std::string ext )
	{
		std::vector<std::string> list;
		bool bIgnoreHidden = true;
		DIR* dirFile = opendir( dirPath.c_str() );
		if ( dirFile )
		{
			struct dirent* hFile;
			while (( hFile = readdir( dirFile )) != NULL )
			{
				if ( !strcmp( hFile->d_name, "."  )) continue;
				if ( !strcmp( hFile->d_name, ".." )) continue;

				// in linux hidden files all start with '.'
				if ( bIgnoreHidden && ( hFile->d_name[0] == '.' )) continue;

				if(ext ==".*" || ext=="*" || ext=="*.*")
				{
					list.push_back(hFile->d_name);
				}
				else
				{
					if ( strstr( hFile->d_name, ext.c_str() ))
					{
						list.push_back(hFile->d_name);
					}
				}
			}
			closedir( dirFile );
		}

		return list;
	}

#endif // _WIN32

	size_t writeFile(std::string path, const std::vector<uint8_t> &buffer)
	{
		FILE *f = fopen(path.c_str(), "wb");
		if(!f) return 0;

		fwrite(buffer.data(), buffer.size(), 1, f);
		fclose(f);
		
		return buffer.size();
	}

	size_t readFile(std::string path, std::vector<uint8_t> &buffer)
	{
		FILE *f = fopen(path.c_str(), "rb");
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
