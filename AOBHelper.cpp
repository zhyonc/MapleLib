#include "MapleLib.h"
#include "detours.h"

ULONG_PTR GetFuncAddress(ULONG_PTR uAddr) {
	auto pByte = (BYTE*)uAddr;
	// CALL (E8 xx xx xx xx)
	if (pByte[0x00] == 0xE8) {
		return uAddr + 0x05 + *(ULONG_PTR*)(uAddr + 0x01);
	}
	// CALL DWORD PTR (FF 15 xx xx xx xx)
	if (pByte[0x00] == 0xFF && pByte[0x01] == 0x15) {
		return (*(ULONG_PTR*)(uAddr + 0x02));
	}
	// JMP DWORD PTR (FF 25 xx xx xx xx)
	if (pByte[0] == 0xFF && pByte[1] == 0x25) {
		return (*(ULONG_PTR*)(uAddr + 0x02));
	}
	// PUSH addr (68 xx xx xx xx)
	if (pByte[0] == 0x68) {
		return *(ULONG_PTR*)(uAddr + 1);
	}
	// MOV reg, imm32 (B8 ~ BF xx xx xx xx)
	if (pByte[0] >= 0xB8 && pByte[0] <= 0xBF) {
		return *(ULONG_PTR*)(uAddr + 1);
	}
	return 0;
}

ULONG_PTR GetDWORDAddress(ULONG_PTR uAddr) {
	BYTE* pByte = (BYTE*)uAddr;
	// PUSH imm32 (68 xx xx xx xx)
	if (pByte[0] == 0x68) {
		return *(DWORD*)(uAddr + 1);
	}
	// MOV reg, [addr] (8B 0D xx xx xx xx)
	// MOV eax, [addr] (8B 80 xx xx xx xx)
	if (pByte[0] == 0x8B) {
		return *(DWORD*)(uAddr + 2);
	}
	// MOV eax, dword ptr ds:[addr] (A1 xx xx xx xx)
	if (pByte[0] == 0xA1) {
		return *(DWORD*)(uAddr + 1);
	}
	// MOV dword ptr ds:[addr], eax (A3 xx xx xx xx)
	if (pByte[0] == 0xA3) {
		return *(DWORD*)(uAddr + 1);
	}
	// MOV reg, imm32 (B8 ~ BF xx xx xx xx)
	if (pByte[0] >= 0xB8 && pByte[0] <= 0xBF) {
		return *(DWORD*)(uAddr + 1);
	}
	return 0;
}

BOOL SetHook(BOOL bInstall, PVOID* ppPointer, PVOID pDetour) {
	if (DetourTransactionBegin() != NO_ERROR) {
		return FALSE;
	}

	auto tid = GetCurrentThread();

	if (DetourUpdateThread(tid) != NO_ERROR) {
		DetourTransactionAbort();
		return FALSE;
	}

	if ((bInstall ? DetourAttach(ppPointer, pDetour) : DetourDetach(ppPointer, pDetour)) != NO_ERROR) {
		DetourTransactionAbort();
		return FALSE;
	}

	if (DetourTransactionCommit() != NO_ERROR) {
		DetourTransactionAbort();
		return FALSE;
	}

	return TRUE;
}