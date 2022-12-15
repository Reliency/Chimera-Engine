#include <Shlwapi.h>
#include <Tlhelp32.h>
#include <mmsystem.h>
#include <windows.h>
#include "ASIUtilities.hpp"

static HMODULE ourModule;

namespace ASIUtility
{
	/* String */
	std::string GetModuleFullName(const HMODULE module) {

		char fileName[MAX_PATH];
		GetModuleFileNameA(module, fileName, MAX_PATH);
		return fileName;
	}
	std::string GetFilename(std::string filename)
	{
		size_t lastIndex = filename.find_last_of("\\") + 1;
		return filename.substr(lastIndex, filename.length() - lastIndex);
	}
	std::wstring str_to_wstr(const std::string& string)
	{
		std::wstring wstring;

		return MultiByteToWideChar(CP_UTF8, 0, string.data(), static_cast<int>(string.size()), &wstring[0], (int)wstring.size()) ? wstring : std::wstring();
	}

	std::string wstr_to_str(const std::wstring& wstring)
	{
		std::string string;

		return WideCharToMultiByte(CP_UTF8, 0, wstring.data(), static_cast<int>(wstring.size()), &string[0], static_cast<int>(string.size()), nullptr, nullptr) ? string : std::string();
	}


	/* Module / Process Related */

	void SetOurModuleHandle(const HMODULE module) {

		ourModule = module;
	}

	const HMODULE GetOurModuleHandle() {

		return ourModule;
	}

	const std::string GetModuleName(const HMODULE module) {

		char fileName[MAX_PATH];
		GetModuleFileNameA(module, fileName, MAX_PATH);

		std::string fullPath = fileName;

		size_t lastIndex = fullPath.find_last_of("\\") + 1;
		return fullPath.substr(lastIndex, fullPath.length() - lastIndex);
	}

	const std::string GetModuleNameWithoutExtension(const HMODULE module) {

		const std::string fileNameWithExtension = GetModuleName(module);

		size_t lastIndex = fileNameWithExtension.find_last_of(".");
		if (lastIndex == -1) {
			return fileNameWithExtension;
		}

		return fileNameWithExtension.substr(0, lastIndex);
	}

	bool GetProcess(const std::string& filename, HANDLE& handle)
	{
		PROCESSENTRY32 processInfo;
		processInfo.dwSize = sizeof(processInfo);

		HANDLE processesSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (processesSnapshot == INVALID_HANDLE_VALUE) return false;

		Process32First(processesSnapshot, &processInfo);
		if (!filename.compare((char*)processInfo.szExeFile))
		{
			SetPrivilege("SeDebugPrivilege", true);
			handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInfo.th32ProcessID);
			CloseHandle(processesSnapshot);
			return true;
		}

		while (Process32Next(processesSnapshot, &processInfo))
		{
			if (!filename.compare((char*)processInfo.szExeFile))
			{
				SetPrivilege("SeDebugPrivilege", true);
				handle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processInfo.th32ProcessID);
				CloseHandle(processesSnapshot);
				return true;
			}
		}

		CloseHandle(processesSnapshot);
		return false;
	}

	DWORD GetProcessID(const std::string& processName)
	{
		DWORD processID = 0;
		HANDLE handle;
		if (GetProcess(processName, handle))
		{
			processID = GetProcessId(handle);
			CloseHandle(handle);
		}

		return processID;
	}

	void StartProcess(LPCTSTR lpApplicationName)
	{
		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		CreateProcess(lpApplicationName,   // the path
			NULL,        // Command line
			NULL,           // Process handle not inheritable
			NULL,           // Thread handle not inheritable
			FALSE,          // Set handle inheritance to FALSE
			0,              // No creation flags
			NULL,           // Use parent's environment block
			NULL,           // Use parent's starting directory 
			&si,            // Pointer to STARTUPINFO structure
			&pi);           // Pointer to PROCESS_INFORMATION structure
		// Close process and thread handles. 
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);
	}

	HANDLE StartProcessPaused(LPCTSTR lpApplicationName, PHANDLE ptr_thread)
	{
		if (ptr_thread == nullptr) return nullptr;

		// additional information
		STARTUPINFO si;
		PROCESS_INFORMATION pi;

		// set the size of the structures
		ZeroMemory(&si, sizeof(si));
		si.cb = sizeof(si);
		ZeroMemory(&pi, sizeof(pi));

		// start the program up
		CreateProcess(
			lpApplicationName,  // the path
			NULL,				// Command line
			NULL,				// Process handle not inheritable
			NULL,				// Thread handle not inheritable
			FALSE,				// Set handle inheritance to FALSE
			CREATE_SUSPENDED,	// Creation flags
			NULL,				// Use parent's environment block
			NULL,				// Use parent's starting directory 
			&si,				// Pointer to STARTUPINFO structure
			&pi);				// Pointer to PROCESS_INFORMATION structure

		*ptr_thread = pi.hThread;
		return pi.hProcess;
	}

	bool CreateElevatedThread(LPTHREAD_START_ROUTINE thread)
	{
		return CloseHandle(CreateThread(NULL, NULL, thread, NULL, THREAD_PRIORITY_HIGHEST, NULL)) == TRUE;
	}

	void killProcessByName(const char* filename)
	{
		HANDLE hSnapShot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, NULL);
		PROCESSENTRY32 pEntry;
		pEntry.dwSize = sizeof(pEntry);
		BOOL hRes = Process32First(hSnapShot, &pEntry);
		while (hRes)
		{
			if (strcmp((char*)pEntry.szExeFile, filename) == 0)
			{
				HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, 0,
					(DWORD)pEntry.th32ProcessID);
				if (hProcess != NULL)
				{
					TerminateProcess(hProcess, 9);
					CloseHandle(hProcess);
				}
			}
			hRes = Process32Next(hSnapShot, &pEntry);
		}
		CloseHandle(hSnapShot);
	}

	bool SetPrivilege(const char* szPrivilege, bool bState)
	{
		HANDLE hToken = nullptr;
		if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY | TOKEN_ADJUST_PRIVILEGES, &hToken))
			return false;

		TOKEN_PRIVILEGES TokenPrivileges = { 0 };
		TokenPrivileges.PrivilegeCount = 1;
		TokenPrivileges.Privileges[0].Attributes = bState ? SE_PRIVILEGE_ENABLED : 0;

		if (!LookupPrivilegeValueA(nullptr, szPrivilege, &TokenPrivileges.Privileges[0].Luid))
		{
			CloseHandle(hToken);
			return false;
		}

		if (!AdjustTokenPrivileges(hToken, FALSE, &TokenPrivileges, sizeof(TOKEN_PRIVILEGES), nullptr, nullptr))
		{
			CloseHandle(hToken);
			return false;
		}

		CloseHandle(hToken);

		return true;
	}

	bool Is64BitProcess(HANDLE hProc)
	{
		bool Is64BitWin = false;
		BOOL Out = 0;
		IsWow64Process(GetCurrentProcess(), &Out);
		if (Out)
			Is64BitWin = true;

		if (!IsWow64Process(hProc, &Out))
			return false;

		if (Is64BitWin && !Out)
			return true;

		return false;
	}

}
