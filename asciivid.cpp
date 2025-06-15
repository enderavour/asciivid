#include <iostream>
#include <Windows.h>
#include <cstdlib>
#include <opencv2/opencv.hpp>
#include <chrono>
#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"
#define WIDTH 100
#define HEIGHT 30
#define FPS 38

const std::string asciich = " .:-=+*#%@";
const int levels = asciich.size();

DWORD SetTerminalAttrs(HANDLE &hCons, short width, short height)
{
    CONSOLE_CURSOR_INFO cci;
    GetConsoleCursorInfo(hCons, &cci);
    SMALL_RECT sm = {420, 51, 102, 32};
    cci.bVisible = false;
    SetConsoleScreenBufferSize(hCons, {102, 80});
    SetConsoleWindowInfo(hCons, TRUE, &sm);
    SetConsoleCursorInfo(hCons, &cci);
    return cci.dwSize;
}

std::string imgToAscii(const cv::Mat &m)
{
    int rows = m.rows;
    int cols = m.cols;

    std::string ascii;

    for (int i = 0; i < rows; ++i)
    {
        for (int j = 0; j < cols; ++j)
        {
            uchar pixv = m.at<uchar>(i, j);

            int ind = (pixv * (levels - 1)) / 255;
            ascii += asciich[ind];
        }
        ascii += "\n";
    }
    return ascii;
}

void display(const std::string &path, const std::string &audpath, int width, int height, HANDLE &buf)
{
    cv::VideoCapture vc(path);
    ma_engine eng;
    ma_engine_init(nullptr, &eng);

    cv::Mat frame, grayframe;

    ma_engine_play_sound(&eng, audpath.c_str(), nullptr);
    while (vc.read(frame))
    {
        SetConsoleCursorPosition(buf, {0, 0});

        cv::cvtColor(frame, grayframe, cv::COLOR_BGR2GRAY);

        cv::resize(grayframe, grayframe, cv::Size(width, height));

        std::string ascii = imgToAscii(grayframe);

        std::cout << ascii << std::endl;

        std::this_thread::sleep_for(std::chrono::milliseconds(1000 / FPS));
    }

    vc.release();
    ma_engine_uninit(&eng);
}


int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        std::cout << "Usage: asciivid <*.mp4>\n";
        return -1;
    }
    HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);
    std::string video_name = std::string(argv[1]);
    char cmd[50] = {0};
    std::sprintf(cmd, "ffmpeg -i %s -vn audio.wav", video_name.c_str());
    STARTUPINFOA si = {0};
    PROCESS_INFORMATION pi = {0};
    si.cb = sizeof(si);
    si.dwFlags |= STARTF_USESTDHANDLES;
    HANDLE hNull = CreateFileA("NUL", GENERIC_WRITE, FILE_SHARE_READ | FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, 0, nullptr);
    si.hStdOutput = hNull;
    si.hStdInput = hNull;
    si.hStdError = hNull;

    std::cout << "Extracting audio..." << std::endl;
    CreateProcessA(
        nullptr,
        cmd,
        nullptr,
        nullptr,
        FALSE,
        0,
        nullptr,
        nullptr,
        &si,
        &pi
    );
    WaitForSingleObject(pi.hProcess, INFINITE);
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
    CloseHandle(hNull);
    std::cout << "Ready!" << std::endl;
    system("cls");
    DWORD dws = SetTerminalAttrs(h, 98, 80);
    SetConsoleCursorPosition(h, {0, 0});
    display(video_name, "audio.wav", 100, 30, h);
    system("cls");
    CONSOLE_CURSOR_INFO ci{dws, true};
    SetConsoleCursorInfo(h, &ci);
    std::cout << "Deleting audio...\n";
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    system("del audio.wav");
    return 0;
}