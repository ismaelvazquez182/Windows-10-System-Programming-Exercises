/*
*	Windows 10 System Programming Part 1 by Pavel Yosifovich
* 
*	Exercise 1
*	Write a console application that prints more information about the system than the HelloWin
*	application shown earlier, by calling the following APIs: GetNativeSystemInfo, GetComputerName,
*	GetWindowsDirectory, QueryPerformanceCounter, GetProductInfo, GetComputerObjectName.
*	Handle errors if they occur.
*/

#define SECURITY_WIN32
#include <iostream>
#include <format>

#include <Windows.h>
#include <security.h>

void PrintSystemDirectory();
void PrintVersionNumber();
void PrintMoreInformation();

int main()
{
	SYSTEM_INFO si{ 0 };
	::GetNativeSystemInfo(&si);

	std::wcout << "Number of logical processors: " << si.dwNumberOfProcessors << std::endl;
	std::wcout << "Page size: " << si.dwPageSize << std::endl;
	std::wcout << "Processor mask: 0x" << std::hex << si.dwActiveProcessorMask << std::endl;
	std::wcout << "Minumum process address: 0x" << si.lpMinimumApplicationAddress << std::endl;
	std::wcout << "Maximum process address: 0x" << si.lpMaximumApplicationAddress << std::endl;
	std::wcout << std::endl;

	PrintSystemDirectory();
	PrintVersionNumber();

	std::wcout << std::endl;

	PrintMoreInformation();

	return 0;
}

void PrintSystemDirectory()
{
	WCHAR path[MAX_PATH]; // path is a pointer to a buffer
	if (!::GetSystemDirectory(path, MAX_PATH)) {
		std::wcout << "Call to GetSystemDirectory failed, see MSDN error: " << ::GetLastError() << std::endl;
		return;
	}

	std::wcout << "System directory: " << path << std::endl;
}

void PrintVersionNumber()
{
	auto sharedUserData = (BYTE*)0x7FFE0000;
	std::wcout << std::dec 
		<< "Version: " << *(ULONG*)(sharedUserData + 0x26C)
		<< "." << *(ULONG*)(sharedUserData + 0x270)
		<< "." << *(ULONG*)(sharedUserData + 0x260) << std::endl;
}

void PrintComputerName()
{
	// Computer Name
	WCHAR compName[MAX_COMPUTERNAME_LENGTH + 1]{ 0 };
	DWORD compNameBuffer{ MAX_COMPUTERNAME_LENGTH + 1 };
	if (::GetComputerName(compName, &compNameBuffer) == 0)
		std::wcout << "Call to GetComputerName failed, see MSDN error: " << ::GetLastError() << std::endl;
	else
		std::wcout << "Computer Name: " << compName << std::endl;
}

void PrintWindowsDirectory()
{
	// Windows Directory
	WCHAR windowsPath[MAX_PATH]{ 0 };
	if (::GetWindowsDirectory(windowsPath, MAX_PATH) == 0)
		std::wcout << "Call to GetWindowsDirectory failed, see MSDN error: " << ::GetLastError() << std::endl;
	else
		std::wcout << "Windows directory: " << windowsPath << std::endl;
}

void PrintPerformanceCounterInformation()
{
	// Performance Counter
	LARGE_INTEGER performanceCount{ 0 };
	if (::QueryPerformanceCounter(&performanceCount) == 0)
		std::wcout << "Call to QueryPerformanceCounter failed, see MSDN error: " << ::GetLastError() << std::endl;
	else {
		std::wcout << "Performance counter low part: " << performanceCount.LowPart << std::endl;
		std::wcout << "Performance counter high part: " << performanceCount.HighPart << std::endl;
		std::wcout << "Performance counter quad part: " << performanceCount.QuadPart << std::endl;
	}

}

void PrintProductInfo()
{
	// Product info
	OSVERSIONINFOEXW osInfo{ 0 };
	osInfo.dwOSVersionInfoSize = sizeof(OSVERSIONINFOEXW);
#pragma warning(suppress : 4996)
	if (::GetVersionExW((LPOSVERSIONINFOW)&osInfo) == 0) {
		std::wcout << "Shockingly, the call to getVersionExW failed, see MSDN error: " << ::GetLastError() << std::endl;
	}
	else {
		DWORD productType{ 0 };
		if (::GetProductInfo(osInfo.dwMajorVersion, 
			osInfo.dwMinorVersion, osInfo.wServicePackMajor, 
			osInfo.wServicePackMinor, &productType) == 0) {
			std::wcout << "Call to GetProductInfo failed. Invalid parameters received." << std::endl;
		}
		else {
			std::wcout << "Product type: 0x" << std::hex << productType << std::endl;
		}
	}
}

void PrintComputerObjectName()
{
	// requires computer to be part of an active directory environment
#pragma comment(lib, "Secur32.lib")
	WCHAR name[MAX_COMPUTERNAME_LENGTH + 1]{ 0 };
	unsigned long int nameBufSize = MAX_COMPUTERNAME_LENGTH + 1;
	if (::GetComputerObjectName(EXTENDED_NAME_FORMAT::NameUniqueId, name, &nameBufSize) == 0)
		std::wcout << "Call to GetComputerObjectName failed, see MSDN error: " << std::dec << ::GetLastError() << std::endl;
	else
		std::wcout << "Computer object name: " << name << std::endl;
}

void PrintMoreInformation()
{
	PrintComputerName();
	PrintWindowsDirectory();
	PrintPerformanceCounterInformation();
	PrintProductInfo();
	PrintComputerObjectName();
}
