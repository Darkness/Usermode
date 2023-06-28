#pragma once
#include "Request.h"

#include <mutex>

namespace Driver {

	class RemoteProcess
	{
	public:
		RemoteProcess() = delete;
		RemoteProcess(uint32_t processID);
		virtual ~RemoteProcess() = default;
		RemoteProcess(const RemoteProcess&&) = delete;
	public:
		NTSTATUS Module(const char* ModuleName, uint8_t** OutAddress, uint32_t* OutSize);
		NTSTATUS MainBase(uint8_t** OutAddress);
		NTSTATUS Read(void* Destination, void* Source, size_t Size, bool UsePhysical);
		NTSTATUS Write(void* Destination, void* Source, size_t Size, bool UsePhysical);
		NTSTATUS Query(void* Address, MEMORY_BASIC_INFORMATION* MemoryInfo);
		NTSTATUS Protect(void* Address, size_t Size, uint32_t Prot);
		NTSTATUS Alloc(void** OutAddress, size_t Size, DWORD Prot);
		NTSTATUS Free(void* Address);
	private:
		NTSTATUS Call(ERequestType Type, RequestProcess* Parms);
	private:
		uint32_t m_ProcessID = 0;
		std::mutex m_RequestMutex;
	private:
		static int64_t(__fastcall* NtUserCreateDesktopEx)(Request*, uint64_t);
	};

}
