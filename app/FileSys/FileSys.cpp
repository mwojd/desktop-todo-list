#include "FileSys.h"
std::vector<std::string> FileSys::loadFile(const std::string& file) {
    std::vector<std::string> list;
    std::ifstream listFile(globals::appData + file);
    std::string entry;
    if (!listFile) {
        std::ofstream createFile(globals::appData + file);
    }

    while (std::getline(listFile, entry)) {
        list.push_back(entry);
    }

    return list;
}

void FileSys::saveFile(const std::vector<std::string>& list, const std::string& file) {
    std::ofstream listFile(globals::appData + file);
    for (const std::string& entry : list) {
        listFile << entry << '\n';
    }
}