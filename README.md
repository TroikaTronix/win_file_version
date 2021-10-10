# win_file_verson
This command line utilty for macOS allows you to read version information from a Windows EXE or DLL file. The command format is as follows:

**win_file_version [-fv/-pv/-sfv/-spv] <path_to_exe_or_dll_file>**

Choose one of these four options to determine what version information is sent to the standard output:
- -fv will output the numeric FILEVERSION
- -pv will output the numeric PRODUCTVERSION
- -sfv will output the FILEVERSION string from the STRING FILE INFO section
- -spv will output the PRODUCTVERSION string from the STRING FILE INFO section
