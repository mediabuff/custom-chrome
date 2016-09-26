// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#include <Windows.h>
#include "chrome.hpp"

int WINAPI WinMain(HINSTANCE instance, HINSTANCE legacy, LPSTR command_line, int show_command) {

    chrome::application chrome_application{ command_line };
    return chrome_application.execute();

}