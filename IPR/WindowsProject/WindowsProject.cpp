// WindowsProject.cpp : Определяет точку входа для приложения.
//

#include "framework.h"
#include "WindowsProject.h"

#define START 6
#define STOP 7
#define MAX_LOADSTRING 100
#define MAIN_WINDOW_WIDTH 800
#define MAIN_WINDOW_HEIGHT 400

int X = MAIN_WINDOW_WIDTH / 2 - 50; // x coordinate of text
int Y = MAIN_WINDOW_HEIGHT / 2 - 50; // y coordinate of text
bool leftDirFlag = false;  // flag of direction if move
const int TIMER_ID = 1; // timer idz 


// Глобальные переменные:
HINSTANCE hInst;                                // текущий экземпляр
WCHAR szTitle[MAX_LOADSTRING];                  // Текст строки заголовка
WCHAR szWindowClass[MAX_LOADSTRING];            // имя класса главного окна

// Отправить объявления функций, включенных в этот модуль кода:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    // TODO: Разместите код здесь.

    // Инициализация глобальных строк
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_WINDOWSPROJECT, szWindowClass, MAX_LOADSTRING);
    MyRegisterClass(hInstance);

    // Выполнить инициализацию приложения:
    if (!InitInstance (hInstance, nCmdShow))
    {
        return FALSE;
    }

    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_WINDOWSPROJECT));

    MSG msg;

    // Цикл основного сообщения:
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
    }

    return (int) msg.wParam;
}



//
//  ФУНКЦИЯ: MyRegisterClass()
//
//  ЦЕЛЬ: Регистрирует класс окна.
//
ATOM MyRegisterClass(HINSTANCE hInstance)
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = (HICON)LoadImage(nullptr, _T("./icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    //LoadIcon(hInstance, MAKEINTRESOURCE(IDI_WINDOWSPROJECT));
    wcex.hCursor        = (HICON)LoadImage(nullptr, _T("./cursor.cur"), IMAGE_CURSOR, 0, 0, LR_LOADFROMFILE);
    //LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)(CreateSolidBrush(RGB(255, 255, 255)));
    //(HBRUSH)(COLOR_WINDOW+1);
    wcex.lpszMenuName   = MAKEINTRESOURCEW(IDC_WINDOWSPROJECT);
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = (HICON)LoadImage(wcex.hInstance, _T("./icon.ico"), IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
    //LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));

    return RegisterClassExW(&wcex);
}

//
//   ФУНКЦИЯ: InitInstance(HINSTANCE, int)
//
//   ЦЕЛЬ: Сохраняет маркер экземпляра и создает главное окно
//
//   КОММЕНТАРИИ:
//
//        В этой функции маркер экземпляра сохраняется в глобальной переменной, а также
//        создается и выводится главное окно программы.
//
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
   hInst = hInstance; // Сохранить маркер экземпляра в глобальной переменной

   HWND hWnd = CreateWindowW(szWindowClass, _T("Merry Christmas"), 
       WS_OVERLAPPEDWINDOW | WS_VISIBLE, 50, 50, MAIN_WINDOW_WIDTH, MAIN_WINDOW_HEIGHT, NULL, NULL,
       hInstance, NULL);

   if (!hWnd)
   {
      return FALSE;
   }

   HMENU hMenuBar = CreateMenu();
   HMENU hSubMenu = CreateMenu();

   AppendMenu(hMenuBar, MF_POPUP, (UINT_PTR)hSubMenu, _T("Menu"));
   AppendMenu(hSubMenu, MF_STRING, START, _T("Start"));
   AppendMenu(hSubMenu, MF_STRING, STOP, _T("Stop"));

   SetMenu(hWnd, hMenuBar);

   ShowWindow(hWnd, nCmdShow);
   UpdateWindow(hWnd);

   return TRUE;
}

//
//  ФУНКЦИЯ: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  ЦЕЛЬ: Обрабатывает сообщения в главном окне.
//
//  WM_COMMAND  - обработать меню приложения
//  WM_PAINT    - Отрисовка главного окна
//  WM_DESTROY  - отправить сообщение о выходе и вернуться
//
//
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    switch (message)
    {
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Разобрать выбор в меню:
            switch (wmId)
            {
            case START:
                // Set the timer
                SetTimer(hWnd, TIMER_ID, 100, NULL);
                break;
            case STOP:
                KillTimer(hWnd, TIMER_ID);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_TIMER:
        if (X >= MAIN_WINDOW_WIDTH - 125)
        {
            leftDirFlag = true;
        }
        if (X <= 5)
        {
            leftDirFlag = false;
        }
        if (!leftDirFlag)
        {
            X += 10;
        }
        else
        {
            X -= 10;
        }
        InvalidateRect(hWnd, NULL, TRUE);
        break;
    case WM_PAINT:
        {
            const char text[] = "merry christmas";
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            TextOutA(hdc, X, Y, text, strlen(text));
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}