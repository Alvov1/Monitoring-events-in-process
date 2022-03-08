#include <iostream>
#include <string>
#include <thread>

#include <Windows.h>
#include <TlHelp32.h>

static inline int help() {
    static constexpr auto info = "Usage:\n-pid 123 -func CreateFile\n"
    "-pid 123 -hide C:\\hello.txt\n-name explorer.exe -func CreateFile\n"
    "-name explorer.exe -hide C:\\hello.txt";
    std::cout << info << std::endl;
    return 0;
}

DWORD getProcessByName(const std::string& name) {
    PROCESSENTRY32 pe32;
    pe32.dwSize = sizeof(PROCESSENTRY32);

    DWORD pid = 0;
    HANDLE hTool32 = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
    if(Process32First(hTool32, &pe32))
        while(Process32Next(hTool32, &pe32)) {
            const auto* ptr = pe32.szExeFile;
            if(std::string(ptr) == name) {
                pid = pe32.th32ProcessID;
                break;
            }
        }
    CloseHandle(hTool32);
    return pid;
}

int main(int argc, const char** argv) {
    if(argc < 5)
        throw std::runtime_error("Please enter a proper number of arguments.");

    const std::string processIdType(argv[1]);
    const std::string secondArg(argv[2]);
    const std::string action(argv[3]);
    const std::string argument(argv[4]);

    if(processIdType == "-pid") {
        const auto pId = static_cast<DWORD>(std::stoi(secondArg));
        std::cout << "Tracking proc with id " << pId << "." << std::endl;

        HANDLE proc = nullptr;
        for(proc = OpenProcess(READ_CONTROL, false, pId); proc == nullptr; proc = OpenProcess(READ_CONTROL, false, pId))
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

        CloseHandle(proc);
    } else if(processIdType == "-name") {
        std::cout << "Tracking process " << secondArg << "." << std::endl;

        for(DWORD pId = getProcessByName(secondArg); pId == 0; pId = getProcessByName(secondArg))
            std::this_thread::sleep_for(std::chrono::milliseconds(250));

    } else return help();

    return 0;
}
