#include <windows.h>
#include <shlobj.h>
//#include <stdio.h>
//#include <iostream>
 
int main(int argc, char* argv[])
{
    char desktopDir[MAX_PATH], currentDir[MAX_PATH], workingDir[MAX_PATH];
    OLECHAR lnkPath[MAX_PATH];
    int nSuccess = 0;
    SHGetFolderPath(0, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopDir);
    strcat(desktopDir, "\\Far.lnk");
    GetCurrentDirectory(MAX_PATH, currentDir);
    strcpy(workingDir, currentDir);
    strcat(currentDir, "\\Far.exe");
    //printf("%s\n",desktopDir);
 
    if (SUCCEEDED(CoInitializeEx(0, COINIT_APARTMENTTHREADED)))
    {
        IShellLink*   link;
        if (SUCCEEDED(CoCreateInstance(CLSID_ShellLink, NULL,
                                       CLSCTX_INPROC_SERVER, IID_IShellLink, (void**) &link)))
        {
            //link->SetPath(TEXT(argv[1]));
            link->SetPath(TEXT(currentDir));
            link->SetDescription(TEXT("Far Manager"));
            link->SetWorkingDirectory(TEXT(workingDir));
            link->SetShowCmd(SW_SHOW);
 
            IShellLinkDataList* pdl;
            if (SUCCEEDED(link->QueryInterface(IID_IShellLinkDataList, (void**)&pdl)))
            {
                NT_CONSOLE_PROPS props =
                {
                    {                           // DATABLOCKHEADER dbh
                        sizeof(NT_CONSOLE_PROPS), // DWORD cbSize
                        NT_CONSOLE_PROPS_SIG        // DWORD dwSignature
                    },
                    7,                          // WORD wFillAttribute;
                    245,                        // WORD wPopupFillAttribute;
                    { 125, 40 },              // COORD dwScreenBufferSize;
                    { 125, 40 },                // COORD dwWindowSize;
                    { 0, 0 },                   // COORD dwWindowOrigin;
                    0,                          // DWORD nFont;
                    0,                          // DWORD nInputBufferSize;
                    { 9, 20 },                  // COORD dwFontSize;
                    54,                         // UINT uFontFamily;
                    400,                        // UINT uFontWeight;
                    L"Consolas",          // WCHAR FaceName[LF_FACESIZE];
                    15,                         // UINT uCursorSize;
                    0,                          // BOOL bFullScreen;
                    1,                          // BOOL bQuickEdit;
                    1,                          // BOOL bInsertMode;
                    0,                          // BOOL bAutoPosition;
                    50,                         // UINT uHistoryBufferSize;
                    4,                          // UINT uNumberOfHistoryBuffers;
                    0,                          // BOOL bHistoryNoDup;
                    {                           // COLORREF ColorTable[16];
                        RGB(0, 0, 0),
                        RGB(31, 59, 105),
                        RGB(56, 150, 56),
                        RGB(50, 135, 145),
                        RGB(128, 0, 68),
                        RGB(142, 92, 147),
                        RGB(128, 128, 0),
                        RGB(192, 192, 192),
                        RGB(128, 128, 128),
                        RGB(41, 69, 255),
                        RGB(0, 255, 0),
                        RGB(70, 255, 255),
                        RGB(245, 15, 15),
                        RGB(250, 60, 250),
                        RGB(255, 230, 70),
                        RGB(255, 255, 255)
                    }
                };
                if (SUCCEEDED(pdl->AddDataBlock(&props)))
                {
                    IPersistFile* file;
                    if (SUCCEEDED(link->QueryInterface(IID_IPersistFile,
                                                       (void**) &file)))
                    {
            mbstowcs((OLECHAR*)&lnkPath, (char*)&desktopDir, sizeof(desktopDir));
            BSTR name = SysAllocString(lnkPath);
                        if (name)
                        {
                            if (SUCCEEDED(file->Save(name, TRUE)))
                            {
                              //  std::cout << "done\n";
                              //printf("done.\n");
                              nSuccess = 1;
                            }
                            SysFreeString(name);
                        }
                    }
                }
                pdl->Release();
            }
            link->Release();
        }
        CoUninitialize();
    }
 
    if(!nSuccess)
    {
        MessageBox(0, "Failed to create shortcut!", "Error!", MB_OK|MB_ICONERROR);
    }
}