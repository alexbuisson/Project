// ProcessMemDumper.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <windows.h>

#include <string>
#include <iostream>
#include <fstream>
#include <vector>
#include <stdexcept>
#include <sstream>

//#include "VersionInfoLib/VersionInfo.h"

#include "boost/shared_ptr.hpp"

//http://www.codeproject.com/Articles/177344/VSDebugHelper-Capture-memory-snapshot-of-a-debugge
//http://stackoverflow.com/questions/4155624/save-data-from-visual-studio-memory-window
//http://msdn.microsoft.com/en-us/library/windows/desktop/ms680553%28v=vs.85%29.aspx

//HANDLE WINAPI OpenProcess(
//	_In_  DWORD dwDesiredAccess,
//	_In_  BOOL bInheritHandle,
//	_In_  DWORD dwProcessId
//	);
//BOOL WINAPI ReadProcessMemory(
//	_In_   HANDLE hProcess,
//	_In_   LPCVOID lpBaseAddress,
//	_Out_  LPVOID lpBuffer,
//	_In_   SIZE_T nSize,
//	_Out_  SIZE_T *lpNumberOfBytesRead
//	);

void delete_HANDLE(HANDLE p) 
{ 
	if(p != NULL)   
	{
		CloseHandle(p);  
	}
};


int _tmain(int argc, char* argv[])
{
	try
	{
		std::cout << "Process Memory Dumper " << std::endl;
		//std::cout << "Build Date " << GetBuildDate() << " - Version " << GetBuildInfo() << std::endl;
		if(argc < 5)
		{
			std::cout << "Invalid number of argument." << std::endl;
			std::cout << "Usage, require 4 parameters. " << std::endl;
			std::cout << "- Process PID" << std::endl;
			std::cout << "- Memory address in hexadecimal with 0x prefix" << std::endl;
			std::cout << "- Number of bytes to read in memory and dump in the output file" << std::endl;
			std::cout << "- Output file full path " << std::endl;
			std::cout << "=> Example: ProcessMemDumper 2812 0x00d68618 16384 dump.bin" << std::endl;
			return -1;
		}

		std::cout << "PID " << argv[1] << " Address " << argv[2] << " nb bytes " << argv[3] << " file " << argv[4] << std::endl;

		unsigned int procPID = atoi(argv[1]);
		std::string procAddressHexa = std::string(argv[2]);
		std::stringstream hexToInt;
		hexToInt << std::hex << procAddressHexa;
		unsigned int procAddress = 0;
		hexToInt >> procAddress;
		unsigned int nbbytetoRead = atoi(argv[3]);
		std::string targetFileName = std::string(argv[4]);

		boost::shared_ptr<void> procHandle(OpenProcess(PROCESS_VM_READ, false, DWORD(procPID)), &::delete_HANDLE);
		if(!procHandle)
		{
			std::cout << "Unable to open process " << procPID << std::endl;
			return -1;
		}
				
		std::vector<char> memCopyofProcMem(nbbytetoRead);

		SIZE_T NumberOfBytesRead = 0;
		if(ReadProcessMemory(procHandle.get(), LPCVOID(procAddress), LPVOID(&memCopyofProcMem[0]), SIZE_T(nbbytetoRead), &NumberOfBytesRead) && NumberOfBytesRead == nbbytetoRead)
		{
			std::ofstream targetFile(targetFileName.c_str(), std::ios::binary|std::ios::out);
			targetFile.write(&memCopyofProcMem[0], nbbytetoRead);
			targetFile.close();
		}
		else
		{
			std::cout << "ReadProcessMemory return false, or NumberOfBytesRead != of the requested number of byte." << std::endl;
			return -1;
		}
	}
	catch (const std::exception& e)
	{
		std::cout << "std::exception: " << e.what() << std::endl;
		return -1;
	}
	catch (...)
	{
		std::cout << "unknown exception: " << std::endl;
		return -1;
	}
	std::cout << "Dump done."  << std::endl;
	return 0;
}

