#include "Encryption.h"

#include <algorithm>
#include <fstream>
#include <random>

bool Encryption::TransformFile(
    const std::string& fileName,
    bool encrypt,
    const Code& code)
{
    std::fstream file(
        fileName,
        std::ios::in | std::ios::out | std::ios::binary);

    if (!file)
        return false;

    file.seekg(0, std::ios::end);
    const std::streamoff length = file.tellg();
    file.seekg(0, std::ios::beg);

    for (std::streamoff i = 0; i < length; ++i)
    {
        char ch;

        file.seekg(i);
        file.read(&ch, 1);

        if (!file)
            return false;

        std::uint8_t value =
            static_cast<std::uint8_t>(
                static_cast<unsigned char>(ch));

        if (encrypt)
        {
            value = static_cast<std::uint8_t>(
                value + code[static_cast<std::size_t>(i) % 256]);
        }
        else
        {
            value = static_cast<std::uint8_t>(
                value - code[static_cast<std::size_t>(i) % 256]);
        }

        file.seekp(i);
        const char output =
            static_cast<char>(value);

        file.write(&output, 1);

        if (!file)
            return false;
    }

    return true;
}

bool Encryption::ReadCodeFile(
    const std::string& fileName,
    Code& code)
{
    std::ifstream file(fileName, std::ios::binary);

    if (!file)
        return false;

    file.read(
        reinterpret_cast<char*>(code.data()),
        static_cast<std::streamsize>(code.size()));

    return file.gcount() == 256;
}

bool Encryption::SaveCodeFile(
    const std::string& fileName,
    const Code& code)
{
    std::ofstream file(
        fileName,
        std::ios::binary | std::ios::trunc);

    if (!file)
        return false;

    file.write(
        reinterpret_cast<const char*>(code.data()),
        static_cast<std::streamsize>(code.size()));

    return file.good();
}

bool Encryption::GenerateCode(Code& code)
{
    std::array<std::uint8_t, 256> values{};

    for (int i = 0; i < 256; ++i)
        values[i] = static_cast<std::uint8_t>(i);

    std::random_device rd;
    std::mt19937 generator(rd());

    for (int i = 0; i < 256; ++i)
    {
        std::uniform_int_distribution<int> distribution(
            0,
            255 - i);

        const int random =
            distribution(generator);

        code[i] = values[random];

        for (int j = random + 1; j < 256 - i; ++j)
            values[j - 1] = values[j];
    }

    return true;
}