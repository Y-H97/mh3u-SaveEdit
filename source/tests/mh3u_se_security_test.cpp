#include "mh3u_se.hpp"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace
{

bool writeFile(const std::string& filename, const std::vector<uint8_t>& contents)
{
    std::ofstream output(filename.c_str(), std::ios::binary);
    output.write(
        reinterpret_cast<const char*>(contents.data()),
        static_cast<std::streamsize>(contents.size())
    );
    return output.good();
}

std::vector<uint8_t> readFile(const std::string& filename)
{
    std::ifstream input(filename.c_str(), std::ios::binary);
    return std::vector<uint8_t>(
        std::istreambuf_iterator<char>(input),
        std::istreambuf_iterator<char>()
    );
}

bool require(bool condition, const char* message)
{
    if (!condition)
    {
        std::cerr << message << std::endl;
    }
    return condition;
}

}

int main()
{
    const std::string shortPath = "short-save.bin";
    const std::string validPath = "valid-save.bin";
    const std::string largePath = "large-save.bin";
    const std::string outputPath = "roundtrip-save.bin";

    const std::vector<uint8_t> shortSave(SAVEFILE_SIZE - 1, 0x41);
    const std::vector<uint8_t> largeSave(SAVEFILE_SIZE + 1, 0x42);
    std::vector<uint8_t> validSave(SAVEFILE_SIZE);
    for (std::size_t i = 0; i < validSave.size(); i++)
    {
        validSave[i] = static_cast<uint8_t>(i % 251);
    }

    bool success =
        require(writeFile(shortPath, shortSave), "Could not create short save") &&
        require(writeFile(validPath, validSave), "Could not create valid save") &&
        require(writeFile(largePath, largeSave), "Could not create large save");

    MH3U_SE editor;
    success =
        require(!editor.load(shortPath), "Short save was accepted") &&
        require(!editor.loaded(), "Rejected save changed loaded state") &&
        require(editor.load(validPath), "Valid save was rejected") &&
        require(editor.loaded(), "Valid save did not set loaded state") &&
        require(!editor.load(largePath), "Oversized save was accepted") &&
        require(editor.loaded(), "Rejected save discarded existing data") &&
        require(editor.save(outputPath), "Valid save could not be written") &&
        require(readFile(outputPath) == validSave, "Roundtrip changed save bytes") &&
        success;

    std::remove(shortPath.c_str());
    std::remove(validPath.c_str());
    std::remove(largePath.c_str());
    std::remove(outputPath.c_str());

    return success ? 0 : 1;
}
