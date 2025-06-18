#include <Windows.h>
#include <iostream>
#include <vector>
#include <cmath>

const int Threshold = 30;
const int OffsetX = 2;
const int OffsetY = 2;

void MouseClick() {
    INPUT input[2] = {};

    input[0].type = INPUT_MOUSE;
    input[0].mi.dwFlags = MOUSEEVENTF_LEFTDOWN;

    input[1].type = INPUT_MOUSE;
    input[1].mi.dwFlags = MOUSEEVENTF_LEFTUP;

    SendInput(2, input, sizeof(INPUT));
}

double ColorDistance(BYTE r1, BYTE g1, BYTE b1, BYTE r2, BYTE g2, BYTE b2) {
    int dr = r1 - r2;
    int dg = g1 - g2;
    int db = b1 - b2;
    return sqrt(dr * dr + dg * dg + db * db);
}

COLORREF GetPixelFromMemory(const std::vector<BYTE>& buffer, int x, int y, int width) {
    int index = (y * width + x) * 4;
    BYTE blue = buffer[index];
    BYTE green = buffer[index + 1];
    BYTE red = buffer[index + 2];
    return RGB(red, green, blue);
}

int main() {
    std::cout << "TriggerBot Eye Edition - Developed By Sir. ZOZ\n";

    HDC hdcScreen = GetDC(NULL);
    HDC hdcMem = CreateCompatibleDC(hdcScreen);
    const int width = 1, height = 1;

    BITMAPINFO bmi = {};
    bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bmi.bmiHeader.biWidth = width;
    bmi.bmiHeader.biHeight = -height; // top-down
    bmi.bmiHeader.biPlanes = 1;
    bmi.bmiHeader.biBitCount = 32;
    bmi.bmiHeader.biCompression = BI_RGB;

    std::vector<BYTE> pixels(width * height * 4);
    HBITMAP hBitmap = CreateCompatibleBitmap(hdcScreen, width, height);
    SelectObject(hdcMem, hBitmap);

    COLORREF baseColor = RGB(0, 0, 0);

    while (true) {
        if (GetAsyncKeyState(VK_END) & 0x8000) {
            std::cout << "Program terminated by user\n";
            break;
        }

        if (GetAsyncKeyState('X') & 0x8000) {
            POINT pt;
            GetCursorPos(&pt);

            BitBlt(hdcMem, 0, 0, width, height, hdcScreen, pt.x + OffsetX, pt.y + OffsetY, SRCCOPY);
            GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(), &bmi, DIB_RGB_COLORS);

            baseColor = GetPixelFromMemory(pixels, 0, 0, width);

            BYTE r1 = GetRValue(baseColor);
            BYTE g1 = GetGValue(baseColor);
            BYTE b1 = GetBValue(baseColor);

            while (GetAsyncKeyState('X') & 0x8000) {
                Sleep(5);
                GetCursorPos(&pt);

                BitBlt(hdcMem, 0, 0, width, height, hdcScreen, pt.x + OffsetX, pt.y + OffsetY, SRCCOPY);
                GetDIBits(hdcMem, hBitmap, 0, height, pixels.data(), &bmi, DIB_RGB_COLORS);

                COLORREF current = GetPixelFromMemory(pixels, 0, 0, width);
                BYTE r2 = GetRValue(current);
                BYTE g2 = GetGValue(current);
                BYTE b2 = GetBValue(current);

                if (ColorDistance(r1, g1, b1, r2, g2, b2) > Threshold) {
                    MouseClick();
                    Sleep(300);
                }

                baseColor = current;
            }
        }

        Sleep(30);
    }

    DeleteObject(hBitmap);
    DeleteDC(hdcMem);
    ReleaseDC(NULL, hdcScreen);

    return 0;
}
