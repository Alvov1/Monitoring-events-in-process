#ifndef PROJECT_HIDE_H
#define PROJECT_HIDE_H

#include <iostream>
#include <string>
#include <filesystem>
#include <Windows.h>

std::string FileName;
std::filesystem::path Path;

HANDLE(WINAPI* pCreateFileA) (LPCSTR lpFileName, DWORD dwDesiredAccess,
        DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile) = CreateFileA;
HANDLE(WINAPI* pCreateFileW) (LPCWSTR lpFileName, DWORD dwDesiredAccess,
        DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes,
        DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes,
        HANDLE hTemplateFile) = CreateFileW;

HANDLE(WINAPI* pFindFirstFileW)
    (LPCWSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) = FindFirstFileW;
HANDLE(WINAPI* pFindFirstFileA)
    (LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = FindFirstFileA;

BOOL(WINAPI* pFindNextFileW)
    (HANDLE hFindFile, LPWIN32_FIND_DATA lpFindFileData) = FindNextFileW;
BOOL(WINAPI* pFindNextFileA)
    (HANDLE hFindFile, LPWIN32_FIND_DATAA  lpFindFileData) = FindNextFileA;

HANDLE(WINAPI* pFindFirstFileExA)
    (LPCSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp,
    LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExA;
HANDLE(WINAPI* pFindFirstFileExW)
    (LPCWSTR lpFileName, FINDEX_INFO_LEVELS fInfoLevelId,
    LPVOID lpFindFileData, FINDEX_SEARCH_OPS  fSearchOp,
    LPVOID lpSearchFilter, DWORD dwAdditionalFlags) = FindFirstFileExW;

HANDLE WINAPI MyCreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD
    dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD
    dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if(Path.string() == lpFileName)
        return INVALID_HANDLE_VALUE;

    return pCreateFileA(lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI MyCreateFileW(LPCWSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
    if (!lstrcmpW(lpFileName, Path.wstring().c_str()))
        return INVALID_HANDLE_VALUE;

    return pCreateFileW(lpFileName, dwDesiredAccess, dwShareMode,
        lpSecurityAttributes, dwCreationDisposition,
        dwFlagsAndAttributes, hTemplateFile);
}

HANDLE WINAPI MyFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) {
    if (Path.string() == lpFileName)
        return INVALID_HANDLE_VALUE;
    return pFindFirstFileA(lpFileName, lpFindFileData);
}

HANDLE WINAPI MyFindFirstFileW(LPCWSTR lpFileName, LPWIN32_FIND_DATA lpFindFileData) {
    if (!lstrcmpW(lpFileName, Path.wstring().c_str()))
        return INVALID_HANDLE_VALUE;
    return pFindFirstFileW(lpFileName, lpFindFileData);
}

BOOL WINAPI MyFindNextFileA(HANDLE hFindFile, LPWIN32_FIND_DATAA lpFindFileData) {
    bool ret = pFindNextFileA(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == Path.string())
        ret = pFindNextFileA(hFindFile, lpFindFileData);
    return ret;
}

BOOL WINAPI MyFindNextFileW(HANDLE hFindFile, LPWIN32_FIND_DATAW lpFindFileData) {
    bool ret = pFindNextFileW(hFindFile, lpFindFileData);
    if (lpFindFileData->cFileName == Path.wstring())
        ret = pFindNextFileW(hFindFile, lpFindFileData);
    return ret;
}

HANDLE WINAPI MyFindFirstFileExW(LPCWSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAW a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5) {
    HANDLE ret = pFindFirstFileExW(a0, a1, a2, a3, a4, a5);
    if (a2->cFileName == Path.wstring())
        ret = INVALID_HANDLE_VALUE;
    return ret;
}

HANDLE WINAPI MyFindFirstFileExA(LPCSTR a0, FINDEX_INFO_LEVELS a1, LPWIN32_FIND_DATAA a2, FINDEX_SEARCH_OPS a3, LPVOID a4, DWORD a5){
    HANDLE ret = pFindFirstFileExA(a0, a1, a2, a3, a4, a5);
    if (a2->cFileName == Path.string())
        ret = INVALID_HANDLE_VALUE;
    return ret;
}

#endif //PROJECT_HIDE_H
