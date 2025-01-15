#pragma once
#include "../globals/globals.h"
class FileSys {
public:
    static std::vector<std::string> loadFile(const std::string& file);
    static void saveFile(const std::vector<std::string>& list, const std::string& file);
};

