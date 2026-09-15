#include "BackupManager.h"

#include <filesystem>
#include <fstream>

bool BackupManager::BackUp(
    const std::string& fileName,
    const std::string& backupDirectory,
    Decision choice,
    const Encryption::Code& encryptionCode)
{
    const std::filesystem::path sourceFile =
        fileName;

    const std::filesystem::path backupDir =
        backupDirectory;

    const std::filesystem::path backupFile =
        backupDir /
        sourceFile.filename();

    // ========================================================
    // MAKE BACKUP
    //
    // Exact byte-for-byte copy.
    //
    // IMPORTANT:
    // No encryption is performed here.
    // ========================================================

    if (choice ==
        Decision::MAKE_THE_BACKUP_FILE)
    {
        std::error_code error;

        std::filesystem::create_directories(
            backupDir,
            error);

        if (error)
            return false;

        if (!std::filesystem::exists(
            sourceFile,
            error))
        {
            return false;
        }

        const std::filesystem::path temporaryFile =
            backupDir /
            (sourceFile.filename().string() +
                ".backup_tmp");

        std::ifstream input(
            sourceFile,
            std::ios::binary);

        if (!input)
            return false;

        std::ofstream output(
            temporaryFile,
            std::ios::binary |
            std::ios::trunc);

        if (!output)
        {
            input.close();
            return false;
        }

        output << input.rdbuf();

        if (!output.good())
        {
            input.close();
            output.close();

            std::filesystem::remove(
                temporaryFile,
                error);

            return false;
        }

        input.close();
        output.close();

        std::filesystem::remove(
            backupFile,
            error);

        error.clear();

        std::filesystem::rename(
            temporaryFile,
            backupFile,
            error);

        if (error)
        {
            std::filesystem::remove(
                temporaryFile,
                error);

            return false;
        }

        return true;
    }

    // ========================================================
    // REMOVE BACKUP
    // ========================================================

    if (choice ==
        Decision::REMOVE_THE_BACKUP_FILE)
    {
        std::error_code error;

        std::filesystem::remove(
            backupFile,
            error);

        if (error)
            return false;

        return true;
    }

    return false;
}


// ============================================================
// CREATE ENCRYPTED BACKUP
//
// Source file is currently DECRYPTED.
//
// The original source remains untouched.
// Only the temporary copy is encrypted.
// ============================================================

bool BackupManager::CreateEncryptedBackup(
    const std::string& fileName,
    const std::string& backupDirectory,
    const Encryption::Code& encryptionCode)
{
    const std::filesystem::path sourceFile =
        fileName;

    const std::filesystem::path backupDir =
        backupDirectory;

    const std::filesystem::path backupFile =
        backupDir /
        sourceFile.filename();

    std::error_code error;

    // Create Backup directory.
    std::filesystem::create_directories(
        backupDir,
        error);

    if (error)
        return false;

    // Source must exist.
    if (!std::filesystem::exists(
        sourceFile,
        error))
    {
        return false;
    }

    const std::filesystem::path temporaryFile =
        backupDir /
        (sourceFile.filename().string() +
            ".backup_tmp");

    // --------------------------------------------------------
    // Copy decrypted source to temporary file.
    // --------------------------------------------------------

    std::ifstream input(
        sourceFile,
        std::ios::binary);

    if (!input)
        return false;

    std::ofstream output(
        temporaryFile,
        std::ios::binary |
        std::ios::trunc);

    if (!output)
    {
        input.close();
        return false;
    }

    output << input.rdbuf();

    if (!output.good())
    {
        input.close();
        output.close();

        std::filesystem::remove(
            temporaryFile,
            error);

        return false;
    }

    input.close();
    output.close();

    // --------------------------------------------------------
    // Encrypt ONLY the temporary copy.
    //
    // Original source remains decrypted.
    // --------------------------------------------------------

    if (!Encryption::TransformFile(
        temporaryFile.string(),
        true,
        encryptionCode))
    {
        std::filesystem::remove(
            temporaryFile,
            error);

        return false;
    }

    // --------------------------------------------------------
    // Replace existing backup.
    // --------------------------------------------------------

    std::filesystem::remove(
        backupFile,
        error);

    error.clear();

    std::filesystem::rename(
        temporaryFile,
        backupFile,
        error);

    if (error)
    {
        std::filesystem::remove(
            temporaryFile,
            error);

        return false;
    }

    return true;
}