#pragma once
#include <windows.h>
#include <cstdint>
#include <mutex>
enum class ERequestType : uint8_t
{
	Write, Read, Query, Protect, Alloc, Free,
	Module, MainBase,
};

struct Request
{
	Request(ERequestType type, struct RequestProcess* parms, NTSTATUS* status, bool* finished)
		: Type(type), Parms(parms), Status(status), Finished(finished)
	{
	}

	ERequestType Type;
	struct RequestProcess* Parms;
	NTSTATUS* Status;
	bool* Finished;
};

struct RequestProcess
{
	uint32_t ProcessID;
};

struct RequestCopy : public RequestProcess
{
	RequestCopy(uint32_t processID, void* dst, void* src, size_t size, bool usePhysical)
		: Destination(dst), Source(src), Size(size), UsePhysical(usePhysical)
	{
		ProcessID = processID;
	}

	void* Destination;
	void* Source;
	size_t Size;
	bool UsePhysical;
};

struct RequestQuery : public RequestProcess
{
	RequestQuery(uint32_t processID, void* address, MEMORY_BASIC_INFORMATION* memoryInfo)
		: Address(address), MemoryInfo(memoryInfo)
	{
		ProcessID = processID;
	}

	void* Address;
	MEMORY_BASIC_INFORMATION* MemoryInfo;
};

struct RequestProtect : public RequestProcess
{
	RequestProtect(uint32_t processID, void* address, size_t size, uint32_t prot)
		: Address(address), Size(size), Prot(prot)
	{
		ProcessID = processID;
	}

	void* Address;
	size_t Size;
	uint32_t Prot;
};

struct RequestAlloc : public RequestProcess
{
	RequestAlloc(uint32_t processID, void** outAddress, size_t size, uint32_t prot)
		: OutAddress(outAddress), Size(size), Prot(prot)
	{
		ProcessID = processID;
	}

	void** OutAddress;
	size_t Size;
	uint32_t Prot;
};

struct RequestFree : public RequestProcess
{
	RequestFree(uint32_t processID, void* address)
		: Address(address)
	{
		ProcessID = processID;
	}

	void* Address;
};

struct RequestModule : public RequestProcess
{
	RequestModule(uint32_t processID, const char* name, uint8_t** address, uint32_t* size)
		: ModuleName(name), OutAddress(address), OutSize(size)
	{
		ProcessID = processID;
	}

	const char* ModuleName;
	uint8_t** OutAddress;
	uint32_t* OutSize;
};

struct RequestMainBase : public RequestProcess
{
	RequestMainBase(uint32_t processID, uint8_t** outAddress)
		: OutAddress(outAddress)
	{
		ProcessID = processID;
	}

	uint8_t** OutAddress;
};

class memory
{
public:
	void attach(int32_t process_id);
	NTSTATUS module(const char* ModuleName, uint8_t** OutAddress, uint32_t* OutSize);
	NTSTATUS main_base(uint8_t** OutAddress);
	NTSTATUS read(void* Destination, void* Source, size_t Size, bool UsePhysical);
	NTSTATUS write(void* Destination, void* Source, size_t Size, bool UsePhysical);
	NTSTATUS query(void* Address, MEMORY_BASIC_INFORMATION* MemoryInfo);
	NTSTATUS protect(void* Address, size_t Size, uint32_t Prot);
	NTSTATUS alloc(void** OutAddress, size_t Size, DWORD Prot);
	NTSTATUS free(void* Address);
	template<typename T>
	T rpm(uintptr_t address);
	template<typename T>
	void wpm(uintptr_t address, T value);
private:
	NTSTATUS call(ERequestType Type, RequestProcess* Parms);
private:
	uint32_t m_ProcessID = 0;
	std::mutex m_RequestMutex;
private:
	static int64_t(__fastcall* NtUserCreateDesktopEx)(Request*, uint64_t);

}; inline memory mem;

template<typename T>
inline T memory::rpm(uintptr_t address)
{
	if (address >= 0x7FFFFFFFFFFF || address <= 0x0)
		return { };

	T buffer;
	mem.read(&buffer, (void*)address, sizeof(T), false);

	return buffer;
}

template<typename T>
inline void memory::wpm(uintptr_t address, T value)
{
	if (address >= 0x7FFFFFFFFFFF || address <= 0x0)
		return;

	mem.write((void*)address, &value, sizeof(T), false);
}