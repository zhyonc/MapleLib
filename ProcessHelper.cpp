#include "MapleLib.h"

void GetModuleEntryList(std::vector<MODULEENTRY32W>& vModuleEntryList)
{
	DWORD pid = GetCurrentProcessId();

	if (!pid) {
		return;
	}

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);

	if (hSnapshot == INVALID_HANDLE_VALUE) {
		return;
	}

	MODULEENTRY32W me32;
	memset(&me32, 0, sizeof(me32));
	me32.dwSize = sizeof(me32);
	if (!Module32FirstW(hSnapshot, &me32)) {
		CloseHandle(hSnapshot);
		return;
	}

	vModuleEntryList.clear();

	do {
		vModuleEntryList.push_back(me32);
	} while (Module32NextW(hSnapshot, &me32));

	CloseHandle(hSnapshot);

}

bool FindModuleEntry(const std::wstring& wModuleName, BYTE** ppBaseAddr, DWORD& baseSize) {
	std::vector<MODULEENTRY32W> vModuleEntryList;
	GetModuleEntryList(vModuleEntryList);
	if (vModuleEntryList.empty()) {
		return false;
	}
	for (const auto& me32 : vModuleEntryList) {
		if (_wcsicmp(me32.szModule, wModuleName.c_str()) == 0) {
			*ppBaseAddr = me32.modBaseAddr;
			baseSize = me32.modBaseSize;
			return true;
		}
	}
	return false;
}

bool IsModuleCalled(const std::wstring& wModuleName, void* pReturnAddress) {
	if (!pReturnAddress) {
		return false;
	}
	BYTE* pBaseAddr = nullptr;
	DWORD baseSize = 0;
	if (!FindModuleEntry(wModuleName, &pBaseAddr, baseSize)) {
		DEBUG(wModuleName + L" hasn't been call yet");
		return false;
	}
	if (!pBaseAddr || baseSize == 0) {
		DEBUG(wModuleName + L" failed to get entry addr");
		return false;
	}
	ULONG_PTR retAddr = (ULONG_PTR)pReturnAddress;
	ULONG_PTR baseAddr = (ULONG_PTR)pBaseAddr;
	ULONG_PTR endAddr = baseAddr + baseSize;
	return retAddr >= baseAddr && retAddr < endAddr;
}


bool ExtractSectionList(const std::wstring& wModuleName, std::vector<MEMORY_BASIC_INFORMATION>& vTextSection, std::vector<MEMORY_BASIC_INFORMATION>& vDataSection) {
	BYTE* pBaseAddr = nullptr;
	DWORD baseSize = 0;

	if (!FindModuleEntry(wModuleName, &pBaseAddr, baseSize)) {
		return false;
	}

	ULONG_PTR baseAddr = (ULONG_PTR)(pBaseAddr);
	ULONG_PTR endAddr = baseAddr + baseSize;

	MEMORY_BASIC_INFORMATION mbi;
	memset(&mbi, 0, sizeof(mbi));

	while (baseAddr < endAddr)
	{
		if (VirtualQuery((void*)baseAddr, &mbi, sizeof(mbi)) == sizeof(mbi)) {
			// text section for aob scan
			if (mbi.Protect & (PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY)) {
				vTextSection.push_back(mbi);
			}
			// data section for string search
			if (vTextSection.size() && mbi.Protect & (PAGE_READWRITE | PAGE_WRITECOPY)) {
				// PAGE_READONLY and EXECUTE are currently ignored
				vDataSection.push_back(mbi);
			}
			baseAddr += mbi.RegionSize;
		}
	}
	if (vTextSection.empty()) {
		return false;
	}

	return true;
}