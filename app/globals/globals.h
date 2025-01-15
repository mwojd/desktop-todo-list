#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib> 
#include "../../imgui/imgui.h"
#include "../../imgui/imgui_impl_dx9.h"
#include "../../imgui/imgui_impl_win32.h"
#include "../../imgui/imgui_stdlib.h"
namespace globals {
    extern ImFont* iconFont;

    extern std::vector<std::string> tasks;
    extern std::vector<std::string> tasksCompleted;

    extern std::string appData;
    extern std::string taskName;
    extern int screen;
    // 0 - task list
    // 1 - add task screen
}

