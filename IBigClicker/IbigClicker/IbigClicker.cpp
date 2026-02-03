#include <windows.h>
#include <iostream>
#include <thread>
#include <atomic>
#include <locale>

// ===== НАСТРОЙКИ =====
std::atomic<int> delayBetweenClicksMs{ 10 };   // между кликами
std::atomic<int> delayBeforeKeyMs{ 10 };       // перед нажатием клавиши
std::atomic<int> keyHoldMs{ 25 };               // удержание клавиши
std::atomic<int> loopDelayMs{ 0 };             // задержка цикла
// =====================

POINT p1{}, p2{};
std::atomic<bool> running{ false };
std::atomic<int> mode{ 0 }; // 0=stop, 3=two points, 4=point+space

void leftClick(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    Sleep(keyHoldMs);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void pressKey(WORD vk) {
    keybd_event(vk, 0, 0, 0);                    // нажать
    Sleep(keyHoldMs);                            // удержание
    keybd_event(vk, 0, KEYEVENTF_KEYUP, 0);      // отпустить
}

void clickLoop() {
    while (true) {
        if (!running) {
            Sleep(50);
            continue;
        }

        if (mode == 3) {
            leftClick(p1.x, p1.y);
            Sleep(delayBetweenClicksMs);
            leftClick(p2.x, p2.y);
        }
        else if (mode == 4) {
            leftClick(p1.x, p1.y);
            Sleep(delayBeforeKeyMs);
            pressKey(VK_SPACE);
        }

        Sleep(loopDelayMs);
    }
}

int main() {
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    std::locale::global(std::locale(""));

    std::wcout << L"=== СУПЕР АЛЬФА - КЛИКЕР ===\n"
        L"Ctrl + Shift + 1 - установить координату 1\n"
        L"Ctrl + Shift + 2 - установить координату 2\n"
        L"Ctrl + Shift + 3 - режим: клик по 2 точкам\n"
        L"Ctrl + Shift + 4 - режим: координата 1 + клавиша\n"
        L"Ctrl + Shift + 5 - остановить\n\n";

    RegisterHotKey(nullptr, 1, MOD_CONTROL | MOD_SHIFT, '1');
    RegisterHotKey(nullptr, 2, MOD_CONTROL | MOD_SHIFT, '2');
    RegisterHotKey(nullptr, 3, MOD_CONTROL | MOD_SHIFT, '3');
    RegisterHotKey(nullptr, 4, MOD_CONTROL | MOD_SHIFT, '4');
    RegisterHotKey(nullptr, 5, MOD_CONTROL | MOD_SHIFT, '5');

    std::thread worker(clickLoop);
    worker.detach();

    MSG msg{};
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (msg.message == WM_HOTKEY) {
            switch (msg.wParam) {
            case 1:
                GetCursorPos(&p1);
                std::wcout << L"P1: " << p1.x << L" " << p1.y << L"\n";
                break;

            case 2:
                GetCursorPos(&p2);
                std::wcout << L"P2: " << p2.x << L" " << p2.y << L"\n";
                break;

            case 3:
                running = true;
                mode = 3;
                std::wcout << L"Режим: 2 точки\n";
                break;

            case 4:
                running = true;
                mode = 4;
                std::wcout << L"Режим: точка + клавиша\n";
                break;

            case 5:
                running = false;
                mode = 0;
                std::wcout << L"Остановка\n";
                break;
            }
        }
    }

    return 0;
}