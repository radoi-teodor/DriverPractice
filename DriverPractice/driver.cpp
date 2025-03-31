#include <ntifs.h>
#include <ntddk.h>
#include "LsaUtils.h"

#define DRIVER_NAME "DriverPractice"

#define IOCTL_SEND_DATA_FROM_USER CTL_CODE(FILE_DEVICE_UNKNOWN, 0x800, METHOD_BUFFERED, FILE_ANY_ACCESS)

PDEVICE_OBJECT deviceObject = NULL;
UNICODE_STRING deviceName = RTL_CONSTANT_STRING(L"\\Device\\LsaDevice");
UNICODE_STRING symbolicLinkName = RTL_CONSTANT_STRING(L"\\??\\MyDriverSymbol");

NTSTATUS CustomDriverDispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp);
NTSTATUS CreateCloseHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp);

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject);

extern "C" {
	NTSTATUS DriverEntry(_In_ PDRIVER_OBJECT DriverObject, _In_ PUNICODE_STRING RegistryPath) {

		UNREFERENCED_PARAMETER(RegistryPath);

		// set the unload driver callback
		DriverObject->DriverUnload = UnloadDriver;

		NTSTATUS status;

		status = IoCreateDevice(DriverObject, 0, &deviceName, FILE_DEVICE_UNKNOWN, 0, FALSE, &deviceObject);
		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "[%s] IoCreateDevice failed", DRIVER_NAME);
			return status;
		}

		status = IoCreateSymbolicLink(&symbolicLinkName, &deviceName);

		if (!NT_SUCCESS(status)) {
			DbgPrintEx(0, 0, "[%s] IoCreateSymbolicLink failed %d", DRIVER_NAME, status);
			IoDeleteDevice(deviceObject);
			return status;
		}

		DbgPrintEx(0, 0, "[%s] Driver Loaded", DRIVER_NAME);

		// control function
		DriverObject->MajorFunction[IRP_MJ_DEVICE_CONTROL] = CustomDriverDispatchDeviceControl;
		// open/close handle function
		DriverObject->MajorFunction[IRP_MJ_CREATE] = CreateCloseHandler;
		DriverObject->MajorFunction[IRP_MJ_CLOSE] = CreateCloseHandler;

		return STATUS_SUCCESS;

	}
}

NTSTATUS CustomDriverDispatchDeviceControl(PDEVICE_OBJECT DeviceObject, PIRP Irp) {
	UNREFERENCED_PARAMETER(DeviceObject);

	PIO_STACK_LOCATION pIoStackLocation;
	PCHAR welcome = "Hello from kerneland.";
	PVOID pBuf = Irp->AssociatedIrp.SystemBuffer;

	pIoStackLocation = IoGetCurrentIrpStackLocation(Irp);
	switch (pIoStackLocation->Parameters.DeviceIoControl.IoControlCode)
	{
	case IOCTL_SEND_DATA_FROM_USER:
		DbgPrint("[%s] IOCTL HELLO.", DRIVER_NAME);
		DbgPrint("[%s] Message received : %s", DRIVER_NAME, pBuf);

		RtlZeroMemory(pBuf, pIoStackLocation->Parameters.DeviceIoControl.InputBufferLength);
		RtlCopyMemory(pBuf, welcome, strlen(welcome));

		break;
	}

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = strlen(welcome);
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

NTSTATUS CreateCloseHandler(PDEVICE_OBJECT DeviceObject, PIRP Irp)
{
	UNREFERENCED_PARAMETER(DeviceObject);

	Irp->IoStatus.Status = STATUS_SUCCESS;
	Irp->IoStatus.Information = 0;
	IoCompleteRequest(Irp, IO_NO_INCREMENT);

	return STATUS_SUCCESS;
}

VOID UnloadDriver(_In_ PDRIVER_OBJECT DriverObject) {

	UNREFERENCED_PARAMETER(DriverObject);

	IoDeleteDevice(deviceObject);
	IoDeleteSymbolicLink(&symbolicLinkName);

	DbgPrintEx(0, 0, "[%s] Driver Unloaded", DRIVER_NAME);
}