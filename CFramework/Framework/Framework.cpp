#include "Framework.h"
#include <shlobj.h>

namespace utils
{
	bool IsKeyDown(DWORD VK)
	{
		return (GetAsyncKeyState(VK) & 0x8000) != 0;
	}

	LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam)
	{
		if (nCode >= 0 && wParam == WM_KEYDOWN) {
			KBDLLHOOKSTRUCT* pKey = (KBDLLHOOKSTRUCT*)lParam;
			char ch = '\0';
			if (pKey->vkCode == VK_BACK) {
				if (!g.newConfigName.empty() && g.bGenerateFlag) {
					g.newConfigName.pop_back();
				}
			}
			else if (pKey->vkCode >= 'A' && pKey->vkCode <= 'Z') {
				ch = (char)pKey->vkCode;
				if (!(GetAsyncKeyState(VK_SHIFT) & 0x8000)) {
					ch = tolower(ch);
				}
			}
			else if (pKey->vkCode >= '0' && pKey->vkCode <= '9') {
				ch = (char)pKey->vkCode;
			}
			else if (pKey->vkCode == VK_SPACE) {
				ch = ' ';
			}

			if (ch != '\0' && g.bGenerateFlag) {
				g.newConfigName += ch;
			}
		}

		return CallNextHookEx(NULL, nCode, wParam, lParam);
	}

	void KeyBinder(DWORD& target_key, int& BindID)
	{
		target_key = NULL;

		while (!target_key)
		{
			for (int i = 0; i < 0x87; i++)
			{
				if (i == VK_LWIN || i == VK_RWIN)
					continue;

				if (IsKeyDown(i))
				{
					target_key = i == VK_ESCAPE ? NULL : i;
					break;
				}
			}
		}

		BindID = NULL;
	}

	void EnableKeyboardHook() {
		if (!hKeyboardHook) {
			hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
		}
	}

	void DisableKeyboardHook() {
		if (hKeyboardHook) {
			UnhookWindowsHookEx(hKeyboardHook);
			hKeyboardHook = NULL;
		}
	}

	std::vector<std::string> GetPhysicalDriveList()
	{
		std::vector<std::string> driveList;
		DWORD driveMask = GetLogicalDrives();

		if (driveMask == 0)
			return driveList;

		for (char drive = 'A'; drive <= 'Z'; ++drive)
		{
			if (driveMask & (1 << (drive - 'A')))
			{
				std::string drivePath = std::string(1, drive) + ":\\";
				UINT driveType = GetDriveType(drivePath.c_str());
				if (driveType == DRIVE_FIXED)
					driveList.push_back(drivePath);
			}
		}
		return driveList;
	}
	std::string ConvertWideToMultiByte(const std::wstring& target_str)
	{
		if (target_str.empty())
			return std::string();

		const int bufferSize = WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, nullptr, 0, nullptr, nullptr);

		if (bufferSize == 0)
			return std::string();

		std::string vOut(bufferSize - 1, '\0');
		WideCharToMultiByte(CP_UTF8, 0, target_str.c_str(), -1, &vOut[0], bufferSize, nullptr, nullptr);

		return vOut;
	}

	namespace file
	{
		bool IsExistsFile(const std::string path)
		{
			return std::filesystem::exists(path) && std::filesystem::is_regular_file(path);
		}
		bool IsExistsDirectory(const std::string path)
		{
			return std::filesystem::exists(path) && std::filesystem::is_directory(path);
		}
		bool DoesFileExistInDirectory(const std::string path, const std::string file)
		{
			for (const auto& file : std::filesystem::directory_iterator(path)) {
				if (file.is_regular_file()) {
					if (!file.path().compare(file)) {
						return true;
					}
				}
			}

			return false;
		}
		void SelectFilePath(std::string& vOut)
		{
			OPENFILENAME ofn;
			char szFile[260]{};

			ZeroMemory(&ofn, sizeof(ofn));
			ofn.lStructSize = sizeof(ofn);
			ofn.hwndOwner = NULL;
			ofn.lpstrFile = szFile;
			ofn.nMaxFile = sizeof(szFile);
			ofn.lpstrFilter = "All Files\0*.*\0Text Files\0*.TXT\0";
			ofn.nFilterIndex = 1;
			ofn.lpstrFileTitle = NULL;
			ofn.nMaxFileTitle = 0;
			ofn.lpstrInitialDir = NULL;
			ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

			if (GetOpenFileName(&ofn) == TRUE) {
				vOut = ofn.lpstrFile;
			}
		}
		void SelectDirectoryPath(const char* str, std::string& vOut)
		{
			// Path
			BROWSEINFO bf{};
			char szFolderPath[MAX_PATH]{};

			bf.lpszTitle = str;
			bf.ulFlags = BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE;

			// ダイアログを表示
			LPITEMIDLIST pidl = SHBrowseForFolder(&bf);
			if (pidl != NULL) {
				// 選択されたフォルダーのパスを取得
				if (SHGetPathFromIDList(pidl, szFolderPath)) {
					vOut = szFolderPath;
				}
				CoTaskMemFree(pidl); // メモリを解放
			}
		}
		// 参照：https://learn.microsoft.com/ja-jp/windows/win32/shell/knownfolderid
		std::string GetAppDataPath(const GUID id)
		{
			PWSTR path = nullptr;
			std::string result;

			if (SUCCEEDED(SHGetKnownFolderPath(id, 0, NULL, &path)))
				result = ConvertWideToMultiByte(path);
			else
				return std::string();

			CoTaskMemFree(path);

			return result;
		}
		std::vector<std::string> GetFileList(const std::string path, const std::string extension)
		{
			std::vector<std::string> filelist;

			for (const auto& file : std::filesystem::directory_iterator(path))
			{
				if (file.is_regular_file())
				{
					if (!extension.compare(file.path().extension().string()))
						filelist.push_back(file.path().filename().string());
				}
			}

			return filelist;
		}
		std::optional<std::string> FindDirectory(const std::string& startDir, const std::string& targetName)
		{
			WIN32_FIND_DATA findFileData;
			HANDLE hFind = FindFirstFile((startDir + "\\*").c_str(), &findFileData);

			if (hFind == INVALID_HANDLE_VALUE)
				return std::nullopt;

			do {
				if (findFileData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
				{
					std::string dirName = findFileData.cFileName;
					if (dirName != "." && dirName != "..")
					{
						if (dirName == targetName)
						{
							FindClose(hFind);
							return startDir + "\\" + dirName;
						}
						else
						{
							auto result = FindDirectory(startDir + "\\" + dirName, targetName);
							if (result) {
								FindClose(hFind);
								return result;
							}
						}
					}
				}
			} while (FindNextFile(hFind, &findFileData) != 0);

			FindClose(hFind);
			return std::nullopt;
		}
	}
	namespace process
	{
		DWORD GetProcessIDByName(const std::string processName)
		{
			DWORD PID = 0;
			PROCESSENTRY32 entry{};
			entry.dwSize = sizeof(PROCESSENTRY32);
			const auto snapShot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

			while (Process32Next(snapShot, &entry)) {
				if (!processName.compare(entry.szExeFile))
					PID = entry.th32ProcessID;
			}

			CloseHandle(snapShot);

			return PID;
		}
		bool IsProcessRunning(const std::string processName)
		{
			return GetProcessIDByName(processName) != 0;
		}
		bool StartProcess(const std::string targetPath, const std::string options) // for my VRChat project
		{
			STARTUPINFO si = { sizeof(STARTUPINFO) };
			PROCESS_INFORMATION pi{};

			std::string command = std::string("\"") + targetPath + "\" " + options;

			if (CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi)) {
				std::cout << "[+] VRChat launched" << std::endl;

				CloseHandle(pi.hProcess);
				CloseHandle(pi.hThread);
			}
			else {
				std::cerr << "[-] Failed to launch VRChat - Error : " << GetLastError() << std::endl;
				return false;
			}

			return true;
		}
		bool StopProcess(const std::string processName)
		{
			HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
			if (hSnap == INVALID_HANDLE_VALUE) {
				return false;
			}

			PROCESSENTRY32 pe;
			pe.dwSize = sizeof(PROCESSENTRY32);

			if (Process32First(hSnap, &pe)) {
				do {
					if (_stricmp(pe.szExeFile, processName.c_str()) == 0)
					{
						HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pe.th32ProcessID);

						if (hProcess) {
							TerminateProcess(hProcess, 0);
							CloseHandle(hProcess);
							CloseHandle(hSnap);

							return true;
						}
					}
				} while (Process32Next(hSnap, &pe));
			}

			CloseHandle(hSnap);
			return false;
		}
	}

	const char* KeyNames[] =
	{
		"-",
		"Mouse Left",
		"Mouse Right",
		"Cancel",
		"Middle Center",
		"MouseSide1",
		"MouseSide2",
		"",
		"Backspace",
		"Tab",
		"",
		"",
		"Clear",
		"Enter",
		"",
		"",
		"Shift",
		"Ctrl",
		"Alt",
		"Pause",
		"CapsLock",
		"",
		"",
		"",
		"",
		"",
		"",
		"Escape",
		"",
		"",
		"",
		"",
		"Space",
		"Page Up",
		"Page Down",
		"End",
		"Home",
		"Left",
		"Up",
		"Right",
		"Down",
		"",
		"",
		"",
		"Print",
		"Insert",
		"Delete",
		"",
		"0",
		"1",
		"2",
		"3",
		"4",
		"5",
		"6",
		"7",
		"8",
		"9",
		"",
		"",
		"",
		"",
		"",
		"",
		"",
		"A",
		"B",
		"C",
		"D",
		"E",
		"F",
		"G",
		"H",
		"I",
		"J",
		"K",
		"L",
		"M",
		"N",
		"O",
		"P",
		"Q",
		"R",
		"S",
		"T",
		"U",
		"V",
		"W",
		"X",
		"Y",
		"Z",
		"",
		"",
		"",
		"",
		"",
		"Numpad 0",
		"Numpad 1",
		"Numpad 2",
		"Numpad 3",
		"Numpad 4",
		"Numpad 5",
		"Numpad 6",
		"Numpad 7",
		"Numpad 8",
		"Numpad 9",
		"Multiply",
		"Add",
		"",
		"Subtract",
		"Decimal",
		"Divide",
		"F1",
		"F2",
		"F3",
		"F4",
		"F5",
		"F6",
		"F7",
		"F8",
		"F9",
		"F10",
		"F11",
		"F12",
	};
}