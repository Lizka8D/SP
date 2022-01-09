// ConsoleApplication.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <stdio.h>
#include <vector>
#include <Windows.h>
#include <Shlwapi.h>
#include <tchar.h>
#include <string>
#include <cstring>

using namespace std;


vector<wstring> FindFile(const wstring& directory, vector<wstring>& all_files)
{
    
    //cout << "Liza\n";

    wstring tmp = directory + L"\\*";
    WIN32_FIND_DATAW file;
    HANDLE search_handle = FindFirstFileW(tmp.c_str(), &file);
    
    if (search_handle != INVALID_HANDLE_VALUE)
    {
        vector<wstring> directories;

        do
        {
            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                if ((!lstrcmpW(file.cFileName, L".")) || (!lstrcmpW(file.cFileName, L"..")) || (!lstrcmpW(file.cFileName, L"$")))
                    continue;
            }

            tmp = directory + L"\\" + wstring(file.cFileName);
            //wcout << tmp << endl;
            all_files.push_back(tmp);
            //a.push_back(wstring(file.cFileName));

            if (file.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
            {
                directories.push_back(tmp);
                //all_files.push_back(tmp);
            }
        } while (FindNextFileW(search_handle, &file));

        FindClose(search_handle);

        for (vector<wstring>::iterator iter = directories.begin(), end = directories.end(); iter != end; ++iter)
        {
            FindFile(*iter, all_files);
        }
        return all_files;
        
    }

    return all_files;
}

SIZE_T* GetFileClusters(
    PCTSTR lpFileName,
    ULONG ClusterSize,
    ULONG* ClCount,
    ULONG* FileSize)
{
    ClusterSize = 4096;
    HANDLE hFile;
    ULONG OutSize;
    ULONG Bytes, Cls, CnCount, r;
    SIZE_T* Clusters = NULL;
    BOOLEAN Result = FALSE;
    LARGE_INTEGER PrevVCN, Lcn;
    STARTING_VCN_INPUT_BUFFER InBuf;
    PRETRIEVAL_POINTERS_BUFFER OutBuf;
    hFile = CreateFile(lpFileName, GENERIC_READ, // FILE_READ_ATTRIBUTES
        FILE_SHARE_READ, // | FILE_SHARE_WRITE | FILE_SHARE_DELETE
        NULL, OPEN_EXISTING, 0, 0);

    if (hFile != INVALID_HANDLE_VALUE)
    {
        *FileSize = GetFileSize(hFile, NULL);
        OutSize = sizeof(RETRIEVAL_POINTERS_BUFFER) + (*FileSize / ClusterSize) * sizeof(OutBuf->Extents);
        OutBuf = (PRETRIEVAL_POINTERS_BUFFER)malloc(OutSize);
        InBuf.StartingVcn.QuadPart = 0;
        if (DeviceIoControl(hFile, FSCTL_GET_RETRIEVAL_POINTERS, &InBuf,
            sizeof(InBuf), OutBuf, OutSize, &Bytes, NULL))
        {
            *ClCount = (*FileSize + ClusterSize - 1) / ClusterSize;
            Clusters = (SIZE_T*)malloc(*ClCount * sizeof(SIZE_T));
            PrevVCN = OutBuf->StartingVcn;
            for (r = 0, Cls = 0; r < OutBuf->ExtentCount; r++)
            {
                Lcn = OutBuf->Extents[r].Lcn;
                for (CnCount = OutBuf->Extents[r].NextVcn.QuadPart - PrevVCN.QuadPart;
                    CnCount;
                    CnCount--, Cls++, Lcn.QuadPart++)
                    Clusters[Cls] = Lcn.QuadPart;
                PrevVCN = OutBuf->Extents[r].NextVcn;
            }
        }
        free(OutBuf);
        CloseHandle(hFile);
    }
    return Clusters;
}

bool FragmentedFiles(PCTSTR FilePath)
{
    ULONG64 d1 = 0, d2 = 0;
    char DriveName[3] = { 0 };
    ULONG SecPerCl, BtPerSec, ClusterSize, ClCount, FileSize;

    memcpy(DriveName, FilePath, 2);
    GetDiskFreeSpaceA(DriveName, &SecPerCl, &BtPerSec, NULL, NULL);

    ClusterSize = SecPerCl * BtPerSec;

    PSIZE_T x = GetFileClusters(FilePath, ClusterSize, &ClCount, &FileSize);

    //cout << FilePath << "\t" << ClCount << endl;

    if (ClCount > 1 && ClCount != 3435973836)
    {
        return true;
    }
    else
    {
        return false;
    }
}

bool UnfragmentedFiles(PCTSTR FilePath)
{
    ULONG64 d1 = 0, d2 = 0;
    char DriveName[3] = { 0 };
    ULONG SecPerCl, BtPerSec, ClusterSize, ClCount, FileSize;

    memcpy(DriveName, FilePath, 2);
    GetDiskFreeSpaceA(DriveName, &SecPerCl, &BtPerSec, NULL, NULL);

    ClusterSize = SecPerCl * BtPerSec;

    PSIZE_T x = GetFileClusters(FilePath, ClusterSize, &ClCount, &FileSize);

    return false;
}

bool OccupyingOneClusterFiles(PCTSTR FilePath)
{
    ULONG64 d1 = 0, d2 = 0;
    char DriveName[3] = { 0 };
    ULONG SecPerCl, BtPerSec, ClusterSize, ClCount, FileSize;

    memcpy(DriveName, FilePath, 2);
    GetDiskFreeSpaceA(DriveName, &SecPerCl, &BtPerSec, NULL, NULL);

    ClusterSize = SecPerCl * BtPerSec;

    PSIZE_T x = GetFileClusters(FilePath, ClusterSize, &ClCount, &FileSize);

    if (ClCount == 1)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int main()
{
    
    int choice = 0;
    bool empty = true;
    vector<wstring> all_files;

    cout << "Enter the number of the file type you want to get a list of.\n\n";
    cout << "1. Fragmented files\n";
    cout << "2. Unfragmented files\n";
    cout << "3. Occupying one cluster\n";

    cout << "\n--> ";
    cin >> choice;

    all_files = FindFile(L"C:\\Users\\lizab\\source", all_files);

    switch (choice)
    {
    case 1:
        {            
            cout << "\nFiles:\n";

            for (unsigned int i = 0; i < all_files.size(); i++)
            {
                if (FragmentedFiles(all_files[i].c_str()))
                {
                    wcout << all_files[i] << endl;
                    empty = false;
                }
            }
            break;
        }
    case 2:
        {
            for (unsigned int i = 0; i < all_files.size(); i++)
            {
                if (UnfragmentedFiles(all_files[i].c_str()))
                {
                    wcout << all_files[i] << endl;
                    empty = false;
                }
            }
            break;
        }
    case 3:
        {
            for (unsigned int i = 0; i < all_files.size(); i++)
            {
                if (OccupyingOneClusterFiles(all_files[i].c_str()))
                {
                    wcout << all_files[i] << endl;
                    empty = false;
                }
            }
            break;
        }
    default:
        cout << "\nError.\n";
        empty = false;
    }

    if (empty)
    {
        cout << "There are no such files.\n";
    }
}

