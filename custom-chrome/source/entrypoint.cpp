// Copyright ©2016 Domagoj "oberth" Pandža
// MIT license | Read LICENSE.txt for details.

#include <application.hpp>

auto main(int argument_count, char* arguments[]) -> int {
    chrome::application chrome_application { arguments, argument_count };
    return chrome_application.execute();
}