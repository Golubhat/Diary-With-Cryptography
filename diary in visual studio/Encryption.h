#pragma once

#include <array>
#include <cstdint>
#include <string>
#include <vector>

class Encryption
{
public:
    using Code = std::array<std::uint8_t, 256>;

    static bool TransformFile(
        const std::string& fileName,
        bool encrypt,
        const Code& code);

    static bool ReadCodeFile(
        const std::string& fileName,
        Code& code);

    static bool SaveCodeFile(
        const std::string& fileName,
        const Code& code);

    static bool GenerateCode(Code& code);
};