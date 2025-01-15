#pragma once
#include "./globals.h"

namespace globals {
    ImFont* iconFont = nullptr;

    std::vector<std::string> tasks = {};
    std::vector<std::string> tasksCompleted = {};

    std::string appData = "";
    std::string taskName = "";
    int screen = 0;
    // 0 - task list
    // 1 - add task screen
}

