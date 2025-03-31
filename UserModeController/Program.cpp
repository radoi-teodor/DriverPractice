#include <Windows.h>
#include <iostream>

#define IOCTL_SEND_DATA_FROM_USER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

void main() {
	HANDLE hDevice;
	char welcome[50] = "Hello from userland.";
	DWORD dwBytesRead = 0;
	char ReadBuffer[50] = { 0 };

	hDevice = CreateFile(L"\\\\.\\MyDriverSymbol", GENERIC_WRITE | GENERIC_READ, 0, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hDevice == INVALID_HANDLE_VALUE) {
		std::cout << "[!] Invalid device handle: " << GetLastError();
		return;
	}

	if (DeviceIoControl(hDevice, IOCTL_SEND_DATA_FROM_USER, welcome, strlen(welcome), ReadBuffer, sizeof(ReadBuffer), &dwBytesRead, NULL) == 0) {
		std::cout << "[!] DeviceIoControl error: " << GetLastError();
		return;
	}
	
	std::cout << "Message received from kerneland: " << ReadBuffer << std::endl;
	std::cout << "Bytes read: " << dwBytesRead << std::endl;

	CloseHandle(hDevice);

	return;
}