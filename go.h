#ifndef PROJECT_GO_H
#define PROJECT_GO_H

#include <iostream>
#include <filesystem>
#include <thread>
#include <string>

#include <Windows.h>
#include <TlHelp32.h>

static const std::filesystem::path LibraryPath("Project.dll");

static HANDLE pipe;
static void initPipe() {
    pipe = CreateNamedPipe(R"(\\.\pipe\monitor_pipe)", PIPE_ACCESS_DUPLEX,
       PIPE_TYPE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES, 512, 512, 5000, nullptr);
    if(pipe == nullptr || pipe == INVALID_HANDLE_VALUE)
        throw std::runtime_error("Named pipe creation error: " + std::to_string(GetLastError()));
}

static void Inject(DWORD pId, const std::filesystem::path& path) {
    if(pId == 0)
        throw std::invalid_argument("[-] Got PID of the process equal to zero. Terminating.");

    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, false, pId);
    if(process == nullptr)
        throw std::runtime_error("[-] Failed to open the process.");

    LPCVOID loadLibAddy = GetProcAddress(GetModuleHandle("kernel32.dll"), "LoadLibraryA");

    LPVOID remoteString = VirtualAllocEx(process, nullptr, path.string().size(),
     MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);

    WriteProcessMemory(process, remoteString, path.string().c_str(),
       path.string().size(), nullptr);

    if(CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE) loadLibAddy, remoteString, 0, nullptr) == nullptr)
        throw std::runtime_error("[-] Failed to create remote thread.");

    CloseHandle(process);
}

static void Monitoring(DWORD pId, const std::string& function) {
    initPipe();

    if(!std::filesystem::exists(LibraryPath))
        throw std::runtime_error("[-] Library is not found on LibraryPath '" + LibraryPath.string() + "'.");

    std::thread thread(Inject, pId, LibraryPath);
    std::wcout << "[+] Waiting for a client to connect to the pipe." << std::endl;

    if(!ConnectNamedPipe(pipe, nullptr))
        throw std::runtime_error("[-] CreateNamedPipe failed with code: " + std::to_string(GetLastError()) + ".");

    const auto data = "-func " + function;
    std::cout << "[+] Sending data to pipe." << std::endl;

    DWORD numBytesWriten = 0;
    if(WriteFile(pipe, data.c_str(), data.size(), &numBytesWriten, nullptr))
        std::cout << "[+] Sent " + std::to_string(numBytesWriten) + "." << std::endl;
    else
        throw std::runtime_error("[-] Failed to send data.");

    thread.detach();

    std::cout << "[+] Function calls:" << std::endl;

    DWORD numBytesRead = 0;
    char buffer[1000];
    while(true) {
        if(!ReadFile(pipe, buffer, 1000, &numBytesRead, nullptr))
            throw std::runtime_error("[-] Reading file failed.");
        buffer[numBytesRead] = 0;
        std::cout << "[+] Read: " << buffer << std::endl;
    }

    CloseHandle(pipe);
    std::cout << "[+] Done." << std::endl;
}
static void Hiding(DWORD pId, const std::filesystem::path& path) {
    initPipe();

    if(!std::filesystem::exists(LibraryPath))
        throw std::runtime_error("[-] Library is not found on LibraryPath '" + LibraryPath.string() + "'.");

    std::thread thread(Inject, pId, LibraryPath);
    std::wcout << "[+] Waiting for a client to connect to the pipe." << std::endl;

    if(!ConnectNamedPipe(pipe, nullptr))
        throw std::runtime_error("[-] CreateNamedPipe failed with code: " + std::to_string(GetLastError()) + ".");

    const std::string data("-hide " + path.string());
    std::cout << "[+] Sending data to pipe." << std::endl;

    DWORD numBytesWriten = 0;
    if(WriteFile(pipe, data.c_str(), data.size(), &numBytesWriten, nullptr))
        std::cout << "[+] Sent " + std::to_string(numBytesWriten) + "." << std::endl;
    else
        throw std::runtime_error("[-] Failed to send data.");

    thread.detach();
};

#endif //PROJECT_GO_H
