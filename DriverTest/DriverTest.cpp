// DriverTest.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "driver/RemoteProcess.h"
//Driver::RemoteProcess* m_Process = nullptr;
bool m_PhysicalMemory = false;

int main()
{
    //m_Process = new Driver::RemoteProcess(7744);
    mem.attach(7744);
    uint8_t* base = nullptr;
    mem.main_base(&base);
    printf("Base address -> 0x%p\n", base);

    auto output = nullptr;
   mem.read(&output, (void*)(base + 0x10), sizeof(uint64_t), m_PhysicalMemory);
    printf("value -> %p\n", output);

    uint8_t* processModuleBase = nullptr;
    uint32_t processModuleBaseSize = 0;
    mem.module("ntdll.dll", &processModuleBase, &processModuleBaseSize);
    printf("ntdll.dll -> %p\n ", processModuleBase);
    
    getchar();
}

