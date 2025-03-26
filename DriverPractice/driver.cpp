#include <ntddk.h>

#define DRIVER_NAME "DriverPractice"

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject);

extern "C" {
	NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

		UNREFERENCED_PARAMETER(RegistryPath);

		// set the unload driver callback
		DriverObject->DriverUnload = UnloadDriver;

		DbgPrintEx(0, 0, "[%s] Driver Loaded", DRIVER_NAME);

		return STATUS_SUCCESS;

	}
}

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject) {

	UNREFERENCED_PARAMETER(DriverObject);

	DbgPrintEx(0, 0, "[%s] Driver Unloaded", DRIVER_NAME);

}