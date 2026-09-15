#pragma once

#include <string>

#include "Encryption.h"

class BackupManager
{
public:
    enum class Decision
    {
        MAKE_THE_BACKUP_FILE,
        REMOVE_THE_BACKUP_FILE
    };

    // Makes/removes an exact backup copy.
    // The source file is NOT modified.
    static bool BackUp(
        const std::string& fileName,
        const std::string& backupDirectory,
        Decision choice,
        const Encryption::Code& encryptionCode);

    // Creates an encrypted backup from a
    // currently decrypted source file.
    // The source file remains decrypted.
    static bool CreateEncryptedBackup(
        const std::string& fileName,
        const std::string& backupDirectory,
        const Encryption::Code& encryptionCode);
};