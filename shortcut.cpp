#include <windows.h>
#include <shlobj.h>
#include <objbase.h>
#include <shobjidl.h>
#include <strsafe.h>

int wmain(int argc, wchar_t* argv[])
{
    wchar_t desktopDir[MAX_PATH] = {0};
    wchar_t currentDir[MAX_PATH] = {0};
    wchar_t workingDir[MAX_PATH] = {0};
    wchar_t lnkPath[MAX_PATH] = {0};
    int nSuccess = 0;

    // Получаем путь к рабочему столу (Unicode версия)
    if (FAILED(SHGetFolderPathW(NULL, CSIDL_DESKTOPDIRECTORY, NULL, 0, desktopDir)))
    {
        MessageBoxW(NULL, L"Не удалось получить путь к рабочему столу", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Формируем полный путь ярлыка
    HRESULT hr = StringCchCatW(desktopDir, MAX_PATH, L"\\Far.lnk");
    if (FAILED(hr))
    {
        MessageBoxW(NULL, L"Ошибка обработки пути ярлыка", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Получаем текущий каталог
    if (GetCurrentDirectoryW(MAX_PATH, currentDir) == 0)
    {
        MessageBoxW(NULL, L"Не удалось получить текущий каталог", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Сохраняем рабочую директорию
    hr = StringCchCopyW(workingDir, MAX_PATH, currentDir);
    if (FAILED(hr))
    {
        MessageBoxW(NULL, L"Ошибка обработки рабочего каталога", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Формируем полный путь к исполняемому файлу Far.exe
    hr = StringCchCatW(currentDir, MAX_PATH, L"\\Far.exe");
    if (FAILED(hr))
    {
        MessageBoxW(NULL, L"Ошибка формирования пути к Far.exe", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    // Инициализация COM библиотеки
    hr = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);
    if (FAILED(hr))
    {
        MessageBoxW(NULL, L"Не удалось инициализировать COM", L"Ошибка", MB_OK | MB_ICONERROR);
        return 1;
    }

    IShellLinkW* link = nullptr;
    hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_INPROC_SERVER, IID_IShellLinkW, (void**)&link);
    if (SUCCEEDED(hr) && link)
    {
        link->SetPath(currentDir);
        link->SetDescription(L"Far Manager");
        link->SetWorkingDirectory(workingDir);
        link->SetShowCmd(SW_SHOWNORMAL);

        IShellLinkDataList* pdl = nullptr;
        hr = link->QueryInterface(IID_IShellLinkDataList, (void**)&pdl);
        if (SUCCEEDED(hr) && pdl)
        {
            NT_CONSOLE_PROPS props =
            {
                { sizeof(NT_CONSOLE_PROPS), NT_CONSOLE_PROPS_SIG },
                7,
                245,
                { 125, 40 },
                { 125, 40 },
                { 0, 0 },
                0,
                0,
                { 9, 20 },
                54,
                400,
                L"Consolas",
                15,
                FALSE,
                TRUE,
                TRUE,
                FALSE,
                50,
                4,
                FALSE,
                {
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
            hr = pdl->AddDataBlock(&props);
            if (SUCCEEDED(hr))
            {
                IPersistFile* file = nullptr;
                hr = link->QueryInterface(IID_IPersistFile, (void**)&file);
                if (SUCCEEDED(hr) && file)
                {
                    hr = file->Save(desktopDir, TRUE);
                    if (SUCCEEDED(hr))
                    {
                        nSuccess = 1;
                    }
                    file->Release();
                }
            }
            pdl->Release();
        }
        link->Release();
    }

    CoUninitialize();

    if (!nSuccess)
    {
        MessageBoxW(NULL, L"Не удалось создать ярлык!", L"Ошибка!", MB_OK | MB_ICONERROR);
        return 1;
    }

    return 0;
}
