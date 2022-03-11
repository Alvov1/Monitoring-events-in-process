#include <iostream>
#include <string>
#include <Windows.h>
#include "Hide.h"

HANDLE pipe;
std::string FuncName;
LPVOID DynamicTarget = nullptr;

void NotifyMonitor() {
    DWORD WrittenBytes = 0;
    const auto msg = FuncName + " is called.";
    WriteFile(pipe, msg.c_str(), msg.size(), &WrittenBytes, nullptr);
}

void __declspec(naked) HookingRoutine32() {
    __asm {
        PUSHFD
        PUSHAD
    }
    NotifyMonitor();
    __asm {
        POPAD
        POPFD
        mov eax, DynamicTarget;
        push eax;
        ret;
    }
}

bool Monitoring() {
    static const std::string data("[-] Error in DetourFindFunction.");

    DWORD bytesWritten = 0;
    DynamicTarget = (PDWOID) DetourFindFunction((LPCSTR) "kernel32.dll", (LPCSTR) FuncName.c_str());
    if(DynamicTarget == nullptr) {
        WriteFile(pipe, data.c_str(), data.size(), &bytesWritten, nullptr);
        return false;
    }

    DetourTransactionBegin();
    DetourUpdateThread(GetCurrentThread());
    DetourAttach(&(PVOID&)DynamicTarget, HookingRoutine32);
    DetourTransactionCommit();
    return true;
}

bool Hiding() {
    /* TODO: Realise 'Hiding' function. */
}

bool APIENTRY LibraryDll(HMODULE hModule, DWORD ul_reason_for_call, LPVOID lpReserved) {
    switch (ul_reason_for_call) {
        case DLL_PROCESS_ATTACH: {
            pipe = CreateFileW(
                    L"\\\\.\\pipe\\monitor_pipe",
                    GENERIC_READ | GENERIC_WRITE,
                    FILE_SHARE_READ | FILE_SHARE_WRITE,
                    nullptr,
                    OPEN_EXISTING,
                    FILE_ATTRIBUTE_NORMAL,
                    nullptr);
            if (pipe == INVALID_HANDLE_VALUE)
                throw std::runtime_error(
                        "[-] Connection to pipe failed with code " + std::to_string(GetLastError()) + ".");

            std::string buffer;
            buffer.reserve(513);
            DWORD bytesWritten = 0;
            if (!ReadFile(pipe, buffer.data(), 512 * sizeof(char), &bytesWritten, nullptr))
                throw std::runtime_error("[-] Reading pipe failed.");

            if (buffer.substr(0, 5) == "-func") {
                FuncName = buffer.substr(6);
                return Monitoring();
            }

            if (buffer.substr(0, 5) == "-hide") {
                FileName = buffer.substr(6);
                return Hiding();
            }
        } break;
        case DLL_PROCESS_DETACH:
//            CloseHandle(pipe);
            break;
        default:
            break;
    }
    return true;
}