#include "FileSys.h"
#include <filesystem> // Include filesystem library
#include <fstream>

namespace fs = std::filesystem;

std::vector<std::string> FileSys::loadFile(const std::string& file) {
    std::vector<std::string> list;

    // Define the full path to the file
    std::string fullPath = globals::appData + file;

    // Extract the folder path from the full file path
    std::string folderPath = fullPath.substr(0, fullPath.find_last_of("\\"));

    // Check if the folder exists, and if not, create it
    if (!fs::exists(folderPath)) {
        fs::create_directories(folderPath); // Create the folder and any necessary parent folders
    }

    // Open the file for reading
    std::ifstream listFile(fullPath);
    std::string entry;
    if (!listFile) {
        // If file doesn't exist, create an empty file
        std::ofstream createFile(fullPath);
    }

    while (std::getline(listFile, entry)) {
        list.push_back(entry);
    }

    return list;
}

void FileSys::saveFile(const std::vector<std::string>& list, const std::string& file) {
    // Define the full path to the file
    std::string fullPath = globals::appData + file;

    // Extract the folder path from the full file path
    std::string folderPath = fullPath.substr(0, fullPath.find_last_of("\\"));

    // Check if the folder exists, and if not, create it
    if (!fs::exists(folderPath)) {
        fs::create_directories(folderPath); // Create the folder and any necessary parent folders
    }

    // Open the file for writing
    std::ofstream listFile(fullPath);
    for (const std::string& entry : list) {
        listFile << entry << '\n';
    }
}