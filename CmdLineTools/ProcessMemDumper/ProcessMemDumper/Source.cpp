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

#include "boost/shared_ptr.hpp"

std::vector<double> vec(10);
std::ofstream output("out.bin", std::ios::binary|std::ios::out);
output.write(reinterpret_cast<char*>(&vec[0]), vec.size()*sizeof(vec[0]));
output.close();

void delete_HANDLE(HANDLE p) 
{ 
  std::cout << "Releasing Handle ...." << std::endl;
  CloseHandle(p);  
};


int _tmain(int argc, _TCHAR* argv[])
{
	try
	{
		unsigned int procPID = atoi(argv[1]);
		std::string procAddressHexa = std::string(argv[2]);
		std::stringstream hexToInt;
		hexToInt << std::hex << procAddressHexa;
		unsigned int procAddress = 0;
		hexToInt >> procAddress;
		unsigned int nbbytetoRead = atoi(argv[3]);
		std::string targetFileName = std::string(argv[4]);

		boost::shared_ptr<void> procHandle(OpenProcess(PROCESS_VM_READ, false, DWORD(procPID)), &::delete_HANDLE);
		if(procHandle)
		{
			std::vector<char> memCopyofProcMem(nbbytetoRead);

			SIZE_T NumberOfBytesRead = 0;
			if(ReadProcessMemory(procHandle.get(), LPCVOID(procAddress), LPVOID(&memCopyofProcMem[0]), SIZE_T(nbbytetoRead), &NumberOfBytesRead) && NumberOfBytesRead == nbbytetoRead)
			{
				std::ofstream targetFile(targetFileName, std::ios::binary|std::ios::out);
				targetFile.write(&memCopyofProcMem[0], nbbytetoRead);
				targetFile.close();
			}
			else
			{
				std::cout << "ReadProcessMemory return false, or NumberOfBytesRead != of the requested number of byte." << std::endl;
				return -1;
			}
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
	return 0;
}

