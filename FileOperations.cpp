///
/// This file will handle file operations such as checking for compatibility of files, 
/// file I/O, saving, etc.
/// 
/// 

#include <stdio.h>
#include <string.h>
#include <fstream>
using namespace std;


/*
* This function will handle opening binary and ASCII files. 
* Client program will call to this function when a transmission begins. 
* The file will open and be used as the file to transmit.
* 
* returns: file pointer to valid file after opening
*/
FILE* OpenNewFile(char* filename) {

	//check for compatible file formats, ASCII first.
	if (strstr(filename, ".txt") != NULL || strstr(filename, ".asc") != NULL) {
		FILE* newfp = fopen(filename, "r");
		if (!newfp) {				//check for errors in file open function, may need to tweak things. 
			return NULL;			//return NULL if file open fails.
		}
		return newfp;				//return the new file pointer if successful.
	}
	//check for binary files.
	//**TWEAK CONDITIONAL TO ENSURE PROPER BINARY CHECKING**
	else if (strstr(filename, ".bin") != NULL || strstr(filename, ".jpg")) {					
		ifstream(filename, );
	}
	else {

	}
	return NULL;
}