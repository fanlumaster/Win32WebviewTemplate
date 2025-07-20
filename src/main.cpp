#include "main.h"
#include "my_webview.h"
#include <dwmapi.h>
#include <intsafe.h>
#include <stdlib.h>
#include <synchapi.h>
#include <tchar.h>
#include <windows.h>
#include <winnt.h>
#include <winuser.h>
#include <wrl.h>
#include <wrl/client.h>

#pragma comment(lib, "dwmapi.lib")

using namespace Microsoft::WRL;

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam,
                                LPARAM lParam)
{

    switch (message)
    {
    case WM_ERASEBKGND: { // Make the background dark
        HDC hdc = (HDC)wParam;
        RECT rc;
        GetClientRect(hWnd, &rc);

        HBRUSH darkBrush = CreateSolidBrush(RGB(32, 32, 32));
        FillRect(hdc, &rc, darkBrush);
        DeleteObject(darkBrush);
        return 1;
    }
    case WM_SIZE: {
        if (webviewController)
        {
            RECT rect;
            GetClientRect(hWnd, &rect);
            webviewController->put_Bounds(rect);
        }
        break;
    }
    case WM_DESTROY: {
        PostQuitMessage(0);
        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }

    return 0;
}

int CALLBACK WinMain(_In_ HINSTANCE hInstance, _In_ HINSTANCE hPrevInstance,
                     _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
    // Set DPI awareness to per-monitor awareness
    SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

    WNDCLASSEX wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc = WndProc;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = hInstance;
    wcex.hIcon = LoadIcon(hInstance, IDI_APPLICATION);
    wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName = szWindowClass;
    wcex.hIconSm = LoadIcon(wcex.hInstance, IDI_APPLICATION);

    if (!RegisterClassEx(&wcex))
    {
        MessageBox(NULL, _T("Call to RegisterClassEx failed!"),
                   _T("Windows Desktop Guided Tour"), NULL);

        return 1;
    }

    HWND hWnd = CreateWindowEx(0,                             //
                               szWindowClass,                 //
                               L"Win32WebViewTemplateWindow", //
                               WS_OVERLAPPEDWINDOW,           //
                               100,                           //
                               100,                           //
                               (108 + 15) * 1.5 * 4,          //
                               (246 + 15) * 1.5 * 2,          //
                               nullptr,                       //
                               nullptr,                       //
                               hInstance,                     //
                               nullptr);                      //

    if (!hWnd)
    {
        MessageBox(NULL,                            //
                   L"Call to CreateWindow failed!", //
                   L"Windows Desktop Guided Tour",  //
                   NULL);                           //
        return 1;
    }

    BOOL useDarkMode = TRUE;
    DwmSetWindowAttribute(             //
        hWnd,                          //
        DWMWA_USE_IMMERSIVE_DARK_MODE, //
        &useDarkMode,                  //
        sizeof(useDarkMode)            //
    );

    ShowWindow(hWnd, SW_SHOW);
    UpdateWindow(hWnd);

    PrepareWindowHtml();
    InitWebview(hWnd);

    MSG msg;
    while (GetMessage(&msg, NULL, 0, 0))
    {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return (int)msg.wParam;
}