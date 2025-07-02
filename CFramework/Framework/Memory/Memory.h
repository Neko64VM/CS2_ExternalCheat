#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <string>
#include <vector>
#include <Psapi.h>
#include <sstream>

/*	[+] メモリやオーバーレイの初期化モードを設定します

	WINDOW_TITLE : ウィンドウのタイトルを使用します
	WINDOW_CLASS : ウィンドウのクラス名を使用します
	PROCESS      : 実行ファイル名を使用します				*/
enum InitializeMode : int
{
	WINDOW_TITLE,
	WINDOW_CLASS,
	PROCESS
};

class Memory
{
private:
	DWORD m_dwPID;
	HANDLE m_hProcess;

	void SetBaseAddress();
	MODULEENTRY32 GetModule(const std::string moduleName);
	PROCESSENTRY32 GetProcess(const std::string processName);
public:
	uintptr_t m_dwClientBaseAddr;
	//uintptr_t m_dwEngineBaseAddr;
	//uintptr_t m_dwProcessBaseAddr;

	bool AttachProcess(const char* targetName, InitializeMode InitMode);
	void DetachProcess();
	uintptr_t GetModuleBaseAddress(const std::string moduleName);

	// PattenScan
	MODULEINFO GetModuleInfo(const std::string moduleName);
	uintptr_t FindPattern(const std::vector<uint8_t>& read_data, const uintptr_t baseAddress, const std::string pattern, int offset, int extra);

	uintptr_t FindPattern_M(const std::vector<uint8_t>& read_data, const uintptr_t baseAddress, const std::string pattern, int offset, int extra);

	template <typename T>
	constexpr const T Read(const uintptr_t& address) const noexcept
	{
		T value{};
		ReadProcessMemory(m_hProcess, reinterpret_cast<const void*>(address), &value, sizeof(T), NULL);
		return value;
	}
	template <typename T>
	constexpr void Write(const uintptr_t& address, const T& value) const noexcept
	{
		WriteProcessMemory(m_hProcess, reinterpret_cast<void*>(address), &value, sizeof(T), NULL);
	}
	bool ReadString(const uintptr_t address, LPVOID buffer, SIZE_T size) const noexcept
	{
		return !!::ReadProcessMemory(m_hProcess, (void*)(address), buffer, size, nullptr);
	}
	std::string ReadStringA(const uintptr_t address) const noexcept
	{
		char vOut[MAX_PATH]{};
		ReadProcessMemory(m_hProcess, (void*)(address), vOut, sizeof(vOut), nullptr);

		return std::string(vOut);
	}
	std::vector<uint8_t> ReadBytes(uintptr_t address, size_t size)
	{
		std::vector<uint8_t> buffer(size);
		ReadProcessMemory(m_hProcess, reinterpret_cast<LPCVOID>(address), buffer.data(), size, nullptr);
		return buffer;
	}
	uintptr_t ReadChain(uintptr_t address, const std::vector<uintptr_t>& offsets)
	{
		uintptr_t result = Read<uintptr_t>(address + offsets.at(0));
		for (int i = 1; i < offsets.size(); i++)
			result = Read<uintptr_t>(result + offsets.at(i));

		return result;
	}
};

extern Memory m;