#include "Request.h"

#include <iostream>

int64_t(__fastcall* memory::NtUserCreateDesktopEx)(Request*, uint64_t) = nullptr;

void memory::attach(int32_t process_id)
{
	m_ProcessID = process_id;

	if (!NtUserCreateDesktopEx)
	{
		LoadLibraryA("user32.dll");
		LoadLibraryA("win32u.dll");
		LoadLibraryA("ntdll.dll");

		auto win32u = GetModuleHandleA("win32u.dll");
		if (!win32u) return;

		*(void**)&NtUserCreateDesktopEx = GetProcAddress(win32u, "NtUserCreateDesktopEx"); 
	}
}

NTSTATUS memory::module(const char* ModuleName, uint8_t** OutAddress, uint32_t* OutSize)
{
	auto parms = RequestModule(m_ProcessID, ModuleName, OutAddress, OutSize);
	return call(ERequestType::Module, &parms);
}

NTSTATUS memory::main_base(uint8_t** OutAddress)
{
	auto parms = RequestMainBase(m_ProcessID, OutAddress);
	return call(ERequestType::MainBase, &parms);
}

NTSTATUS memory::read(void* Destination, void* Source, size_t Size, bool UsePhysical)
{
	auto parms = RequestCopy(m_ProcessID, Destination, Source, Size, UsePhysical);
	return call(ERequestType::Read, &parms);
}

NTSTATUS memory::write(void* Destination, void* Source, size_t Size, bool UsePhysical)
{
	auto parms = RequestCopy(m_ProcessID, Destination, Source, Size, UsePhysical);
	return call(ERequestType::Write, &parms);
}

NTSTATUS memory::query(void* Address, MEMORY_BASIC_INFORMATION* MemoryInfo)
{
	auto parms = RequestQuery(m_ProcessID, Address, MemoryInfo);
	return call(ERequestType::Query, &parms);
}

NTSTATUS memory::protect(void* Address, size_t Size, uint32_t Prot)
{
	auto parms = RequestProtect(m_ProcessID, Address, Size, Prot);
	return call(ERequestType::Protect, &parms);
}

NTSTATUS memory::alloc(void** OutAddress, size_t Size, DWORD Prot)
{
	auto parms = RequestAlloc(m_ProcessID, OutAddress, Size, Prot);
	return call(ERequestType::Alloc, &parms);
}

NTSTATUS memory::free(void* Address)
{
	auto parms = RequestFree(m_ProcessID, Address);
	return call(ERequestType::Free, &parms);
}

NTSTATUS memory::call(ERequestType Type, RequestProcess* Parms)
{
	std::lock_guard<std::mutex> lock(m_RequestMutex);

	NTSTATUS status = 0;
	bool finished = false;

	auto req = Request(Type, Parms, &status, &finished);
	NtUserCreateDesktopEx(&req, 'MAD?');

	while (!finished);
	return status;
}