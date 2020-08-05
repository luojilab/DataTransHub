
#ifndef __FILEINFO_H__
#define __FILEINFO_H__
#include <string>

namespace future{

	struct FileInfo {
		bool Exists;
		bool IsDirectory;
		std::size_t Size;
		std::size_t MTime;

		FileInfo();
	};

	inline FileInfo::FileInfo() : Exists(false), IsDirectory(false), Size(0), MTime(0) {
	}
}

#endif
