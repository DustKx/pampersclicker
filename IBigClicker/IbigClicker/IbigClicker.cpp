#include <windows.h>
#include <iostream>
#include <fstream>
#include <thread>
#include <atomic>
#include <locale>

POINT p1{}, p2{};
std::atomic<bool> running{ false };
std::atomic<int> mode{ 0 }; // 0=stop, 3=two points, 4=point+space

void savePoint(const char* name, POINT p) {
    std::ofstream file("points.txt", std::ios::app);
    file << name << ": " << p.x << " " << p.y << "\n";
}

void leftClick(int x, int y) {
    SetCursorPos(x, y);
    mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
    mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
}

void pressSpace() {
    keybd_event(VK_SPACE, 0, 0, 0);
    keybd_event(VK_SPACE, 0, KEYEVENTF_KEYUP, 0);
}

void clickLoop() {
    while (true) {
        if (!running) {
            Sleep(50);
            continue;
        }

        if (mode == 3) {
            leftClick(p1.x, p1.y);
            Sleep(10);
            leftClick(p2.x, p2.y);
        }
        else if (mode == 4) {
            leftClick(p1.x, p1.y);
            Sleep(10);
            pressSpace();
        }

        Sleep(10);
    }
}

int main() {
    // Устанавливаем UTF-8 для консоли
    SetConsoleCP(65001);
    SetConsoleOutputCP(65001);
    std::locale::global(std::locale(""));

    std::wcout << L"=== СУПЕР АЛЬФА - КЛИКЕР ===\n"
        L"Ctrl + Shift + 1 - установить координату 1\n"
        L"Ctrl + Shift + 2 - установить координату 2\n"
        L"Ctrl + Shift + 3 - режим: клик по 2 точкам\n"
        L"Ctrl + Shift + 4 - режим: координата 1 + пробел\n"
        L"Ctrl + Shift + 5 - остановить АЛЬФА - КЛИКЕР\n\n";

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
                savePoint("P1", p1);
                std::wcout << L"Координата 1 сохранена: " << p1.x << L" " << p1.y << L"\n";
                break;

            case 2:
                GetCursorPos(&p2);
                savePoint("P2", p2);
                std::wcout << L"Координата 2 сохранена: " << p2.x << L" " << p2.y << L"\n";
                break;

            case 3:
                running = true;
                mode = 3;
                std::wcout << L"Режим: клик по 2 точкам\n";
                break;

            case 4:
                running = true;
                mode = 4;
                std::wcout << L"Режим: координата 1 + пробел\n";
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