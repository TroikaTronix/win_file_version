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

int main(int argc, const char * argv[])
{
	if (argc < 2) {
		fprintf(stderr, "error: you must provide a path to the file\n");
	}
	std::ifstream file(argv[1], std::ios::binary | std::ios::ate);
	std::streamsize size = file.tellg();
	if (size > 0) {
		file.seekg(0, std::ios::beg);

		std::vector<char> fileVersion;
		for (std::string::const_iterator i=sFileVersion.begin(); i<sFileVersion.end(); i++) {
			fileVersion.push_back(*i);
			fileVersion.push_back(0);
		}
		
		std::string vers;
		std::vector<char> buffer(size);
		if (file.read(buffer.data(), size)) {
			if (buffer.size() >= fileVersion.size()) {
				size_t max = buffer.size() - fileVersion.size();
				for (size_t i=0; i<max; i++) {
					if (memcmp(&buffer[i], &fileVersion[0], fileVersion.size()) == 0) {
						i += fileVersion.size() + 4;
						int j = 0;
						for (; j<5; j++) {
							vers.push_back(buffer[i]);
							i += 2;
						}
						i += 2;
						vers.push_back('.');
						vers.push_back(buffer[i]);
					}
				}
			}
		}
		printf("%s\n", vers.c_str());
		return 0;
	} else {
		fprintf(stderr, "error: file not found!\n");
		return 1;
	}
}
