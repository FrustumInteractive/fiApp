#ifndef _FILE_IO_H
#define _FILE_IO_H

#include <string>
#include <vector>

namespace FI {
	std::vector<std::string> filesInDirectory( std::string dirPath, std::string ext );

	size_t writeFile(std::string path, const std::vector<uint8_t> &buffer);

	size_t readFile(std::string path, std::vector<uint8_t> &buffer);

} // NS FI

#endif /*_FILE_IO_H*/
