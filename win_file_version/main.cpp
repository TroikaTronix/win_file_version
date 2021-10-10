//
//  main.cpp
//  win_file_version
//
//  Created by Mark Coniglio on 10/9/21.
//  Copyright © 2021 Mark Coniglio. All rights reserved.
//

#include <iostream>
#include <fstream>
#include <vector>
#include <string>

static std::string sFileVersion = "FileVersion";
static std::string sProductVersion = "ProductVersion";

enum GetOption {
	kFileVersionNumeric,
	kProductVersionNumeric,
	kFileVersionString,
	kProductVersionString
};

bool
FindOffsetToString(
	std::string					inASCIISearchString,
	const std::vector<char>&	inBuffer,
	size_t						inBufferOffset,
	size_t&						outOffset)
{
	bool success = false;
	
	// create a pseudo unicode version of the string, with the
	// ASCII character first and a zero after it; this will be
	// the data we search for to find the app or exe's version
		
	std::vector<char> uniSearchString;
	for (std::string::const_iterator i=inASCIISearchString.begin(); i<inASCIISearchString.end(); i++) {
		uniSearchString.push_back(*i);
		uniSearchString.push_back(0);
	}
	
	// starting at the offset inBufferOffset, iterate through the buffer
	// to find a match for the string provided by the caller; if we find
	// it set outOffset to give the offset to that string and return true
	if (inBuffer.size() > inBufferOffset) {
		if ((inBuffer.size() - inBufferOffset) >= uniSearchString.size()) {
			size_t max = inBuffer.size() - uniSearchString.size();
			for (size_t i=inBufferOffset && !success; i<max; i++) {
				if (memcmp(&inBuffer[i], &uniSearchString[0], uniSearchString.size()) == 0) {
					outOffset = i;
					success = true;
				}
			}
		}
	}
	
	return success;
}

int main(int argc, const char * argv[])
{
	// HELP TEXT
	if (argc == 1) {
		printf("win_file_version: outputs the file version or product version stored int the VERSIONINFO resource of a windows DLL or EXE file.\n");
		printf("usage: win_file_version [-fv/-pv/-fvs/-pvs] exe_or_dll_path\n");
		printf("  -fv will output the numeric FILEVERSION\n");
		printf("  -pv will output the numeric PRODUCTVERSION\n");
		printf("  -sfv will output the FILEVERSION string from the STRING FILE INFO section\n");
		printf("  -spv will output the PRODUCTVERSION string from the STRING FILE INFO section\n");
		printf("  [defaults to -fv if no option is specified]\n");
		printf("  exe_or_dll_path is the path to a valid windows DLL or EXE file\n");
		return 0;
	}
	
	// path to the file provided by user
	std::string file_path;
	
	std::string searchString = sFileVersion;
	
	
	GetOption getOption = kFileVersionNumeric;
	
	// if user has specified only the file path, store it in file_path
	if (argc == 2) {
	
		file_path = argv[1];
	
	// if user has specified an option and the file path...
	} else if (argc == 3) {

		// get the option in a string
		std::string option = argv[1];
		// if it is fvs, then set that option
		if (option == "-fv") {
			getOption = kFileVersionNumeric;
		// if it is fvs, then set that option
		} else if (option == "-pv") {
			getOption = kProductVersionNumeric;
		// if it is fvs, then set that option
		} else if (option == "-sfv") {
			getOption = kFileVersionString;
			searchString = sFileVersion;
		// if it is pvs then set that option
		} else if (option == "-spv") {
			getOption = kProductVersionString;
			searchString = sProductVersion;
		// otherwise show an error
		} else {
			fprintf(stderr, "%s", "Illegal option %s. It must be either -fv, -pv, -sfv, -spv\n");
			return 1;
		}

		file_path = argv[2];

	} else {
	
		fprintf(stderr, "%s", "Too many parameters. Usage: in_file_version [-fv/-pv] exe_or_dll_path.\n");
		return 1;
	}
	
	std::ifstream file(file_path.c_str(), std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	if (size > 0) {
	
		file.seekg(0, std::ios::beg);

		std::string outVersion;
		std::vector<char> buffer(size);
		if (file.read(buffer.data(), size)) {
		
			size_t vi_offset = 0;
			std::string vsVersInfo = "VS_VERSION_INFO";
			if (FindOffsetToString(vsVersInfo, buffer, 0, vi_offset)) {
				
				const uint16_t* numericFileVersion = (const uint16_t*)(&buffer[vi_offset + vsVersInfo.length() * 2] + 12);
				
				char buf[256];
				
				// get numeric file version
				uint16_t fv[4];
				for (size_t i=0; i<4; i++) {
					fv[i] = *(numericFileVersion++);
				}
				snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (int) fv[1], (int) fv[0], (int) fv[3], (int) fv[2]);
				std::string fvStr = buf;
				
				// get numeric product version
				uint16_t pv[4];
				for (size_t i=0; i<4; i++) {
					pv[i] = *(numericFileVersion++);
				}
				snprintf(buf, sizeof(buf), "%d.%d.%d.%d", (int) pv[1], (int) pv[0], (int) pv[3], (int) pv[2]);
				std::string pvStr = buf;
				
				if (getOption == kFileVersionNumeric) {
				
					outVersion = fvStr;
					
				} else if (getOption == kProductVersionNumeric) {
				
					outVersion = pvStr;
					
				} else {
				
					size_t vers_offset = 0;
					if (FindOffsetToString(searchString, buffer, vi_offset, vers_offset)) {
						uint16_t* data = (uint16_t*) &buffer[vers_offset + searchString.length() * 2];
						// skip the null terminator
						data++;
						// if there is an extra null terminator, skip that too
						if (*data == 0)
							data++;
						// now read the unicode string that is the versiopn number until
						// we find a null terminator for that string
						while (*data != 0) {
							char c = *data & 0xFF;
							outVersion.push_back(c);
							data++;
						}
					}
				
				}
			}
		}
		
		printf("%s\n", outVersion.c_str());
		return 0;
		
	} else {
		fprintf(stderr, "error: file not found!\n");
		return 1;
	}
}
