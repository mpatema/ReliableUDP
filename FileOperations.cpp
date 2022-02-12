#include <string>

using namespace std;
///
/// This file will handle file operations such as checking for compatibility of files, 
/// file I/O, saving, etc.
/// 
/// 



int OpenFile (const char* fileName[]) {
	FILE* newFile;
	fopen_s(&newFile, *fileName, "r");
	if (newFile != NULL) {
		
		return 0;
	}
	else {
		//return error code 
		return 1;
	}
}

int FileExtensionVal () {
	return 0;
}